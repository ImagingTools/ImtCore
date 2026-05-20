# QML Component Tutorials

Practical tutorials and patterns for developing with ImtCore QML components.

---

## Tutorial 1: Table with Sorting and Filtering

### Goal
Create a table with sortable columns and a filter field.

### Structure

```qml
import QtQuick 2.15
import imtcontrols 1.0
import imtgui 1.0

Item {
    id: root

    // ── Search Field ──
    SearchTextInput {
        id: searchInput
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        placeholderText: "Search..."
    }

    // ── Table ──
    Table {
        id: table
        anchors.top: searchInput.bottom
        anchors.topMargin: 8
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        model: sortFilterProxy

        TableHeaderDelegate { title: "Name";    role: "name";    width: 200; sortable: true }
        TableHeaderDelegate { title: "Email";   role: "email";   width: 250; sortable: true }
        TableHeaderDelegate { title: "Status";  role: "status";  width: 120; sortable: true }
        TableHeaderDelegate { title: "Created"; role: "created"; width: 150; sortable: true }
    }

    // ── Data Model ──
    ListModel {
        id: dataModel
        ListElement { name: "Anna Müller";   email: "anna@example.com";   status: "Active";   created: "2025-01-15" }
        ListElement { name: "Max Schmidt";   email: "max@example.com";    status: "Inactive"; created: "2025-02-20" }
        ListElement { name: "Lisa Weber";    email: "lisa@example.com";   status: "Active";   created: "2025-03-10" }
    }

    // ── Sort/Filter Proxy ──
    SortFilterProxyModel {
        id: sortFilterProxy
        sourceModel: dataModel
        filterRole: "name"
        filterString: searchInput.text
        sortRole: "name"
    }
}
```

### Explanation

1. `SearchTextInput` provides a search field with automatic debouncing
2. `SortFilterProxyModel` filters and sorts the model reactively
3. `TableHeaderDelegate` defines columns with automatic sorting
4. The binding `filterString: searchInput.text` connects search and filter

---

## Tutorial 2: StackView Navigation with Breadcrumbs

### Goal
Implement multi-page navigation with StackView and a breadcrumb header.

### Structure

```qml
import QtQuick 2.15
import imtcontrols 1.0

Item {
    id: root

    // ── Breadcrumb Header ──
    StackViewHeader {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 48

        onCloseClicked: {
            stackView.previous()
            header.popHeader()
        }
    }

    // ── Stack Navigation ──
    StackView {
        id: stackView
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Component.onCompleted: {
            addPage(listPage)
            header.addHeader("Overview")
        }
    }

    // ── Page 1: List ──
    Component {
        id: listPage

        ListView {
            model: itemModel
            delegate: ItemDelegate {
                text: model.name

                onClicked: {
                    // Navigate to detail page
                    stackView.addPage(detailPage)
                    stackView.next()
                    header.addHeader(model.name)
                }
            }
        }
    }

    // ── Page 2: Detail ──
    Component {
        id: detailPage

        Column {
            spacing: 16
            padding: 20

            Title { text: "Detail View" }

            TextFieldWithTitle {
                title: "Name"
                text: "Detail Information"
            }

            Button {
                text: "Open Subpage"
                onClicked: {
                    stackView.addPage(subPage)
                    stackView.next()
                    header.addHeader("Subpage")
                }
            }
        }
    }

    // ── Page 3: Subpage ──
    Component {
        id: subPage

        Item {
            Title { text: "Subpage" }
        }
    }
}
```

### Explanation

- `StackView` manages pages by index (`addPage`, `next`, `previous`)
- `StackViewHeader` displays a `/`-separated breadcrumb path (`addHeader`, `popHeader`)
- Navigation is possible both forward (`next`) and backward (`previous`)
- **Important**: This is the ImtCore `StackView`, not `QtQuick.Controls.StackView`

---

## Tutorial 3: Loading and Displaying GraphQL Data

### Goal
Load data via GraphQL and display it in a table.

### Structure

