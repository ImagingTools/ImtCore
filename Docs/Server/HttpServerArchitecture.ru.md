# HTTP(S)/WebSocket-сервер imtrest на базе QtHttpServer

Документ описывает серверный сетевой слой `Include/imtrest` после перехода на QtHttpServer (Qt 6.8):
архитектуру, потоки выполнения, путь запроса, работу WebSocket на одном порту, ограничения,
результаты замеров и перечень заменённых сущностей.

## 1. Архитектура

Слой разделён на две части с чёткой границей ответственности.

| Уровень | Классы | Ответственность |
|---|---|---|
| Транспорт (без ACF) | `CHttpTransport`, `CHttpListener`, `CHttpServerShard`, `CHttpConnectionQueue`, интерфейс `IHttpTransportHandler` | TCP/TLS, разбор HTTP/1.1 (QtHttpServer), keep-alive, тайм-ауты, распознавание WebSocket-upgrade, запись ответов |
| Доменная модель (ACF) | `CHttpServerComp` (`ImtRestPck/HttpServer`) | Преобразование транспортного запроса в `IRequest` через `IProtocolEngine`, вызов `IRequestServlet`, приём ответов через `IResponseDispatcher`, SSL-конфигурация, `IServer` |
| WebSocket | `CWebSocketServerComp` + интерфейс `IWebSocketUpgradeHandler` | Handshake (с поддержкой subprotocol), обработка WS-сообщений как и раньше |
| Обработка | `CWorkerManagerComp`, сервлеты, `CHttpProtocolEngineComp`, `CHttpResponse` | Без изменений |

Доменные абстракции `IRequest`/`IResponse`, `IRequestServlet`, `IProtocolEngine`, `IResponseDispatcher`
и `IServer` сохранены. Сервлеты, `CWorkerManagerComp` и `.acc`-каркасы продолжают работать с теми же
интерфейсами.

```
            ┌──────────── поток компонента (main) ──────────────┐
 клиент ──► │ CHttpListener (QTcpServer, только accept)         │
            │   │ дескриптор сокета → наименее загруженный shard │
            │ CHttpServerComp ──► IRequestServlet (WorkerManager)│
            │ CWebSocketServerComp (QWebSocketServer)           │
            └───────────────────────────────────────────────────┘
                    │ queued                       ▲ queued
            ┌───────▼──── I/O-поток k (N штук) ────┴────────────┐
            │ CHttpServerShard (QAbstractHttpServer)            │
            │  TLS-handshake, классификация HTTP / WS-upgrade,  │
            │  разбор HTTP/1.1, keep-alive, запись ответа       │
            └───────────────────────────────────────────────────┘
                                                   ▲ SendResponse (любой поток)
            ┌──────────── рабочие потоки WorkerManager ──────────┐
            │ сервлет формирует IResponse → IResponseDispatcher  │
            └────────────────────────────────────────────────────┘
```

## 2. Потоки выполнения

* **Поток компонента (main).** Здесь принимаются соединения: `CHttpListener::incomingConnection`
  получает только дескриптор, сокет не создаёт. Здесь же вызывается `IRequestServlet::ProcessRequest`
  (как раньше в `CTcpServerComp::OnNewThreadConnection`) и выполняется WebSocket-handshake.
* **I/O-потоки.** Фиксированный пул из `IoThreadCount` потоков (0 — по числу ядер) создаётся один раз
  при первом `Listen`. В каждом потоке живёт один `CHttpServerShard` (наследник `QAbstractHttpServer`)
  со своими сокетами. Соединение закреплено за одним потоком на всё время жизни. Новые потоки на
  соединение или запрос не создаются. В старом `CMultiThreadServer` на каждое соединение запускался
  отдельный `CSocketThread`.
* **Рабочие потоки.** Пул `CWorkerManagerComp` (атрибут `ThreadsLimit`) не изменился.
  Ответ отправляется из рабочего потока через `IResponseDispatcher::SendResponse`.

### Асинхронность и отсутствие блокировок

* I/O-поток никогда не ждёт ответа. `IHttpTransportHandler::OnHttpRequest` ставит обработку в очередь
  потока компонента (`Qt::QueuedConnection`) и сразу возвращается.
