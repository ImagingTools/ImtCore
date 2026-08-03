# Структура QML-модулей ImtCore — руководство по размещению

Этот документ определяет порядок размещения нового QML-компонента в структуре
ImtCore. Сначала выбирается **модуль**, затем **подпапка** внутри модуля. См.
также [README.md](../README.md) (обзор и навигация), [STRUCTURE-app.md](STRUCTURE-app.md)
(каталог прикладных модулей) и [STRUCTURE-base.md](STRUCTURE-base.md)
(каталог базовых модулей).

---

## Модули

| Модуль | Назначение |
|---|---|
| **`imtqmlutils`** | Невизуальные утилиты общего назначения: математика и геометрия, валидаторы, генераторы (UUID), модельные помощники (сортировка и фильтрация, конечный автомат), общие функции, дизайн-схема, шина событий. |
| **`imtstyle`** | Стили и оформление: базовые стили и наборы свойств шрифта/иконок (`StyleBase`, `StyleComponents`, `FontProperties`, `IconProperties`). |
| **`imtcontrols`** | Базовые переиспользуемые контролы и примитивы: кнопки, поля ввода, диалоги, таблицы/деревья, меню, всплывающие окна, индикаторы, а также базовые классы. |
| **`imtgraphics2d`** | 2D-графика на HTML5 Canvas: контейнер сцены, слои, фигуры и их параметры, графики (Graph2d/MultiGraph2d), матрицы. |
| **`imtgui`** | Каркас прикладного интерфейса поверх imtcontrols: оболочка приложения, панели, редакторы параметров (Params), настройки, представления и элементы форм (ElementView). |
| **`imtguigql`** | Инфраструктура поверх GraphQL/SDL: запросы, подписки, WebSocket, GraphQL-версии контроллеров, провайдеров и сервисов, удалённые коллекции, выбор серверных путей. |
| **`imtauthgui`** | Аутентификация, пользователи, роли, группы, права, организации (tenant), профиль. |
| **`imtcolgui`** | Обобщённые коллекции и представления коллекций, фильтры, метаданные. |
| **`imtdocgui`** | Документо-ориентированные рабочие области, история и ревизии, undo/redo. |
| **`imtdeskgui`** | Служба поддержки (тикеты). |
| **`imtchatgui`** | Чат и обмен сообщениями. |
| **`imtlicgui`** | Лицензирование (продукты, функции, лицензии). |
| **`imtgeogui`** | Адреса и географические структуры. |
| **`imt3dgui`** | 3D-сцены, обёртки над Qt Quick 3D. |

**Правило доменности:** если компонент относится к конкретной предметной области
(пользователи, документы, лицензии, чат и т. п.), его следует размещать в
соответствующем доменном модуле. Если компонент **общего назначения** и не
привязан к предметной области, его следует размещать в базовом модуле
(`imtcontrols` / `imtqmlutils` / `imtgraphics2d` / `imtgui`).

---

## Выбрать подпапки модулей (по роли)

Структура подпапок единообразна во всех модулях. При выборе папки следует
ориентироваться на **роль** компонента, которая обычно определяется по суффиксу
имени:

