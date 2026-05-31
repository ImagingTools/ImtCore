# Sentra — AI Visual Testing Platform

## Название продукта

**Sentra** (от лат. *sentire* — воспринимать, чувствовать + *centra* — центр)

Название отражает суть: платформа, которая **воспринимает** интерфейс так, как его видит пользователь, и является **центром** управления визуальным тестированием.

Варианты написания:
- **Sentra** — основное название продукта
- **Sentra VT** — полное (Visual Testing)
- **ImtSentra** — техническое имя пакета/репозитория

---

## Путь использования продукта (Use Flow)

### Обзор

Sentra предоставляет полный цикл визуального AI-тестирования веб-приложений: от создания проекта до интеграции в CI/CD. Ниже описан весь путь использования на уровне абстракций.

---

### 1. Создание проекта (Project)

**Абстракция:** `Project` — корневая сущность, объединяющая всё тестирование одного приложения.

```
Project
├── name: "My Web App"
├── baseUrl: "https://staging.myapp.com"
├── environments: [staging, production, local]
├── credentials: [admin, user, guest]  (зашифрованные)
├── settings: { viewport, timeout, locale, ... }
├── scenarios: [...]
├── baselines: [...]
├── executions: [...]
└── appMap: AppMap
```

**Действия пользователя:**
1. Создаёт проект, указывая URL целевого приложения
2. Настраивает окружения (staging, production, local dev)
3. Задаёт учётные данные для авторизации (хранятся зашифрованно)
4. Определяет параметры по умолчанию (viewport, таймауты, язык)

---

### 2. Создание сценария (Scenario Authoring)

**Абстракция:** `Scenario` — ориентированный граф шагов, описывающих пользовательский путь.

**Способы создания сценария:**

| Способ | Описание |
|--------|----------|
| **Визуальный редактор** | Drag & drop узлов на canvas, соединение рёбрами |
| **Запись (Recording)** | Пользователь взаимодействует с приложением, система записывает действия как граф |
| **AI-генерация** | Пользователь описывает цель текстом, AI генерирует сценарий |
| **Импорт** | Загрузка сценария из файла (JSON/YAML) |

**Процесс в визуальном редакторе:**
```
1. Открыть Graph Editor
2. Перетащить узел "OpenUrl" → задать URL
3. Добавить узел "ClickElement" → описать: "Кнопка Login"
4. Добавить узел "InputText" → описать: "Поле Email", текст: "admin@test.com"
5. Добавить узел "InputText" → описать: "Поле Password", текст: "***"
6. Добавить узел "ClickElement" → описать: "Кнопка Submit"
7. Добавить узел "TakeScreenshot" → имя: "after_login"
8. Добавить узел "CompareScreenshot" → baseline: "after_login"
9. Соединить все узлы рёбрами в последовательность
```

**Процесс записи (Recording):**
```
1. Нажать "Record"
2. Система открывает браузер с целевым приложением
3. Пользователь выполняет действия в браузере
4. Каждое действие автоматически становится узлом графа
5. Система делает скриншот после каждого действия
6. Пользователь останавливает запись
7. Сценарий доступен для редактирования в Graph Editor
```

---

### 3. Конфигурация узлов (Node Configuration)

**Абстракция:** Каждый узел имеет `intent` (намерение) и опциональный `config` (детали).

Пользователь описывает **что** нужно сделать, а не **как**:

```
Узел: ClickElement
├── intent: "Нажать кнопку создания нового пользователя"
├── screenshot_before: (автоматически)
├── ai_hints: ["Обычно находится в правом верхнем углу", "Текст: 'Create' или '+'"]
└── fallback_selector: null  (опционально, для стабильности)
```

**Система при выполнении:**
1. Получает текущий DOM + accessibility tree + скриншот
2. AI определяет целевой элемент по описанию
3. Выполняет действие
4. Делает скриншот результата
5. Валидирует успешность

---

### 4. Выполнение сценария (Execution)

**Абстракция:** `Execution` — один прогон сценария с полным контекстом.

```
Execution
├── id: "exec-001"
├── scenario: Scenario
├── environment: "staging"
├── status: Running | Passed | Failed
├── startedAt / completedAt
├── nodeResults: [NodeResult, ...]
├── artifacts: [screenshots, dom_snapshots, logs]
└── report: ExecutionReport
```

