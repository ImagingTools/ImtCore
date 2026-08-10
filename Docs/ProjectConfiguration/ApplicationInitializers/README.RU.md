# Инициализаторы приложения ImtCore

## 1. Назначение

Документ описывает модульную архитектуру инициализации в ImtCore, введенную взамен устаревшей монолитной статической инициализации.

Новый подход основан на:

- `imtcore::IApplicationInitializer` как общем контракте
- небольших инициализаторах с одной ответственностью
- композиции через `imtcore::CCascadedApplicationInitializer`

Это улучшает поддерживаемость, тестопригодность и повторное использование логики старта.

## 2. Цель

Старая статическая инициализация смешивала в одном месте:

- локализацию
- UI-ресурсы
- ресурсы тем
- QML-ресурсы
- SDL-схемы/ресурсы
- настройку стиля

Новая структура разделяет эти зоны ответственности и позволяет настраивать точные профили запуска.

## 3. Базовые контракты

### 3.1 Базовый интерфейс

- `imtcore::IApplicationInitializer`
  - Метод: `void Initialize()`

### 3.2 Композиция

- `imtcore::CCascadedApplicationInitializer`
  - Хранит и выполняет упорядоченную цепочку инициализаторов
  - Владеет переданными инициализаторами (ownership transfer)

### 3.3 Интеграция с runner

- `imtcore::CApplicationRunner::Run(..., IApplicationInitializer& initializer)`
  - Вызывает `initializer.Initialize()` перед запуском приложения

## 4. Слои инициализации

### 4.1 Атомарные инициализаторы (одна ответственность)

Что это:
Минимальные инициализаторы, каждый из которых выполняет одну техническую операцию или инициализирует один узкий набор ресурсов.

Что включает:
- Ровно одну зону ответственности: локализация, конкретный набор ресурсов, конкретный QML-набор, отдельная тема, отдельная группа SDL, отдельный шаг стиля.

Когда включать в приложениях:
- Когда нужен очень точный и минимальный профиль запуска.
- Когда вы строите собственный специализированный pipeline инициализации.
- Когда важно строго управлять порядком инициализации зависимых подсистем.

Примеры:

- Локализация:
  - `CAcfLocInitializer`
  - `CImtCoreLocalizationInitializer`
- Ресурсы:
  - `CImtCoreAuthResourcesInitializer`
  - `CImtCoreLicResourcesInitializer`
  - `CImtCoreDeskResourcesInitializer`
  - `CImtCoreBaseResourcesInitializer`
  - `CImtCoreStyleResourcesInitializer`
- QML:
  - `CImtCoreBaseQmlInitializer`
  - `CImtCoreGuiQmlInitializer`
  - `CImtCoreColGuiQmlInitializer`
  - `CImtCoreDocGuiQmlInitializer`
  - `CImtCoreGuiGqlQmlInitializer`
  - `CImtCoreDeskQmlInitializer` (композиционный над desk QML атомарными инициализаторами)
  - `CImtCoreAuthQmlInitializer`
  - `CImtCoreLicQmlInitializer`
  - `CImtCoreGeoQmlInitializer`
- Темы:
  - `CImtCoreGuiThemeInitializer`
  - `CImtCoreAuthThemeInitializer`
- SDL:
  - `CImtCoreBaseSdlInitializer`
  - `CImtCoreAuthSdlInitializer`
  - `CImtCoreDomainSdlInitializer`
- Настройка стиля:
  - `CImtCoreStyleTypeInitializer`
  - `CImtCoreFusionBaseStyleInitializer`
  - `CImtCoreLightThemePropertyInitializer`

#### 4.1.1 Подробно по атомарным инициализаторам

Ниже приведен практический каталог атомарных инициализаторов: что делает каждый класс и когда его стоит подключать напрямую.