```qml
import QtQuick 2.15
import imtcontrols 1.0
import imtgui 1.0
import imtguigql 1.0

Item {
    id: root

    // ── GraphQL Request ──
    GqlRequest {
        id: usersRequest
        query: '
            query GetUsers($filter: String) {
                users(filter: $filter) {
                    id
                    name
                    email
                    role
                }
            }
        '

        onCompleted: {
            userModel.clear()
            var users = result.data.users
            for (var i = 0; i < users.length; i++) {
                userModel.append(users[i])
            }
        }

        onError: {
            console.error("Error loading:", errorMessage)
        }
    }

    // ── Data Model ──
    ListModel {
        id: userModel
    }

    // ── Search Field ──
    SearchTextInput {
        id: searchField
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        onTextChanged: {
            usersRequest.variables = { "filter": text }
            usersRequest.send()
        }
    }

    // ── Results Table ──
    Table {
        anchors.top: searchField.bottom
        anchors.topMargin: 8
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        model: userModel

        TableHeaderDelegate { title: "Name";   role: "name";  width: 200 }
        TableHeaderDelegate { title: "Email";  role: "email"; width: 250 }
        TableHeaderDelegate { title: "Role";   role: "role";  width: 150 }
    }

    // ── Initial load ──
    Component.onCompleted: {
        usersRequest.send()
    }
}
```

---

## Tutorial 4: FilterableSelect with GraphQL Backend

### Goal
A filterable dropdown with a server-side data provider.

### Structure

```qml
import QtQuick 2.15
import imtcontrols 1.0
import imtguigql 1.0

Item {
    id: root

    // ── Data Provider ──
    FilterableSelectGqlDataProvider {
        id: rolesProvider
        collectionId: "Roles"
    }

    // ── Filterable Dropdown ──
    Column {
        spacing: 8

        Text { text: "Select role:" }

        SearchTextInput {
            id: roleSearch
            placeholderText: "Search role..."
            onTextChanged: rolesProvider.fetch(text)
        }

        ListView {
            width: 300
            height: 200
            model: rolesProvider.items

            delegate: ItemDelegate {
                text: model.name
                onClicked: {
                    selectedRole = model.id
                    roleSearch.text = model.name
                }
            }
        }
    }

    property string selectedRole: ""
}
```

### Explanation

- `FilterableSelectGqlDataProvider` automatically sends GraphQL requests
- `collectionId` determines which collection is queried
- `fetch(text)` triggers a filtered query
- `items` contains the result list as a model

---

## Tutorial 5: Dialog System

### Goal
Use different dialog types.

### Structure

```qml
import QtQuick 2.15
import imtcontrols 1.0

Item {
    id: root

    Column {
        spacing: 12

        Button {
            text: "Show Message"
            onClicked: messageDialog.open()
        }

        Button {
            text: "Input Dialog"
            onClicked: inputDialog.open()
        }

        Button {
            text: "Confirmation Dialog"
            onClicked: confirmDialog.open()
        }
    }

    // ── Message Dialog ──
    MessageDialog {
        id: messageDialog
        title: "Information"
        text: "The action was completed successfully."
    }

    // ── Input Dialog ──
    InputDialog {
        id: inputDialog
        title: "New Element"
        placeholderText: "Enter name..."

        onAccepted: {
            console.log("Input:", inputText)
            // Create element
        }
    }

    // ── Confirmation Dialog ──
    Dialog {
        id: confirmDialog
        title: "Confirm Deletion"
        width: 400
        height: 180

        Column {
            anchors.centerIn: parent
            spacing: 12

            Text {
                text: "Are you sure you want to delete this element?"
            }

            DialogButtons {
                acceptText: "Delete"
                rejectText: "Cancel"

                onAccepted: {
                    performDelete()
                    confirmDialog.close()
                }
                onRejected: confirmDialog.close()
            }
        }
    }
}
```

---

## Tutorial 6: Tree View with TreeModelBuilder

### Goal
Build and display a hierarchical tree model.

### Structure

