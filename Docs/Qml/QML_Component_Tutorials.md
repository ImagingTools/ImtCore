# QML Komponenten-Tutorials

Praktische Tutorials und Patterns für die Entwicklung mit ImtCore QML-Komponenten.

---

## Tutorial 1: Tabelle mit Sortierung und Filterung

### Ziel
Eine Tabelle mit sortierbaren Spalten und einem Filterfeld erstellen.

### Aufbau

```qml
import QtQuick 2.15
import imtcontrols 1.0
import imtgui 1.0

Item {
    id: root

    // ── Suchfeld ──
    SearchTextInput {
        id: searchInput
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        placeholderText: "Suchen..."
    }

    // ── Tabelle ──
    Table {
        id: table
        anchors.top: searchInput.bottom
        anchors.topMargin: 8
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        model: sortFilterProxy

        TableHeaderDelegate { title: "Name";    role: "name";    width: 200; sortable: true }
        TableHeaderDelegate { title: "E-Mail";  role: "email";   width: 250; sortable: true }
        TableHeaderDelegate { title: "Status";  role: "status";  width: 120; sortable: true }
        TableHeaderDelegate { title: "Erstellt"; role: "created"; width: 150; sortable: true }
    }

    // ── Datenmodell ──
    ListModel {
        id: dataModel
        ListElement { name: "Anna Müller";   email: "anna@example.com";   status: "Aktiv";   created: "2025-01-15" }
        ListElement { name: "Max Schmidt";   email: "max@example.com";    status: "Inaktiv"; created: "2025-02-20" }
        ListElement { name: "Lisa Weber";    email: "lisa@example.com";   status: "Aktiv";   created: "2025-03-10" }
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

### Erklärung

1. `SearchTextInput` bietet ein Suchfeld mit automatischem Debouncing
2. `SortFilterProxyModel` filtert und sortiert das Modell reaktiv
3. `TableHeaderDelegate` definiert Spalten mit automatischer Sortierung
4. Das Binding `filterString: searchInput.text` verbindet Suche und Filter

---

## Tutorial 2: StackView-Navigation mit Breadcrumbs

### Ziel
Eine mehrseitige Navigation mit StackView und Breadcrumb-Header implementieren.

### Aufbau

```qml
import QtQuick 2.15
import imtcontrols 1.0

Item {
    id: root

    // ── Breadcrumb-Header ──
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

    // ── Stack-Navigation ──
    StackView {
        id: stackView
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Component.onCompleted: {
            addPage(listPage)
            header.addHeader("Übersicht")
        }
    }

    // ── Seite 1: Liste ──
    Component {
        id: listPage

        ListView {
            model: itemModel
            delegate: ItemDelegate {
                text: model.name

                onClicked: {
                    // Zur Detail-Seite navigieren
                    stackView.addPage(detailPage)
                    stackView.next()
                    header.addHeader(model.name)
                }
            }
        }
    }

    // ── Seite 2: Detail ──
    Component {
        id: detailPage

        Column {
            spacing: 16
            padding: 20

            Title { text: "Detail-Ansicht" }

            TextFieldWithTitle {
                title: "Name"
                text: "Detail-Information"
            }

            Button {
                text: "Unterseite öffnen"
                onClicked: {
                    stackView.addPage(subPage)
                    stackView.next()
                    header.addHeader("Unterseite")
                }
            }
        }
    }

    // ── Seite 3: Unterseite ──
    Component {
        id: subPage

        Item {
            Title { text: "Unterseite" }
        }
    }
}
```

### Erklärung

- `StackView` verwaltet Seiten per Index (`addPage`, `next`, `previous`)
- `StackViewHeader` zeigt einen `/`-getrennten Breadcrumb-Pfad (`addHeader`, `popHeader`)
- Navigation ist vorwärts (`next`) und rückwärts (`previous`) möglich
- **Wichtig**: Dies ist der ImtCore `StackView`, nicht `QtQuick.Controls.StackView`

---

## Tutorial 3: GraphQL-Daten laden und anzeigen

### Ziel
Daten per GraphQL laden und in einer Tabelle anzeigen.

### Aufbau

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
            console.error("Fehler beim Laden:", errorMessage)
        }
    }

    // ── Datenmodell ──
    ListModel {
        id: userModel
    }

    // ── Suchfeld ──
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

    // ── Ergebnis-Tabelle ──
    Table {
        anchors.top: searchField.bottom
        anchors.topMargin: 8
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        model: userModel

        TableHeaderDelegate { title: "Name";   role: "name";  width: 200 }
        TableHeaderDelegate { title: "E-Mail"; role: "email"; width: 250 }
        TableHeaderDelegate { title: "Rolle";  role: "role";  width: 150 }
    }

    // ── Initial laden ──
    Component.onCompleted: {
        usersRequest.send()
    }
}
```

