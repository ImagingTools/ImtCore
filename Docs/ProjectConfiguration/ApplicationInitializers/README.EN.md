# ImtCore Application Initializers

## 1. Goal

This document describes a simplified, macro-level initialization model for ImtCore applications.

Important:
- The initializer set in this document is a baseline for ImtCore, not an exhaustive final setup.
- Each final product/application should define its own initializer with the exact domain/resource set it needs.

## 2. Core API

- Global static initialization functions (no `imtcore` namespace wrappers in initializer headers).
- `imtcore::CApplicationRunner::Run(..., autoInit)` for app execution.

## 3. Domain Split (Core vs UI)

- Core/non-UI functions:
	- `InitializeImtCoreBase()`
	- Auth core resources: `ImtCoreInitAuthResources()`
	- `InitializeImtCoreDesk()`
	- `InitializeImtCoreLic()`
- UI functions:
	- `InitializeImtCoreBaseUi()`
	- Auth UI resources: `ImtCoreInitAuthUiResources()`
	- Auth tenant UI resources: `ImtCoreInitAuthTenantUiResources()`
	- `InitializeImtCoreDeskUi()`
	- `InitializeImtCoreLicUi()`

Rule:
- SDL resources are initialized in UI functions.
- Tenant resources are opt-in and explicitly marked via tenant functions.

## 4. Macro Profiles

### 4.1 Server with authorization

Function:
- `InitializeImtCoreServerAuth()`

### 4.2 Server with authorization and tenant

Function:
- `InitializeImtCoreServerAuthTenant()`

### 4.3 Server with licensing

Function:
- `InitializeImtCoreServerLic()`

### 4.4 Client with authorization

Function:
- `InitializeImtCoreClientAuth()`

### 4.5 Client with licensing

Function:
- `InitializeImtCoreClientLic()`

### 4.6 Full client application profile

Function:
- `InitializeImtCoreClientApp()`

## 5. Usage Examples

### 5.1 Server with authorization

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthInitializer.h>

InitializeImtCoreServerAuth();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.2 Server with authorization and tenant

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthTenantInitializer.h>

InitializeImtCoreServerAuthTenant();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.3 Server with licensing

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerLicInitializer.h>

InitializeImtCoreServerLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.4 Client with authorization

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientAuthInitializer.h>

InitializeImtCoreClientAuth();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.5 Client with licensing

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientLicInitializer.h>

InitializeImtCoreClientLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.6 Full client application

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientAppInitializer.h>

InitializeImtCoreClientApp();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

## 6. Selection Guide

- Choose `InitializeImtCoreServerAuth()` for auth-centric servers.
- Choose `InitializeImtCoreServerAuthTenant()` when server auth must include tenant resources.
- Choose `InitializeImtCoreServerLic()` for licensing-centric servers.
- Choose `InitializeImtCoreClientAuth()` for UI clients with auth.
- Choose `InitializeImtCoreClientLic()` for UI clients with licensing.
- Choose `InitializeImtCoreClientApp()` for full client/UI startup.

For product applications:
- Define a dedicated product initializer and call only the required domain initializers/resources.
- For auth, use `ImtCoreInitAuthResources()`, `ImtCoreInitAuthUiResources()`, and (if needed) `ImtCoreInitAuthTenantUiResources()`.
