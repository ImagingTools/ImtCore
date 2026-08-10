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
  - `CImtCoreDeskQmlInitializer`
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

### 4.2 Агрегаторы среднего уровня

- `CImtCoreResourcesInitializer`
- `CImtCoreQmlInitializer`
- `CImtCoreThemeInitializer`
- `CImtCoreSdlInitializer`
- `CImtCoreStyleInitializer`

Эти классы агрегируют атомарные инициализаторы внутри одной технической области.

### 4.3 Доменные агрегаторы

- `CImtCoreAuthInitializer`
- `CImtCoreBaseInitializer`
- `CImtCoreDeskInitializer`
- `CImtCoreLicInitializer`

Эти классы объединяют ресурсы, QML, темы и SDL в рамках предметной области.

### 4.4 Полный агрегатор по умолчанию

- `CDefaultImtCoreQmlInitializer`

Используется, когда нужно полное стандартное поведение запуска.

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
