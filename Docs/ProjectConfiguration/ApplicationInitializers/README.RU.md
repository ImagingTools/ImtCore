# Инициализаторы приложений ImtCore

## 1. Назначение

Документ описывает упрощенную макро-модель инициализации приложений ImtCore.

Важно:
- Набор инициализаторов в этом документе является базовым для ImtCore, а не исчерпывающим.
- Для каждого конечного продукта/приложения нужно создавать собственный инициализатор с необходимым набором доменов и ресурсов.

## 2. Базовый API

- Глобальные статические функции инициализации (без namespace-оберток в initializer headers).
- `imtcore::CApplicationRunner::Run(..., autoInit)` для запуска приложения.

## 3. Разделение доменов (Core и UI)

- Core/non-UI функции:
  - `InitializeImtCoreBase()`
  - Core-ресурсы auth: `ImtCoreInitAuthResources()`
  - `InitializeImtCoreDesk()`
  - `InitializeImtCoreLic()`
- UI функции:
  - `InitializeImtCoreBaseUi()`
  - UI-ресурсы auth: `ImtCoreInitAuthUiResources()`
  - Tenant UI-ресурсы auth: `ImtCoreInitAuthTenantUiResources()`
  - `InitializeImtCoreDeskUi()`
  - `InitializeImtCoreLicUi()`

Правила:
- SDL-ресурсы инициализируются в UI-функциях.
- Tenant-часть подключается только явно через tenant-функции.

## 4. Макро-профили

### 4.1 Сервер с авторизацией

Функция:
- `InitializeImtCoreServerAuth()`

### 4.2 Сервер с авторизацией и tenant

Функция:
- `InitializeImtCoreServerAuthTenant()`

### 4.3 Сервер с лицензиями

Функция:
- `InitializeImtCoreServerLic()`

### 4.4 Клиент с авторизацией

Функция:
- `InitializeImtCoreClientAuth()`

### 4.5 Клиент с лицензиями

Функция:
- `InitializeImtCoreClientLic()`

### 4.6 Полный профиль клиентского приложения

Функция:
- `InitializeImtCoreClientApp()`

## 5. Примеры использования

### 5.1 Сервер с авторизацией

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthInitializer.h>

InitializeImtCoreServerAuth();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.2 Сервер с авторизацией и tenant

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthTenantInitializer.h>

InitializeImtCoreServerAuthTenant();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.3 Сервер с лицензиями

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerLicInitializer.h>

InitializeImtCoreServerLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.4 Клиент с авторизацией

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientAuthInitializer.h>

InitializeImtCoreClientAuth();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.5 Клиент с лицензиями

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientLicInitializer.h>

InitializeImtCoreClientLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.6 Полный клиентский профиль

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientAppInitializer.h>

InitializeImtCoreClientApp();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

## 6. Как выбрать профиль

- `InitializeImtCoreServerAuth()` для auth-ориентированных серверов.
- `InitializeImtCoreServerAuthTenant()` для auth-серверов с tenant-частью.
- `InitializeImtCoreServerLic()` для lic-ориентированных серверов.
- `InitializeImtCoreClientAuth()` для UI-клиентов с auth.
- `InitializeImtCoreClientLic()` для UI-клиентов с lic.
- `InitializeImtCoreClientApp()` для полного клиентского/UI запуска.

Для продуктового приложения:
- Создайте собственный инициализатор приложения и явно вызовите только нужные доменные инициализаторы/ресурсы.
- Для auth используйте `ImtCoreInitAuthResources()`, `ImtCoreInitAuthUiResources()` и (при необходимости) `ImtCoreInitAuthTenantUiResources()`.
