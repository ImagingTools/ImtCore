# Инициализаторы приложений ImtCore

## 1. Назначение

Документ описывает упрощенную макро-модель инициализации приложений ImtCore.

Важно:
- Набор инициализаторов в этом документе является базовым для ImtCore, а не исчерпывающим.
- Для каждого конечного продукта/приложения нужно создавать собственный инициализатор с необходимым набором доменов и ресурсов.

## 2. Базовый API

- Глобальные статические функции инициализации определяются без namespace-оберток в initializer headers.
- `imtcore::CApplicationRunner::Run(..., autoInit)` используется как точка запуска приложения.

## 3. Разделение доменов (Core, UI и QML-UI)

- Core/non-UI функции:
  - `ImtCoreInitBaseResources()`
  - `ImtCoreInitAuthResources()`
  - `ImtCoreInitDeskResources()`
- UI функции (не-QML):
  - `ImtCoreInitBaseUiResources()`
  - `ImtCoreInitAuthUiResources()`
  - `ImtCoreInitLicUiResources()`
- QML-UI функции:
  - `ImtCoreInitBaseQmlSdlResources()`
  - `ImtCoreInitBaseQmlUiResources()`
  - `ImtCoreInitAuthQmlSdlResources()`
  - `ImtCoreInitAuthQmlUiResources()`
  - `ImtCoreInitTenantQmlSdlResources()`
  - `ImtCoreInitLicQmlUiResources()`

Правила:
- UI-функции не должны инициализировать SDL или QML ресурсы.
- SDL-ресурсы инициализируются только QML-UI функции.
- QML-ресурсы инициализируются только QML-UI функции.

## 4. Макро-профили

### 4.1 Авторизуемый сервер

Функция:
- `InitializeImtCoreAuthorizableServer()`

## 5. Примеры использования

### 5.1 Авторизуемый сервер

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreAuthorizableServerInitializer.h>

InitializeImtCoreAuthorizableServer();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

Для клиентских приложений собирайте запуск в продуктовом инициализаторе, вызывая только нужные доменные helper-функции.

## 6. Как выбрать профиль

- `InitializeImtCoreAuthorizableServer()` для auth-ориентированных серверов.

Для продуктового приложения:
- Создайте собственный инициализатор приложения и явно вызовите только нужные доменные инициализаторы/ресурсы.
- Для auth используйте `ImtCoreInitAuthResources()`, `ImtCoreInitAuthUiResources()`, `ImtCoreInitAuthQmlSdlResources()`, `ImtCoreInitAuthQmlUiResources()` и `ImtCoreInitTenantQmlSdlResources()` при необходимости.
- Для client UI запуска явно комбинируйте base/auth/desk/lic helper-функции, включая QML-UI helper-функции там, где нужен QML.
