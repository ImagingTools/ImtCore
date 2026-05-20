# Руководство по JQML — QML для веба

JQML — это инструментарий компиляции QML в JavaScript от ImtCore. Он позволяет запускать один и тот же исходный код QML как в виде нативного настольного приложения Qt, так и в виде веб-приложения в браузере.

---

## Обзор

```
┌──────────────────────┐
│   Исходный код QML   │   ← Написать один раз
│  (imtcontrols, etc.) │
└──────┬───────────────┘
       │
       ├──→ Qt Desktop Build (native)
       │
       └──→ JQML Compiler ──→ JavaScript ──→ Browser
```

### Версии JQML

| Версия | Каталог | Статус | Описание |
|--------|---------|--------|----------|
| **v1** | `Tools/JQML/v1/` | Legacy | PureQML — первое поколение |
| **v2** | `Tools/JQML/v2/` | Активен | JQML2 — с поддержкой 3D, 130+ компонентов |
| **v3** | `Tools/JQML/v3/` | Активен | JQ — новая архитектура, собственный парсер, Source Maps |

---

## JQML v2 — JQML2

### Архитектура

```
Tools/JQML/v2/
├── compiler/           ← Компилятор QML в JS
├── components/         ← 130+ реализаций компонентов (JS)
│   ├── Item.js         ← Базовый элемент
│   ├── Rectangle.js    ← Прямоугольник
│   ├── Text.js         ← Текст
│   ├── ListView.js     ← Представление списка
│   ├── View3D.js       ← 3D-сцена (Three.js)
│   ├── TreeItemModel.js ← Модель дерева
│   └── list.js         ← Реестр компонентов
├── core/               ← Движок времени выполнения
├── utils/              ← Вспомогательные функции
├── dist/               ← Скомпилированный бандл
├── package.json
└── webpack.config.js
```

### Компиляция

```bash
cd Tools/JQML/v2

# Установить зависимости
npm install

# Собрать бандл времени выполнения (продакшен)
npm run build-core

# Собрать бандл времени выполнения (разработка, с Source Maps)
npm run build-core-dev

# Скомпилировать QML-проект
node compiler/compiler.js \
    -c config.json \
    -e MainApp.qml \
    -o output/ \
    -n myapp \
    -m js
```

### Маппинг компонентов (v2)

JQML v2 реализует более 130 компонентов в виде JavaScript-классов:

| Категория | Компоненты |
|-----------|------------|
| **Базовые** | Item, Rectangle, Text, TextInput, TextEdit, Image, MouseArea, FocusScope |
| **Компоновка** | Column, Row, Flow, SplitView, Repeater |
| **Списки** | ListView, GridView, TableView, TreeView |
| **Модели** | ListModel, ListElement, TableModel, TreeItemModel, GqlModel |
| **Анимация** | Animation, NumberAnimation, PropertyAnimation, PauseAnimation |
| **Диалоги** | Popup, Drawer, Menu, MenuItem, MenuBar, FileDialog, FolderDialog |
| **3D** | View3D, Node3D, Model, PerspectiveCamera, DirectionalLight, PrincipledMaterial |
| **Ввод** | DragHandler, WheelHandler, Shortcut |
| **Сеть** | WebSocket, WebView |
| **Карты** | Map, MapCircle, MapPolygon, MapPolyline, MapQuickItem |
| **Утилиты** | Timer, Settings, Component, Connections, Binding, Loader |

### Поддержка 3D

JQML v2 включает 3D-визуализацию через Three.js (лицензия MIT):

```qml
import imt3dgui 1.0

View3D {
    width: 800
    height: 600

    PerspectiveCamera {
        position: Qt.vector3d(0, 200, 300)
    }

    DirectionalLight {
        eulerRotation: Qt.vector3d(-30, -30, 0)
    }

    Model {
        source: "#Cube"
        materials: PrincipledMaterial {
            baseColor: "steelblue"
        }
    }
}
```

**Маппинг Qt Quick 3D ↔ Three.js:**

| Тип QML | Тип Three.js |
|---------|--------------|
| `View3D` | `WebGLRenderer` + `Scene` |
| `Node3D` | `Object3D` / `Group` |
| `Model` | `Mesh` |
| `PerspectiveCamera` | `PerspectiveCamera` |
| `DirectionalLight` | `DirectionalLight` |
| `PrincipledMaterial` | `MeshStandardMaterial` |

