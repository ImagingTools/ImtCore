# QML Development with ImtCore — Getting Started

This tutorial guides you step by step into QML development with ImtCore.

## Prerequisites

- Qt 5.15+ or Qt 6.x installed
- ImtCore repository cloned (with submodules)
- Basic knowledge of QML/JavaScript

## 1. Understanding the Project Structure

The QML files in the ImtCore project are organized as follows:

```
ImtCore/
├── Qml/                          ← All QML modules
│   ├── imtcontrols/              ← Base UI components
│   │   ├── Actions/              ← Actions & Interactions
│   │   ├── Base/                 ← Base types & Singletons
│   │   ├── Buttons/              ← Button variants
│   │   ├── Dialogs/              ← Dialog components
│   │   ├── GraphicsView/         ← 2D graphics drawing
│   │   ├── Inputs/               ← Input fields
│   │   ├── Models/               ← Data models
│   │   ├── Views/                ← Lists, Tables, Trees
│   │   └── qmldir                ← Module definition
│   ├── imtgui/                   ← Application framework
│   │   ├── Application/          ← App lifecycle
│   │   ├── Panels/               ← Layout panels
│   │   ├── Params/               ← Parameter editors
│   │   ├── Settings/             ← Settings management
│   │   └── View/                 ← View components
│   ├── imtguigql/                ← GraphQL integration
│   └── ...                       ← Domain modules
├── Tools/
│   └── JQML/                     ← Web compiler
│       ├── v1/                   ← PureQML
│       ├── v2/                   ← JQML2 (with 3D)
│       └── v3/                   ← JQ (current)
└── Impl/
    └── ImtControlsGallery/       ← Demo application
```

## 2. Importing Modules

Each QML module is included via an `import` statement:

```qml
import imtcontrols 1.0    // Base UI components
import imtgui 1.0         // Application framework
import imtguigql 1.0      // GraphQL integration
```

The available components of a module are defined in the respective `qmldir` file.

## 3. First Application

### 3.1 Basic Scaffold with ApplicationMain

`ApplicationMain` from `imtgui` is the entry point of every ImtCore application:

```qml
import QtQuick 2.15
import imtcontrols 1.0
import imtgui 1.0

ApplicationMain {
    id: app

    Configurator {
        id: configurator
    }

    PagesManager {
        id: pagesManager
    }

    LanguageProvider {
        id: languageProvider
    }
}
```

### 3.2 Defining Pages

Pages are managed via `PagesProvider` and `PagesManager`:

```qml
PagesProvider {
    id: pagesProvider

    property var pageDefinitions: [
        { "id": "home",     "title": "Home",     "icon": Icon.home },
        { "id": "settings", "title": "Settings", "icon": Icon.settings }
    ]
}
```

### 3.3 Menu Navigation

The `MenuPanel` provides a sidebar navigation:

```qml
MenuPanel {
    id: menuPanel
    model: pagesProvider.pageDefinitions

    MenuPanelButton {
        // Automatically generated from the model
    }
}
```

## 4. Base Components (imtcontrols)

### 4.1 Buttons

ImtCore offers various button types:

```qml
import imtcontrols 1.0

// Standard button
Button {
    text: "Click me"
    onClicked: console.log("Button clicked")
}

// Text button (without border)
TextButton {
    text: "Text action"
    onClicked: doSomething()
}

// Tool button (icon-based)
ToolButton {
    iconSource: Icon.edit
    onClicked: editItem()
}

// Segmented button
SegmentedButton {
    model: ["Option A", "Option B", "Option C"]
    onSelectedIndexChanged: handleSelection(selectedIndex)
}

// Radio button
RadioButton {
    text: "Option 1"
    checked: true
}
```

### 4.2 Input Fields

```qml
// Text input
CustomTextField {
    placeholderText: "Enter name..."
    onTextChanged: validate(text)
}

// Text field with title
TextFieldWithTitle {
    title: "Username"
    text: model.username
}

// Search field
SearchTextInput {
    placeholderText: "Search..."
    onTextChanged: filterResults(text)
}

// ComboBox
ComboBox {
    model: ["Deutsch", "English", "Français"]
    currentIndex: 0
}

// Filterable dropdown
FilterableComboBox {
    model: largeDataModel
    filterRole: "name"
}

// CheckBox
CheckBox {
    text: "Active"
    checked: model.isActive
}

// Date
DatePicker {
    selectedDate: new Date()
    onDateChanged: handleDateChange(selectedDate)
}

// SpinBox
SpinBox {
    minimumValue: 0
    maximumValue: 100
    value: 50
}

// Slider
Slider {
    minimumValue: 0.0
    maximumValue: 1.0
    value: 0.5
}
```

