# ImtCore Application Initializers

## 1. Goal

This document describes a simplified, macro-level initialization model for ImtCore applications.

Important:
- The initializer set in this document is a baseline for ImtCore, not an exhaustive final setup.
- Each final product/application should define its own initializer with the exact domain/resource set it needs.

## 2. Core API

- Global static initialization functions are defined without namespace wrappers in the initializer headers.
- `imtcore::CApplicationRunner::Run(..., autoInit)` is the application entry point.

## 3. Domain Split (Core vs UI and QML)

- Core/non-UI functions:
  - `ImtCoreInitBaseResources()`
  - `ImtCoreInitAuthSqlResources()`
  - `ImtCoreInitDeskSqlResources()`
- UI functions (non-QML):
  - `ImtCoreInitStyleResources()`
  - `ImtCoreInitAuthStyleResources()`
  - `ImtCoreInitLicStyleResources()`
- QML functions:
  - `ImtCoreInitQmlApplicationCoreResources()`
  - `ImtCoreInitQmlCollectionResources()`
  - `ImtCoreInitQmlDocumentManagementResources()`
  - `ImtCoreInitQmlPaintResources()`
  - `ImtCoreInitQmlResources()`
  - `ImtCoreInitAuthQmlResources()`
  - `ImtCoreInitTenantQmlResources()`
  - `ImtCoreInitLicQmlResources()`

Rule:
- UI functions must not initialize SDL or QML resources.
- SDL resources are initialized in QML functions.
- QML resources are initialized in QML functions.
- `ImtCoreInitQmlResources()` aggregates base QML resources transitively.
- `ImtCoreInitAuthQmlResources()` initializes auth QML resources and includes tenant QML resources transitively.

## 4. Macro Profiles

### 4.1 Authorizable server

Function:
- `InitializeImtCoreAuthorizableServer()`

## 5. Usage Examples

### 5.1 Authorizable server

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreAuthorizableServerInitializer.h>

InitializeImtCoreAuthorizableServer();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

For client applications, compose startup in a product initializer by calling only the required domain helpers.

## 6. Selection Guide

- Choose `InitializeImtCoreAuthorizableServer()` for auth-centric servers.

For product applications:
- Define a dedicated product initializer and call only the required domain initializers/resources.
- For auth, use `ImtCoreInitAuthSqlResources()`, `ImtCoreInitAuthStyleResources()`, and `ImtCoreInitAuthQmlResources()`.
- `ImtCoreInitAuthQmlResources()` already initializes tenant QML resources transitively via `ImtCoreInitTenantQmlResources()`.
- For client UI startup, combine base/auth/desk/lic helpers explicitly, including QML helpers where QML is required.
