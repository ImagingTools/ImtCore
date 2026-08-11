# Инициализаторы приложений ImtCore

## 1. Назначение

Документ описывает упрощенную макро-модель инициализации приложений ImtCore.

Модель ориентирована на команды прикладной разработки: вместо большого количества микро-инициализаторов используются готовые крупные профили запуска.

## 2. Базовый API

- Статические функции инициализации в пространстве имен `imtcore`
- `imtcore::CApplicationRunner::Run(..., autoInit)` для запуска приложения

## 3. Разделение доменов (Core и UI)

Разделение на Core и UI нужно, чтобы серверные и клиентские приложения использовали один и тот же доменный API, но подключали разный объем ресурсов.

Что считается Core:
- non-UI ресурсы и схемы (DB, SDL, серверные модели/контракты)
- логика, не требующая QML, тем и графических ресурсов

Что считается UI:
- QML ресурсы
- theme ресурсы
- light/dark GUI ресурсы

### 3.1 Функции доменов

`Base`:
- Core: `InitializeImtCoreBase()`
- UI: `InitializeImtCoreBaseUi()`

`Auth`:
- Core: `InitializeImtCoreAuth()`
- UI: `InitializeImtCoreAuthUi()`

`Desk`:
- Core: `InitializeImtCoreDesk()`
- UI: `InitializeImtCoreDeskUi()`

`Lic`:
- Core: `InitializeImtCoreLic()`
- UI: `InitializeImtCoreLicUi()`

### 3.2 Стандартные клиентские инициализаторы (в отдельных файлах)

- `CImtCoreBaseUiInitializer.h` -> `InitializeImtCoreBaseUiInit()`
- `CImtCoreAuthUiInitializer.h` -> `InitializeImtCoreAuthUiInit()`
- `CImtCoreDeskUiInitializer.h` -> `InitializeImtCoreDeskUiInit()`
- `CImtCoreLicUiInitializer.h` -> `InitializeImtCoreLicUiInit()`

Каждый `*UiInit()` вызывает пару функций одного домена в правильной последовательности:
1. Core-функция домена
2. UI-функция домена

Это гарантирует, что UI-слой поднимается только после non-UI ресурсов домена.

### 3.3 Практическое правило использования

- Серверные профили используют только Core-функции доменов.
- Клиентский профиль использует `*UiInit()` обертки из отдельных файлов.
- Если приложению нужен смешанный профиль, выбирайте минимально необходимый набор:
  - Core-only для headless сценариев
  - Core + UI только для реально используемых доменов

## 4. Макро-профили

### 4.1 Сервер с авторизацией

Функция:
- `imtcore::InitializeImtCoreServerAuth()`

Включает:
- Локализацию
- Базовый core слой (без UI)
- Auth-домен core (без UI)

Когда использовать:
- Сервис требует auth-доменную логику
- Лицензионный домен не обязателен по умолчанию

### 4.2 Сервер с лицензиями

Функция:
- `imtcore::InitializeImtCoreServerLic()`

Включает:
- Локализацию
- Базовый core слой (без UI)
- Lic-домен core (без UI)

Когда использовать:
- Сервис ориентирован на лицензионные сценарии
- Авторизация опциональна или вынесена наружу

### 4.3 Сервер с авторизацией и лицензиями

Функция:
- `imtcore::InitializeImtCoreServerAuthLic()`

Включает:
- Локализацию
- Базовый core слой (без UI)
- Auth-домен core (без UI)
- Lic-домен core (без UI)

Когда использовать:
- И авторизация, и лицензирование являются ключевыми требованиями сервиса

### 4.4 Профиль клиентского приложения

Функция:
- `imtcore::InitializeImtCoreClientApp()`

Включает:
- Локализацию
- Настройку стиля/UI
- Base (core + UI)
- Auth (core + UI)
- Desk (core + UI)
- Lic (core + UI)

Когда использовать:
- Desktop/QML клиентские приложения
- Полнофункциональные UI-приложения

## 5. Примеры использования

### 5.1 Сервер с авторизацией

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthInitializer.h>

imtcore::InitializeImtCoreServerAuth();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.2 Сервер с лицензиями

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerLicInitializer.h>

imtcore::InitializeImtCoreServerLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.3 Сервер с авторизацией и лицензиями

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthLicInitializer.h>

imtcore::InitializeImtCoreServerAuthLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.4 Клиентское приложение

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientAppInitializer.h>

imtcore::InitializeImtCoreClientApp();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

## 6. Как выбрать профиль

- `InitializeImtCoreServerAuth()` для auth-ориентированных серверов.
- `InitializeImtCoreServerLic()` для lic-ориентированных серверов.
- `InitializeImtCoreServerAuthLic()`, когда нужны оба домена.
- `InitializeImtCoreClientApp()` для клиентских/UI приложений.

Практическое правило:
- Серверные профили инициализируют только non-UI части доменов.
- Клиентский профиль инициализирует и core, и UI части через отдельные `*UiInitializer` обертки.
