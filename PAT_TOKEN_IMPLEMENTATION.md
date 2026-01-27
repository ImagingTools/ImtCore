# PAT Token Editor Implementation

## Overview
This implementation adds a Personal Access Token (PAT) editor to the QML Settings page in the ImtCore application. The PAT token editor allows users to manage multiple tokens for API authentication.

## Implementation Details

### Files Added

1. **Qml/imtgui/Params/PatTokenListParam.qml**
   - Model class for storing PAT tokens as a list
   - Extends `Param` base class
   - Provides JSON serialization/deserialization
   - Methods: `addToken()`, `removeToken()`, `updateToken()`, `toJson()`, `createFromJson()`

2. **Qml/imtgui/Params/PatTokenParamEditor.qml**
   - UI editor component for PAT tokens
   - Extends `ParamEditorBase`
   - Features:
     - List view of all tokens
     - Add new token dialog
     - Show/hide token values
     - Delete token functionality
     - Token fields: Name, Value, Description, Created Date

3. **Qml/imtgui/Settings/PatTokenSettingsController.qml**
   - Controller for PAT token settings
   - Extends `ParamsSetController`
   - Initializes the PAT tokens parameter in settings

### Files Modified

1. **Qml/imtgui/Settings/SettingsController.qml**
   - Registered `PatTokenParamEditor` for "PatTokenList" type
   - Added component property `patTokenParamEditorComp`

2. **Qml/imtgui/Application/ApplicationMain.qml**
   - Added `PatTokenSettingsController` instance
   - Registered PAT tokens in `fillPreferenceParamsSet()` function
   - PAT tokens appear in Settings dialog alongside Network, General, and About sections

3. **Qml/imtgui/qmldir**
   - Added exports for new QML components

4. **Qml/imtgui/imtguiqml.qrc**
   - Added new files to Qt resource system

## How to Use

### Accessing PAT Token Settings

1. Open the application
2. Navigate to Settings/Preferences
3. Select "PAT Tokens" from the settings menu
4. Add, edit, or delete tokens as needed

### Adding a New Token

1. Click "Add PAT Token" button
2. Enter token name (e.g., "GitHub API Token")
3. Enter token value (displayed as password)
4. Optionally add description
5. Click "Add" to save

### Managing Tokens

- **View Token**: Click "Show" button next to hidden token
- **Delete Token**: Click "Delete" button for the token
- **Apply Changes**: Click "Apply" in the dialog to save all changes

## Architecture

### Settings System Flow

```
PreferenceDialog.qml
    ↓
SettingsController.qml (registers param editors)
    ↓
PatTokenSettingsController.qml (creates parameters)
    ↓
PatTokenParamEditor.qml (UI)
    ↓
PatTokenListParam.qml (data model)
```

### How Settings Work in ImtCore

The ImtCore settings system uses a three-tier architecture:

1. **Controllers** (`ParamsSetController`):
   - Manage groups of parameters
   - Examples: `ClientSettingsController`, `UserSettingsController`
   - Handle save/load operations

2. **Parameters** (`Param`):
   - Individual setting items with type and data
   - Serialized as JSON
   - Examples: `TextParam`, `PasswordParam`, `PatTokenListParam`

3. **Editors** (`ParamEditorBase`):
   - UI components for editing parameters
   - Registered by type in `SettingsController`
   - Examples: `TextParamEditor`, `PasswordParamEditor`, `PatTokenParamEditor`

## Suggestions for Simplifying Settings

### 1. Create Base Parameter Types

For common patterns, create reusable parameter types:

```qml
// Example: ListParam.qml - Generic list parameter
Param {
    property ListModel items: ListModel {}
    property string itemTypeId: ""
    
    function addItem(itemData) { /* ... */ }
    function removeItem(index) { /* ... */ }
    function toJson() { /* ... */ }
    function createFromJson(json) { /* ... */ }
}
```

### 2. Simplify Controller Registration

Create a helper function in `ApplicationMain.qml`:

```qml
function registerSettingsSection(id, name, controllerComp, condition) {
    if (condition === undefined || condition) {
        let controller = controllerComp.createObject(application)
        settingsController.registerParamsSetController(id, name, controller)
    }
}

// Usage:
registerSettingsSection("PatTokens", qsTr("PAT Tokens"), patTokensControllerComp)
registerSettingsSection("Network", qsTr("Network"), clientSettingsControllerComp, Qt.platform.os !== "web")
```

### 3. Declarative Settings Definition

Consider creating a declarative settings definition:

```qml
// SettingsSchema.qml
QtObject {
    property list<QtObject> sections: [
        QtObject {
            property string id: "PatTokens"
            property string name: qsTr("PAT Tokens")
            property Component controller: Component { PatTokenSettingsController {} }
            property bool visible: true
        },
        QtObject {
            property string id: "Network"
            property string name: qsTr("Network")
            property Component controller: Component { ClientSettingsController {} }
            property bool visible: Qt.platform.os !== "web"
        }
    ]
}
```

### 4. Auto-Registration Pattern

Use Component.onCompleted in controllers to auto-register:

```qml
PatTokenSettingsController {
    Component.onCompleted: {
        if (settingsController) {
            settingsController.registerParamsSetController(
                "PatTokens", 
                qsTr("PAT Tokens"), 
                this
            )
        }
    }
}
```

### 5. Settings Validation

Add validation helpers:

```qml
// In Param base class
property var validators: []

function validate() {
    for (let validator of validators) {
        if (!validator.validate(this)) {
            return false
        }
    }
    return true
}
```

### 6. Settings Change Tracking

Implement undo/redo for settings:

```qml
SettingsHistoryManager {
    property var history: []
    property int currentIndex: -1
    
    function recordChange(paramId, oldValue, newValue) { /* ... */ }
    function undo() { /* ... */ }
    function redo() { /* ... */ }
}
```

## Storage

PAT tokens are stored in the application settings as JSON:

```json
{
    "tokens": [
        {
            "name": "GitHub API",
            "token": "ghp_xxxxxxxxxxxx",
            "description": "Token for GitHub API access",
            "createdDate": "2026-01-26T15:55:12.092Z"
        }
    ]
}
```

### Security Considerations

- Tokens are displayed as password fields by default
- Show/hide toggle for viewing token values
- Consider encrypting tokens in storage (future enhancement)
- Consider adding token expiration dates
- Consider integrating with system keychain for secure storage

## Future Enhancements

1. **Token Validation**: Add API endpoint to validate tokens
2. **Token Expiration**: Add expiration date field and warnings
3. **Token Permissions**: Add scope/permission management
4. **Token Usage Tracking**: Track when and where tokens are used
5. **Secure Storage**: Integrate with system keychain/credential manager
6. **Import/Export**: Allow importing tokens from files
7. **Token Templates**: Pre-configured token templates for common services

## Testing

To test the PAT token editor:

1. Build the application
2. Run the application
3. Open Settings
4. Navigate to "PAT Tokens" section
5. Test adding, editing, and deleting tokens
6. Verify changes persist after closing and reopening settings
7. Verify "Apply" saves changes and "Close" discards them

## Technical Notes

- The implementation follows the existing parameter editor pattern in ImtCore
- Uses Qt Quick 2.12 and QML best practices
- Compatible with both desktop and web platforms
- Integrates seamlessly with existing settings infrastructure
- No server-side changes required (client-side only)
