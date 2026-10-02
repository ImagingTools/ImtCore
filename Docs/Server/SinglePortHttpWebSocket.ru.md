# HTTP и WebSocket на одном порту

По умолчанию сервер ImtCore слушает два порта: HTTP (`CTcpServerComp`) и WebSocket (`CWebSocketServerComp`).
Режим одного порта позволяет принимать WebSocket-подключения (в том числе WSS) на порту HTTP(S).
Режим выключен по умолчанию и включается настройкой в .acc.

## Как устроено

1. Соединение принимает HTTP-сервер (`CMultiThreadServer` → `CSocketThread` → `CSocket`), как и раньше.
2. Если у `TcpServer` задана ссылка `WebSocketUpgradeHandler` и `CSocket` получил полный запрос
   `GET` с заголовком `Upgrade: websocket`, запрос не передаётся сервлетам:
   - транзакция чтения откатывается (`QIODevice::rollbackTransaction`), байты handshake снова лежат в буфере сокета;
   - `CSocket` отключается от сигналов сокета и отложенным вызовом (в своём потоке) передаёт сокет
     в `imtrest::IWebSocketUpgradeHandler::HandleWebSocketUpgrade`;
   - поток `CSocketThread` освобождается (соединение больше не учитывается в `ThreadsLimit`).
3. `CWebSocketServerComp` (реализует `IWebSocketUpgradeHandler`) переносит сокет в свой поток
   (`moveToThread` вызывается из потока-владельца) и отложенным вызовом отдаёт его в
   `QWebSocketServer::handleConnection`. Handshake выполняет `QWebSocketServer`, дальше подключение
   обрабатывается обычным путём (`HandleNewConnections` → `CWebSocketThread` → сервлеты).

Для HTTPS/WSS используется TLS-соединение HTTP-сервера: сокет уже зашифрован (`QSslSocket`),
`QWebSocketServer` читает из него расшифрованные данные.

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
- Путь запроса на порту HTTP не проверяется: любой `GET` с `Upgrade: websocket` передаётся WebSocket-серверу.

## Тесты

`Include/imtrest/Test/CSinglePortServerTest` (цель `imtrestTest`).
