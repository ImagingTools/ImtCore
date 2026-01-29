# PAT Editor Refactoring - Removing GraphQL Dependency

## Problem

The PAT (Personal Access Token) editor in QML (`PatTokenParamEditor.qml`) had direct dependencies on GraphQL through `GqlSdlRequestSender` components. This tight coupling meant that:

1. **Protocol lock-in**: The editor could only work with GraphQL
2. **Hard to maintain**: Protocol changes required editing the UI component
3. **Not reusable**: Other components couldn't easily reuse the PAT operations
4. **Hard to test**: Cannot mock or test the UI without a GraphQL server

## Solution: Controller Pattern

We introduced a **Controller Pattern** following the established architecture in the codebase (e.g., `CommandsController`/`GqlBasedCommandsController`, `CollectionDataController`/`GqlBasedCollectionDataController`).

### Architecture

```
┌─────────────────────────────────────┐
│   PatTokenParamEditor.qml           │
│   (UI Component - Protocol agnostic)│
│   - Uses signals and functions      │
└───────────────┬─────────────────────┘
                │
                │ Uses controller
                ▼
┌─────────────────────────────────────┐
│   GqlBasedPatTokenController.qml    │
│   (in imtguigql module)             │
│   - GraphQL implementation          │
└───────────────┬─────────────────────┘
                │
                │ Inherits from
                ▼
┌─────────────────────────────────────┐
│   PatTokenController.qml            │
│   (in imtauthgui module)            │
│   - Protocol-agnostic interface     │
│   - Defines signals & functions     │
└─────────────────────────────────────┘
```

## Implementation Details

### PatTokenController.qml (Base)

The base controller defines a protocol-agnostic interface:

**Signals** (for results):
- `tokenListReceived(var tokenList)` - Token list loaded
- `tokenCreated(bool success, string message, string token)` - Token created
- `tokenDeleted(bool success, string message)` - Token deleted
- `tokenRevoked(bool success, string message)` - Token revoked

**Functions** (for operations):
- `getTokenList(userId)` - Fetch user's tokens
- `createToken(userId, name, description, scopes, expiresAt)` - Create new token
- `deleteToken(tokenId)` - Delete token permanently
- `revokeToken(tokenId)` - Revoke token (keep for audit)

Default implementations emit warnings and call signals with default values.

### GqlBasedPatTokenController.qml (Implementation)

The GraphQL-based controller:
1. **Inherits** from `PatTokenController`
2. **Overrides** all functions with GraphQL implementations
3. **Uses** `GqlSdlRequestSender` for GraphQL communication
4. **Located** in `Qml/imtguigql/` module (where all GQL implementations live)

### PatTokenParamEditor.qml (Consumer)

The editor now:
1. Creates a `GqlBasedPatTokenController` instance
2. Connects to controller signals for results
3. Calls controller functions for operations
4. Has **zero knowledge** of GraphQL or any protocol

**Before refactoring:**
```qml
GqlSdlRequestSender {
    id: createTokenRequest
    gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_createToken
    requestType: 1
    sdlObjectComp: Component {
        CreateTokenPayload {
            onFinished: {
                if (m_success) {
                    // handle success
                }
            }
        }
    }
}
```

**After refactoring:**
```qml
property GqlBasedPatTokenController patTokenController: GqlBasedPatTokenController {
    onTokenCreated: function(success, message, token) {
        if (success) {
            // handle success
        } else {
            ModalDialogManager.showErrorDialog(message)
        }
    }
}

function addNewToken(name, description, scopes, expiresAt) {
    patTokenController.createToken(
        AuthorizationController.getUserId(),
        name,
        description,
        scopes,
        expiresAt
    )
}
```

## Benefits

1. **Protocol independence**: Can switch from GraphQL to REST/gRPC by creating new controller implementation
2. **Separation of concerns**: UI logic separate from protocol logic
3. **Testability**: Can create mock controllers for testing
4. **Reusability**: Other components can use the same controller
5. **Maintainability**: Protocol changes don't affect UI code
6. **Pattern consistency**: Follows established architecture in codebase

## Switching to Another Protocol

To use a different protocol (e.g., REST):

1. Create `RestBasedPatTokenController.qml` that inherits from `PatTokenController`
2. Implement functions using REST API calls instead of GraphQL
3. Replace `GqlBasedPatTokenController` with `RestBasedPatTokenController` in the editor

**No changes needed to PatTokenParamEditor.qml or PatTokenController.qml!**

## Files Modified

- **Qml/imtauthgui/PatTokenController.qml** (new) - Base controller interface
- **Qml/imtguigql/GqlBasedPatTokenController.qml** (new) - GraphQL implementation
- **Qml/imtgui/Params/PatTokenParamEditor.qml** - Refactored to use controller
- **Qml/imtauthgui/qmldir** - Registered base controller
- **Qml/imtauthgui/imtauthguiqml.qrc** - Added controller to resources
- **Qml/imtguigql/qmldir** - Registered GQL controller
- **Qml/imtguigql/imtguigqlqml.qrc** - Added GQL controller to resources

## Testing

The refactoring maintains the same functionality:
1. Loading token list on component completion
2. Creating new tokens with success/error handling
3. Deleting tokens with confirmation
4. Revoking tokens
5. Displaying token details in the UI

All operations work identically, but now through the controller abstraction layer.

## Pattern Consistency

This refactoring follows the **Controller Pattern** already established in the codebase:

| Base Controller | GQL Implementation | Location |
|-----------------|-------------------|----------|
| CommandsController | GqlBasedCommandsController | Qml/imtguigql/ |
| CollectionDataController | GqlBasedCollectionDataController | Qml/imtguigql/ |
| DataModelController | GqlBasedDataModelController | Qml/imtguigql/ |
| **PatTokenController** | **GqlBasedPatTokenController** | **Qml/imtguigql/** |
