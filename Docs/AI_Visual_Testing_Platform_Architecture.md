# AI Visual Testing Platform — Architecture & Design Document

## Общая идея

Разработать платформу визуального AI-тестирования веб-приложений как **отдельный проект на базе ImtCore/ACF**, в которой пользователь описывает сценарии тестирования в виде последовательности визуальных шагов, соединённых между собой графом, без необходимости писать код, локаторы, XPath, CSS-селекторы или Playwright-скрипты.

**Тип проекта:** Отдельный репозиторий, зависящий от ImtCore/ACF  
**Зависимости от ImtCore:** imtbase, imtcore, imtcom, imtgql, imtservergql, imtclientgql, imtcontrols, imtgui, imthttp, imtimg, imtdb/imtmdbx, imtsdl + кодогенераторы

---

## Ключевое отличие от существующих решений

Большинство систем автоматизации тестирования строятся вокруг кода и локаторов.

Данная платформа строится вокруг:

* визуального графа сценария
* скриншотов
* семантической модели интерфейса
* карты приложения
* AI-понимания пользовательских намерений

Основной объект системы — не тестовый код, а **модель пользовательского интерфейса и пользовательских сценариев**.

---

## Концепция сценариев

Сценарий представляет собой **ориентированный граф**.

Каждый узел графа описывает отдельное действие, проверку или условие.

### Типы узлов:

| Тип | Описание |
|-----|----------|
| `OpenUrl` | Открыть URL (url, waitForLoad) |
| `ClickElement` | Клик по элементу (description для AI, fallbackSelector) |
| `InputText` | Ввод текста (description, text, clearBefore) |
| `Wait` | Ожидание (duration, condition) |
| `TakeScreenshot` | Снимок экрана (name, fullPage, element) |
| `CompareScreenshot` | Сравнение с baseline (baselineId, threshold, ignoreRegions) |
| `AssertText` | Проверка текста (text, element, contains/equals) |
| `AssertElement` | Проверка элемента (description, visible/enabled/exists) |
| `AiValidation` | AI-проверка (assertion на естественном языке) |
| `ConditionalBranch` | Условный переход (condition, trueBranch, falseBranch) |
| `Loop` | Цикл (count, condition, body) |

### Пример сценария:

```
Open Login Page
     ↓
Click Login Button
     ↓
Wait For Loading
     ↓
Take Screenshot
     ↓
Compare With Baseline
     ↓
Open Users Page
     ↓
Create User
     ↓
Verify User Exists
```

---

## Основной принцип работы

Пользователь описывает **намерение**, а не способ реализации.

Вместо:
```javascript
click("#loginBtn")
```

используется:
```
"Нажать кнопку Login"
```

AI самостоятельно определяет:
* необходимый элемент
* наиболее подходящий способ взаимодействия
* доступные варианты выполнения действия

Система использует:
* DOM-структуру
* Accessibility-дерево
* Скриншоты
* Историю предыдущих действий

---

## Архитектура системы

### Структура проекта