```qml
import QtQuick 2.15
import imtcontrols 1.0

Item {
    id: root

    // ── TreeModelBuilder ──
    TreeModelBuilder {
        id: treeBuilder
    }

    // ── Source Data ──
    ListModel {
        id: sourceModel
        ListElement {
            nodeId: "1"; nodeName: "Root"; parentId: ""
        }
        ListElement {
            nodeId: "2"; nodeName: "Child 1"; parentId: "1"
        }
        ListElement {
            nodeId: "3"; nodeName: "Child 2"; parentId: "1"
        }
        ListElement {
            nodeId: "4"; nodeName: "Grandchild 1"; parentId: "2"
        }
    }

    // ── Build Tree ──
    Component.onCompleted: {
        treeBuilder.fromListModelByFields(sourceModel, {
            key: "nodeId",
            text: "nodeName",
            children: "parentId",
            checkable: true,
            checked: false,
            expanded: true
        })
    }

    // ── Display Tree ──
    BasicTreeView {
        anchors.fill: parent
        model: treeBuilder.model

        delegate: TreeItemDelegate {
            text: model.text
            checkable: model.checkable
            checked: model.checked
        }
    }
}
```

### TreeModelBuilder API

There are two API styles:

**Field-based** (recommended):
```qml
treeBuilder.fromListModelByFields(model, {
    key: "id",           // Key field
    text: "name",        // Display text
    children: "children", // Children field or parent ID
    checkable: true,     // Selectable
    checked: false,      // Initially selected
    enabled: true,       // Enabled
    expanded: true       // Initially expanded
})
```

**Callback-based**:
```qml
treeBuilder.fromListModel(model, {
    mapItem: function(item) {
        return { key: item.id, text: item.name }
    },
    getChildren: function(item) {
        return item.children
    }
})
```

---

## Tutorial 7: Parameter Form

### Goal
A settings form with various parameter editors.

### Structure

```qml
import QtQuick 2.15
import imtcontrols 1.0
import imtgui 1.0

Item {
    id: root

    Column {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        Title { text: "Server Configuration" }

        TextParamEditor {
            title: "Hostname"
            paramName: "hostname"
            text: "localhost"
        }

        IntegerParamEditor {
            title: "Port"
            paramName: "port"
            value: 9001
            minValue: 1
            maxValue: 65535
        }

        SelectionParamEditor {
            title: "Protocol"
            paramName: "protocol"
            model: ["HTTP", "HTTPS", "WebSocket", "gRPC"]
            currentIndex: 1
        }

        PasswordParamEditor {
            title: "API Key"
            paramName: "apiKey"
        }

        UrlParamEditor {
            title: "Endpoint URL"
            paramName: "endpoint"
            text: "https://api.example.com/graphql"
        }

        EnableableParamEditor {
            title: "Enable SSL"
            paramName: "sslEnabled"
            checked: true
        }

        ServerConnectionParamEditor {
            title: "Main Server"
            paramName: "mainServer"
        }

        // Save Button
        Button {
            text: "Save"
            onClicked: saveSettings()
        }
    }
}
```

---

## Tutorial 8: GraphicsView — 2D Drawing

### Goal
Draw shapes on a canvas.

### Structure

```qml
import QtQuick 2.15
import imtcontrols 1.0

GraphicsView {
    id: graphicsView
    width: 800
    height: 600

    GraphicsLayer {
        id: mainLayer

        // Rectangle
        RectangleShape {
            x: 50; y: 50
            width: 200; height: 100
            params: RectangleParams {
                fillColor: "lightblue"
                strokeColor: "navy"
                strokeWidth: 2
            }
        }

        // Circle
        CircleShape {
            x: 400; y: 150
            params: CircleParams {
                radius: 60
                fillColor: "lightgreen"
                strokeColor: "darkgreen"
                strokeWidth: 2
            }
        }

        // Polyline
        PolyLineShape {
            params: LineParams {
                strokeColor: "red"
                strokeWidth: 3
            }
            points: [
                Qt.point(100, 300),
                Qt.point(200, 250),
                Qt.point(300, 350),
                Qt.point(400, 280)
            ]
        }

        // Text
        TextShape {
            x: 50; y: 450
            params: TextParams {
                text: "Hello GraphicsView!"
                fontFamily: "Arial"
                fontSize: 24
                fillColor: "darkblue"
            }
        }

        // Bézier Curve
        BezierCurveShape {
            params: BezierCurveParams {
                startPoint: Qt.point(500, 100)
                controlPoint1: Qt.point(550, 50)
                controlPoint2: Qt.point(650, 200)
                endPoint: Qt.point(700, 100)
                strokeColor: "purple"
                strokeWidth: 3
            }
        }
    }
}
```

