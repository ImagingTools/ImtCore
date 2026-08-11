# ImtCore Application Initializers

## 1. Goal

This document describes a simplified, macro-level initialization model for ImtCore applications.

The model is designed for application teams that need clear startup profiles instead of many low-level initializer building blocks.

## 2. Core API

- Static initialization functions in `imtcore` namespace
- `imtcore::CApplicationRunner::Run(..., autoInit)` for app execution

## 3. Domain Split (Core vs UI)

Each domain is split into two functions:

- Core/non-UI:
	- `InitializeImtCoreBase()`
	- `InitializeImtCoreAuth()`
	- `InitializeImtCoreDesk()`
	- `InitializeImtCoreLic()`
- UI:
	- `InitializeImtCoreBaseUi()`
	- `InitializeImtCoreAuthUi()`
	- `InitializeImtCoreDeskUi()`
	- `InitializeImtCoreLicUi()`

Client profiles do not use intermediate `*UiInit()` wrappers.

For each domain, the profile code explicitly calls:
1. `InitializeImtCore<Domain>()`
2. `InitializeImtCore<Domain>Ui()`

## 4. Macro Profiles

### 4.1 Server with authorization

Function:
- `imtcore::InitializeImtCoreServerAuth()`

Includes:
- Localization
- Base core layer (non-UI)
- Authorization core domain (non-UI)

Use when:
- Backend service requires auth-related domain logic
- Licensing domain is not required by default

### 4.2 Server with licensing

Function:
- `imtcore::InitializeImtCoreServerLic()`

Includes:
- Localization
- Base core layer (non-UI)
- Licensing core domain (non-UI)

Use when:
- Backend service focuses on licensing workflows
- Auth domain is optional or externalized

### 4.3 Server with authorization and licensing

Function:
- `imtcore::InitializeImtCoreServerAuthLic()`

Includes:
- Localization
- Base core layer (non-UI)
- Authorization core domain (non-UI)
- Licensing core domain (non-UI)

Use when:
- Both auth and licensing are core service responsibilities

### 4.4 Client with authorization

Function:
- `imtcore::InitializeImtCoreClientAuth()`

Includes:
- Localization
- Style/UI setup
- Base (core + UI)
- Auth (core + UI)

Use when:
- UI clients need only the auth domain
- Licensing and desk domains are not required

### 4.5 Client with licensing

Function:
- `imtcore::InitializeImtCoreClientLic()`

Includes:
- Localization
- Style/UI setup
- Base (core + UI)
- Lic (core + UI)

Use when:
- UI clients focus on licensing workflows
- Auth and desk domains are not required

### 4.6 Client with authorization and licensing

Function:
- `imtcore::InitializeImtCoreClientAuthLic()`

Includes:
- Localization
- Style/UI setup
- Base (core + UI)
- Auth (core + UI)
- Lic (core + UI)

Use when:
- UI clients require both auth and licensing domains
- Desk domain is not required

### 4.7 Client application profile

Function:
- `imtcore::InitializeImtCoreClientApp()`

Includes:
- Localization
- Style/UI setup
- Base (core + UI)
- Auth (core + UI)
- Desk (core + UI)
- Lic (core + UI)

Use when:
- Desktop/QML client applications
- Full-featured UI applications

## 5. Usage Examples

### 5.1 Server with authorization

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthInitializer.h>

imtcore::InitializeImtCoreServerAuth();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.2 Server with licensing

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerLicInitializer.h>

imtcore::InitializeImtCoreServerLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.3 Server with authorization and licensing

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthLicInitializer.h>

imtcore::InitializeImtCoreServerAuthLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.4 Client with authorization

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientAuthInitializer.h>

imtcore::InitializeImtCoreClientAuth();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.5 Client with licensing

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientLicInitializer.h>

imtcore::InitializeImtCoreClientLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.6 Client with authorization and licensing

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientAuthLicInitializer.h>

imtcore::InitializeImtCoreClientAuthLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 5.7 Client application

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientAppInitializer.h>

imtcore::InitializeImtCoreClientApp();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

## 6. Selection Guide

- Choose `InitializeImtCoreServerAuth()` for auth-centric servers.
- Choose `InitializeImtCoreServerLic()` for licensing-centric servers.
- Choose `InitializeImtCoreServerAuthLic()` when both domains are required.
- Choose `InitializeImtCoreClientAuth()` for UI clients with the auth domain.
- Choose `InitializeImtCoreClientLic()` for UI clients with the licensing domain.
- Choose `InitializeImtCoreClientAuthLic()` for UI clients with auth and licensing domains.
- Choose `InitializeImtCoreClientApp()` for client/UI apps.

Rule of thumb:
- Server profiles initialize only non-UI domain parts.
- Client profiles initialize both non-UI and UI parts through explicit domain function calls.