```
ImtVTest/                            # Отдельный репозиторий
├── Include/
│   ├── imtvtest/                    # Core library
│   │   ├── IScenarioGraph.h
│   │   ├── CScenarioGraphComp.h/.cpp
│   │   ├── IScenarioExecutor.h
│   │   ├── CScenarioExecutorComp.h/.cpp
│   │   ├── INodeAction.h
│   │   ├── CNodeActionFactory.h/.cpp
│   │   ├── CExecutionEngineComp.h/.cpp
│   │   ├── IImageComparator.h
│   │   ├── CPixelDiffComparatorComp.h/.cpp
│   │   ├── CBaselineManagerComp.h/.cpp
│   │   ├── CScreenshotStorageComp.h/.cpp
│   │   ├── CAppMapBuilderComp.h/.cpp
│   │   └── imtvtest.h
│   └── imtvtestgql/                 # GraphQL API
│       ├── CScenarioServiceControllerComp.h/.cpp
│       ├── CExecutionServiceControllerComp.h/.cpp
│       └── imtvtestgql.h
├── Sdl/imtvtest/1.0/                # SDL schemas
│   ├── Scenario.sdl
│   ├── Execution.sdl
│   ├── Baseline.sdl
│   ├── SemanticModel.sdl
│   └── AppMap.sdl
├── Qml/imtvtestgui/                 # QML UI
│   ├── GraphEditor/
│   ├── Timeline/
│   ├── DiffViewer/
│   ├── AppMap/
│   ├── ScreenshotViewer/
│   └── qmldir
├── Tools/
│   ├── Browser/                     # Playwright controller
│   ├── AI/                          # AI intent resolver
│   ├── Semantic/                    # Semantic model builder
│   ├── Exploration/                 # Auto-exploration agent
│   └── CLI/                         # CLI runner
├── Impl/                            # Implementation packages (Pck)
├── Partitura/                       # ACF component wiring (.acc/.arp)
├── Tests/
│   ├── ScenarioEngineTest/
│   ├── ImageComparisonTest/
│   ├── SemanticModelTest/
│   └── Docker/
├── Config/
├── conanfile.py
└── CMakeLists.txt
```

---

## Компоненты системы

### 1. Scenario Engine (C++ / ACF)

**Модель данных (SDL):**

```graphql
schema {
    namespace: imtvtest
    version: 1.0
}

enum NodeType {
    OpenUrl
    ClickElement
    InputText
    Wait
    TakeScreenshot
    CompareScreenshot
    AssertText
    AssertElement
    AiValidation
    ConditionalBranch
    Loop
}

enum ExecutionStatus {
    Pending
    Running
    Passed
    Failed
    Skipped
    TimedOut
}

type ScenarioNode {
    id: String!
    type: NodeType!
    label: String!
    description: String
    config: String
    posX: Float
    posY: Float
}

type ScenarioEdge {
    id: String!
    sourceNodeId: String!
    targetNodeId: String!
    condition: String
    label: String
}

type Scenario {
    id: String!
    name: String!
    description: String
    nodes: [ScenarioNode]!
    edges: [ScenarioEdge]!
    createdAt: String!
    updatedAt: String!
}
```

**C++ компоненты (ACF pattern):**
- `IScenarioGraph` — интерфейс графа сценария
- `CScenarioGraphComp` — реализация ориентированного графа (DAG с условными переходами)
- `IScenarioExecutor` — интерфейс выполнения сценария
- `CScenarioExecutorComp` — последовательное выполнение узлов по графу
- `INodeAction` — базовый интерфейс действия узла
- `CNodeActionFactory` — фабрика действий по типу узла

### 2. Browser Controller

**Playwright-based (Node.js/Python):**
- Управление браузером через WebSocket (JSON-RPC)
- Навигация, клики, ввод текста
- Захват скриншотов (full-page, element)
- Получение DOM и accessibility tree
- Ожидание состояний

**Протокол связи:**
```
C++ Engine ←→ WebSocket (JSON-RPC) ←→ Playwright Controller
```

**Команды:** `navigate`, `click`, `type`, `screenshot`, `getDom`, `getAccessibilityTree`, `waitFor`

### 3. Screenshot Service & Visual Comparison

**Хранение:**
- PNG (lossless) для baselines
- JPEG для превью
- Организация: `{scenarioId}/{executionId}/{nodeId}/{timestamp}.png`
- Метаданные в imtmdbx

**Алгоритмы сравнения:**
- Pixel-by-pixel diff с настраиваемым порогом
- Perceptual diff (anti-aliasing tolerance)
- Region-based comparison (ignore dynamic areas)
- Layout shift detection (bounding boxes)
- Structural similarity (SSIM)

