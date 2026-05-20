# QML Entwicklung mit ImtCore — Getting Started

Dieses Tutorial führt Sie Schritt für Schritt in die QML-Entwicklung mit ImtCore ein.

## Voraussetzungen

- Qt 5.15+ oder Qt 6.x installiert
- ImtCore Repository geklont (mit Submodulen)
- Grundkenntnisse in QML/JavaScript

## 1. Projekt-Struktur verstehen

Die QML-Dateien im ImtCore-Projekt sind wie folgt organisiert:

```
ImtCore/
├── Qml/                          ← Alle QML-Module
│   ├── imtcontrols/              ← Basis-UI-Komponenten
│   │   ├── Actions/              ← Aktionen & Interaktionen
│   │   ├── Base/                 ← Grundlagen-Typen & Singletons
│   │   ├── Buttons/              ← Button-Varianten
│   │   ├── Dialogs/              ← Dialog-Komponenten
│   │   ├── GraphicsView/         ← 2D-Grafik-Zeichnung
│   │   ├── Inputs/               ← Eingabe-Felder
│   │   ├── Models/               ← Daten-Modelle
│   │   ├── Views/                ← Listen, Tabellen, Trees
│   │   └── qmldir                ← Modul-Definition
│   ├── imtgui/                   ← Applikations-Framework
│   │   ├── Application/          ← App-Lifecycle
│   │   ├── Panels/               ← Layout-Panels
│   │   ├── Params/               ← Parameter-Editoren
│   │   ├── Settings/             ← Einstellungs-Verwaltung
│   │   └── View/                 ← View-Komponenten
│   ├── imtguigql/                ← GraphQL-Integration
│   └── ...                       ← Domain-Module
├── Tools/
│   └── JQML/                     ← Web-Compiler
│       ├── v1/                   ← PureQML
│       ├── v2/                   ← JQML2 (mit 3D)
│       └── v3/                   ← JQ (aktuell)
└── Impl/
    └── ImtControlsGallery/       ← Demo-Applikation
```

## 2. Module importieren

Jedes QML-Modul wird per `import`-Statement eingebunden:

```qml
import imtcontrols 1.0    // Basis-UI-Komponenten
import imtgui 1.0         // Applikations-Framework
import imtguigql 1.0      // GraphQL-Integration
```

Die verfügbaren Komponenten eines Moduls sind in der jeweiligen `qmldir`-Datei definiert.

## 3. Erste Applikation

### 3.1 Grundgerüst mit ApplicationMain

`ApplicationMain` aus `imtgui` ist der Einstiegspunkt jeder ImtCore-Applikation:

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

### 3.2 Seiten definieren

Seiten werden über `PagesProvider` und `PagesManager` verwaltet:

```qml
PagesProvider {
    id: pagesProvider

    property var pageDefinitions: [
        { "id": "home",     "title": "Home",     "icon": Icon.home },
        { "id": "settings", "title": "Settings", "icon": Icon.settings }
    ]
}
```

### 3.3 Menü-Navigation

Das `MenuPanel` bietet eine Seitenleisten-Navigation:

```qml
MenuPanel {
    id: menuPanel
    model: pagesProvider.pageDefinitions

    MenuPanelButton {
        // Automatisch aus dem Modell generiert
    }
}
```

## 4. Basis-Komponenten (imtcontrols)

### 4.1 Buttons

ImtCore bietet verschiedene Button-Typen:

```qml
import imtcontrols 1.0

// Standard-Button
Button {
    text: "Klick mich"
    onClicked: console.log("Button geklickt")
}

// Text-Button (ohne Rahmen)
TextButton {
    text: "Text-Aktion"
    onClicked: doSomething()
}

// Tool-Button (Icon-basiert)
ToolButton {
    iconSource: Icon.edit
    onClicked: editItem()
}

// Segmentierter Button
SegmentedButton {
    model: ["Option A", "Option B", "Option C"]
    onSelectedIndexChanged: handleSelection(selectedIndex)
}

// Radio-Button
RadioButton {
    text: "Option 1"
    checked: true
}
```

### 4.2 Eingabefelder

