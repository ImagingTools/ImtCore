# QML-разработка с ImtCore — Начало работы

Это руководство шаг за шагом введёт вас в QML-разработку с ImtCore.

## Предварительные требования

- Установлен Qt 5.15+ или Qt 6.x
- Склонирован репозиторий ImtCore (с подмодулями)
- Базовые знания QML/JavaScript

## 1. Понимание структуры проекта

QML-файлы в проекте ImtCore организованы следующим образом:

```
ImtCore/
├── Qml/                          ← Все QML-модули
│   ├── imtcontrols/              ← Базовые UI-компоненты
│   │   ├── Actions/              ← Действия и взаимодействия
│   │   ├── Base/                 ← Базовые типы и синглтоны
│   │   ├── Buttons/              ← Варианты кнопок
│   │   ├── Dialogs/              ← Компоненты диалогов
│   │   ├── GraphicsView/         ← 2D-графика
│   │   ├── Inputs/               ← Поля ввода
│   │   ├── Models/               ← Модели данных
│   │   ├── Views/                ← Списки, таблицы, деревья
│   │   └── qmldir                ← Определение модуля
│   ├── imtgui/                   ← Фреймворк приложения
│   │   ├── Application/          ← Жизненный цикл приложения
│   │   ├── Panels/               ← Панели компоновки
│   │   ├── Params/               ← Редакторы параметров
│   │   ├── Settings/             ← Управление настройками
│   │   └── View/                 ← Компоненты представлений
│   ├── imtguigql/                ← Интеграция с GraphQL
│   └── ...                       ← Доменные модули
├── Tools/
│   └── JQML/                     ← Веб-компилятор
│       ├── v1/                   ← PureQML
│       ├── v2/                   ← JQML2 (с 3D)
│       └── v3/                   ← JQ (текущий)
└── Impl/
    └── ImtControlsGallery/       ← Демо-приложение
```

## 2. Импорт модулей

Каждый QML-модуль подключается через оператор `import`:

```qml
import imtcontrols 1.0    // Базовые UI-компоненты
import imtgui 1.0         // Фреймворк приложения
import imtguigql 1.0      // Интеграция с GraphQL
```

Доступные компоненты модуля определены в соответствующем файле `qmldir`.

## 3. Первое приложение

### 3.1 Каркас с ApplicationMain

`ApplicationMain` из `imtgui` является точкой входа каждого приложения ImtCore:

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

### 3.2 Определение страниц

Страницы управляются через `PagesProvider` и `PagesManager`:

```qml
PagesProvider {
    id: pagesProvider

    property var pageDefinitions: [
        { "id": "home",     "title": "Home",     "icon": Icon.home },
        { "id": "settings", "title": "Settings", "icon": Icon.settings }
    ]
}
```

### 3.3 Навигация по меню

`MenuPanel` предоставляет навигацию через боковую панель:

```qml
MenuPanel {
    id: menuPanel
    model: pagesProvider.pageDefinitions

    MenuPanelButton {
        // Автоматически генерируется из модели
    }
}
```

## 4. Базовые компоненты (imtcontrols)

### 4.1 Кнопки

ImtCore предлагает различные типы кнопок:

```qml
import imtcontrols 1.0

// Стандартная кнопка
Button {
    text: "Нажми меня"
    onClicked: console.log("Кнопка нажата")
}

// Текстовая кнопка (без рамки)
TextButton {
    text: "Текстовое действие"
    onClicked: doSomething()
}

// Кнопка-инструмент (на основе иконки)
ToolButton {
    iconSource: Icon.edit
    onClicked: editItem()
}

// Сегментированная кнопка
SegmentedButton {
    model: ["Option A", "Option B", "Option C"]
    onSelectedIndexChanged: handleSelection(selectedIndex)
}

// Радио-кнопка
RadioButton {
    text: "Option 1"
    checked: true
}
```

### 4.2 Поля ввода