### 4.3 Views & Lists

```qml
// Table
Table {
    model: dataModel

    TableHeaderDelegate { title: "Name"; role: "name"; width: 200 }
    TableHeaderDelegate { title: "Status"; role: "status"; width: 100 }
    TableHeaderDelegate { title: "Date"; role: "date"; width: 150 }
}

// Tree view
BasicTreeView {
    model: treeModel

    delegate: TreeItemDelegate {
        text: model.display
    }
}

// Stack view (breadcrumb navigation)
StackView {
    id: stackView

    Component.onCompleted: {
        addPage(firstPageComponent)
    }

    function navigateToDetail(item) {
        addPage(detailPageComponent)
        next()
    }
}

StackViewHeader {
    stackView: stackView
    onCloseClicked: stackView.previous()
}

// SwipeView
SwipeView {
    id: swipeView
    // Pages as children
}
```

### 4.4 Dialogs

```qml
// Standard dialog
Dialog {
    title: "Confirmation"
    width: 400
    height: 200

    DialogButtons {
        onAccepted: doAction()
        onRejected: close()
    }
}

// Message dialog
MessageDialog {
    title: "Information"
    text: "Operation completed."
    icon: MessageDialog.Information
}

// Input dialog
InputDialog {
    title: "New Name"
    placeholderText: "Enter name..."
    onAccepted: rename(inputText)
}

// Error dialog
ErrorDialog {
    title: "Error"
    errorText: "Connection failed."
}
```

### 4.5 Models

```qml
// JSON-based list model
JSONListModel {
    source: "data.json"
    query: "$.items[*]"
}

// Tree model
TreeModelBuilder {
    id: treeBuilder

    // Field-based construction
    function buildTree() {
        treeBuilder.fromListModelByFields(sourceModel, {
            key: "id",
            text: "name",
            children: "children",
            checkable: true,
            checked: false,
            expanded: true
        })
    }
}

// Sort/Filter Proxy
SortFilterProxyModel {
    sourceModel: originalModel
    sortRole: "name"
    filterRole: "category"
    filterString: "active"
}
```

## 5. GraphQL Integration (imtguigql)

### 5.1 GraphQL Requests

```qml
import imtguigql 1.0

GqlRequest {
    id: myRequest

    query: "query GetUsers { users { id name email } }"

    onCompleted: {
        var users = result.data.users
        userModel.clear()
        users.forEach(function(user) {
            userModel.append(user)
        })
    }

    onError: {
        console.error("GraphQL error:", errorMessage)
    }
}

// Send request
Button {
    text: "Load"
    onClicked: myRequest.send()
}
```

### 5.2 SDL-based Request

```qml
GqlSdlRequestSender {
    id: sdlSender
    commandId: "s_getUsers"

    onResponseReceived: {
        // Automatic SDL processing
    }
}
```

### 5.3 Subscriptions (WebSocket)

```qml
SubscriptionClient {
    id: subscriptionClient
    url: webSocketUrl

    onMessageReceived: {
        handleRealtimeUpdate(message)
    }
}
```

### 5.4 Collection Management

```qml
// Load and display data from a collection
GqlBasedCollectionDataController {
    id: collectionController
    collectionId: "users"

    onDataChanged: {
        updateView()
    }
}

CollectionRepresentation {
    dataController: collectionController

    // Automatic table/list display
}
```

### 5.5 FilterableSelect with GraphQL

```qml
FilterableSelectGqlDataProvider {
    id: rolesDataProvider
    collectionId: "Roles"

    // Load data from server
    function loadRoles(filterText) {
        fetch(filterText)
    }
}

// Use in a view
FilterableComboBox {
    model: rolesDataProvider.items
}
```

## 6. Panels & Layout (imtgui)

### 6.1 Top Panel

```qml
TopPanel {
    TopLeftPanel {
        TopLeftPanelDecorator {
            // Logo, app name
        }
    }

    TopCenterPanel {
        TopCenterPanelDecorator {
            // Search bar, navigation
        }
    }

    TopRightPanel {
        TopRightPanelDecorator {
            // User menu, notifications
        }
    }
}
```

