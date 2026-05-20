# QML & JQML Documentation — Overview

Welcome to the QML and JQML documentation of the ImtCore SDK. This documentation provides a complete reference of all QML modules, tutorials for getting started, and a guide to JQML web compilation.

## 📚 Documentation Overview

| Document | Description | Scope |
|----------|-------------|-------|
| **[Getting Started](QML_Getting_Started.md)** | Getting started with QML development using ImtCore | ⭐ START HERE |
| **[Module Reference](QML_Module_Reference.md)** | Detailed reference of all QML modules and components | Reference |
| **[JQML Guide](QML_JQML_Guide.md)** | JQML versions, web compilation, cross-platform development | Web Deployment |
| **[Component Tutorials](QML_Component_Tutorials.md)** | Practical tutorials for common UI patterns | Practical Examples |

---

## 🏗️ Architecture Overview

The QML world of ImtCore is organized into several layers:

```
┌─────────────────────────────────────────────────────────────┐
│                    Domain-specific Modules                   │
│  imtauthgui · imtchatgui · imtdeskgui · imtlicgui · ...    │
├─────────────────────────────────────────────────────────────┤
│              GraphQL Integration (imtguigql)                 │
│  GqlRequest · SubscriptionClient · CollectionRepresentation │
├─────────────────────────────────────────────────────────────┤
│            Application Framework (imtgui)                    │
│  ApplicationMain · PagesManager · Settings · Params          │
├─────────────────────────────────────────────────────────────┤
│             Base UI Components (imtcontrols)                 │
│  Buttons · Inputs · Views · Dialogs · Models · Actions       │
├─────────────────────────────────────────────────────────────┤
│              JQML – Web Runtime & Compiler                   │
│           v1 (PureQML) · v2 (JQML2) · v3 (JQ)              │
└─────────────────────────────────────────────────────────────┘
```

### Dependency Chain

```
imtcontrols (Base UI Framework)
    ↓
imtgui (Application Framework, uses imtcontrols)
    ↓
imtguigql (GraphQL Integration, uses imtgui)
    ↓
Domain-specific Modules:
├── imtauthgui  (Authentication & Users)
├── imtchatgui  (Chat System)
├── imtcolgui   (Collection Management)
├── imtdeskgui  (Ticket System)
├── imtdocgui   (Document Management)
├── imtlicgui   (License Management)
├── imtgeogui   (Geo Visualization)
├── imt3dgui    (3D Visualization)
└── web         (Web-specific Components)
```

---

## 🌐 JQML — QML for the Web

JQML is a compiler toolchain that transpiles QML code into JavaScript, allowing the same QML codebase to run both as a native Qt desktop application and as a web application in the browser.

| Version | Name | Description |
|---------|------|-------------|
| **v1** | PureQML | First generation of the web compiler |
| **v2** | JQML2 | Extended version with 3D support (Three.js), 130+ components |
| **v3** | JQ | Current generation with custom parser, source maps, 60+ QtQml + 50+ QtQuick types |

➡️ Details: **[JQML Guide](QML_JQML_Guide.md)**

---

## 📦 Modules at a Glance

### Core UI Modules

| Module | Components | Description |
|--------|-----------|-------------|
| `imtcontrols` | ~160 | Base UI Framework: Buttons, Inputs, Views, Dialogs, Models, Actions, GraphicsView |
| `imtgui` | ~90 | Application Framework: ApplicationMain, Panels, Params, Settings, Views |
| `imtguigql` | ~35 | GraphQL Integration: GqlRequest, Subscriptions, Collection Management |

### Domain-specific Modules

| Module | Description |
|--------|-------------|
| `imtauthgui` | Login, Registration, Tenant Management, User Panel |
| `imtchatgui` | Chat Interface and Messaging |
| `imtcolgui` | Collection Management and Display |
| `imtdeskgui` | Ticket/Desk System UI |
| `imtdocgui` | Document Management and Editor |
| `imtlicgui` | License Management Interface |
| `imtgeogui` | Geo Data Visualization |
| `imt3dgui` | 3D Scenes and Visualization |

### Web-specific

| Module | Description |
|--------|-------------|
| `web` | Web-specific QML components for JQML builds |

➡️ Details: **[Module Reference](QML_Module_Reference.md)**

---

## 🚀 Quick Start

```qml
import imtcontrols 1.0
import imtgui 1.0

ApplicationMain {
    PagesManager {
        // Page definitions here
    }
}
```

➡️ Full Tutorial: **[Getting Started](QML_Getting_Started.md)**

---

## 🔗 Further Reading

- [Include/README.md — User Interface](../../Include/README.md#user-interface-16-libraries-total-7-core--9-domain-specific) — C++ Backend Libraries
- [Tools/JQML/v2/README-3D.md](../../Tools/JQML/v2/README-3D.md) — JQML2 3D Documentation
- [Tools/JQML/v3/compiler/README.md](../../Tools/JQML/v3/compiler/README.md) — JQML v3 Compiler Architecture
- [Docs/Tutorials](../Tutorials/) — Existing Tutorials and Demos
