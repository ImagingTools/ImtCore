# HTTP и WebSocket на одном порту

По умолчанию сервер ImtCore слушает два порта: HTTP (`CTcpServerComp`) и WebSocket (`CWebSocketServerComp`).
Режим одного порта позволяет принимать WebSocket-подключения (в том числе WSS) на порту HTTP(S).
Режим выключен по умолчанию и включается настройкой в .acc.

## Как устроено

1. Соединение принимает HTTP-сервер (`CMultiThreadServer` → `CSocketThread` → `CSocket`), как и раньше.
2. Если у `TcpServer` задана ссылка `WebSocketUpgradeHandler`, заголовок каждого нового запроса читается
   внутри транзакции сокета (`QIODevice::startTransaction`). Признак upgrade даёт протокол соединения
   (`CConnectionTimeoutBehavior::isWebSocketUpgradeRequest`), для HTTP это `CHttpRequest::IsWebSocketUpgradeRequested()`:
   `GET` с `Upgrade: websocket` и `Connection: upgrade` (флаг `upgrade` парсера). Когда заголовок получен:
   - обычный запрос: транзакция подтверждается, дальше всё как раньше;
   - upgrade: транзакция откатывается (`rollbackTransaction`), байты handshake снова лежат в буфере сокета.
     `CSocket` отключается от сигналов сокета, удаляет свой `CSocketTimeoutWatcher` и потоковый отправитель
     и отложенным вызовом (в своём потоке) передаёт сокет в `imtrest::IWebSocketUpgradeHandler::HandleWebSocketUpgrade`.
     Поток `CSocketThread` освобождается (соединение больше не учитывается в `ThreadsLimit`).
3. `CWebSocketServerComp` (реализует `IWebSocketUpgradeHandler`) переносит сокет в свой поток
   (`moveToThread` вызывается из потока-владельца) и отложенным вызовом отдаёт его в
   `QWebSocketServer::handleConnection`. Handshake выполняет `QWebSocketServer`, дальше подключение
   обрабатывается обычным путём (`HandleNewConnections` → `CWebSocketThread` → сервлеты).

Сокет передаётся только после того, как заголовок получен полностью: пока он читается, действуют таймауты
HTTP-сервера, а `QWebSocketServer` получает уже готовый handshake.

Для HTTPS/WSS используется TLS-соединение HTTP-сервера: сокет уже зашифрован (`QSslSocket`),
`QWebSocketServer` читает из него расшифрованные данные. При `moveToThread` переносятся сокет и его дочерние
объекты, но объект TLS-бэкенда Qt (`QTlsPrivate::TlsCryptograph`, QObject без родителя) остаётся в старом потоке.
Свой поток бэкенд использует только во время TLS-рукопожатия (OpenSSL на Windows ставит в очередь загрузку корневых
сертификатов), поэтому `HandleWebSocketUpgrade` принимает `QSslSocket` только после рукопожатия (`isEncrypted()`).

## Как включить в .acc

В приложении, где есть элементы `HttpServerFramework` и `WebSocketServerFramework`
(пакет `ImtHttpServerVoce`):

| Где | Атрибут | Значение |
| --- | --- | --- |
| `HttpServerFramework` | `WebSocketUpgradeHandler` (Reference) | элемент `WebSocketServerFramework` |
| `WebSocketServerFramework` | `ListenWebSocketPort` (Boolean) | `true` (по умолчанию) — слушать и отдельный порт WS; `false` — только порт HTTP |
| параметры соединения (`ServerConnectionInterfaceParam`) | `DefaultWebSocketPort` | не задавать — у сервера нет своего порта WS |

Если используется `ImtCoreServerBase` (`ImtServerVoce`) или `AuthorizableServerFramework`, атрибуты
`WebSocketUpgradeHandler` и `ListenWebSocketPort` экспортированы из них; ссылку задайте на
`<элемент>/WebSocketServerFramework`.

Приложение без своего порта WS (`DefaultWebSocketPort` не задан):

- `IServerConnectionInterface::GetUrl(PT_WEBSOCKET)` возвращает адрес с портом HTTP;
- порт WS, сохранённый ранее в файле настроек, при загрузке отбрасывается;
- `GetApplicationInfo` не отдаёт `webSocketUrl`.

Пример:

```xml
<AttributeInfo Id="WebSocketUpgradeHandler" Type="Reference" ExportId="">
    <Data IsEnabled="true" Value="WebSocketServerFramework"/>
</AttributeInfo>
```

Если отдельный порт WS оставлен включённым, он должен отличаться от порта HTTP.

## Что меняется для клиентов

- **QML** (`ApplicationMain.qml`, `getWebSocketUrl`): если в ответе `GetApplicationInfo` нет `webSocketUrl`,
  клиент подключается к тому же хосту и порту, что и сервер: `ws(s)://host:<порт HTTP>/<appId>/wssub`.
  Так работают и web (адрес страницы), и desktop (адрес сервера из настроек клиента). Если `webSocketUrl`
  есть, используется его порт, как раньше. Поле «Web Socket Port» в настройках соединения скрывается,
  когда порта нет.
- **C++-клиенты** (`imtclientgql`) берут адрес через `GetUrl(PT_WEBSOCKET)`: без заданного порта WS это порт HTTP.
- Путь запроса на порту HTTP не проверяется: любой WebSocket upgrade передаётся WebSocket-серверу.

## Тесты

Цель `imtrestTest`:

- `CSinglePortServerTest`: HTTP на порту в режиме одного порта, подписка по WS на порту HTTP, обрыв соединения
  без закрывающего кадра, `ListenWebSocketPort=false`, WSS и HTTPS на одном порту (`SecureWebSocketOnSinglePortTest`,
  сертификат `SinglePortTestCertificate.h`), режим двух портов (`TwoPortModeTest`, в нём `CWebSocketServerComp`
  действительно разрушается, см. [#1120](https://github.com/ImagingTools/ImtCore/issues/1120)).
- `CHttpRequestTest::WebSocketUpgradeRequestedTest`: распознавание upgrade-запроса.