### 6.2 Command Panel

```qml
CommandsPanel {
    CommandsDecorator {
        // Toolbar actions
        Action { text: "New"; icon: Icon.add; onTriggered: createNew() }
        Action { text: "Edit"; icon: Icon.edit; onTriggered: editSelected() }
        Action { text: "Delete"; icon: Icon.delete_; onTriggered: deleteSelected() }
    }
}
```

### 6.3 Filter Panel

```qml
FilterPanelDecorator {
    FilterMenu {
        FieldFilterDelegate {
            title: "Status"
            model: ["Active", "Inactive", "Pending"]
        }

        TimeFilterDelegate {
            title: "Time Period"
        }

        OptionsFilterDelegate {
            title: "Category"
            model: categoriesModel
        }
    }
}
```

## 7. Parameter Editors (imtgui/Params)

Parameter editors provide standardized forms:

```qml
// Text parameter
TextParamEditor {
    paramName: "serverName"
    title: "Server Name"
}

// Password parameter
PasswordParamEditor {
    paramName: "apiKey"
    title: "API Key"
}

// Integer parameter
IntegerParamEditor {
    paramName: "port"
    title: "Port"
    minValue: 1
    maxValue: 65535
}

// Selection parameter
SelectionParamEditor {
    paramName: "protocol"
    title: "Protocol"
    options: ["HTTP", "HTTPS", "WebSocket"]
}

// URL parameter
UrlParamEditor {
    paramName: "endpoint"
    title: "API Endpoint"
}

// File parameter
FileNameParamEditor {
    paramName: "configPath"
    title: "Configuration File"
    filter: "*.json"
}

// Server connection
ServerConnectionParamEditor {
    paramName: "mainServer"
    title: "Main Server"
}
```

## 8. Important Singletons

The following singletons from `imtcontrols` are globally available:

| Singleton | Description |
|-----------|-------------|
| `Events` | Global event system |
| `EventManager` | Event registration and dispatch |
| `Enums` | Common enumerations |
| `Functions` | Utility functions |
| `DesignScheme` | Colors, fonts, design tokens |
| `AnalyticGeometry` | Geometry calculations |
| `LinearAlgebra` | Matrix/vector operations |

### Example: DesignScheme

```qml
Rectangle {
    color: DesignScheme.backgroundColor
    border.color: DesignScheme.borderColor

    Text {
        font.family: DesignScheme.fontFamily
        font.pixelSize: DesignScheme.fontSize
        color: DesignScheme.textColor
    }
}
```

### Example: Events

```qml
// Send event
Events.sendEvent("userLoggedIn", { userId: currentUser.id })

// Receive event
Connections {
    target: Events
    onEventReceived: {
        if (eventName === "userLoggedIn") {
            loadUserData(eventData.userId)
        }
    }
}
```

## 9. JQML Compatibility

When writing QML code that should run both on desktop and via JQML on the web, observe the following rules:

### ✅ Allowed

- Standard QML properties and bindings
- `ListModel` for static data
- Property bindings via data providers (`dataProvider.items`)
- `import imtcontrols 1.0` / `import imtgui 1.0`

### ❌ Avoid

- **`prop: { ... JS Block ... }`** — Use `ListModel` or data provider bindings instead
- Qt desktop-specific types that have no JQML equivalent
- Direct C++ calls without QML wrappers

### Example: Correct Model Assignment

```qml
// ❌ WRONG — JS block as property
// comboBox.model: { return myArray.map(...) }

// ✅ CORRECT — ListModel
ListModel {
    id: myModel
    ListElement { text: "Option A"; value: "a" }
    ListElement { text: "Option B"; value: "b" }
}
ComboBox {
    model: myModel
}

// ✅ CORRECT — Data Provider Binding
ComboBox {
    model: dataProvider.items
}
```

➡️ More details: **[JQML Guide](QML_JQML_Guide.md)**

## 10. Next Steps

- **[Module Reference](QML_Module_Reference.md)** — All components in detail
- **[JQML Guide](QML_JQML_Guide.md)** — Web deployment with JQML
- **[Component Tutorials](QML_Component_Tutorials.md)** — Practical patterns
- **[ImtControlsGallery](../../Impl/ImtControlsGallery/)** — Demo application with all components