**C++ компоненты:**
- `IImageComparator` — интерфейс
- `CPixelDiffComparatorComp` — попиксельное сравнение
- `CStructuralDiffComparatorComp` — структурное (SSIM)
- `CLayoutDiffComparatorComp` — layout сравнение
- `CDiffReportGeneratorComp` — визуальный отчёт различий

### 4. AI Agent

**Intent Resolution:**
- Преобразование текстового описания в действие
- Input: текст + DOM + accessibility tree + screenshot + история
- Output: selector + coordinates + action type
- Fallback цепочка: accessibility → text content → visual → CSS selector

**AI Validation:**
- Natural language assertions:
  - "На экране должен отображаться список пользователей"
  - "Диалог должен содержать поля Name и Email"
  - "Таблица содержит 5 строк"
- Input: screenshot + semantic snapshot + assertion text
- Output: pass/fail + confidence + explanation

**AI Provider Abstraction:**
- `IAiProvider` — интерфейс
- Реализации: OpenAI GPT-4V, Anthropic Claude, локальные модели
- Rate limiting, кэширование, retry с exponential backoff

### 5. Semantic UI Model

**Сущности:**
- `SemanticPage` — страница (url, title, elements, layout)
- `SemanticDialog`, `SemanticForm`, `SemanticButton`, `SemanticTable`
- `SemanticTreeView`, `SemanticInput`, `SemanticMenu`, `SemanticNotification`

**Пример:**
```
Users Page
├─ Users Table
├─ Create User Button
├─ Search Input
└─ Pagination
```

**Builder:**
- Парсинг ARIA-ролей и атрибутов
- Распознавание UI-паттернов (table + thead + tbody = таблица)
- Иерархия: Page → Section → Component → Element

### 6. Application Map

**Автоматическое построение карты экранов:**

```
Login
  ↓
Dashboard
├─ Users
├─ Reports
├─ Settings
└─ Audit Log
```

Для каждого экрана:
- Скриншоты
- Доступные действия
- Переходы
- Найденные элементы управления

**Builder:**
- Отслеживание навигации при выполнении тестов
- Обнаружение новых экранов (URL change, major DOM change)
- Merge результатов из разных прогонов

### 7. Execution Engine

- Оркестрация выполнения сценария
- Параллельное выполнение независимых веток
- Retry logic для flaky шагов
- Timeout management
- Conditional branching / Loop execution

**Для каждого шага сохраняется:**
- Скриншот
- DOM-состояние
- Semantic snapshot
- Результат выполнения
- Время выполнения

### 8. Autonomous Exploration

Пользователь задаёт цель: "Исследовать модуль Users"

AI:
- Открывает страницы
- Строит карту экранов
- Определяет доступные действия
- Выявляет формы
- Генерирует тестовые сценарии
- Формирует отчёт

---

## UI Компоненты (QML + imtcontrols)

### Graph Editor
- `GraphEditor.qml` — основной редактор (canvas + toolbar + properties)
- `GraphCanvas.qml` — canvas (drag & drop, zoom, pan)
- `GraphNode.qml` — визуальный узел (иконка + текст + порты)
- `GraphEdge.qml` — стрелка (bezier curve)
- `NodePalette.qml` — палитра типов узлов
- `NodeProperties.qml` — панель свойств
- Auto-layout, minimap, undo/redo

### Execution Timeline
- `ExecutionTimeline.qml` — горизонтальная временная шкала
- `TimelineStep.qml` — шаг (цвет по статусу)
- `StepDetailPanel.qml` — детали (screenshot, DOM, время)

### Diff Viewer
- `SideBySideView.qml` — два изображения рядом
- `OverlayView.qml` — наложение с прозрачностью
- `SliderView.qml` — slider переключения baseline/actual
- `HighlightView.qml` — подсветка различий

### App Map Viewer
- `AppMapViewer.qml` — визуализация карты как графа
- `ScreenPreview.qml` — миниатюра при наведении
- `TransitionLabel.qml` — подпись действия на ребре

---

