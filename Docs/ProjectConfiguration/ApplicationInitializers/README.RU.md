# Инициализаторы приложений ImtCore

## 1. Назначение

Документ описывает упрощенную макро-модель инициализации приложений ImtCore.

Модель ориентирована на команды прикладной разработки: вместо большого количества микро-инициализаторов используются готовые крупные профили запуска.

Важно:
- Набор инициализаторов в этом документе является базовым для ImtCore, а не исчерпывающим.
- Для каждого конечного продукта/приложения нужно создавать собственный инициализатор с необходимым набором доменов и ресурсов.

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
- Core (tenant-дополнение): `InitializeImtCoreAuthTenant()`
- UI: `InitializeImtCoreAuthUi()`

`Desk`:
- Core: `InitializeImtCoreDesk()`
- UI: `InitializeImtCoreDeskUi()`

`Lic`:
- Core: `InitializeImtCoreLic()`
- UI: `InitializeImtCoreLicUi()`

### 3.2 Явный вызов Core и UI в клиентских профилях

В клиентских макро-профилях нет промежуточных `*UiInit()` оберток.

Для каждого домена вызовы выполняются явно в коде профиля:
1. `InitializeImtCore<Domain>()`
2. `InitializeImtCore<Domain>Ui()`

Это гарантирует раздельную инициализацию non-UI и UI и явный порядок вызовов.

### 3.3 Практическое правило использования

- Серверные профили используют только Core-функции доменов.
- Клиентские профили явно вызывают пары Core/UI функций доменов.
- Tenant-часть auth подключается только там, где это явно нужно.
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
- Auth core (без tenant)

Когда использовать:
- Сервис требует auth-доменную логику без tenant-ресурсов

### 4.2 Сервер с авторизацией и tenant

Функция:
- `imtcore::InitializeImtCoreServerAuthTenant()`

Включает:
- Локализацию
- Базовый core слой (без UI)
- Auth core
- Tenant-дополнение auth

Когда использовать:
- Сервису нужны tenant-ресурсы auth

### 4.3 Сервер с лицензиями

Функция:
- `imtcore::InitializeImtCoreServerLic()`

Включает:
- Локализацию
- Базовый core слой (без UI)
- Lic-домен core (без UI)

Когда использовать:
- Сервис ориентирован на лицензионные сценарии
- Авторизация опциональна или вынесена наружу

### 4.4 Сервер с авторизацией и лицензиями

Функция:
- `imtcore::InitializeImtCoreServerAuthLic()`

Включает:
- Локализацию
- Базовый core слой (без UI)
- Auth core (без tenant)
- Lic-домен core (без UI)

Когда использовать:
- И авторизация, и лицензирование являются ключевыми требованиями сервиса

### 4.5 Клиент с авторизацией

Функция:
- `imtcore::InitializeImtCoreClientAuth()`

Включает:
- Локализацию
- Настройку стиля/UI
- Base (core + UI)
- Auth (core + UI)

Когда использовать:
- UI-клиентам, которым нужен auth без tenant

### 4.6 Клиент с лицензиями

Функция:
- `imtcore::InitializeImtCoreClientLic()`

Включает:
- Локализацию
- Настройку стиля/UI
- Base (core + UI)
- Lic (core + UI)

Когда использовать:
- UI-клиентам, ориентированным на lic-сценарии
- Auth и desk-домены не требуются

### 4.7 Клиент с авторизацией и лицензиями

Функция:
- `imtcore::InitializeImtCoreClientAuthLic()`

Включает:
- Локализацию
- Настройку стиля/UI
- Base (core + UI)
- Auth (core + UI)
- Lic (core + UI)

Когда использовать:
- UI-клиентам, где нужны auth и lic без tenant

### 4.8 Профиль клиентского приложения

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

### 5.2 Сервер с авторизацией и tenant

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthTenantInitializer.h>

imtcore::InitializeImtCoreServerAuthTenant();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.3 Сервер с лицензиями

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerLicInitializer.h>

imtcore::InitializeImtCoreServerLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.4 Сервер с авторизацией и лицензиями

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthLicInitializer.h>

imtcore::InitializeImtCoreServerAuthLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.5 Клиент с авторизацией

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientAuthInitializer.h>

imtcore::InitializeImtCoreClientAuth();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.6 Клиент с лицензиями

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientLicInitializer.h>

imtcore::InitializeImtCoreClientLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.7 Клиент с авторизацией и лицензиями

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientAuthLicInitializer.h>

imtcore::InitializeImtCoreClientAuthLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.8 Клиентское приложение

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientAppInitializer.h>

imtcore::InitializeImtCoreClientApp();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

## 6. Как выбрать профиль

- `InitializeImtCoreServerAuth()` для auth-ориентированных серверов.
- `InitializeImtCoreServerAuthTenant()` для auth-серверов с tenant-частью.
- `InitializeImtCoreServerLic()` для lic-ориентированных серверов.
- `InitializeImtCoreServerAuthLic()`, когда нужны оба домена.
- `InitializeImtCoreClientAuth()` для UI-клиентов с auth-доменом.
- `InitializeImtCoreClientLic()` для UI-клиентов с lic-доменом.
- `InitializeImtCoreClientAuthLic()` для UI-клиентов с auth+lic.
- `InitializeImtCoreClientApp()` для клиентских/UI приложений.

Для продуктового приложения:
- Создайте собственный инициализатор приложения и явно вызовите только нужные `InitializeImtCore<Domain>()` и `InitializeImtCore<Domain>Ui()`.
- Если нужен набор `auth+tenant+lic` или другой специфический профиль, собирайте его только в продуктовом инициализаторе, а не в ImtCore.

Практическое правило:
- Серверные профили инициализируют только non-UI части доменов.
- Клиентский профиль инициализирует и core, и UI части явными вызовами доменных функций.