---

## Tutorial 9: JQML-Compatible 3D Scene

### Goal
A 3D scene that works on both desktop and web.

### Structure

```qml
import QtQuick 2.15
import imt3dgui 1.0

Item {
    width: 800
    height: 600

    View3D {
        anchors.fill: parent

        // Camera
        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(0, 200, 400)
            eulerRotation: Qt.vector3d(-20, 0, 0)
        }

        // Lighting
        DirectionalLight {
            eulerRotation: Qt.vector3d(-45, -45, 0)
            brightness: 1.0
        }

        AmbientLight {
            brightness: 0.3
        }

        // Environment
        SceneEnvironment {
            clearColor: "#f0f0f0"
        }

        // Objects
        Model {
            source: "#Cube"
            position: Qt.vector3d(-100, 0, 0)
            materials: PrincipledMaterial {
                baseColor: "steelblue"
                metalness: 0.3
                roughness: 0.5
            }
        }

        Model {
            source: "#Sphere"
            position: Qt.vector3d(100, 0, 0)
            materials: PrincipledMaterial {
                baseColor: "coral"
                metalness: 0.1
                roughness: 0.8
            }
        }

        Model {
            source: "#Cylinder"
            position: Qt.vector3d(0, 0, -100)
            eulerRotation: Qt.vector3d(0, 45, 0)
            materials: DefaultMaterial {
                diffuseColor: "lightgreen"
            }
        }
    }
}
```

### Desktop vs. Web

| Aspect | Desktop | Web (JQML) |
|--------|---------|------------|
| Renderer | C++ Backend (CRhiWidget) | Three.js WebGLRenderer |
| Import | `import imt3dgui 1.0` | `import imt3dgui 1.0` (same!) |
| Mesh Primitives | Qt Quick 3D | Three.js Geometries |
| glTF Loading | Qt Quick 3D Balsam | Three.js GLTFLoader |

---

## Tutorial 10: Event System and Cross-Component Communication

### Goal
Enable different components to communicate via the event system.

### Structure

```qml
import QtQuick 2.15
import imtcontrols 1.0

Item {
    id: root

    // ── Sender Component ──
    Column {
        anchors.left: parent.left
        width: parent.width / 2
        spacing: 8

        Title { text: "Sender" }

        Button {
            text: "Update User"
            onClicked: {
                Events.sendEvent("userUpdated", {
                    userId: "123",
                    name: "Max Mustermann"
                })
            }
        }

        Button {
            text: "Switch Theme"
            onClicked: {
                Events.sendEvent("themeChanged", {
                    theme: "dark"
                })
            }
        }
    }

    // ── Receiver Component ──
    Column {
        anchors.right: parent.right
        width: parent.width / 2
        spacing: 8

        Title { text: "Receiver" }

        Text {
            id: statusText
            text: "Waiting for events..."
        }

        Connections {
            target: Events
            onEventReceived: {
                if (eventName === "userUpdated") {
                    statusText.text = "User: " + eventData.name
                } else if (eventName === "themeChanged") {
                    statusText.text = "Theme: " + eventData.theme
                }
            }
        }
    }
}
```

---

## See Also

- **[QML Documentation Index](QML_Documentation_Index.md)** — Overview
- **[Getting Started](QML_Getting_Started.md)** — Getting Started
- **[Module Reference](QML_Module_Reference.md)** — All Components
- **[JQML Guide](QML_JQML_Guide.md)** — Web Compilation
- **[ImtControlsGallery](../../Impl/ImtControlsGallery/)** — Demo Application