```qml
// Текстовое поле ввода
CustomTextField {
    placeholderText: "Введите имя..."
    onTextChanged: validate(text)
}

// Текстовое поле с заголовком
TextFieldWithTitle {
    title: "Имя пользователя"
    text: model.username
}

// Поле поиска
SearchTextInput {
    placeholderText: "Поиск..."
    onTextChanged: filterResults(text)
}

// ComboBox
ComboBox {
    model: ["Deutsch", "English", "Français"]
    currentIndex: 0
}

// Фильтруемый выпадающий список
FilterableComboBox {
    model: largeDataModel
    filterRole: "name"
}

// CheckBox
CheckBox {
    text: "Активный"
    checked: model.isActive
}

// Дата
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

### 4.3 Представления и списки

```qml
// Таблица
Table {
    model: dataModel

    TableHeaderDelegate { title: "Имя"; role: "name"; width: 200 }
    TableHeaderDelegate { title: "Статус"; role: "status"; width: 100 }
    TableHeaderDelegate { title: "Дата"; role: "date"; width: 150 }
}

// Древовидное представление
BasicTreeView {
    model: treeModel

    delegate: TreeItemDelegate {
        text: model.display
    }
}

// Stack-View (навигация «хлебные крошки»)
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
    // Страницы как дочерние элементы
}
```

### 4.4 Диалоги

```qml
// Стандартный диалог
Dialog {
    title: "Подтверждение"
    width: 400
    height: 200

    DialogButtons {
        onAccepted: doAction()
        onRejected: close()
    }
}

// Диалог сообщения
MessageDialog {
    title: "Информация"
    text: "Операция завершена."
    icon: MessageDialog.Information
}

// Диалог ввода
InputDialog {
    title: "Новое имя"
    placeholderText: "Введите имя..."
    onAccepted: rename(inputText)
}

// Диалог ошибки
ErrorDialog {
    title: "Ошибка"
    errorText: "Ошибка соединения."
}
```

### 4.5 Модели

```qml
// Модель списка на основе JSON
JSONListModel {
    source: "data.json"
    query: "$.items[*]"
}

