# ApplicationMain.qml Refactoring Documentation

## Обзор (Overview)

Файл `ApplicationMain.qml` был рефакторен с использованием SOLID принципов для улучшения поддерживаемости, тестируемости и расширяемости кода.

The `ApplicationMain.qml` file has been refactored following SOLID principles to improve maintainability, testability, and extensibility.

## Проблема (Problem)

Исходный файл `ApplicationMain.qml` содержал смешанную логику:
- Управление состоянием подключения (WebSocket, Auth server)
- Логика авторизации
- Управление состоянием UI
- Конфигурация приложения
- Логика сервера

The original `ApplicationMain.qml` file contained mixed logic for connection state management, authorization, UI state, configuration, and server communication.

## Решение (Solution)

Код разделён на 5 независимых менеджеров, каждый с единственной ответственностью:

The code is separated into 5 independent managers, each with a single responsibility:

### 1. ConnectionStateManager.qml

**Ответственность (Responsibility):** Управление состояниями подключения

**Функции:**
- Отслеживание состояния WebSocket
- Отслеживание состояния Auth сервера
- Расчёт общего статуса подключения
- Предоставление сообщений о статусе

**API:**
```qml
ConnectionStateManager {
    useWebSocketSubscription: bool
    webSocketStatus: int
    authConnectionState: int
    
    readonly property int totalStatus
    readonly property bool isConnected
    
    signal connectionStateChanged(int status)
    
    function getStatusMessage(): string
    function isLoadingState(): bool
}
```

### 2. AuthorizationManager.qml

**Ответственность (Responsibility):** Управление логикой авторизации

**Функции:**
- Инициализация авторизации
- Обработка изменений режима пользователя
- Управление логином/логаутом
- Проверка существования суперпользователя

**API:**
```qml
AuthorizationManager {
    property var authorizationController
    
    signal authorizationRequired()
    signal superuserCreationRequired()
    signal authorizationSuccessful()
    signal authorizationFailed()
    signal loggedOut()
    
    function initialize()
    function handleUserModeChange(userMode)
    function isUserLoggedIn(): bool
}
```

### 3. ServerConnectionManager.qml

**Ответственность (Responsibility):** Управление подключениями к серверу

**Функции:**
- Управление WebSocket подключением
- Конфигурация URL сервера
- Реконнект логика
- Генерация WebSocket URL

**API:**
```qml
ServerConnectionManager {
    property var subscriptionManager
    property var webSocketPortProvider
    property bool useWebSocketSubscription
    property string appId
    
    signal serverConnected()
    signal serverDisconnected()
    signal serverUrlChanged(string url)
    
    function getServerUrl(): string
    function setServerUrl(url)
    function reconnect()
    function connectToWebSocket()
    function isConnected(): bool
}
```

### 4. ApplicationStateManager.qml

**Ответственность (Responsibility):** Управление состоянием UI приложения

**Функции:**
- Отслеживание инициализации
- Управление сообщениями об ошибках
- Контроль видимости UI элементов
- Обработка состояний подключения

**API:**
```qml
ApplicationStateManager {
    property bool firstModelsInitialized
    property string currentErrorMessage
    property bool loadingVisible
    
    signal showErrorPage(string message, bool loading)
    signal clearErrorPage()
    signal modelsInitialized()
    
    function showMessagePage(message, loading)
    function initializeModels(force)
    function handleConnectionStateChange(status)
}
```

### 5. ApplicationConfigurator.qml

**Ответственность (Responsibility):** Конфигурация режима приложения

**Функции:**
- Определение режима приложения
- Предоставление флагов функций
- Конфигурация менеджеров

**Режимы приложения:**
- `STANDALONE` - без сервера, без авторизации
- `SERVER_ONLY` - с сервером, без авторизации
- `FULL` - с сервером и авторизацией

**API:**
```qml
ApplicationConfigurator {
    readonly property var ApplicationMode
    property string mode
    
    readonly property bool hasServer
    readonly property bool hasAuthorization
    readonly property bool requiresConnection
    
    signal modeChanged(string newMode)
    
    function setMode(newMode)
    function autoDetectMode(hasServerConnection, hasAuthController)
    function configureManagers(managers)
}
```

## Применение SOLID принципов

### Single Responsibility Principle (SRP)
Каждый менеджер имеет одну чёткую ответственность и одну причину для изменения.

Each manager has one clear responsibility and one reason to change.

### Open/Closed Principle (OCP)
Новые режимы приложения могут быть добавлены через ApplicationConfigurator без изменения существующего кода.

New application modes can be added through ApplicationConfigurator without modifying existing code.

### Liskov Substitution Principle (LSP)
Менеджеры могут быть заменены альтернативными реализациями с тем же интерфейсом.

Managers can be replaced with alternative implementations with the same interface.

### Interface Segregation Principle (ISP)
Каждый менеджер предоставляет минимальный необходимый интерфейс для своей функциональности.

Each manager provides a minimal interface necessary for its functionality.

### Dependency Inversion Principle (DIP)
ApplicationMain зависит от абстракций (менеджеров), а не от конкретных реализаций.

ApplicationMain depends on abstractions (managers), not concrete implementations.

## Обратная совместимость

Все существующие публичные свойства и методы ApplicationMain.qml сохранены для обратной совместимости.

All existing public properties and methods of ApplicationMain.qml are preserved for backward compatibility.

## Использование (Usage)

### Пример 1: Standalone приложение
```qml
ApplicationMain {
    // Режим автоматически определяется как STANDALONE
    // если нет subscriptionManager и AuthorizationController
}
```

### Пример 2: Приложение с сервером
```qml
ApplicationMain {
    useWebSocketSubscription: true
    // Режим автоматически определяется как SERVER_ONLY
}
```

### Пример 3: Полное приложение с авторизацией
```qml
ApplicationMain {
    useWebSocketSubscription: true
    // Режим автоматически определяется как FULL
    // если доступен AuthorizationController
}
```

## Тестирование (Testing)

Каждый менеджер может быть протестирован независимо:

```qml
// Test ConnectionStateManager
ConnectionStateManager {
    id: testManager
    useWebSocketSubscription: true
    webSocketStatus: 0 // Connecting
    
    // Assert: totalStatus should be TRY_CONNECTING_TO_APP_SERVER
}
```

## Миграция (Migration)

Существующий код продолжит работать без изменений благодаря сохранённой обратной совместимости API.

Existing code will continue to work without changes thanks to preserved API backward compatibility.

## Будущие улучшения (Future Improvements)

1. Удалить deprecated свойства и методы
2. Добавить unit тесты для каждого менеджера
3. Рассмотреть вынос legacy кода в отдельные компоненты
4. Добавить возможность динамической смены режима

1. Remove deprecated properties and methods
2. Add unit tests for each manager
3. Consider extracting legacy code into separate components
4. Add ability to dynamically switch modes