| Класс | Что инициализирует | Когда подключать |
| --- | --- | --- |
| `CAcfLocInitializer` | Локализацию ACF (`AcfLoc`, `AcfSlnLoc`) | Когда приложению нужны базовые переводы ACF, даже без остальных модулей ImtCore |
| `CImtCoreLocalizationInitializer` | Локализацию ImtCore + базовую локализацию ACF | Когда нужен полный базовый слой локализации для ImtCore-UI |
| `CImtCoreAuthResourcesInitializer` | Auth ресурсы и auth UI resource-паки | Для auth-экранов/сценариев без подключения всего приложения |
| `CImtCoreLicResourcesInitializer` | Lic resource-паки | Для лицензирования и экранов лицензий |
| `CImtCoreDeskResourcesInitializer` | Desk/chat/db ресурсы и основные GUI ресурсы | Для desktop-функциональности и рабочих экранов |
| `CImtCoreBaseResourcesInitializer` | Базовые core-ресурсы (`imtbase`) | Для инфраструктурных сценариев и базовых сервисов |
| `CImtCoreStyleResourcesInitializer` | Ресурсы стиля (style + style variants) | Когда нужно оформление без полного доменного старта |
| `CImtCoreBaseQmlInitializer` | Базовый QML модуль (`imtcontrolsqml`) | Для минимального QML-фундамента |
| `CImtCoreGuiQmlInitializer` | Базовый desk GUI QML-модуль (`imtguiqml`) | Для базового GUI-слоя рабочего пространства |
| `CImtCoreColGuiQmlInitializer` | QML-модуль коллекций (`imtcolguiqml`) | Для UI-функциональности, связанной с коллекциями |
| `CImtCoreDocGuiQmlInitializer` | QML-модуль документов (`imtdocguiqml`) | Для UI-сценариев, ориентированных на документы |
| `CImtCoreGuiGqlQmlInitializer` | GUI GQL QML-модуль (`imtguigqlqml`) | Для интеграции GUI с GraphQL/QML-слоем |
| `CImtCoreDeskQmlInitializer` | Композиционный desk QML-инициализатор над четырьмя desk атомарными QML-инициализаторами выше | Когда нужен полный desk QML-слой без ручной сборки |
| `CImtCoreAuthQmlInitializer` | Auth QML модуль (`imtauthguiqml`) | Для auth UI |
| `CImtCoreLicQmlInitializer` | Lic QML модуль (`imtlicguiqml`) | Для экранов лицензирования |
| `CImtCoreGeoQmlInitializer` | Geo QML модуль (`imtgeoguiqml`) | Для гео-функциональности, подключаемой отдельно |
| `CImtCoreGuiThemeInitializer` | GUI theme ресурсы (`imtguiTheme`) | Когда desktop/UI темы нужны отдельно от auth темы |
| `CImtCoreAuthThemeInitializer` | Auth theme ресурсы (`imtauthguiTheme`) | Когда нужны только auth темы |
| `CImtCoreBaseSdlInitializer` | Базовые SDL-схемы (`imtbase*`) | Для core-сервисов, базовых фильтров, коллекций и операций |
| `CImtCoreAuthSdlInitializer` | Auth SDL-схемы (`imtauth*`) | Для auth-domain API и сервисов |
| `CImtCoreDomainSdlInitializer` | Domain SDL (`imtapp*`, `imtcolor*`, `imt2d*`) | Для предметных доменов приложения (app/color/2d) |
| `CImtCoreStyleTypeInitializer` | Тип стиля ImtStyle (`ST_IMAGINGTOOLS`) | Когда нужно установить policy стиля без применения базовой Qt-темы |
| `CImtCoreFusionBaseStyleInitializer` | Базовый Qt-стиль `fusion` + подключение `CImtStyle` к `QApplication` | Когда нужно применить базовый визуальный стиль |
| `CImtCoreLightThemePropertyInitializer` | Свойство приложения `ThemeId=Light` | Когда нужно явно зафиксировать light theme на старте |

Практическое правило:
если вы не уверены, что выбираете правильный набор атомарных классов, используйте доменный агрегатор. К атомарному уровню переходите только когда действительно нужен узкий профиль запуска.

### 4.2 Агрегаторы среднего уровня

Что это:
Технические агрегаторы, которые собирают атомарные инициализаторы внутри одной технической области.

Что включает:
- `CImtCoreResourcesInitializer`: атомарные инициализаторы ресурсов.
- `CImtCoreQmlInitializer`: атомарные инициализаторы QML.
- `CImtCoreThemeInitializer`: атомарные инициализаторы тем.
- `CImtCoreSdlInitializer`: атомарные инициализаторы SDL.
- `CImtCoreStyleInitializer`: атомарные шаги настройки стиля.

Когда включать в приложениях:
- Когда приложение нуждается в полной технической области, но не во всех доменах.
- Когда миграция со старого монолита идет поэтапно: сначала техническими слоями, затем доменными.
- Когда нужно стандартизировать инициализацию конкретной технологии между несколькими приложениями.

- `CImtCoreResourcesInitializer`
- `CImtCoreQmlInitializer`
- `CImtCoreThemeInitializer`
- `CImtCoreSdlInitializer`
- `CImtCoreStyleInitializer`

Эти классы агрегируют атомарные инициализаторы внутри одной технической области.

### 4.3 Доменные агрегаторы

Что это:
Бизнес/предметные агрегаторы, которые объединяют ресурсы, QML, темы и SDL в рамках одного домена.

Что включает:
- `CImtCoreAuthInitializer`: auth-ресурсы, auth-theme, auth-QML, auth-SDL.
- `CImtCoreBaseInitializer`: base-ресурсы, style-ресурсы, base-QML, base-SDL.
- `CImtCoreDeskInitializer`: desk-ресурсы, gui-theme, desk-QML, domain-SDL.
- `CImtCoreLicInitializer`: lic-ресурсы, lic-QML.

