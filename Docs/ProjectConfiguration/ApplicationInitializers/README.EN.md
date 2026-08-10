# ImtCore Application Initializers

## 1. Purpose

This document describes the modular initializer architecture introduced in ImtCore to replace legacy monolithic static initialization.

The new approach is based on:

- `imtcore::IApplicationInitializer` as a common contract
- small initializer classes with one clear responsibility
- compositional aggregators built with `imtcore::CCascadedApplicationInitializer`

This improves maintainability, testability, and reuse of startup logic across applications and subsystems.

## 2. Goal

The legacy static initializer mixed many concerns in one place:

- localization
- UI resources
- theme resources
- QML resources
- SDL schemas/resources
- style setup

The new structure separates these concerns into domain modules and allows precise startup profiles.

## 3. Core Contracts

### 3.1 Base interface

- `imtcore::IApplicationInitializer`
  - Method: `void Initialize()`

### 3.2 Composition

- `imtcore::CCascadedApplicationInitializer`
  - Stores and executes an ordered chain of initializers
  - Ownership of added initializers is transferred to the chain

### 3.3 Runner integration

- `imtcore::CApplicationRunner::Run(..., IApplicationInitializer& initializer)`
  - Calls `initializer.Initialize()` before application execution

## 4. Initializer Layers

### 4.1 Atomic initializers (single concern)

Examples:

- Localization:
  - `CAcfLocInitializer`
  - `CImtCoreLocalizationInitializer`
- Resources:
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
- Theme:
  - `CImtCoreGuiThemeInitializer`
  - `CImtCoreAuthThemeInitializer`
- SDL:
  - `CImtCoreBaseSdlInitializer`
  - `CImtCoreAuthSdlInitializer`
  - `CImtCoreDomainSdlInitializer`
- Style setup:
  - `CImtCoreStyleTypeInitializer`
  - `CImtCoreFusionBaseStyleInitializer`
  - `CImtCoreLightThemePropertyInitializer`

### 4.2 Mid-level aggregators

- `CImtCoreResourcesInitializer`
- `CImtCoreQmlInitializer`
- `CImtCoreThemeInitializer`
- `CImtCoreSdlInitializer`
- `CImtCoreStyleInitializer`

These aggregators compose atomic initializers of one technical area.

### 4.3 Domain aggregators

- `CImtCoreAuthInitializer`
- `CImtCoreBaseInitializer`
- `CImtCoreDeskInitializer`
- `CImtCoreLicInitializer`

These aggregators combine resources, QML, themes, and SDL per business/domain area.

### 4.4 Full default aggregator

- `CDefaultImtCoreQmlInitializer`

Use this when full default startup behavior is required.

## 5. Recommended Usage Patterns

### 5.1 Full default app startup

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CDefaultImtCoreQmlInitializer.h>

imtcore::CDefaultImtCoreQmlInitializer initializer;
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true, initializer);
```

### 5.2 Domain-specific startup (Auth only)

```cpp
#include <imtcore/CApplicationRunner.h>
#include <imtcore/CImtCoreAuthInitializer.h>

imtcore::CImtCoreAuthInitializer initializer;
return imtcore::CApplicationRunner::Run(argc, argv, appComponent, true, initializer);
```

### 5.3 Custom composition for a specialized app

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

## 6. Ordering Rules

Initialization order can affect runtime behavior. Follow these practical rules:

1. Localization first
2. Core/resources next
3. Style before UI rendering
4. Theme before QML that depends on theme properties
5. Domain SDL/QML initialization before runtime use

If you compose your own chain, keep order explicit and deterministic.

## 7. Migration Guidance

When replacing legacy static initialization:

1. Identify required subsystem(s)
2. Prefer existing domain aggregators
3. If no aggregator fits, create a local cascade from existing atomic initializers
4. Keep the chain minimal for your app profile

## 8. Extension Guidelines

When adding a new initializer:

1. Implement `IApplicationInitializer`
2. Keep a single responsibility
3. Use a clear domain-oriented class name
4. Add it to the nearest existing aggregator where appropriate
5. Validate ordering effects with dependent subsystems

## 9. Notes

- The legacy file `Include/imtbase/Init.h` remains unchanged by design.
- New code should prefer `imtcore` initializer classes and `CApplicationRunner` integration.
