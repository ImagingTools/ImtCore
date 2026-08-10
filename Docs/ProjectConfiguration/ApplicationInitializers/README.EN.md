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

What this layer is:
The smallest initialization units. Each class performs one narrow initialization task.

What it includes:
- One precise responsibility only: localization, one resource group, one QML set, one theme group, one SDL group, or one style setup step.

When to include in applications:
- You need a strict minimal startup profile.
- You are building a custom specialized initialization pipeline.
- You need explicit low-level control over initialization ordering.

Examples:

- Localization:
  - `CAcfLocInitializer`
  - `CImtCoreLocalizationInitializer`
- Resources:
  - `CImtCoreAuthResourcesInitializer`
  - `CImtCoreLicResourcesInitializer`
  - `CImtCoreChatDbResourcesInitializer`
  - `CImtCoreDeskDbResourcesInitializer`
  - `CImtCoreCommonDbResourcesInitializer`
  - `CImtCoreGuiResourcesInitializer`
  - `CImtCoreGuiLightResourcesInitializer`
  - `CImtCoreGuiDarkResourcesInitializer`
  - `CImtCoreDeskResourcesInitializer` (composite over desk resource atomic initializers)
  - `CImtCoreBaseResourcesInitializer`
  - `CImtCoreStyleResourcesInitializer`
- QML:
  - `CImtCoreBaseQmlInitializer`
  - `CImtCoreGuiQmlInitializer`
  - `CImtCoreColGuiQmlInitializer`
  - `CImtCoreDocGuiQmlInitializer`
  - `CImtCoreGuiGqlQmlInitializer`
  - `CImtCoreDeskQmlInitializer` (composite over desk QML atomic initializers)
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

#### 4.1.1 Atomic initializer reference

The table below provides a practical reference for each atomic initializer: what it initializes and when to include it directly.

| Class | What it initializes | When to include |
| --- | --- | --- |
| `CAcfLocInitializer` | ACF localization resources (`AcfLoc`, `AcfSlnLoc`) | When the app needs baseline ACF translations, even without full ImtCore startup |
| `CImtCoreLocalizationInitializer` | ImtCore localization plus baseline ACF localization | When full localization baseline is required for ImtCore UI |
| `CImtCoreAuthResourcesInitializer` | Auth resources and auth UI resource packs | For auth screens/workflows without full app startup |
| `CImtCoreLicResourcesInitializer` | Licensing resource packs | For licensing flows and license-related screens |
| `CImtCoreChatDbResourcesInitializer` | Chat DB resource (`imtchatdb`) | When chat data resources are required |
| `CImtCoreDeskDbResourcesInitializer` | Desk DB resource (`imtdeskdb`) | When desk-specific data resources are required |
| `CImtCoreCommonDbResourcesInitializer` | Common DB resource (`imtdb`) | When shared DB resource payload is required |
| `CImtCoreGuiResourcesInitializer` | Main GUI resource package (`imtgui`) | When base GUI resources are required |
| `CImtCoreGuiLightResourcesInitializer` | Light GUI resource package (`imtguilight`) | When light GUI resources are required |
| `CImtCoreGuiDarkResourcesInitializer` | Dark GUI resource package (`imtguidark`) | When dark GUI resources are required |
| `CImtCoreDeskResourcesInitializer` | Composite desk resource initializer over the six desk resource atomic initializers above | When full desk resource scope is needed without manual assembly |
| `CImtCoreBaseResourcesInitializer` | Base core resources (`imtbase`) | For infrastructure services and minimal core scenarios |
| `CImtCoreStyleResourcesInitializer` | Style resources (style + style variants) | When visual styling is needed without full domain startup |
| `CImtCoreBaseQmlInitializer` | Base QML module (`imtcontrolsqml`) | For minimal QML foundation |
| `CImtCoreGuiQmlInitializer` | Core desk GUI QML module (`imtguiqml`) | For base workspace GUI layer |
| `CImtCoreColGuiQmlInitializer` | Collection GUI QML module (`imtcolguiqml`) | For collection-centric GUI functionality |
| `CImtCoreDocGuiQmlInitializer` | Document GUI QML module (`imtdocguiqml`) | For document-oriented GUI workflows |
| `CImtCoreGuiGqlQmlInitializer` | GUI GQL QML module (`imtguigqlqml`) | For GUI GraphQL-related QML integration |
| `CImtCoreDeskQmlInitializer` | Composite desk QML initializer over the four desk atomic QML initializers above | When you need full desk QML scope without manual assembly |
| `CImtCoreAuthQmlInitializer` | Auth QML module (`imtauthguiqml`) | For auth UI |
| `CImtCoreLicQmlInitializer` | Licensing QML module (`imtlicguiqml`) | For licensing screens |
| `CImtCoreGeoQmlInitializer` | Geo QML module (`imtgeoguiqml`) | For optional geo functionality |
| `CImtCoreGuiThemeInitializer` | GUI theme resources (`imtguiTheme`) | When desktop themes are needed separately from auth themes |
| `CImtCoreAuthThemeInitializer` | Auth theme resources (`imtauthguiTheme`) | When only auth theme assets are required |
| `CImtCoreBaseSdlInitializer` | Base SDL schemas (`imtbase*`) | For core services, base filters, collections, and operations |
| `CImtCoreAuthSdlInitializer` | Auth SDL schemas (`imtauth*`) | For auth domain API/services |
| `CImtCoreDomainSdlInitializer` | Domain SDL (`imtapp*`, `imtcolor*`, `imt2d*`) | For application domains like app/color/2d |
| `CImtCoreStyleTypeInitializer` | ImtStyle type policy (`ST_IMAGINGTOOLS`) | When style policy must be set before visual base style application |
| `CImtCoreFusionBaseStyleInitializer` | Qt `fusion` base style and `CImtStyle` attachment to `QApplication` | When base visual style must be applied |
| `CImtCoreLightThemePropertyInitializer` | App property `ThemeId=Light` | When startup must explicitly lock light theme |