```qml
// Text-Eingabe
CustomTextField {
    placeholderText: "Name eingeben..."
    onTextChanged: validate(text)
}

// Textfeld mit Titel
TextFieldWithTitle {
    title: "Benutzername"
    text: model.username
}

// Suchfeld
SearchTextInput {
    placeholderText: "Suchen..."
    onTextChanged: filterResults(text)
}

// ComboBox
ComboBox {
    model: ["Deutsch", "English", "Français"]
    currentIndex: 0
}

// Filterbares Dropdown
FilterableComboBox {
    model: largeDataModel
    filterRole: "name"
}

// CheckBox
CheckBox {
    text: "Aktiv"
    checked: model.isActive
}

// Datum
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

### 4.3 Views & Listen

```qml
// Tabelle
Table {
    model: dataModel

    TableHeaderDelegate { title: "Name"; role: "name"; width: 200 }
    TableHeaderDelegate { title: "Status"; role: "status"; width: 100 }
    TableHeaderDelegate { title: "Datum"; role: "date"; width: 150 }
}

// Tree-View
BasicTreeView {
    model: treeModel

    delegate: TreeItemDelegate {
        text: model.display
    }
}

// Stack-View (Breadcrumb-Navigation)
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
    // Seiten als Children
}
```

### 4.4 Dialoge

```qml
// Standard-Dialog
Dialog {
    title: "Bestätigung"
    width: 400
    height: 200

    DialogButtons {
        onAccepted: doAction()
        onRejected: close()
    }
}

// Nachricht-Dialog
MessageDialog {
    title: "Information"
    text: "Vorgang abgeschlossen."
    icon: MessageDialog.Information
}

// Eingabe-Dialog
InputDialog {
    title: "Neuer Name"
    placeholderText: "Name eingeben..."
    onAccepted: rename(inputText)
}

// Fehler-Dialog
ErrorDialog {
    title: "Fehler"
    errorText: "Verbindung fehlgeschlagen."
}
```

### 4.5 Models

```qml
// JSON-basiertes Listenmodell
JSONListModel {
    source: "data.json"
    query: "$.items[*]"
}

// Tree-Modell
TreeModelBuilder {
    id: treeBuilder

    // Feld-basierter Aufbau
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

## 5. GraphQL-Integration (imtguigql)

### 5.1 GraphQL-Anfragen

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
        console.error("GraphQL-Fehler:", errorMessage)
    }
}