| Папка | Назначение | Типичные имена / суффиксы |
|---|---|---|
| `Pages/` | Экранные страницы верхнего уровня и контейнеры страниц. | `*Page`, `*PageView` |
| `Views/` (в `imtgui` — `View/`) | Визуальные представления данных, панели-виджеты, бейджи, карточки, баннеры. | `*View`, `*ViewBase`, `*Badge`, `*Card`, `*Banner` |
| `Panels/` | Панели (боковые, списочные, нижние области). | `*Panel` |
| `Dialogs/` | Модальные диалоги и всплывающие окна. | `*Dialog`, `*Popup` |
| `Editors/` | Формы создания/редактирования сущностей. | `*Editor`, `*EditorShell` |
| `Inputs/` | Элементы ввода. | `*Input`, `ComboBox*`, `*Picker`, `*Field`, `*Bar` (поиск) |
| `Delegates/` | Делегаты элементов списков и таблиц. | `*Delegate`, `*DelegateBase` |
| `Controllers/` | Невизуальная координация и логика. | `*Controller`, `*Manager`, `*StateManager`, `*Observer` |
| `Providers/` | Поставщики данных. | `*Provider`, `*DataProvider` |
| `Services/` | Сервисы над сущностью и жизненным циклом. | `*Service`, `*ServiceBase` |
| `Api/` | Сетевой слой: запросы, клиенты, сокеты, подписки, прокси. | `*ApiClient`, `*Client`, `*Sender`, `*Proxy`, `*Request`, `*Listener`, `*Checker`, `*Subscription*` |
| `Models/` | Модели, кэши, фильтры, валидаторы, структуры, регистраторы. | `Cached*`, `*Model`, `*Filter`, `*Validator`, `*Registrar`, `*Structure` |
| `Base/` | Базовые классы для наследования, перечисления, JS-хелперы типов. | `*Base`, `ControlBase`, `DecoratorBase`, `Enums`, `Icon.js` |
| `Params/` | Редакторы/просмотр параметров (в `imtgui`). | `*ParamEditor`, `*ParamView`, `*ParamController` |
| `Settings/` | Настройки приложения (в `imtgui`). | `Settings*`, `Preference*` |
| `Actions/`, `Buttons/`, `Menu/`, `Popup/` | Специализированные группы контролов в `imtcontrols`. | действия и жесты, кнопки, меню, всплывающие элементы |

