# Code Comparison: Before and After Refactoring

## Example 1: Connection State Management

### Before (Old Code - Mixed Responsibilities)
```qml
// In ApplicationMain.qml - everything mixed together
QtObject {
    id: connectionState
    property int totalStatus: -1
    property var status : ({
        TRY_CONNECTING_TO_AUTH_SERVER: 0,
        AUTH_SERVER_CONNECTION_ERROR: 1,
        TRY_CONNECTING_TO_APP_SERVER: 2,
        APP_SERVER_CONNECTION_ERROR: 3,
        CONNECTION_SUCCESFUL: 4
    })
    
    property int appConnectionState: application.useWebSocketSubscription ? subscriptionManager_.status : 1
    property int authConnectionState: 1
    
    onAppConnectionStateChanged: { checkState() }
    onAuthConnectionStateChanged: { checkState() }
    
    function checkState(){
        if (authConnectionState == 1 && appConnectionState == 1){
            totalStatus = status.CONNECTION_SUCCESFUL
        }
        else if (appConnectionState == 0){
            totalStatus = status.TRY_CONNECTING_TO_APP_SERVER
        }
        // ... more logic mixed with UI updates
    }
    
    onTotalStatusChanged: {
        if (totalStatus == status.TRY_CONNECTING_TO_AUTH_SERVER){
            internal.errorMessage = qsTr("Try connecting to authorization server ...")
            internal.loadingVisible = true
        }
        // ... UI updates mixed with state logic
    }
}

Timer {
    id: statusTimer
    interval: 1000
    repeat: false
    onTriggered: {
        thumbnailDecorator.stackView.clear();
        Events.sendEvent("SearchVisible", false);
        Events.sendEvent("SetUserPanelEnabled", false);
        application.showMessagePage(internal.errorMessage, internal.loadingVisible)
    }
}
```

### After (New Code - Separated Responsibilities)

**ConnectionStateManager.qml** - Only connection state logic
```qml
QtObject {
    id: root
    
    property bool useWebSocketSubscription: false
    property int webSocketStatus: 1
    property int authConnectionState: 1
    
    readonly property int totalStatus: internal.totalStatus
    readonly property bool isConnected: totalStatus === ConnectionStatus.CONNECTION_SUCCESFUL
    
    readonly property var ConnectionStatus: ({
        TRY_CONNECTING_TO_AUTH_SERVER: 0,
        AUTH_SERVER_CONNECTION_ERROR: 1,
        TRY_CONNECTING_TO_APP_SERVER: 2,
        APP_SERVER_CONNECTION_ERROR: 3,
        CONNECTION_SUCCESFUL: 4
    })
    
    signal connectionStateChanged(int status)
    
    function checkState() {
        // Pure logic - no UI updates
        let newStatus = calculateStatus()
        if (internal.totalStatus !== newStatus) {
            internal.totalStatus = newStatus
            connectionStateChanged(newStatus)
        }
    }
}
```

**ApplicationStateManager.qml** - Only UI state management
```qml
QtObject {
    id: root
    
    function handleConnectionStateChange(connectionStatus) {
        if (connectionStatus === ConnectionStatus.CONNECTION_SUCCESFUL) {
            handleSuccessfulConnection()
        } else {
            handleConnectionError(connectionStatus)
        }
    }
    
    function handleSuccessfulConnection() {
        thumbnailDecorator.stackView.clear()
        Events.sendEvent("SearchVisible", true)
        Events.sendEvent("SetUserPanelEnabled", true)
    }
}
```

## Example 2: Authorization Logic

### Before (Old Code - Mixed with UI)
```qml
// In ApplicationMain.qml - 50+ lines mixed together
Connections {
    target: AuthorizationController;
    
    function onUserModeChanged(userMode){
        if (AuthorizationController.isStrongUserManagement()){
            application.onStrongUserManagement();
        }
        else if (AuthorizationController.isSimpleUserManagement()){
            application.onSimpleUserManagement();
        }
    }
    
    function onSuperuserExistResult(status, message){
        if (status === "EXISTS"){
            thumbnailDecorator.showPage(thumbnailDecorator.authorizationPageComp)
        }
        else if (status === "NOT_EXISTS"){
            thumbnailDecorator.showPage(thumbnailDecorator.superuserPasswordPageComp)
        }
        else{
            application.showMessagePage(message);
        }
    }
    
    function onLoggedIn(){
        thumbnailDecorator.drawingContainer.content = Style.drawingContainerDecorator;
        thumbnailDecorator.showPage(undefined)
        application.updateAllModels();
    }
    
    function onLoggedOut(){
        thumbnailDecorator.stopLoading();
        application.firstModelsInit(true);
        NavigationController.clear();
        subscriptionManager_.clear();
    }
}

function onStrongUserManagement(){
    let loggedUserId = AuthorizationController.getLoggedUserId();
    if (loggedUserId === ""){
        AuthorizationController.updateSuperuserModel();
    }
}

function firstModelsInit(force){
    if (!force && firstModelsIsInit){
        return
    }
    
    let loggedUserId = AuthorizationController.getLoggedUserId();
    if (loggedUserId === ""){
        AuthorizationController.updateUserManagementModel();
    }
    
    firstModelsIsInit = true;
}
```