// Anfrage senden
Button {
    text: "Laden"
    onClicked: myRequest.send()
}
```

### 5.2 SDL-basierter Request

```qml
GqlSdlRequestSender {
    id: sdlSender
    commandId: "s_getUsers"

    onResponseReceived: {
        // Automatische SDL-Verarbeitung
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

### 5.4 Collection-Verwaltung

```qml
// Daten aus einer Collection laden und anzeigen
GqlBasedCollectionDataController {
    id: collectionController
    collectionId: "users"

    onDataChanged: {
        updateView()
    }
}

CollectionRepresentation {
    dataController: collectionController

    // Automatische Tabellen-/Listen-Darstellung
}
```

### 5.5 FilterableSelect mit GraphQL

```qml
FilterableSelectGqlDataProvider {
    id: rolesDataProvider
    collectionId: "Roles"

    // Daten vom Server laden
    function loadRoles(filterText) {
        fetch(filterText)
    }
}

// In einem View verwenden
FilterableComboBox {
    model: rolesDataProvider.items
}
```

## 6. Panels & Layout (imtgui)

### 6.1 Top-Panel

```qml
TopPanel {
    TopLeftPanel {
        TopLeftPanelDecorator {
            // Logo, App-Name
        }
    }

    TopCenterPanel {
        TopCenterPanelDecorator {
            // Suchleiste, Navigation
        }
    }

    TopRightPanel {
        TopRightPanelDecorator {
            // Benutzer-Menü, Benachrichtigungen
        }
    }
}
```

### 6.2 Command-Panel

```qml
CommandsPanel {
    CommandsDecorator {
        // Toolbar-Aktionen
        Action { text: "Neu"; icon: Icon.add; onTriggered: createNew() }
        Action { text: "Bearbeiten"; icon: Icon.edit; onTriggered: editSelected() }
        Action { text: "Löschen"; icon: Icon.delete_; onTriggered: deleteSelected() }
    }
}
```

### 6.3 Filter-Panel

```qml
FilterPanelDecorator {
    FilterMenu {
        FieldFilterDelegate {
            title: "Status"
            model: ["Aktiv", "Inaktiv", "Ausstehend"]
        }

        TimeFilterDelegate {
            title: "Zeitraum"
        }

        OptionsFilterDelegate {
            title: "Kategorie"
            model: categoriesModel
        }
    }
}
```

## 7. Parameter-Editoren (imtgui/Params)

Parameter-Editoren bieten standardisierte Formulare:

```qml
// Text-Parameter
TextParamEditor {
    paramName: "serverName"
    title: "Server-Name"
}

// Passwort-Parameter
PasswordParamEditor {
    paramName: "apiKey"
    title: "API-Schlüssel"
}

// Ganzzahl-Parameter
IntegerParamEditor {
    paramName: "port"
    title: "Port"
    minValue: 1
    maxValue: 65535
}

// Auswahl-Parameter
SelectionParamEditor {
    paramName: "protocol"
    title: "Protokoll"
    options: ["HTTP", "HTTPS", "WebSocket"]
}

// URL-Parameter
UrlParamEditor {
    paramName: "endpoint"
    title: "API-Endpunkt"
}

// Datei-Parameter
FileNameParamEditor {
    paramName: "configPath"
    title: "Konfigurationsdatei"
    filter: "*.json"
}

// Server-Verbindung
ServerConnectionParamEditor {
    paramName: "mainServer"
    title: "Hauptserver"
}
```

## 8. Wichtige Singletons

Die folgenden Singletons aus `imtcontrols` sind global verfügbar:

| Singleton | Beschreibung |
|-----------|-------------|
| `Events` | Globales Event-System |
| `EventManager` | Event-Registrierung und -Dispatch |
| `Enums` | Gemeinsame Enumerationen |
| `Functions` | Utility-Funktionen |
| `DesignScheme` | Farben, Schriften, Design-Tokens |
| `AnalyticGeometry` | Geometrie-Berechnungen |
| `LinearAlgebra` | Matrix-/Vektor-Operationen |

### Beispiel: DesignScheme

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

### Beispiel: Events

```qml
// Event senden
Events.sendEvent("userLoggedIn", { userId: currentUser.id })

// Event empfangen
Connections {
    target: Events
    onEventReceived: {
        if (eventName === "userLoggedIn") {
            loadUserData(eventData.userId)
        }
    }
}
```

## 9. JQML-Kompatibilität

Beim Schreiben von QML-Code, der sowohl auf dem Desktop als auch über JQML im Web laufen soll, beachten Sie folgende Regeln:

### ✅ Erlaubt

- Standard-QML-Properties und -Bindings
- `ListModel` für statische Daten
- Property-Bindings über Data-Provider (`dataProvider.items`)
- `import imtcontrols 1.0` / `import imtgui 1.0`

### ❌ Vermeiden

- **`prop: { ... JS Block ... }`** — Verwenden Sie stattdessen `ListModel` oder Data-Provider-Bindings
- Qt-Desktop-spezifische Typen, die kein JQML-Äquivalent haben
- Direkte C++-Aufrufe ohne QML-Wrapper

### Beispiel: Korrekte Model-Zuweisung

```qml
// ❌ FALSCH — JS-Block als Property
// comboBox.model: { return myArray.map(...) }

// ✅ RICHTIG — ListModel
ListModel {
    id: myModel
    ListElement { text: "Option A"; value: "a" }
    ListElement { text: "Option B"; value: "b" }
}
ComboBox {
    model: myModel
}

// ✅ RICHTIG — Data Provider Binding
ComboBox {
    model: dataProvider.items
}
```

➡️ Weitere Details: **[JQML Guide](QML_JQML_Guide.md)**

## 10. Nächste Schritte

- **[Modul-Referenz](QML_Module_Reference.md)** — Alle Komponenten im Detail
- **[JQML Guide](QML_JQML_Guide.md)** — Web-Deployment mit JQML
- **[Komponenten-Tutorials](QML_Component_Tutorials.md)** — Praktische Patterns
- **[ImtControlsGallery](../../Impl/ImtControlsGallery/)** — Demo-Applikation mit allen Komponenten