**Процесс выполнения:**
```
1. Пользователь запускает сценарий (вручную / по расписанию / из CI)
2. Система создаёт Execution
3. Открывает браузер (headless или visible)
4. Для каждого узла графа (в порядке обхода):
   a. AI резолвит intent → конкретное действие
   b. Выполняет действие в браузере
   c. Сохраняет screenshot + DOM + accessibility snapshot
   d. Проверяет условия (assertions)
   e. Записывает NodeResult (pass/fail/skip + время + артефакты)
   f. Переходит к следующему узлу (или по условию)
5. По завершении формируется ExecutionReport
6. Отправляется уведомление (webhook / email / UI)
```

**Режимы выполнения:**

| Режим | Описание |
|-------|----------|
| **Single** | Один прогон по требованию |
| **Scheduled** | По расписанию (cron) |
| **CI-triggered** | При событии в CI (push, PR, deploy) |
| **Parallel** | Несколько сценариев одновременно |
| **Multi-env** | Один сценарий в нескольких окружениях |

---

### 5. Хранение данных (Data Storage)

**Абстракция:** Многоуровневая система хранения, оптимизированная под разные типы данных.

```
Storage Architecture
├── Metadata Store (imtmdbx)
│   ├── Projects
│   ├── Scenarios (граф: узлы + рёбра)
│   ├── Executions (результаты, статусы)
│   ├── Baselines (ссылки на файлы + метаданные)
│   ├── AppMap (граф экранов)
│   └── SemanticModel (модель UI)
│
├── Artifact Storage (filesystem)
│   ├── screenshots/
│   │   ├── baselines/{scenarioId}/{nodeId}/baseline.png
│   │   ├── executions/{executionId}/{nodeId}/actual.png
│   │   └── diffs/{executionId}/{nodeId}/diff.png
│   ├── dom_snapshots/{executionId}/{nodeId}/dom.json
│   └── reports/{executionId}/report.html
│
├── Configuration Store (imtmdbx)
│   ├── Environment configs
│   ├── Credentials (encrypted)
│   └── AI provider settings
│
└── Cache (in-memory + imtmdbx)
    ├── AI response cache
    ├── Semantic model cache
    └── DOM element resolution cache
```

**Жизненный цикл данных:**

| Данные | Retention | Политика |
|--------|-----------|----------|
| Baselines | Постоянно | Обновляются при approve |
| Execution results | Configurable TTL | Последние N / за период |
| Screenshots (actual) | Привязан к execution | Удаляются с execution |
| DOM snapshots | Привязан к execution | Удаляются с execution |
| App Map | Постоянно | Обновляется при каждом прогоне |
| AI Cache | TTL | Автоочистка |

**Синхронизация и экспорт:**
- Сценарии экспортируются в JSON/YAML (version control friendly)
- Baselines могут храниться в Git LFS
- Результаты доступны через GraphQL API
- Артефакты доступны по прямым URL

---

### 6. Управление базовыми изображениями (Baseline Management)

**Абстракция:** `Baseline` — эталонное состояние для каждого узла сценария.

**Процесс:**
```
1. Первый запуск сценария → все скриншоты автоматически становятся baselines
2. Последующие запуски → сравнение actual vs baseline
3. При обнаружении различий:
   a. Различие показывается в Diff Viewer
   b. Пользователь решает:
      - "Accept" → новый скриншот становится baseline
      - "Reject" → тест считается failed
      - "Ignore region" → добавляет область игнорирования
4. Baselines версионируются (история изменений)
```

**Стратегии обновления baselines:**

| Стратегия | Описание |
|-----------|----------|
| **Manual** | Каждое изменение требует ручного approve |
| **Auto-accept on branch** | На feature-ветках auto-accept, на main — manual |
| **Threshold** | Различия ниже порога auto-accept |
| **AI-assisted** | AI определяет: "ожидаемое изменение" vs "баг" |

---

### 7. Анализ результатов (Results & Reporting)

**Абстракция:** `ExecutionReport` — полный отчёт о прогоне.

**Что видит пользователь после выполнения:**
```
Execution Report
├── Summary: 12/15 steps passed, 2 failed, 1 skipped
├── Duration: 45s
├── Timeline: визуальная шкала шагов с цветами
├── Failed Steps:
│   ├── Step 7: "Compare Screenshot — Dashboard"
│   │   ├── Expected: baseline_dashboard.png
│   │   ├── Actual: actual_dashboard.png
│   │   ├── Diff: highlighted differences
│   │   └── Diff %: 3.2%
│   └── Step 11: "Assert Element — User Table"
│       └── Error: "Элемент не найден в течение 10s"
├── Screenshots: полная лента скриншотов по шагам
├── Performance: время каждого шага
└── AI Insights: "Изменился header — новый баннер добавлен"
```

