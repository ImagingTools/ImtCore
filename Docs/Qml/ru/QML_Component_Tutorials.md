# Руководства по QML-компонентам

Практические руководства и паттерны для разработки с QML-компонентами ImtCore.

---

## Руководство 1: Таблица с сортировкой и фильтрацией

### Цель
Создать таблицу с сортируемыми столбцами и полем фильтрации.

### Структура

```qml
import QtQuick 2.15
import imtcontrols 1.0
import imtgui 1.0

Item {
    id: root

    // ── Поле поиска ──
    SearchTextInput {
        id: searchInput
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        placeholderText: "Поиск..."
    }

    // ── Таблица ──
    Table {
        id: table
        anchors.top: searchInput.bottom
        anchors.topMargin: 8
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        model: sortFilterProxy

        TableHeaderDelegate { title: "Имя";      role: "name";    width: 200; sortable: true }
        TableHeaderDelegate { title: "E-Mail";    role: "email";   width: 250; sortable: true }
        TableHeaderDelegate { title: "Статус";    role: "status";  width: 120; sortable: true }
        TableHeaderDelegate { title: "Создано";   role: "created"; width: 150; sortable: true }
    }

    // ── Модель данных ──
    ListModel {
        id: dataModel
        ListElement { name: "Anna Müller";   email: "anna@example.com";   status: "Активен";   created: "2025-01-15" }
        ListElement { name: "Max Schmidt";   email: "max@example.com";    status: "Неактивен"; created: "2025-02-20" }
        ListElement { name: "Lisa Weber";    email: "lisa@example.com";   status: "Активен";   created: "2025-03-10" }
    }

    // ── Прокси сортировки/фильтрации ──
    SortFilterProxyModel {
        id: sortFilterProxy
        sourceModel: dataModel
        filterRole: "name"
        filterString: searchInput.text
        sortRole: "name"
    }
}
```

### Пояснение

1. `SearchTextInput` предоставляет поле поиска с автоматическим дебаунсингом
2. `SortFilterProxyModel` реактивно фильтрует и сортирует модель
3. `TableHeaderDelegate` определяет столбцы с автоматической сортировкой
4. Привязка `filterString: searchInput.text` связывает поиск и фильтр

---

## Руководство 2: Навигация StackView с хлебными крошками

### Цель
Реализовать многостраничную навигацию с StackView и заголовком-хлебными крошками.

### Структура

```qml
import QtQuick 2.15
import imtcontrols 1.0

Item {
    id: root

    // ── Заголовок с хлебными крошками ──
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

    // ── Стековая навигация ──
    StackView {
        id: stackView
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Component.onCompleted: {
            addPage(listPage)
            header.addHeader("Обзор")
        }
    }

    // ── Страница 1: Список ──
    Component {
        id: listPage

        ListView {
            model: itemModel
            delegate: ItemDelegate {
                text: model.name

                onClicked: {
                    // Перейти на страницу деталей
                    stackView.addPage(detailPage)
                    stackView.next()
                    header.addHeader(model.name)
                }
            }
        }
    }

    // ── Страница 2: Детали ──
    Component {
        id: detailPage

        Column {
            spacing: 16
            padding: 20

            Title { text: "Детальный просмотр" }

            TextFieldWithTitle {
                title: "Имя"
                text: "Детальная информация"
            }

            Button {
                text: "Открыть подстраницу"
                onClicked: {
                    stackView.addPage(subPage)
                    stackView.next()
                    header.addHeader("Подстраница")
                }
            }
        }
    }

    // ── Страница 3: Подстраница ──
    Component {
        id: subPage

        Item {
            Title { text: "Подстраница" }
        }
    }
}
```

### Пояснение

- `StackView` управляет страницами по индексу (`addPage`, `next`, `previous`)
- `StackViewHeader` отображает путь хлебных крошек, разделённых `/` (`addHeader`, `popHeader`)
- Навигация возможна как вперёд (`next`), так и назад (`previous`)
- **Важно**: Это `StackView` из ImtCore, а не `QtQuick.Controls.StackView`

---

## Руководство 3: Загрузка и отображение данных GraphQL

### Цель
Загрузить данные через GraphQL и отобразить их в таблице.

### Структура

