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
| `ApplicationInfoController` / `RootDataController` | `UseHttpPortForWebSocket` (Boolean) | `true` — в `webSocketUrl` сообщается порт HTTP |
| `WebSocketServerFramework` | `ListenWebSocketPort` (Boolean) | `true` (по умолчанию) — слушать и отдельный порт WS; `false` — только порт HTTP |

Если используется `ImtCoreServerBase` (`ImtServerVoce`), атрибуты `WebSocketUpgradeHandler` и
`ListenWebSocketPort` экспортированы из него; ссылку задайте на `<элемент>/WebSocketServerFramework`.

Пример:

```xml
<AttributeInfo Id="WebSocketUpgradeHandler" Type="Reference" ExportId="">
    <Data IsEnabled="true" Value="WebSocketServerFramework"/>
</AttributeInfo>
```

Если отдельный порт WS оставлен включённым, он должен отличаться от порта HTTP.

## Что меняется для клиентов

- **QML** (`ApplicationMain.qml`, `getWebSocketUrl`) берёт порт из `webSocketUrl` ответа `GetApplicationInfo`.
  При `UseHttpPortForWebSocket=true` это порт HTTP, и клиент подключается к `ws(s)://host:<порт HTTP>/<appId>/wssub`
  без изменений в QML.
- **C++-клиенты** (`imtclientgql`) продолжают работать через отдельный порт WS, пока `ListenWebSocketPort=true`.
  Чтобы перевести их на один порт, укажите в их конфигурации порт HTTP в качестве порта WebSocket.
- Путь запроса на порту HTTP не проверяется: любой `GET` с `Upgrade: websocket` передаётся WebSocket-серверу.

## Тесты

`Include/imtrest/Test/CSinglePortServerTest` (цель `imtrestTest`).
