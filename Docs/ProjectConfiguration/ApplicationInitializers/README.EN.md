# ImtCore Application Initializers

## 1. Goal

This document describes a simplified, macro-level initialization model for ImtCore applications.

The model is designed for application teams that need clear startup profiles instead of many low-level initializer building blocks.

## 2. Core API

- Static initialization functions in `imtcore` namespace
- `imtcore::CApplicationRunner::Run(..., autoInit)` for app execution

## 3. Macro Profiles

### 3.1 Server with authorization

Function:
- `imtcore::InitializeImtCoreServerAuth()`

Includes:
- Localization
- Base server/core layer
- Authorization domain

Use when:
- Backend service requires auth-related domain logic
- Licensing domain is not required by default

### 3.2 Server with licensing

Function:
- `imtcore::InitializeImtCoreServerLic()`

Includes:
- Localization
- Base server/core layer
- Licensing domain

Use when:
- Backend service focuses on licensing workflows
- Auth domain is optional or externalized

### 3.3 Server with authorization and licensing

Function:
- `imtcore::InitializeImtCoreServerAuthLic()`

Includes:
- Localization
- Base server/core layer
- Authorization domain
- Licensing domain

Use when:
- Both auth and licensing are core service responsibilities

### 3.4 Client application profile

Function:
- `imtcore::InitializeImtCoreClientApp()`

Includes:
- Localization
- Style/UI setup
- Base domain
- Auth domain
- Desk domain
- Lic domain

Use when:
- Desktop/QML client applications
- Full-featured UI applications

### 3.5 Backward-compatible default profile

Function:
- `imtcore::InitializeDefaultImtCoreQml()`

Behavior:
- Delegates to `InitializeImtCoreClientApp()`

Use when:
- You need a default full QML startup profile entry point

## 4. Usage Examples

### 4.1 Server with authorization

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthInitializer.h>

imtcore::InitializeImtCoreServerAuth();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 4.2 Server with licensing

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerLicInitializer.h>

imtcore::InitializeImtCoreServerLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 4.3 Server with authorization and licensing

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreServerAuthLicInitializer.h>

imtcore::InitializeImtCoreServerAuthLic();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

### 4.4 Client application

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreClientAppInitializer.h>

imtcore::InitializeImtCoreClientApp();
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true);
```

## 5. Selection Guide

- Choose `InitializeImtCoreServerAuth()` for auth-centric servers.
- Choose `InitializeImtCoreServerLic()` for licensing-centric servers.
- Choose `InitializeImtCoreServerAuthLic()` when both domains are required.
- Choose `InitializeImtCoreClientApp()` for client/UI apps.
- Use `InitializeDefaultImtCoreQml()` as a full default profile entry point.
