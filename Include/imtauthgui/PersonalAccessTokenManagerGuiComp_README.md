# Personal Access Token Manager GUI Component

## Overview

`CPersonalAccessTokenManagerGuiComp` is a widget-based UI observer component for editing and viewing personal access tokens managed by the `IPersonalAccessTokenManager` interface.

## Features

- **Token List View**: Display all tokens for a specific user
- **Token Details**: Show comprehensive token information including:
  - Token ID, name, and description
  - Permission scopes
  - Creation, expiration, and last used timestamps
  - Revocation status
- **Token Management Operations**:
  - Create new tokens
  - Revoke active tokens
  - Delete tokens permanently
  - Refresh token list

## Architecture

The component follows the standard ImtCore GUI observer pattern:

- **Base Class**: `TDesignerGuiObserverCompBase<Ui::CPersonalAccessTokenManagerGuiComp, IPersonalAccessTokenManager>`
- **Observer Pattern**: Observes `IPersonalAccessTokenManager` directly through the base class
- **UI Designer**: Qt Designer file (`.ui`) for visual layout
- **Component Registration**: Registered in `ImtAuthGuiPck` package

## Usage

### Configuration Example

```xml
<Component Type="PersonalAccessTokenManagerGui" Name="TokenManagerGui">
  <Attr Name="UserId" Value="user123"/>
  <Ref Name="ObservedModel" Source="PersonalAccessTokenManager"/>
</Component>
```

### Parameters

- **ObservedModel** (required): Reference to `IPersonalAccessTokenManager` instance
- **UserId** (attribute): User ID for filtering tokens

## Implementation Details

### Key Methods

- `UpdateGui()`: Refreshes the UI from the model state (called automatically if model implements IChangeable)
- `OnGuiModelAttached()`: Initializes UI when model is attached
- `OnGuiModelDetached()`: Clears UI when model is detached
- `RefreshTokenList()`: Updates the token list widget
- `UpdateTokenDetails()`: Displays selected token information

### UI Widgets

- **TokenListWidget**: QListWidget showing token names
- **TokenDetailsText**: QTextBrowser displaying detailed token information
- **CreateTokenButton**: Creates new tokens
- **RevokeTokenButton**: Revokes selected token
- **DeleteTokenButton**: Permanently deletes selected token
- **RefreshButton**: Manually refreshes the token list

## Security Considerations

- Raw token values are only shown once during creation
- Token hashes are never displayed in the UI
- Deletion and revocation require user confirmation
- Read-only display for sensitive fields

## Future Enhancements

Potential improvements for future versions:

1. **Advanced Token Creation Dialog**:
   - Scope selection UI
   - Expiration date picker
   - Custom token properties

2. **Token Search and Filter**:
   - Search by name or description
   - Filter by status (active/revoked)
   - Sort by creation date or last used

3. **Bulk Operations**:
   - Select multiple tokens
   - Batch revocation or deletion

4. **Token Usage Statistics**:
   - Usage frequency charts
   - Last access information
   - Security alerts for unused tokens

## Related Components

- **IPersonalAccessTokenManager**: Core interface for token management
- **IPersonalAccessToken**: Token data model interface
- **CPersonalAccessTokenManagerComp**: Default implementation
- **ImtAuthGuiPck**: Package containing this component

## Example Screenshot

(Component UI displays a split view with token list on the left and details on the right)

```
+----------------------------------+------------------------------------+
| Personal Access Tokens           | Token Details                      |
|                                  |                                    |
| - API Token 1                    | Token ID: abc123                   |
| - CI/CD Token [REVOKED]          | Name: API Token 1                  |
| - Development Token              | Description: For API access        |
|                                  | Scopes: read, write                |
|                                  | Created At: 2024-01-15T10:30:00    |
| [Create] [Revoke] [Delete] [Refresh] | Expires At: 2025-01-15T10:30:00  |
+----------------------------------+------------------------------------+
```
