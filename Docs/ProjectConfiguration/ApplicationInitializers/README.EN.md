# ImtCore Application Initializers

## 1. Goal

This document describes a simplified, macro-level initialization model for ImtCore applications.

Important:
- The initializer set in this document is a baseline for ImtCore, not an exhaustive final setup.
- Each final product/application should define its own initializer with the exact domain/resource set it needs.

## 2. Core API

- Global static initialization functions are defined without namespace wrappers in the initializer headers.
- `imtcore::CApplicationRunner::Run(..., autoInit)` is the application entry point.

## 3. Domain Split (Core vs UI)

- Core/non-UI functions:
  - `ImtCoreInitBaseResources()`
  - `ImtCoreInitAuthResources()`
  - `ImtCoreInitDeskResources()`
- UI functions:
  - `ImtCoreInitBaseUiResources()`
  - `ImtCoreInitAuthUiResources()`
  - `ImtCoreInitAuthTenantUiResources()`
  - `ImtCoreInitDeskUiResources()`
  - `ImtCoreInitLicUiResources()`

Rule:
- SDL resources are initialized in UI functions.
- Tenant resources are opt-in and explicitly marked via tenant UI resources.

## 4. Macro Profiles

### 4.1 Server with authorization

Function:
- `InitializeImtCoreServerAuth()`

## 5. Usage Examples

### 5.1 Server with authorization

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthInitializer.h>

InitializeImtCoreServerAuth();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

For client applications, compose startup in a product initializer by calling only the required domain helpers.

## 6. Selection Guide

- Choose `InitializeImtCoreServerAuth()` for auth-centric servers.

For product applications:
- Define a dedicated product initializer and call only the required domain initializers/resources.
- For auth, use `ImtCoreInitAuthResources()`, `ImtCoreInitAuthUiResources()`, and `ImtCoreInitAuthTenantUiResources()` when needed.
- For client UI startup, combine base/auth/desk/lic helpers explicitly as required by the product.