Когда включать в приложениях:
- Когда приложение функционально сосредоточено на конкретном домене.
- Когда нужно быстро подключить "все необходимое" для домена без ручной сборки цепочки.
- Когда важна переиспользуемость профиля между сервисами и UI-приложениями одного домена.

- `CImtCoreAuthInitializer`
- `CImtCoreBaseInitializer`
- `CImtCoreDeskInitializer`
- `CImtCoreLicInitializer`

Эти классы объединяют ресурсы, QML, темы и SDL в рамках предметной области.

### 4.4 Полный агрегатор по умолчанию

Что это:
Верхнеуровневый агрегатор, который собирает стандартный профиль инициализации приложения.

Что включает:
- Общие шаги (локализация и стиль) и доменные агрегаторы (`Base`, `Auth`, `Desk`, `Lic`) в фиксированном порядке.

Когда включать в приложениях:
- Для большинства полнофункциональных desktop/QML приложений.
- Когда нужна максимальная совместимость со стандартным startup-профилем ImtCore.
- Когда нет явного требования минимизировать startup до конкретных подсистем.

- `CDefaultImtCoreQmlInitializer`

Используется, когда нужно полное стандартное поведение запуска.

### 4.5 Практическая матрица выбора слоя

| Сценарий приложения | Рекомендуемый слой | Почему |
| --- | --- | --- |
| Полнофункциональное приложение ImtCore | `CDefaultImtCoreQmlInitializer` | Готовый стандартный профиль без ручной сборки |
| Приложение одного домена (например, только Auth) | Доменные агрегаторы (`CImtCoreAuthInitializer`) | Подключает полный набор зависимостей домена |
| Приложение без части UI-функций, но с нужной технологией | Агрегаторы среднего уровня | Можно включить только нужную техническую область |
| Узкоспециализированный startup (ограниченный footprint) | Атомарные инициализаторы | Максимально точный контроль состава и порядка |
| Поэтапная миграция legacy-кода | Сначала агрегаторы среднего уровня, затем доменные | Упрощает безопасный переход без большого одномоментного изменения |

### 4.6 Минимальный чек-лист подключения

1. Определите: вам нужен full-profile, домен или точечная инициализация.
2. Выберите самый высокий подходящий слой (по умолчанию: full, иначе доменный).
3. Если выбираете низкий слой, явно проверьте зависимости по теме, QML и SDL.
4. Зафиксируйте порядок инициализаторов и не меняйте его без проверки поведения.
5. Для новых приложений предпочитайте доменные агрегаторы вместо ручного набора атомарных классов.

## 5. Рекомендуемые сценарии использования

### 5.1 Полный запуск приложения по умолчанию

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CDefaultImtCoreQmlInitializer.h>

imtcore::CDefaultImtCoreQmlInitializer initializer;
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true, initializer);
```

### 5.2 Запуск только нужного домена (пример: Auth)

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreAuthInitializer.h>

imtcore::CImtCoreAuthInitializer initializer;
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true, initializer);
```

### 5.3 Кастомная композиция под специализированное приложение

```cpp
#include <imtcore/CCascadedApplicationInitializer.h>
#include <imtcore/CImtCoreLocalizationInitializer.h>
#include <imtcore/CImtCoreDeskInitializer.h>
#include <imtcore/CImtCoreStyleInitializer.h>

imtcore::CCascadedApplicationInitializer initializer;
initializer.AddInitializer(new imtcore::CImtCoreLocalizationInitializer());
initializer.AddInitializer(new imtcore::CImtCoreDeskInitializer());
initializer.AddInitializer(new imtcore::CImtCoreStyleInitializer());

return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true, initializer);
```

## 6. Правила порядка инициализации

Порядок влияет на поведение во время выполнения. Практические правила:

1. Сначала локализация
2. Затем базовые/доменные ресурсы
3. Настройка стиля до отображения UI
4. Темы до QML, зависящего от theme-свойств
5. SDL/QML домена до фактического использования

Если собираете цепочку вручную, порядок должен быть явным и детерминированным.

## 7. Рекомендации по миграции

При переходе со старой статической инициализации:

1. Определите необходимые подсистемы
2. Предпочитайте готовые доменные агрегаторы
3. Если агрегатор не подходит, соберите локальную цепочку из атомарных инициализаторов
4. Держите цепочку минимальной под профиль приложения

## 8. Правила расширения

При добавлении нового инициализатора:

1. Реализуйте `IApplicationInitializer`
2. Соблюдайте одну ответственность
3. Используйте понятное доменное имя класса
4. Подключайте его в ближайший подходящий агрегатор
5. Проверяйте влияние порядка на зависимые подсистемы

## 9. Примечания

- Legacy-файл `Include/imtbase/Init.h` оставлен без изменений по договоренности.
- В новом коде рекомендуется использовать инициализаторы из `imtcore` и запуск через `CApplicationRunner`.