> **Оформление (стили) — НЕ подпапка, а отдельный модуль [`imtstyle`](STRUCTURE-base.md#imtstyle).** Подпапка `Style/` или `Styles/` внутри любого модуля **запрещена**.

### 2D-графика (`imtgraphics2d`)
| Папка | Назначение |
|---|---|
| `Core/` | Контейнер сцены (`GraphicsView`), слои, фабрики и базовые классы фигур, матрицы, `BoundingBox`. |
| `Graphs/` | Графики: `Graph2d`, `MultiGraph2d`. |
| `Shapes/` | Конкретные фигуры (`*Shape`). |
| `Params/` | Параметры фигур (`*Params`). |

### Утилиты (`imtqmlutils`)
| Папка | Назначение |
|---|---|
| `Math/` | Геометрия и линейная алгебра. |
| `Validators/` | Валидаторы ввода (`*Validator`). |
| `Models/` | Модельные помощники (сортировка и фильтрация, конечный автомат, построитель деревьев, `jsonpath.js`). |
| `Core/` | Общие функции, дизайн-схема, события (`Functions`, `DesignScheme`, `Events`, `EventManager`). |
| `Web/` | JS-реализации для веб-сборки (JQML): `GraphQLRequest.js`, `FileUploader.js` и т. п. |

### 3D (`imt3dgui`)
Предметная группировка по Qt Quick 3D: `Scene/`, `Cameras/`, `Lights/`,
`Materials/`, `Textures/`, `Environment/`, `Geometry/`, `Instancing/`,
`Skeleton/`, `Particles/`, `Effects/`, `Helpers/`.

---

## Быстрый обзор по суффиксу имени

| Назначение | Папка |
|---|---|
| `Page`, `PageView` | `Pages/` |
| `Dialog`, `Popup` | `Dialogs/` |
| `Editor` | `Editors/` |
| `Delegate` | `Delegates/` |
| `Controller`, `Manager`, `Observer` | `Controllers/` |
| `Provider` | `Providers/` |
| `Service` | `Services/` |
| `ApiClient`, `Client`, `Sender`, `Proxy`, `Request`, `Listener`, `Checker` | `Api/` |
| `Model`, `Filter`, `Validator`, `Structure`, `Registrar`, `Cached…` | `Models/` |
| `Panel` | `Panels/` |
| `Input`, `Picker`, `ComboBox…`, `Field` | `Inputs/` |
| `Style…`, `…Properties` | модуль **`imtstyle`** (не подпапка!) |
| `Base` (базовый класс) | `Base/` |
| `View`, `ViewBase`, `Badge`, `Card`, `Banner` | `Views/` (`View/` в imtgui) |
| `Shape` | `Shapes/` (imtgraphics2d) |
| `Params` | `Params/` (imtgraphics2d) |

> Если компонент может быть отнесён к нескольким ролям, выбирайте папку по
> **основному назначению** компонента, а не по второстепенным признакам.
> Например, `UserCollectionView` следует относить к представлениям (`Views/`),
> а не к моделям, хотя он и работает с коллекцией.

---

## Инвентарь папок по модулям (актуальный)

| Модуль | Подпапки |
|---|---|
| `imtcontrols` | Actions, Base (+`Base/web`), Buttons, Dialogs, Inputs, Menu, Models, Popup, Views |
| `imtstyle` | *(плоский модуль)* StyleBase, StyleComponents, FontProperties, IconProperties |
| `imtqmlutils` | Core, Math, Models, Validators, Web |
| `imtgraphics2d` | Core, Graphs, Params, Shapes |
| `imtgui` | Application, Panels, Params, Settings, View |
| `imtauthgui` | Api, Controllers, Delegates, Dialogs, Editors, Models, Pages, Providers, Views |
| `imtcolgui` | Controllers, Delegates, Dialogs, Models, Pages, Providers, Views |
| `imtdocgui` | Controllers, Delegates, Dialogs, Models, Pages, Panels, Providers, Services, Views |
| `imtdeskgui` | Dialogs, Editors, Pages, Panels, Views |
| `imtchatgui` | Dialogs, Editors, Inputs, Panels, Views |
| `imtlicgui` | Delegates, Dialogs, Editors, Models, Providers, Views |
| `imtgeogui` | Editors, Views |
| `imtguigql` | Api, Base, Controllers, Dialogs, Editors, Inputs, Pages, Providers, Services, Views |
| `imt3dgui` | Cameras, Effects, Environment, Geometry, Helpers, Instancing, Lights, Materials, Particles, Scene, Skeleton, Textures |

Если нужной папки нет, создайте её, придерживаясь приведённого выше ролевого
словаря. Папки с одинаковой ролью должны называться единообразно во всех
модулях.

---

## Особые случаи

- **Стили — отдельный модуль, а НЕ подпапка.** Любой компонент оформления
  (стили, наборы свойств шрифта/иконок) размещается в модуле **`imtstyle`**.
  Подпапка `Style/` или `Styles/` внутри любого модуля **запрещена**.
- **Синглтоны** (`pragma Singleton`) размещаются по своей роли (например,
   singleton-провайдер — в `Providers/`) и помечаются словом `singleton` в `qmldir`.
- **`imtcontrols/Base/web/`** — веб-специфичные переопределения контролов для
   JQML-сборки (`RoundButton`, `ScrollBar`, `Switch`, `ToolTip`, `Window`). Это
   намеренная инфраструктура веб-сборки — **не изменяйте её в рамках обычной
   разработки**. Новые desktop-контролы следует размещать в стандартных папках
   (`Buttons/`, `Inputs/` и т. д.).
- **`imtqmlutils/Web/`** — общие JS-реализации, используемые веб-сборкой; на них
  ссылаются из `qmldir` относительными путями.
- **Утилита или контрол?** Если компонент **невизуальный** и общего назначения
   (математика, валидатор, модель-помощник, событие), его следует размещать в
   `imtqmlutils`, а не в `imtcontrols`.

---

## Чек-лист добавления компонента

1. **Модуль** — по таблице «Шаг 1».
2. **Подпапка** — по роли или суффиксу («Шаг 2»).
3. Создать файл `<Модуль>/<Папка>/<Имя>.qml`.
4. Зарегистрировать тип в `<Модуль>/qmldir`:
   `[singleton] <Имя> 1.0 <Папка>/<Имя>.qml`
5. Добавить в ресурс `<Модуль>/<модуль>qml.qrc`:
   `<file alias="<модуль>/<Папка>/<Имя>.qml"><Папка>/<Имя>.qml</file>`
6. Если используются типы **другого** модуля, добавить `import <модуль> 1.0`
   (в пределах собственного модуля типы доступны по имени без импорта).
7. Проверить сборку ресурса: `rcc <модуль>qml.qrc -o /dev/null` (все пути должны
   корректно разрешаться).

### Если создаёте новый модуль
Дополнительно: `CMake/CMakeLists.txt` и `QMake/<модуль>qml.pro` по образцу
соседнего модуля; регистрация в `ImtCore/Build/CMake/CMakeLists.txt`
(`add_subdirectory("${QML_DIR}/<модуль>/CMake" …)`); при необходимости — добавить
модуль в веб-манифест `Qml/web/imtcore.json` и в `-qmldir` конфигураций
развёртывания продуктов.