---

## Tutorial 4: FilterableSelect mit GraphQL-Backend

### Ziel
Ein filterbares Dropdown mit serverseitigem Daten-Provider.

### Aufbau

```qml
import QtQuick 2.15
import imtcontrols 1.0
import imtguigql 1.0

Item {
    id: root

    // ── Daten-Provider ──
    FilterableSelectGqlDataProvider {
        id: rolesProvider
        collectionId: "Roles"
    }

    // ── Filterbares Dropdown ──
    Column {
        spacing: 8

        Text { text: "Rolle auswählen:" }

        SearchTextInput {
            id: roleSearch
            placeholderText: "Rolle suchen..."
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

### Erklärung

- `FilterableSelectGqlDataProvider` sendet automatisch GraphQL-Anfragen
- `collectionId` bestimmt, welche Collection abgefragt wird
- `fetch(text)` löst eine gefilterte Abfrage aus
- `items` enthält die Ergebnis-Liste als Modell

---

## Tutorial 5: Dialog-System

### Ziel
Verschiedene Dialog-Typen verwenden.

### Aufbau

```qml
import QtQuick 2.15
import imtcontrols 1.0

Item {
    id: root

    Column {
        spacing: 12

        Button {
            text: "Nachricht anzeigen"
            onClicked: messageDialog.open()
        }

        Button {
            text: "Eingabe-Dialog"
            onClicked: inputDialog.open()
        }

        Button {
            text: "Bestätigungs-Dialog"
            onClicked: confirmDialog.open()
        }
    }

    // ── Nachricht-Dialog ──
    MessageDialog {
        id: messageDialog
        title: "Information"
        text: "Die Aktion wurde erfolgreich ausgeführt."
    }

    // ── Eingabe-Dialog ──
    InputDialog {
        id: inputDialog
        title: "Neues Element"
        placeholderText: "Name eingeben..."

        onAccepted: {
            console.log("Eingabe:", inputText)
            // Element erstellen
        }
    }

    // ── Bestätigungs-Dialog ──
    Dialog {
        id: confirmDialog
        title: "Löschen bestätigen"
        width: 400
        height: 180

        Column {
            anchors.centerIn: parent
            spacing: 12

            Text {
                text: "Möchten Sie dieses Element wirklich löschen?"
            }

            DialogButtons {
                acceptText: "Löschen"
                rejectText: "Abbrechen"

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

## Tutorial 6: Baum-Ansicht mit TreeModelBuilder

### Ziel
Ein hierarchisches Baum-Modell aufbauen und anzeigen.

### Aufbau

```qml
import QtQuick 2.15
import imtcontrols 1.0

Item {
    id: root

    // ── TreeModelBuilder ──
    TreeModelBuilder {
        id: treeBuilder
    }

    // ── Quelldaten ──
    ListModel {
        id: sourceModel
        ListElement {
            nodeId: "1"; nodeName: "Wurzel"; parentId: ""
        }
        ListElement {
            nodeId: "2"; nodeName: "Kind 1"; parentId: "1"
        }
        ListElement {
            nodeId: "3"; nodeName: "Kind 2"; parentId: "1"
        }
        ListElement {
            nodeId: "4"; nodeName: "Enkel 1"; parentId: "2"
        }
    }

    // ── Baum aufbauen ──
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

    // ── Baum anzeigen ──
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

Es gibt zwei API-Stile:

**Feld-basiert** (empfohlen):
```qml
treeBuilder.fromListModelByFields(model, {
    key: "id",           // Schlüssel-Feld
    text: "name",        // Anzeige-Text
    children: "children", // Kinder-Feld oder Parent-ID
    checkable: true,     // Auswählbar
    checked: false,      // Initial ausgewählt
    enabled: true,       // Aktiviert
    expanded: true       // Initial aufgeklappt
})
```

**Callback-basiert**:
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

## Tutorial 7: Parameter-Formular

### Ziel
Ein Einstellungs-Formular mit verschiedenen Parameter-Editoren.

### Aufbau

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

        Title { text: "Server-Konfiguration" }

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
            title: "Protokoll"
            paramName: "protocol"
            model: ["HTTP", "HTTPS", "WebSocket", "gRPC"]
            currentIndex: 1
        }

        PasswordParamEditor {
            title: "API-Schlüssel"
            paramName: "apiKey"
        }

        UrlParamEditor {
            title: "Endpunkt-URL"
            paramName: "endpoint"
            text: "https://api.example.com/graphql"
        }

        EnableableParamEditor {
            title: "SSL aktivieren"
            paramName: "sslEnabled"
            checked: true
        }

        ServerConnectionParamEditor {
            title: "Hauptserver"
            paramName: "mainServer"
        }

        // Speichern-Button
        Button {
            text: "Speichern"
            onClicked: saveSettings()
        }
    }
}
```

---

## Tutorial 8: GraphicsView — 2D-Zeichnung

### Ziel
Formen auf einem Canvas zeichnen.

### Aufbau

```qml
import QtQuick 2.15
import imtcontrols 1.0

GraphicsView {
    id: graphicsView
    width: 800
    height: 600

    GraphicsLayer {
        id: mainLayer

        // Rechteck
        RectangleShape {
            x: 50; y: 50
            width: 200; height: 100
            params: RectangleParams {
                fillColor: "lightblue"
                strokeColor: "navy"
                strokeWidth: 2
            }
        }

        // Kreis
        CircleShape {
            x: 400; y: 150
            params: CircleParams {
                radius: 60
                fillColor: "lightgreen"
                strokeColor: "darkgreen"
                strokeWidth: 2
            }
        }

        // Polylinie
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

        // Bézier-Kurve
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

## Tutorial 9: JQML-kompatible 3D-Szene

### Ziel
Eine 3D-Szene, die sowohl auf dem Desktop als auch im Web funktioniert.

### Aufbau

```qml
import QtQuick 2.15
import imt3dgui 1.0

Item {
    width: 800
    height: 600

    View3D {
        anchors.fill: parent

        // Kamera
        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(0, 200, 400)
            eulerRotation: Qt.vector3d(-20, 0, 0)
        }

        // Beleuchtung
        DirectionalLight {
            eulerRotation: Qt.vector3d(-45, -45, 0)
            brightness: 1.0
        }

        AmbientLight {
            brightness: 0.3
        }

        // Umgebung
        SceneEnvironment {
            clearColor: "#f0f0f0"
        }

        // Objekte
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

| Aspekt | Desktop | Web (JQML) |
|--------|---------|------------|
| Renderer | C++ Backend (CRhiWidget) | Three.js WebGLRenderer |
| Import | `import imt3dgui 1.0` | `import imt3dgui 1.0` (gleich!) |
| Mesh-Primitives | Qt Quick 3D | Three.js Geometrien |
| glTF-Loading | Qt Quick 3D Balsam | Three.js GLTFLoader |

---

## Tutorial 10: Event-System und komponentenübergreifende Kommunikation

### Ziel
Verschiedene Komponenten über das Event-System kommunizieren lassen.

### Aufbau

```qml
import QtQuick 2.15
import imtcontrols 1.0

Item {
    id: root

    // ── Sender-Komponente ──
    Column {
        anchors.left: parent.left
        width: parent.width / 2
        spacing: 8

        Title { text: "Sender" }

        Button {
            text: "Benutzer aktualisieren"
            onClicked: {
                Events.sendEvent("userUpdated", {
                    userId: "123",
                    name: "Max Mustermann"
                })
            }
        }

        Button {
            text: "Theme wechseln"
            onClicked: {
                Events.sendEvent("themeChanged", {
                    theme: "dark"
                })
            }
        }
    }

    // ── Empfänger-Komponente ──
    Column {
        anchors.right: parent.right
        width: parent.width / 2
        spacing: 8

        Title { text: "Empfänger" }

        Text {
            id: statusText
            text: "Warte auf Events..."
        }

        Connections {
            target: Events
            onEventReceived: {
                if (eventName === "userUpdated") {
                    statusText.text = "Benutzer: " + eventData.name
                } else if (eventName === "themeChanged") {
                    statusText.text = "Theme: " + eventData.theme
                }
            }
        }
    }
}
```

---

## Siehe auch

- **[QML Documentation Index](QML_Documentation_Index.md)** — Übersicht
- **[Getting Started](QML_Getting_Started.md)** — Einstieg
- **[Modul-Referenz](QML_Module_Reference.md)** — Alle Komponenten
- **[JQML Guide](QML_JQML_Guide.md)** — Web-Kompilierung
- **[ImtControlsGallery](../../Impl/ImtControlsGallery/)** — Demo-Applikation