### After (New Code - Clean Separation)

**AuthorizationManager.qml** - Only authorization logic
```qml
QtObject {
    id: root
    
    property var authorizationController: null
    
    signal authorizationRequired()
    signal superuserCreationRequired()
    signal authorizationSuccessful()
    signal authorizationFailed()
    signal loggedOut()
    
    function initialize() {
        if (!authorizationController) {
            return
        }
        
        let loggedUserId = authorizationController.getLoggedUserId()
        if (loggedUserId === "") {
            authorizationController.updateUserManagementModel()
        }
    }
    
    function handleUserModeChange(userMode) {
        if (authorizationController.isStrongUserManagement()) {
            onStrongUserManagement()
        }
        else if (authorizationController.isSimpleUserManagement()) {
            onSimpleUserManagement()
        }
    }
    
    function handleSuperuserExistResult(status, message) {
        if (status === "EXISTS") {
            authorizationRequired()
        }
        else if (status === "NOT_EXISTS") {
            superuserCreationRequired()
        }
        else {
            authorizationFailed()
        }
    }
}
```

**ApplicationMain.qml** - Only composition
```qml
AuthorizationManager {
    id: authMgr
    
    authorizationController: configurator.hasAuthorization ? AuthorizationController : null
    
    onAuthorizationRequired: {
        thumbnailDecorator.showPage(thumbnailDecorator.authorizationPageComp)
    }
    
    onSuperuserCreationRequired: {
        thumbnailDecorator.showPage(thumbnailDecorator.superuserPasswordPageComp)
    }
    
    onAuthorizationSuccessful: {
        thumbnailDecorator.drawingContainer.content = Style.drawingContainerDecorator
        thumbnailDecorator.showPage(undefined)
        stateManager.updateAllModels()
    }
}

Connections {
    target: configurator.hasAuthorization ? AuthorizationController : null
    enabled: configurator.hasAuthorization
    
    function onUserModeChanged(userMode){
        authMgr.handleUserModeChange(userMode)
    }
    
    function onSuperuserExistResult(status, message){
        authMgr.handleSuperuserExistResult(status, message)
    }
}
```

## Example 3: Server Connection

### Before (Old Code - WebSocket logic scattered)
```qml
// In ApplicationMain.qml - scattered across 100+ lines

property bool useWebSocketSubscription: false;
property bool serverConnected: subscriptionManager_.status == 1;

WebSocketPortProvider {
    id: webSocketPortProvider;
    onPortChanged: {
        application.connectToWebSocketServer();
    }
}

function getWebSocketUrl(serverUrl){
    try {
        let url = new URL(serverUrl);
        let protocol = "ws";
        if (url.protocol === "https:"){
            protocol = "wss";
        }
        url.protocol = protocol
        if (webSocketPortProvider.port >= 0){
            url.port = webSocketPortProvider.port;
        }
        // ... 20 more lines
    }
    catch(error){
        console.error(error);
        return "";
    }
}

function connectToWebSocketServer(){
    if (!application.useWebSocketSubscription){
        return;
    }
    subscriptionManager_.active = false;
    let serverUrl = getServerUrl();
    let webSocketServerUrl = getWebSocketUrl(serverUrl);
    subscriptionManager_.url = webSocketServerUrl;
    subscriptionManager_.active = true;
}

function reconnect(){
    webSocketPortProvider.port = -1;
}
```

### After (New Code - Encapsulated in Manager)

**ServerConnectionManager.qml**
```qml
QtObject {
    id: root
    
    property var subscriptionManager: null
    property var webSocketPortProvider: null
    property bool useWebSocketSubscription: false
    property string appId: ""
    
    signal serverConnected()
    signal serverDisconnected()
    signal serverUrlChanged(string url)
    
    function getServerUrl(): string
    function setServerUrl(url)
    function reconnect()
    function connectToWebSocket()
    function isConnected(): bool
    
    // Private
    function getWebSocketUrl(serverUrl) {
        // All WebSocket URL logic encapsulated here
    }
}
```

**ApplicationMain.qml** - Simple usage
```qml
ServerConnectionManager {
    id: serverConnectionMgr
    
    subscriptionManager: subscriptionManager_
    webSocketPortProvider: webSocketPortProvider
    useWebSocketSubscription: configurator.useWebSocketSubscription
    appId: context.appId
    
    onServerUrlChanged: {
        connectToWebSocket()
    }
}

// Property delegation
property bool serverConnected: serverConnectionMgr.isConnected()

// Simple reconnect
function reconnect(){
    serverConnectionMgr.reconnect()
}
```

## Summary of Benefits

### Readability
- **Before:** 442 lines of mixed logic, hard to understand
- **After:** ~100 lines per component, clear purpose

### Maintainability
- **Before:** Change one thing, risk breaking everything
- **After:** Change ConnectionStateManager, only affects connection logic

### Testability
- **Before:** Must set up entire ApplicationMain to test connection logic
- **After:** Test ConnectionStateManager independently

### Extensibility
- **Before:** Adding new mode requires editing main file
- **After:** Add mode to ApplicationConfigurator, configure managers

### Coupling
- **Before:** High coupling between all concerns
- **After:** Low coupling, managers are independent