Practical rule:
if you are not fully sure about selecting atomic classes manually, prefer domain aggregators. Move to atomic composition only when a narrow startup footprint is a real requirement.

### 4.2 Mid-level aggregators

What this layer is:
Technical aggregators that compose atomic initializers by technical area.

What it includes:
- `CImtCoreResourcesInitializer`: resource atomic initializers.
- `CImtCoreQmlInitializer`: QML atomic initializers.
- `CImtCoreThemeInitializer`: theme atomic initializers.
- `CImtCoreSdlInitializer`: SDL atomic initializers.
- `CImtCoreStyleInitializer`: style setup atomic initializers.

When to include in applications:
- The app needs one complete technical area, but not all domains.
- Migration from legacy startup is done incrementally.
- Multiple apps should share standardized initialization of the same technology stack.

- `CImtCoreResourcesInitializer`
- `CImtCoreQmlInitializer`
- `CImtCoreThemeInitializer`
- `CImtCoreSdlInitializer`
- `CImtCoreStyleInitializer`

These aggregators compose atomic initializers of one technical area.

### 4.3 Domain aggregators

What this layer is:
Business/domain-oriented aggregators that combine resources, QML, themes, and SDL for one domain.

What it includes:
- `CImtCoreAuthInitializer`: auth resources, auth theme, auth QML, auth SDL.
- `CImtCoreBaseInitializer`: base resources, style resources, base QML, base SDL.
- `CImtCoreDeskInitializer`: desk resources, gui theme, desk QML, domain SDL.
- `CImtCoreLicInitializer`: lic resources, lic QML.

When to include in applications:
- The app is focused on one functional domain.
- You want the full domain package without manual chain assembly.
- You want reusable startup profiles across apps within the same domain.

- `CImtCoreAuthInitializer`
- `CImtCoreBaseInitializer`
- `CImtCoreDeskInitializer`
- `CImtCoreLicInitializer`

These aggregators combine resources, QML, themes, and SDL per business/domain area.

### 4.4 Full default aggregator

What this layer is:
Top-level aggregator that defines the standard default startup profile.

What it includes:
- Shared steps (localization and style) plus domain aggregators (`Base`, `Auth`, `Desk`, `Lic`) in fixed order.

When to include in applications:
- Most full-featured desktop/QML applications.
- You need maximum compatibility with the standard ImtCore startup profile.
- There is no strict requirement to minimize startup to selected subsystems only.

- `CDefaultImtCoreQmlInitializer`

Use this when full default startup behavior is required.

### 4.5 Practical layer selection matrix

| Application scenario | Recommended layer | Why |
| --- | --- | --- |
| Full-featured ImtCore application | `CDefaultImtCoreQmlInitializer` | Ready standard profile, no manual composition |
| Single-domain app (for example Auth only) | Domain aggregators (`CImtCoreAuthInitializer`) | Brings complete domain dependencies |
| App with partial UI needs but specific technology requirements | Mid-level aggregators | Enables one technical area without unrelated domains |
| Specialized low-footprint startup | Atomic initializers | Maximum precision over scope and ordering |
| Incremental legacy migration | Mid-level first, then domain aggregators | Safer transition with smaller behavioral steps |

### 4.6 Minimal inclusion checklist

1. Decide if you need full profile, domain profile, or narrow selective initialization.
2. Choose the highest suitable layer (default: full, otherwise domain).
3. If you choose a lower layer, verify dependencies for theme, QML, and SDL explicitly.
4. Keep initializer order deterministic and avoid changing order without runtime checks.
5. For new applications, prefer domain aggregators over manual assembly of atomic classes.

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