```qml
import QtQuick 2.15
import imtcontrols 1.0
import imtgui 1.0
import imtguigql 1.0

Item {
    id: root

    // ── GraphQL-запрос ──
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
            console.error("Ошибка загрузки:", errorMessage)
        }
    }

    // ── Модель данных ──
    ListModel {
        id: userModel
    }

    // ── Поле поиска ──
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

    // ── Таблица результатов ──
    Table {
        anchors.top: searchField.bottom
        anchors.topMargin: 8
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        model: userModel

        TableHeaderDelegate { title: "Имя";    role: "name";  width: 200 }
        TableHeaderDelegate { title: "E-Mail";  role: "email"; width: 250 }
        TableHeaderDelegate { title: "Роль";    role: "role";  width: 150 }
    }

    // ── Начальная загрузка ──
    Component.onCompleted: {
        usersRequest.send()
    }
}
```

---

## Руководство 4: FilterableSelect с GraphQL-бэкендом

### Цель
Фильтруемый выпадающий список с серверным поставщиком данных.

### Структура

```qml
import QtQuick 2.15
import imtcontrols 1.0
import imtguigql 1.0

Item {
    id: root

    // ── Поставщик данных ──
    FilterableSelectGqlDataProvider {
        id: rolesProvider
        collectionId: "Roles"
    }

    // ── Фильтруемый выпадающий список ──
    Column {
        spacing: 8

        Text { text: "Выберите роль:" }

        SearchTextInput {
            id: roleSearch
            placeholderText: "Поиск роли..."
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

### Пояснение

- `FilterableSelectGqlDataProvider` автоматически отправляет GraphQL-запросы
- `collectionId` определяет, какая коллекция запрашивается
- `fetch(text)` запускает отфильтрованный запрос
- `items` содержит список результатов в виде модели

---

## Руководство 5: Система диалогов

### Цель
Использование различных типов диалогов.

### Структура

```qml
import QtQuick 2.15
import imtcontrols 1.0