* `CHttpTransport::SendResponse` потокобезопасен: под мьютексом находится shard по `requestId`, а сама
  запись ставится в очередь I/O-потока. Сеть и сервлеты не блокируют друг друга.
* Пока ответ на запрос не записан, QtHttpServer не читает следующий запрос из этого же соединения
  (HTTP/1.1 без параллельного pipelining). Остальные соединения потока обрабатываются параллельно.
* Если клиент отключился до ответа, запрос снимается с регистрации, и поздний `SendResponse`
  возвращает `false`.

## 3. Путь HTTP-запроса

1. `CHttpListener` принимает дескриптор и передаёт его наименее загруженному `CHttpServerShard`.
2. Shard создаёт `QSslSocket` (TLS) или `QTcpSocket` и запускает тайм-аут `RequestHeaderTimeout`
   на TLS-handshake и заголовки первого запроса.
3. **Классификация**: shard читает заголовок первого запроса через `peek`, не потребляя данные.
   * Обычный HTTP-запрос — сокет передаётся в QtHttpServer (`QAbstractHttpServer::bind` через
     внутреннюю очередь `CHttpConnectionQueue`).
   * WebSocket-upgrade — см. раздел 4.
4. QtHttpServer разбирает запрос и вызывает `CHttpServerShard::handleRequest`. Данные копируются в
   `IHttpTransportHandler::RequestData`: идентификатор, метод, URL со схемой `http`/`https`, заголовки,
   тело, адрес клиента. Ответчик (`QHttpServerResponder`) хранится до прихода ответа.
5. `CHttpServerComp` в потоке компонента создаёт `CHttpRequest` через `IProtocolEngine::CreateRequest`,
   заполняет его и вызывает `RequestHandler->ProcessRequest`.
   * Синхронный сервлет вернул ответ — ответ отправляется сразу, запрос удаляется.
   * Асинхронный (`CWorkerManagerComp`) — владение запросом переходит к нему, ответ приходит позже
     через `IResponseDispatcher`.
6. `CHttpServerComp::SendResponse` переводит `IResponse` в HTTP: код берётся из
   `IProtocolEngine::GetProtocolStatusCode`, затем заголовки и `Content-Type` из `GetDataTypeId()`
   (если он не задан явно). `Content-Length` вычисляет QtHttpServer.
7. Shard записывает ответ, выставляет `Connection: close` или `keep-alive` по запросу клиента и
   перезапускает тайм-аут простоя `KeepAliveTimeout`.

### Keep-alive

* HTTP/1.1: соединение постоянное по умолчанию, `Connection: close` закрывает его после ответа.
* HTTP/1.0: соединение постоянное только при `Connection: keep-alive`, иначе закрывается после ответа.
* Простаивающее соединение закрывается через `KeepAliveTimeout` мс (по умолчанию 60000, 0 — без тайм-аута).

## 4. WebSocket на одном порту

* `CHttpServerComp` получает ссылку `WebSocketUpgradeHandler` (`IWebSocketUpgradeHandler`,
  её реализует `CWebSocketServerComp`). Если ссылка задана, первый запрос соединения с заголовком
  `Upgrade: websocket` в QtHttpServer не попадает. Сокет (для TLS — `QSslSocket` после handshake)
  отвязывается от I/O-потока, переносится в поток `CWebSocketServerComp` и передаётся в
  `QWebSocketServer::handleConnection`. Запрос upgrade из сокета не читался, поэтому handshake
  целиком выполняет `QWebSocketServer`.
* Используется собственный `QWebSocketServer` компонента, а не встроенный механизм QtHttpServer:
  * внутренний `QWebSocketServer` QtHttpServer 6.8 не поддерживает subprotocol (`graphql-ws`,
    `graphql-transport-ws`), а без него браузерные GraphQL-подписки не работают;
  * созданные им `QWebSocket` нельзя корректно переносить между потоками.
* После handshake WebSocket-соединения обрабатываются как раньше: `CWebSocketThread`,
  `CWebSocketServletComp`, подписки, `SupportedSubprotocols`.
* Атрибут `ListenWebSocketPort` у `CWebSocketServerComp` (по умолчанию `true`) сохраняет отдельный
  WebSocket-порт для совместимости с существующими клиентами. При `false` WebSocket доступен только
  через HTTP-порт.