// Модель дерева
TreeModelBuilder {
    id: treeBuilder

    // Построение на основе полей
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

// Прокси сортировки/фильтрации
SortFilterProxyModel {
    sourceModel: originalModel
    sortRole: "name"
    filterRole: "category"
    filterString: "active"
}
```

## 5. Интеграция с GraphQL (imtguigql)

### 5.1 GraphQL-запросы

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
        console.error("Ошибка GraphQL:", errorMessage)
    }
}

// Отправить запрос
Button {
    text: "Загрузить"
    onClicked: myRequest.send()
}
```

### 5.2 SDL-запрос

```qml
GqlSdlRequestSender {
    id: sdlSender
    commandId: "s_getUsers"

    onResponseReceived: {
        // Автоматическая обработка SDL
    }
}
```

### 5.3 Подписки (WebSocket)

```qml
SubscriptionClient {
    id: subscriptionClient
    url: webSocketUrl

    onMessageReceived: {
        handleRealtimeUpdate(message)
    }
}
```

### 5.4 Управление коллекциями

```qml
// Загрузка и отображение данных из коллекции
GqlBasedCollectionDataController {
    id: collectionController
    collectionId: "users"

    onDataChanged: {
        updateView()
    }
}

CollectionRepresentation {
    dataController: collectionController

    // Автоматическое представление в виде таблицы/списка
}
```

### 5.5 FilterableSelect с GraphQL

```qml
FilterableSelectGqlDataProvider {
    id: rolesDataProvider
    collectionId: "Roles"

    // Загрузка данных с сервера
    function loadRoles(filterText) {
        fetch(filterText)
    }
}

// Использование в представлении
FilterableComboBox {
    model: rolesDataProvider.items
}
```

## 6. Панели и компоновка (imtgui)

### 6.1 Верхняя панель

```qml
TopPanel {
    TopLeftPanel {
        TopLeftPanelDecorator {
            // Логотип, название приложения
        }
    }

    TopCenterPanel {
        TopCenterPanelDecorator {
            // Строка поиска, навигация
        }
    }

    TopRightPanel {
        TopRightPanelDecorator {
            // Меню пользователя, уведомления
        }
    }
}
```

### 6.2 Панель команд

```qml
CommandsPanel {
    CommandsDecorator {
        // Действия панели инструментов
        Action { text: "Создать"; icon: Icon.add; onTriggered: createNew() }
        Action { text: "Редактировать"; icon: Icon.edit; onTriggered: editSelected() }
        Action { text: "Удалить"; icon: Icon.delete_; onTriggered: deleteSelected() }
    }
}
```

### 6.3 Панель фильтров

```qml
FilterPanelDecorator {
    FilterMenu {
        FieldFilterDelegate {
            title: "Статус"
            model: ["Активный", "Неактивный", "Ожидает"]
        }

        TimeFilterDelegate {
            title: "Период"
        }

        OptionsFilterDelegate {
            title: "Категория"
            model: categoriesModel
        }
    }
}
```

## 7. Редакторы параметров (imtgui/Params)

Редакторы параметров предоставляют стандартизированные формы:

```qml
// Текстовый параметр
TextParamEditor {
    paramName: "serverName"
    title: "Имя сервера"
}

// Параметр пароля
PasswordParamEditor {
    paramName: "apiKey"
    title: "API-ключ"
}

// Целочисленный параметр
IntegerParamEditor {
    paramName: "port"
    title: "Порт"
    minValue: 1
    maxValue: 65535
}

// Параметр выбора
SelectionParamEditor {
    paramName: "protocol"
    title: "Протокол"
    options: ["HTTP", "HTTPS", "WebSocket"]
}

// Параметр URL
UrlParamEditor {
    paramName: "endpoint"
    title: "API-эндпоинт"
}

// Параметр файла
FileNameParamEditor {
    paramName: "configPath"
    title: "Файл конфигурации"
    filter: "*.json"
}

// Подключение к серверу
ServerConnectionParamEditor {
    paramName: "mainServer"
    title: "Главный сервер"
}
```

## 8. Важные синглтоны

Следующие синглтоны из `imtcontrols` доступны глобально:

| Синглтон | Описание |
|----------|----------|
| `Events` | Глобальная система событий |
| `EventManager` | Регистрация и диспетчеризация событий |
| `Enums` | Общие перечисления |
| `Functions` | Утилитарные функции |
| `DesignScheme` | Цвета, шрифты, дизайн-токены |
| `AnalyticGeometry` | Геометрические вычисления |
| `LinearAlgebra` | Операции с матрицами/векторами |

### Пример: DesignScheme

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

### Пример: Events

```qml
// Отправить событие
Events.sendEvent("userLoggedIn", { userId: currentUser.id })

// Получить событие
Connections {
    target: Events
    onEventReceived: {
        if (eventName === "userLoggedIn") {
            loadUserData(eventData.userId)
        }
    }
}
```

## 9. Совместимость с JQML

При написании QML-кода, который должен работать как на десктопе, так и в вебе через JQML, соблюдайте следующие правила:

### ✅ Допустимо

- Стандартные QML-свойства и привязки
- `ListModel` для статических данных
- Привязки свойств через Data-Provider (`dataProvider.items`)
- `import imtcontrols 1.0` / `import imtgui 1.0`

### ❌ Избегайте

- **`prop: { ... JS Block ... }`** — используйте вместо этого `ListModel` или привязки Data-Provider
- Типы, специфичные для Qt Desktop, не имеющие эквивалента в JQML
- Прямые вызовы C++ без QML-обёртки

### Пример: Правильное присвоение модели

```qml
// ❌ НЕПРАВИЛЬНО — JS-блок как свойство
// comboBox.model: { return myArray.map(...) }

// ✅ ПРАВИЛЬНО — ListModel
ListModel {
    id: myModel
    ListElement { text: "Option A"; value: "a" }
    ListElement { text: "Option B"; value: "b" }
}
ComboBox {
    model: myModel
}

// ✅ ПРАВИЛЬНО — Привязка Data Provider
ComboBox {
    model: dataProvider.items
}
```

➡️ Подробности: **[Руководство JQML](QML_JQML_Guide.md)**

## 10. Следующие шаги

- **[Справочник модулей](QML_Module_Reference.md)** — Все компоненты подробно
- **[Руководство JQML](QML_JQML_Guide.md)** — Веб-развёртывание с JQML
- **[Руководства по компонентам](QML_Component_Tutorials.md)** — Практические паттерны
- **[ImtControlsGallery](../../Impl/ImtControlsGallery/)** — Демо-приложение со всеми компонентами