Меш-примитивы: `"#Cube"`, `"#Sphere"`, `"#Cylinder"`, `"#Cone"`, `"#Plane"`, `"#Rectangle"`

glTF-файлы (`.glb`/`.gltf`) загружаются асинхронно через `GLTFLoader`.

➡️ Подробнее: [Tools/JQML/v2/README-3D.md](../../Tools/JQML/v2/README-3D.md)

### Добавление нового компонента в JQML v2

1. Создать JavaScript-файл в `Tools/JQML/v2/components/`
2. Зарегистрировать в `components/list.js`
3. Готово — код компилятора изменять не нужно

```javascript
// components/MyComponent.js
class MyComponent extends Item {
    // Реализация
}
```

### Совместимость API TreeItemModel

При обращении к методам TreeItemModel необходимо поддерживать как верхний, так и нижний регистр:

| Qt/C++ (верхний регистр) | JQML (нижний регистр) |
|--------------------------|----------------------|
| `GetTreeItemModel` | `getTreeItemModel` |
| `GetItemsCount` | `getItemsCount` |
| `GetData` | `getData` |

---

## JQML v3 — JQ

### Архитектура

```
Tools/JQML/v3/
├── compiler/
│   ├── compiler.js     ← Основной компилятор (2 133 строки)
│   └── parser.js       ← QML-парсер на основе UglifyJS (1 935 строк)
├── core/               ← Движок времени выполнения
│   ├── JQApplication   ← Глобальный жизненный цикл
│   ├── JQContext        ← Область выполнения, привязки
│   ├── MouseController  ← Диспетчер событий мыши
│   ├── KeyboardController ← Диспетчер событий клавиатуры
│   ├── ImageController  ← Загрузка и кэширование изображений
│   ├── TextController   ← Измерение текста
│   └── MemoryController ← Жизненный цикл объектов
├── dist/main.js        ← Webpack-бандл (время выполнения)
├── Qt/                 ← Глобальный модуль Qt (перечисления, вспомогательные функции)
├── QtBase/             ← Инфраструктура базовых классов
├── QtQml/              ← ~60 основных типов
├── QtQuick/            ← ~50 UI-компонентов
├── Qt5Compat/          ← Слой совместимости с Qt 5
├── QtLocation/         ← Типы карт
├── QtPositioning/      ← Геокоординаты
├── QtWebSockets/       ← WebSocket
└── package.json
```

### Конвейер компиляции

Компилятор v3 проходит 7 фаз:

```
Фаза 1: Разбор CLI и конфигурация
    ↓
Фаза 2: Загрузка файла конфигурации (JSON с dirs, includes)
    ↓
Фаза 3: Обнаружение модулей (разбор файлов qmldir)
    ↓
Фаза 4: Разбор отдельных файлов (компоненты приложения)
    ↓
Фаза 5: Генерация кода (QML → JavaScript-классы)
    ↓
Фаза 6: Сортировка зависимостей (топологическая)
    ↓
Фаза 7: Вывод (бандл + Source Map)
```

### Конфигурация

Файл конфигурации (JSON):

```json
{
    "dirs": [
        "${IMTCOREDIR}/Qml/imtcontrols",
        "${IMTCOREDIR}/Qml/imtgui",
        "${IMTCOREDIR}/Qml/imtguigql"
    ],
    "includes": [
        "other-config.json"
    ]
}
```

Переменные окружения (`${VAR}`) подставляются автоматически.

### Команды сборки

```bash
cd Tools/JQML/v3

# Установить зависимости
npm install

# Собрать бандл времени выполнения (продакшен)
npm run build-core

# Собрать бандл времени выполнения (разработка)
npm run build-core-dev

# Скомпилировать QML-проект
npm run compile -- \
    -c config.json \
    -e MainApp.qml \
    -o output/ \
    -n myapp \
    -m js
```

Windows:
```bat
jqcompiler.bat -c config.json -e MainApp.qml -o output -n myapp -m js
```

### Режим HTML

Компилятор также может создать шаблонную HTML-страницу:

```bash
npm run compile -- -c config.json -e MainApp.qml -o output -n myapp -m html -i icon.png
```

### Встроенные модули

| Модуль | Содержимое |
|--------|------------|
| `Qt` | Перечисления, `Qt.rgba()`, `Qt.lighter()`, `Qt.binding()` |
| `QtQml` | Component, Connections, Binding, Timer, типы свойств |
| `QtQuick` | Item, Rectangle, Text, Image, MouseArea, анимации, компоновки |
| `Qt5Compat` | Прослойки совместимости с Qt 5 |
| `QtWebSockets` | WebSocket |
| `QtPositioning` | GeoCoordinate |
| `QtLocation` | Map, MapCircle, MapPolygon, MapPolyline |