* В `ImtCoreServerBase.acc` ссылка `HttpServerFramework.WebSocketUpgradeHandler` связана с
  `WebSocketServerFramework`, поэтому WS(S) доступен и на HTTP-порту, и на прежнем отдельном порту.

## 5. TLS

* Используется та же конфигурация: `SslConfiguration` + `SslConfigurationManager`
  (`imtcom::ISslConfigurationManager::CreateSslConfiguration`). Изменение модели SSL-конфигурации
  перезапускает прослушивание с новой конфигурацией, установленные соединения не разрываются.
* В ALPN объявляется только `http/1.1`.
* HTTP-сервер теперь применяет **полную** конфигурацию менеджера, как и WebSocket-сервер: сертификат,
  ключ, CA, протокол, `peerVerifyMode`. Раньше `CSocket` брал только сертификат, ключ и протокол.

## 6. Ограничения и допущения

* **Требуется Qt ≥ 6.8 с модулем QtHttpServer.** Без него (Qt 5, Qt 6 < 6.8, модуль не установлен)
  `CHttpServerComp` не компилируется (макрос `IMTREST_HTTP_SERVER_AVAILABLE` в `CHttpTransport.h`),
  не регистрируется в `ImtRestPck`, и `HttpServerFramework` не загрузится.
* **Лицензия QtHttpServer: `LicenseRef-Qt-Commercial OR GPL-3.0-only`**. Это не LGPL, в отличие от
  QtCore/QtNetwork/QtWebSockets. Распространять продукт с этим модулем можно либо по коммерческой
  лицензии Qt, либо соблюдая GPL-3.0 для всего продукта.
* WebSocket-upgrade распознаётся только в первом запросе соединения. Upgrade после обычного
  HTTP-запроса в том же keep-alive-соединении получает `400`, и соединение закрывается. Браузеры и
  `QWebSocket` всегда открывают для WebSocket новое соединение.
* HTTP/2 не поддерживается (как и раньше).
* Для передачи сокетов в QtHttpServer каждый I/O-поток держит слушающий сокет на `127.0.0.1:0`
  (или `::1`) с приостановленным accept. Реальные подключения к нему отклоняются.
* Сервлеты по-прежнему вызываются в потоке компонента (main). Блокирующий синхронный сервлет
  задерживает диспетчеризацию остальных запросов, но не сетевой ввод-вывод.
* Методы, кроме GET/POST/PUT/DELETE/PATCH/HEAD/OPTIONS, получают `501`.
* Ответы всегда HTTP/1.1, имена заголовков — в нижнем регистре (по RFC 9110 регистр имён заголовков
  не значим).
* Повторяющиеся заголовки запроса объединяются через `, ` (`cookie` — через `; `). Раньше оставалось
  только последнее значение.

## 7. Результаты замеров

Условия: 4 vCPU (GitHub runner), localhost, Qt 6.8.4, QtHttpServer 6.8.3, Release, `wrk -t4`, `ab`.
В обоих стендах та же модель обработки, что и в реальном каркасе: переход в main-поток, затем пул
из 100 рабочих потоков, ответ из рабочего потока через `SendResponse`, тело `hello world`.
Старый стек (`CMultiThreadServer`/`CSocketThread`/`CSocket`/`CHttpRequest`/`CHttpSender`) собран из
текущих исходников с минимальной заглушкой `CTcpServerComp`. Новый стек — `CHttpTransport`.
`/slow` — обработка 200 мс в рабочем потоке.

| Сценарий | Старый стек | Новый стек |
|---|---|---|
| keep-alive, 10 соединений | 7 971 req/s, 0.94 мс | 48 853 req/s, 0.16 мс |
| keep-alive, 100 соединений | 1 575 req/s, 586 914 ошибок записи, затем падение процесса | 57 057 req/s, 1.73 мс, 0 ошибок |
| keep-alive, 500 соединений | — | 53 250 req/s, 9.31 мс, 0 ошибок |
| `Connection: close`, 50 соединений | 2 102 req/s, 510 982 ошибок записи | 25 445 req/s, 1.62 мс, 0 ошибок |
| `/slow` (200 мс), 50 соединений | 231.7 req/s, 201.1 мс | 231.6 req/s, 200.6 мс |
| HTTPS keep-alive, 10 соединений | 185 req/s, 39.9 мс | 41 023 req/s, 0.31 мс |
| `ab` HTTP/1.0 без keep-alive, 50 | зависает: соединение не закрывается после ответа | 18 933 req/s, 0 ошибок |
| `ab -k` HTTP/1.0 keep-alive, 50 | — | 55 085 req/s, 0 ошибок |