## Visual Baselines

Базовые изображения для **каждого шага отдельно**:
- Baseline screenshot
- Semantic snapshot
- Layout metadata

При изменении интерфейса система определяет:
- Изменился внешний вид
- Изменился текст
- Изменилось расположение элементов
- Изменилась логика интерфейса

---

## GraphQL API

### Сценарии
- `listScenarios` / `getScenario(id)`
- `createScenario` / `updateScenario` / `deleteScenario`
- `duplicateScenario`
- `exportScenario` / `importScenario`

### Выполнение
- `executeScenario(scenarioId, config)` → ExecutionId
- `getExecution(executionId)` → Execution с результатами
- `getNodeResult(executionId, nodeId)` → NodeExecutionResult
- `stopExecution(executionId)`
- `retryExecution(executionId, fromNodeId)`
- Subscriptions: `onExecutionProgress`, `onNodeCompleted`

### AI
- `aiResolveIntent(description, context)` — резолв намерения
- `aiValidate(screenshot, assertion)` — AI-валидация
- `aiSuggestActions(currentState)` — предложение действий

---

## Технологический стек

| Компонент | Технология | Обоснование |
|-----------|-----------|-------------|
| Core Engine | C++ / Qt / ACF | База ImtCore |
| Browser Automation | Playwright (Node.js) | Лучший в классе |
| AI Provider | OpenAI / Claude API | Multi-modal для screenshots |
| GraphQL API | imtgql стек | Совместимость с ImtCore |
| Schema | SDL + кодогенерация | Паттерн ImtCore |
| UI | QML + imtcontrols | Переиспользование 160+ компонентов |
| Web UI | JQML v3 | Кросс-платформенность |
| Storage | imtmdbx + filesystem | Быстрый доступ + большие файлы |
| Communication | WebSocket (JSON-RPC) | Real-time + bidirectional |
| CI/CD | Docker | Headless execution |

---

## Порядок реализации

| Этап | Фазы | Описание |
|------|-------|----------|
| **MVP-1** | Инфраструктура + Scenario Engine + Browser Controller | Создание проекта, граф сценариев, управление Playwright |
| **MVP-2** | Visual Comparison + Execution Engine + Timeline | Сравнение скриншотов, движок выполнения, UI таймлайн |
| **MVP-3** | Graph Editor | Визуальный редактор графа сценариев |
| **MVP-4** | AI Agent | Intent resolution + AI validation |
| **MVP-5** | Semantic Model + Application Map | Семантическая модель UI + карта приложения |
| **MVP-6** | Autonomous Exploration + Reporting + CI/CD | Автоисследование + отчёты + интеграция |

---

## Риски и митигации

| Риск | Митигация |
|------|-----------|
| AI нестабильность (flaky) | Retry + confidence threshold + fallback к детерминированным методам |
| Производительность comparison | Кэширование + incremental + parallel |
| Сложность graph editor | Итеративно, начать с linear mode |
| Зависимость от AI API | Абстракция провайдера + локальные модели |
| Объём скриншотов | Compression + TTL + lazy loading |
| Совместимость с ImtCore | ACF component pattern + SDL |

---

## Зависимость от ImtCore

Проект подключает ImtCore как зависимость (Conan или IMTCOREDIR):

**Используемые модули:**
- `imtbase` / `imtcore` / `imtcom` — ACF инфраструктура
- `imtgql` / `imtservergql` / `imtclientgql` — GraphQL
- `imtcontrols` / `imtgui` — UI компоненты
- `imthttp` — WebSocket/HTTP коммуникация
- `imtimg` — работа с изображениями
- `imtmdbx` — embedded database
- `imtsdl` + `imtsdlgencpp` + `imtsdlgenqml` — SDL кодогенерация
- `imtfile` — файловая система
- `imttest` — тестовая инфраструктура

**Partitura:** Используется для конфигурации ACF-компонентов (.acc/.arp файлы)