### Интеграция с QMake

Компилятор JQML автоматически запускается во время сборки Qt через `Config/QMake/WebCompiler.pri`:

- Использует `QMAKE_EXTRA_COMPILERS` для пользовательского шага сборки
- Компилирует при изменениях в QML-файлах
- Генерирует файл `.qrc`, который встраивается в приложение

---

## Кроссплатформенная разработка

### Принципы

1. **Один исходный код QML** для настольных и веб-приложений
2. **Модули через qmldir** — компилятор находит компоненты автоматически
3. **Никаких платформозависимых импортов** в коде бизнес-логики

### Платформозависимый код

Для случаев, когда настольная и веб-версии требуют различного поведения:

```
Qml/imtcontrols/Base/         ← По умолчанию (настольное приложение)
Qml/imtcontrols/Base/web/     ← Веб-переопределения
```

Веб-специфичные файлы в подкаталогах `web/` переопределяют свои настольные аналоги:
- `ScrollBar.qml` → Веб-совместимая полоса прокрутки
- `ToolTip.qml` → Веб-совместимая всплывающая подсказка
- `Switch.qml` → Веб-совместимый переключатель
- `Window.qml` → Веб-совместимая обёртка окна

### Правила QML для совместимости с JQML

#### ✅ Рекомендуется

- Использовать стандартные свойства QML и декларативные привязки
- `ListModel` для статических данных
- Паттерн провайдера данных для динамических данных
- `import imtcontrols 1.0` вместо прямых импортов Qt, где это возможно

#### ❌ Избегать

- **`prop: { ... JS-блок ... }`** для привязок модели
  ```qml
  // ❌ НЕПРАВИЛЬНО
  // model: { return computeModel() }

  // ✅ ПРАВИЛЬНО
  model: myDataProvider.items
  ```

- Настольных типов Qt без эквивалента в JQML
- Прямых вызовов C++ без обёрточного слоя QML
- `Qt.createComponent()` в критичном для производительности коде

#### Доступ к TreeItemModel

Проверять как верхний, так и нижний регистр:

```qml
// В TreeModelBuilder
function childModel(item) {
    if (typeof item.GetTreeItemModel === "function")
        return item.GetTreeItemModel()
    if (typeof item.getTreeItemModel === "function")
        return item.getTreeItemModel()
}
```

### Зависимости (v3)

| Пакет | Версия | Назначение |
|-------|--------|------------|
| `commander` | ^14.0.1 | Разбор аргументов CLI |
| `source-map-generator` | ^0.8.0 | Генерация Source Map |
| `uglify-js` | ^3.19.3 | JS AST (основа для QML-парсера) |
| `ol` | ^7.5.2 | OpenLayers (среда выполнения QtLocation) |
| `webpack-cli` | ^5.1.4 | Сборка среды выполнения (зависимость разработки) |

---

## Отладка

### Source Maps

JQML v3 генерирует Source Maps (`.js.map`), которые позволяют инструментам разработчика в браузере отлаживать код непосредственно в исходном коде QML.

### Стратегия рендеринга

`View3D` (v2 3D) использует **рендеринг по требованию**: только изменения свойств вызывают `requestAnimationFrame`. Неактивные сцены не потребляют ресурсы CPU/GPU.

### Picking (выбор объектов)

`View3D` предоставляет:
- `pick(x, y)` — ближайший `Node3D` под 2D-координатой
- `mapFrom3DScene({x, y, z})` — мир → экран
- `mapTo3DScene({x, y, z})` — экран → мир

---

## Дополнительная документация

- **[Tools/JQML/v2/README-3D.md](../../Tools/JQML/v2/README-3D.md)** — Документация по 3D в JQML2
- **[Tools/JQML/v3/compiler/README.md](../../Tools/JQML/v3/compiler/README.md)** — Архитектура компилятора JQML v3
- **[QML Documentation Index](QML_Documentation_Index.md)** — Обзор документации
- **[QML Module Reference](QML_Module_Reference.md)** — Все QML-компоненты
- **[QML Getting Started](QML_Getting_Started.md)** — Вводное руководство
- **[QML Component Tutorials](QML_Component_Tutorials.md)** — Практические руководства