Выводы:
* Старый сервер отвечал HTTP/1.0 без `Connection: keep-alive`, поэтому клиенты закрывали соединение
  после каждого ответа (для HTTPS — полный TLS-handshake на каждый запрос), а на каждое соединение
  создавался поток. Новый стек действительно держит keep-alive и не создаёт потоков.
* Сценарий `/slow` показывает, что параллелизм обработки не уменьшился: 50 одновременных запросов
  по 200 мс обрабатываются параллельно (теоретический предел — 250 req/s).
* При 100 и более соединениях старый стек выводил `QThread::wait: Thread tried to wait on itself` и
  `QThread: Destroyed while thread is still running`, после чего процесс падал.

Кроме замеров, на тех же стендах проверена функциональность:
* keep-alive, `Connection: close`, HTTP/1.0 с keep-alive и без;
* HTTPS с ALPN;
* WS и WSS на HTTP-порту — с subprotocol `graphql-transport-ws` и без subprotocol;
* отказ в upgrade, если это не первый запрос соединения;
* отключение клиента во время обработки;
* тайм-ауты простоя и заголовков.

## 8. Что удалено или заменено

| Было | Стало | Причина |
|---|---|---|
| `TcpServer` (`CTcpServerComp`) в `HttpServerFramework.acc` | `HttpServer` (`CHttpServerComp`) | Новый HTTP-стек на QtHttpServer |
| Атрибут `ThreadsLimit` у TcpServer (лимит потоков-сокетов) | `IoThreadCount` (фиксированный пул I/O-потоков) | Нет потока на соединение |
| Тайм-аут 5 с до первого полного запроса | `RequestHeaderTimeout` (10 с, TLS + заголовки) | Тайм-аут теперь настраивается |
| Соединения без тайм-аута простоя | `KeepAliveTimeout` (60 с) | Защита от накопления простаивающих соединений |
| Ответы HTTP/1.0, `http_parser.c` для входящих HTTP-запросов | HTTP/1.1 на QtHttpServer | Настоящий keep-alive |
| `CHttpSender` для ответов HTTP-сервера | Запись через `QHttpServerResponder` | — |
| Пустой `CHttpRequest::GetRemoteAddress()` | Адрес клиента заполнен | — |
| `CHttpRequest::SetMethodType` только для GET/POST/PUT | Все методы `MethodType` | — |
| Отдельный порт для WebSocket | Отдельный порт (опционально) + WS на HTTP-порту | HTTP и WS на одном порту |

Новые сущности:
* `IHttpTransportHandler`, `CHttpTransport`, `CHttpServerShard`, `CHttpServerComp`, `IWebSocketUpgradeHandler`;
* сеттеры `CHttpRequest`: `SetUrl`, `SetRemoteAddress`, `SetRequestId`, `SetState`;
* атрибут `ListenWebSocketPort` у `CWebSocketServerComp`.

Классы `CTcpServerComp`, `CMultiThreadServer`, `CSocketThread`, `CSocket` и `CHttpSender` не удалены:
их использует `TcpServerFramework.acc` (сырой TCP-протокол `TcpProtocolEngine`). HTTP-каркасы их
больше не используют.

## 9. Что проверить вручную

* Сборка с Qt ≥ 6.8 и QtHttpServer (CMake и QMake), загрузка `ImtRestPck` и каркасов
  `HttpServerFramework`, `WebSocketServerFramework`, `ImtCoreServerBase`.
* Web-клиент (QML/JQML) по HTTP и HTTPS: загрузка страниц, GraphQL-запросы, загрузка и выгрузка файлов.
* GraphQL-подписки через WS/WSS на HTTP-порту (`ws(s)://host:<HTTP-порт>/...`) и на прежнем WS-порту.
* Смена SSL-конфигурации во время работы: прослушивание перезапускается без разрыва соединений.
* Корректное завершение сервера при активных соединениях и незавершённых запросах.
