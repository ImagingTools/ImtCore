# Architecture Overview

## Before Refactoring

```
┌─────────────────────────────────────────────────────────┐
│                   ApplicationMain.qml                    │
│  (442 lines - all logic mixed together)                 │
│                                                          │
│  • Connection state logic                               │
│  • Authorization logic                                   │
│  • Server connection logic                               │
│  • UI state management                                   │
│  • WebSocket URL generation                              │
│  • Error handling                                        │
│  • Model initialization                                  │
│  • Settings management                                   │
│  • ... and more ...                                      │
└─────────────────────────────────────────────────────────┘
```

## After Refactoring

```
┌──────────────────────────────────────────────────────────────────┐
│                      ApplicationMain.qml                          │
│                    (Composition Root - ~350 lines)                │
│                                                                   │
│  • Composes managers                                              │
│  • Maintains backward compatibility                               │
│  • Delegates to specialized managers                              │
└───────────────────────────────┬──────────────────────────────────┘
                                │
                ┌───────────────┼───────────────┐
                │               │               │
                ▼               ▼               ▼
┌───────────────────────┐ ┌──────────────┐ ┌────────────────────────┐
│ApplicationConfigurator│ │Providers &   │ │ThumbnailDecorator &    │
│                       │ │Controllers   │ │UI Components           │
│• Mode detection       │ │              │ │                        │
│• Feature flags        │ │• Settings    │ │• Visual components     │
│• Manager config       │ │• Language    │ │• Pages                 │
└───────┬───────────────┘ │• Design      │ └────────────────────────┘
        │                 └──────────────┘
        │ configures
        ▼
┌───────────────────────────────────────────────────────────┐
│                       Managers                             │
├───────────────────────────────────────────────────────────┤
│                                                            │
│  ┌─────────────────────────────────────────┐             │
│  │   ConnectionStateManager (103 lines)    │             │
│  │   • WebSocket state                     │             │
│  │   • Auth server state                   │             │
│  │   • Overall status calculation          │             │
│  └─────────────────────────────────────────┘             │
│                                                            │
│  ┌─────────────────────────────────────────┐             │
│  │   AuthorizationManager (105 lines)      │             │
│  │   • User authentication                 │             │
│  │   • Login/Logout handling               │             │
│  │   • User mode changes                   │             │
│  └─────────────────────────────────────────┘             │
│                                                            │
│  ┌─────────────────────────────────────────┐             │
│  │   ServerConnectionManager (144 lines)   │             │
│  │   • WebSocket connection                │             │
│  │   • Server URL management               │             │
│  │   • Reconnection logic                  │             │
│  └─────────────────────────────────────────┘             │
│                                                            │
│  ┌─────────────────────────────────────────┐             │
│  │   ApplicationStateManager (129 lines)   │             │
│  │   • UI state management                 │             │
│  │   • Error page display                  │             │
│  │   • Initialization tracking             │             │
│  └─────────────────────────────────────────┘             │
│                                                            │
└───────────────────────────────────────────────────────────┘
```

## Application Modes

```
┌──────────────────────────────────────────────────────────────┐
│                   ApplicationConfigurator                     │
└───────────────────────────────┬──────────────────────────────┘
                                │
                ┌───────────────┼───────────────┐
                │               │               │
                ▼               ▼               ▼
        ┌──────────────┐ ┌─────────────┐ ┌────────────┐
        │ STANDALONE   │ │ SERVER_ONLY │ │    FULL    │
        ├──────────────┤ ├─────────────┤ ├────────────┤
        │ • No server  │ │ • Server    │ │ • Server   │
        │ • No auth    │ │ • No auth   │ │ • Auth     │
        │              │ │ • WebSocket │ │ • WebSocket│
        │ Local only   │ │             │ │            │
        └──────────────┘ └─────────────┘ └────────────┘
```

## Communication Flow

```
User Action
    │
    ▼
ApplicationMain
    │
    ├──► ApplicationConfigurator ──► Determines mode and features
    │
    ├──► ConnectionStateManager ──► Monitors connection status
    │        │
    │        └──► ApplicationStateManager ──► Updates UI
    │
    ├──► AuthorizationManager ──► Handles auth (if enabled)
    │        │
    │        └──► Shows login/signup pages
    │
    └──► ServerConnectionManager ──► Manages server connection
             │
             └──► WebSocket subscription
```

## Benefits

### Single Responsibility ✅
Each manager has ONE job:
- ConnectionStateManager → Monitor connections
- AuthorizationManager → Handle auth
- ServerConnectionManager → Manage server
- ApplicationStateManager → Manage UI state
- ApplicationConfigurator → Configure mode

### Open/Closed ✅
Easy to add new modes without changing existing code:
```qml
// Add new mode in ApplicationConfigurator
readonly property var ApplicationMode: ({
    STANDALONE: "standalone",
    SERVER_ONLY: "server_only",
    FULL: "full",
    NEW_MODE: "new_mode"  // ← Just add new mode
})
```

### Testability ✅
Each manager can be tested independently:
```qml
// Test ConnectionStateManager alone
ConnectionStateManager {
    webSocketStatus: 4  // Error
    // Assert: should emit connection error
}
```

### Maintainability ✅
Changes are localized:
- Connection logic changes? → Edit ConnectionStateManager
- Auth flow changes? → Edit AuthorizationManager
- New server feature? → Edit ServerConnectionManager
- UI state changes? → Edit ApplicationStateManager

### Extensibility ✅
Easy to extend without breaking existing code:
- Add new connection types
- Support multiple auth providers
- Add offline mode
- Support clustering
