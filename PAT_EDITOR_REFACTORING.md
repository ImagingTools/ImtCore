# PAT Editor Refactoring - Removing GraphQL Dependency

## Problem

The PAT (Personal Access Token) editor in QML (`PatTokenParamEditor.qml`) had direct dependencies on GraphQL through `GqlSdlRequestSender` components. This tight coupling meant that:

1. **Protocol lock-in**: The editor could only work with GraphQL
2. **Hard to maintain**: Protocol changes required editing the UI component
3. **Not reusable**: Other components couldn't easily reuse the PAT operations
4. **Hard to test**: Cannot mock or test the UI without a GraphQL server

## Solution: Provider Pattern

We introduced a **Provider** abstraction layer following the established pattern in `imtauthgui` (e.g., `UserTokenProvider`, `SuperuserProvider`).

### Architecture

```
┌─────────────────────────────────────┐
│   PatTokenParamEditor.qml           │
│   (UI Component - Protocol agnostic)│
│   - Uses signals and functions      │
└───────────────┬─────────────────────┘
                │
                │ Uses interface
                ▼
┌─────────────────────────────────────┐
│   PatTokenProvider.qml              │
│   (Abstraction Layer)               │
│   - Exposes: signals & functions    │
│   - Hides: protocol implementation  │
└───────────────┬─────────────────────┘
                │
                │ Implements using
                ▼
┌─────────────────────────────────────┐
│   GqlSdlRequestSender               │
│   (GraphQL Implementation)          │
│   - Can be replaced with REST/gRPC  │
└─────────────────────────────────────┘
```

## Implementation Details

### PatTokenProvider.qml

The provider exposes a clean interface:

**Signals** (for results):
- `tokenListReceived(var tokenList)` - Token list loaded
- `tokenCreated(bool success, string message, string token)` - Token created
- `tokenDeleted(bool success, string message)` - Token deleted
- `tokenRevoked(bool success, string message)` - Token revoked
- `errorOccurred(string message)` - Error occurred

**Functions** (for operations):
- `getTokenList(userId)` - Fetch user's tokens
- `createToken(userId, name, description, scopes, expiresAt)` - Create new token
- `deleteToken(tokenId)` - Delete token permanently
- `revokeToken(tokenId)` - Revoke token (keep for audit)

### PatTokenParamEditor.qml

The editor now:
1. Creates a `PatTokenProvider` instance
2. Connects to provider signals for results
3. Calls provider functions for operations
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
property PatTokenProvider patTokenProvider: PatTokenProvider {
    onTokenCreated: function(success, message, token) {
        if (success) {
            // handle success
        } else {
            ModalDialogManager.showErrorDialog(message)
        }
    }
}

function addNewToken(name, description, scopes, expiresAt) {
    patTokenProvider.createToken(
        AuthorizationController.getUserId(),
        name,
        description,
        scopes,
        expiresAt
    )
}
```

## Benefits

1. **Protocol independence**: Can switch from GraphQL to REST/gRPC by only changing the provider
2. **Separation of concerns**: UI logic separate from protocol logic
3. **Testability**: Can create mock providers for testing
4. **Reusability**: Other components can use the same provider
5. **Maintainability**: Protocol changes don't affect UI code

## Switching to Another Protocol

To use a different protocol (e.g., REST):

1. Create `PatTokenRestProvider.qml` with the same interface
2. Implement functions using REST API calls instead of GraphQL
3. Replace `PatTokenProvider` with `PatTokenRestProvider` in the editor

**No changes needed to PatTokenParamEditor.qml!**

## Files Modified

- **Qml/imtauthgui/PatTokenProvider.qml** (new) - Provider abstraction
- **Qml/imtgui/Params/PatTokenParamEditor.qml** - Refactored to use provider
- **Qml/imtauthgui/qmldir** - Registered provider component
- **Qml/imtauthgui/imtauthguiqml.qrc** - Added provider to resources

## Testing

The refactoring maintains the same functionality:
1. Loading token list on component completion
2. Creating new tokens with success/error handling
3. Deleting tokens with confirmation
4. Revoking tokens
5. Displaying token details in the UI

All operations work identically, but now through the provider abstraction layer.