Item {
    id: root

    Column {
        spacing: 12

        Button {
            text: "Показать сообщение"
            onClicked: messageDialog.open()
        }

        Button {
            text: "Диалог ввода"
            onClicked: inputDialog.open()
        }

        Button {
            text: "Диалог подтверждения"
            onClicked: confirmDialog.open()
        }
    }

    // ── Диалог сообщения ──
    MessageDialog {
        id: messageDialog
        title: "Информация"
        text: "Действие выполнено успешно."
    }

    // ── Диалог ввода ──
    InputDialog {
        id: inputDialog
        title: "Новый элемент"
        placeholderText: "Введите имя..."

        onAccepted: {
            console.log("Ввод:", inputText)
            // Создать элемент
        }
    }

    // ── Диалог подтверждения ──
    Dialog {
        id: confirmDialog
        title: "Подтверждение удаления"
        width: 400
        height: 180

        Column {
            anchors.centerIn: parent
            spacing: 12

            Text {
                text: "Вы действительно хотите удалить этот элемент?"
            }

            DialogButtons {
                acceptText: "Удалить"
                rejectText: "Отмена"

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

## Руководство 6: Древовидное представление с TreeModelBuilder

### Цель
Построить и отобразить иерархическую древовидную модель.

### Структура

```qml
import QtQuick 2.15
import imtcontrols 1.0

Item {
    id: root

    // ── TreeModelBuilder ──
    TreeModelBuilder {
        id: treeBuilder
    }

    // ── Исходные данные ──
    ListModel {
        id: sourceModel
        ListElement {
            nodeId: "1"; nodeName: "Корень"; parentId: ""
        }
        ListElement {
            nodeId: "2"; nodeName: "Потомок 1"; parentId: "1"
        }
        ListElement {
            nodeId: "3"; nodeName: "Потомок 2"; parentId: "1"
        }
        ListElement {
            nodeId: "4"; nodeName: "Внук 1"; parentId: "2"
        }
    }

    // ── Построение дерева ──
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

    // ── Отображение дерева ──
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

### API TreeModelBuilder

Существует два стиля API:

**На основе полей** (рекомендуется):
```qml
treeBuilder.fromListModelByFields(model, {
    key: "id",           // Поле ключа
    text: "name",        // Отображаемый текст
    children: "children", // Поле потомков или ID родителя
    checkable: true,     // Выбираемый
    checked: false,      // Изначально выбран
    enabled: true,       // Активирован
    expanded: true       // Изначально развёрнут
})
```

**На основе обратного вызова**:
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

## Руководство 7: Форма параметров

### Цель
Форма настроек с различными редакторами параметров.

### Структура

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

        Title { text: "Конфигурация сервера" }

        TextParamEditor {
            title: "Имя хоста"
            paramName: "hostname"
            text: "localhost"
        }

        IntegerParamEditor {
            title: "Порт"
            paramName: "port"
            value: 9001
            minValue: 1
            maxValue: 65535
        }

        SelectionParamEditor {
            title: "Протокол"
            paramName: "protocol"
            model: ["HTTP", "HTTPS", "WebSocket", "gRPC"]
            currentIndex: 1
        }

        PasswordParamEditor {
            title: "API-ключ"
            paramName: "apiKey"
        }

        UrlParamEditor {
            title: "URL конечной точки"
            paramName: "endpoint"
            text: "https://api.example.com/graphql"
        }

        EnableableParamEditor {
            title: "Включить SSL"
            paramName: "sslEnabled"
            checked: true
        }

        ServerConnectionParamEditor {
            title: "Основной сервер"
            paramName: "mainServer"
        }

        // Кнопка сохранения
        Button {
            text: "Сохранить"
            onClicked: saveSettings()
        }
    }
}
```

---

## Руководство 8: GraphicsView — 2D-рисование

### Цель
Рисование фигур на холсте.

### Структура

```qml
import QtQuick 2.15
import imtcontrols 1.0

GraphicsView {
    id: graphicsView
    width: 800
    height: 600

    GraphicsLayer {
        id: mainLayer

        // Прямоугольник
        RectangleShape {
            x: 50; y: 50
            width: 200; height: 100
            params: RectangleParams {
                fillColor: "lightblue"
                strokeColor: "navy"
                strokeWidth: 2
            }
        }

        // Круг
        CircleShape {
            x: 400; y: 150
            params: CircleParams {
                radius: 60
                fillColor: "lightgreen"
                strokeColor: "darkgreen"
                strokeWidth: 2
            }
        }

        // Полилиния
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

        // Текст
        TextShape {
            x: 50; y: 450
            params: TextParams {
                text: "Hello GraphicsView!"
                fontFamily: "Arial"
                fontSize: 24
                fillColor: "darkblue"
            }
        }

        // Кривая Безье
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

## Руководство 9: JQML-совместимая 3D-сцена

### Цель
3D-сцена, работающая как на десктопе, так и в вебе.

### Структура

```qml
import QtQuick 2.15
import imt3dgui 1.0

Item {
    width: 800
    height: 600

    View3D {
        anchors.fill: parent

        // Камера
        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(0, 200, 400)
            eulerRotation: Qt.vector3d(-20, 0, 0)
        }

        // Освещение
        DirectionalLight {
            eulerRotation: Qt.vector3d(-45, -45, 0)
            brightness: 1.0
        }

        AmbientLight {
            brightness: 0.3
        }

        // Окружение
        SceneEnvironment {
            clearColor: "#f0f0f0"
        }

        // Объекты
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

### Десктоп vs. Веб

| Аспект | Десктоп | Веб (JQML) |
|--------|---------|------------|
| Рендерер | C++ бэкенд (CRhiWidget) | Three.js WebGLRenderer |
| Импорт | `import imt3dgui 1.0` | `import imt3dgui 1.0` (одинаково!) |
| Меш-примитивы | Qt Quick 3D | Three.js Geometries |
| Загрузка glTF | Qt Quick 3D Balsam | Three.js GLTFLoader |

---

## Руководство 10: Система событий и межкомпонентное взаимодействие

### Цель
Обеспечить взаимодействие различных компонентов через систему событий.

### Структура

```qml
import QtQuick 2.15
import imtcontrols 1.0

Item {
    id: root

    // ── Компонент-отправитель ──
    Column {
        anchors.left: parent.left
        width: parent.width / 2
        spacing: 8

        Title { text: "Отправитель" }

        Button {
            text: "Обновить пользователя"
            onClicked: {
                Events.sendEvent("userUpdated", {
                    userId: "123",
                    name: "Max Mustermann"
                })
            }
        }

        Button {
            text: "Сменить тему"
            onClicked: {
                Events.sendEvent("themeChanged", {
                    theme: "dark"
                })
            }
        }
    }

    // ── Компонент-получатель ──
    Column {
        anchors.right: parent.right
        width: parent.width / 2
        spacing: 8

        Title { text: "Получатель" }

        Text {
            id: statusText
            text: "Ожидание событий..."
        }

        Connections {
            target: Events
            onEventReceived: {
                if (eventName === "userUpdated") {
                    statusText.text = "Пользователь: " + eventData.name
                } else if (eventName === "themeChanged") {
                    statusText.text = "Тема: " + eventData.theme
                }
            }
        }
    }
}
```

---

## См. также

- **[QML Documentation Index](QML_Documentation_Index.md)** — Обзор
- **[Getting Started](QML_Getting_Started.md)** — Начало работы
- **[Справочник модулей](QML_Module_Reference.md)** — Все компоненты
- **[Руководство JQML](QML_JQML_Guide.md)** — Веб-компиляция
- **[ImtControlsGallery](../../Impl/ImtControlsGallery/)** — Демо-приложение