**Форматы отчётов:**
- Интерактивный (в UI — Timeline + Diff Viewer)
- HTML (для CI/CD артефактов)
- JSON (для программной обработки)
- Markdown (для PR-комментариев)

---

### 8. Карта приложения (Application Map)

**Абстракция:** `AppMap` — автоматически построенный граф экранов приложения.

**Как формируется:**
```
1. При каждом выполнении сценария система отслеживает навигацию
2. Новые URL / major DOM changes → новый экран на карте
3. Действия, приводящие к переходу → рёбра между экранами
4. Со временем карта обогащается и уточняется
```

**Использование карты:**
- Визуализация покрытия: "какие экраны протестированы, какие нет"
- Навигация: "быстро перейти к сценариям конкретного экрана"
- Регрессия: "этот экран изменился — какие сценарии затронуты?"
- Планирование: "для полного покрытия не хватает сценариев на эти экраны"

---

### 9. Интеграция в CI/CD (CI/CD Integration)

**Абстракция:** Sentra как шаг в пайплайне.

**Сценарий использования в CI:**
```yaml
# Абстрактный пайплайн
steps:
  - deploy: staging
  - sentra:
      project: "my-web-app"
      scenarios: ["login-flow", "user-crud", "dashboard-visual"]
      environment: staging
      on_failure: block_merge
      report: pr_comment
```

**Модели интеграции:**

| Модель | Описание |
|--------|----------|
| **CLI** | `sentra run --project X --scenario Y` |
| **Docker** | Контейнер с headless browser + Sentra engine |
| **API** | HTTP/GraphQL вызов из любого CI |
| **Webhook** | Sentra слушает deploy-events и запускается автоматически |

**Результат в CI:**
- Exit code 0/1 (pass/fail)
- Артефакты: HTML-отчёт, скриншоты, диффы
- PR-комментарий с результатами и диффами
- Блокировка merge при критических различиях

---

### 10. Многопользовательская работа (Collaboration)

**Абстракция:** Совместная работа команды над тестовыми сценариями.

```
Team Workflow
├── Shared Project — все видят одни сценарии и результаты
├── Scenario Ownership — каждый сценарий имеет автора/ответственного
├── Baseline Approval — approve новых baselines требует ревью
├── Notifications — уведомления о failures нужным людям
├── History — кто что менял, когда, почему
└── Roles — viewer / editor / admin
```

---

### 11. Полный пользовательский путь (End-to-End Flow)

```
┌─────────────────────────────────────────────────────────────────┐
│                     SENTRA — User Journey                        │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  1. CREATE PROJECT                                               │
│     └── Set URL, envs, credentials                              │
│                        ↓                                         │
│  2. AUTHOR SCENARIOS                                            │
│     ├── Visual Editor (drag & drop)                             │
│     ├── Recording (interact with app)                           │
│     └── AI Generation (describe goal)                           │
│                        ↓                                         │
│  3. FIRST RUN                                                   │
│     └── Establishes baselines for all steps                     │
│                        ↓                                         │
│  4. ITERATE                                                     │
│     ├── Run on demand / scheduled / CI                          │
│     ├── Review diffs                                            │
│     ├── Accept or reject changes                                │
│     └── Refine scenarios                                        │
│                        ↓                                         │
│  5. INTEGRATE                                                   │
│     ├── Add to CI/CD pipeline                                   │
│     ├── Block merges on failure                                 │
│     └── Auto-report to PRs                                      │
│                        ↓                                         │
│  6. SCALE                                                       │
│     ├── App Map shows coverage                                  │
│     ├── AI suggests new scenarios                               │
│     ├── Autonomous exploration finds gaps                       │
│     └── Team collaboration on scenarios                         │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

---

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
ImtSentra/                           # Отдельный репозиторий
├── Include/
│   ├── imtsentra/                    # Core library
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
│   │   └── imtsentra.h
│   └── imtsentragql/                 # GraphQL API
│       ├── CScenarioServiceControllerComp.h/.cpp
│       ├── CExecutionServiceControllerComp.h/.cpp
│       └── imtsentragql.h
├── Sdl/imtsentra/1.0/                # SDL schemas
│   ├── Scenario.sdl
│   ├── Execution.sdl
│   ├── Baseline.sdl
│   ├── SemanticModel.sdl
│   └── AppMap.sdl
├── Qml/imtsentragui/                 # QML UI
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
    namespace: imtsentra
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
