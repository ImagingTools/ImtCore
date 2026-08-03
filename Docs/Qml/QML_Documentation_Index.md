# QML & JQML Dokumentation — Übersicht

Willkommen zur QML- und JQML-Dokumentation des ImtCore SDK. Diese Dokumentation bietet eine vollständige Referenz aller QML-Module, Tutorials für den Einstieg und eine Anleitung zur JQML-Web-Kompilierung.

## 📚 Dokumentations-Übersicht

| Dokument | Beschreibung | Umfang |
|----------|-------------|--------|
| **[Getting Started](QML_Getting_Started.md)** | Einstieg in die QML-Entwicklung mit ImtCore | ⭐ START HIER |
| **[Modul-Referenz](QML_Module_Reference.md)** | Detaillierte Referenz aller QML-Module und Komponenten | Nachschlagewerk |
| **[JQML Guide](QML_JQML_Guide.md)** | JQML-Versionen, Web-Kompilierung, Cross-Platform-Entwicklung | Web-Deployment |
| **[JQML v3 Fixes (RU)](JQML_V3_Fixes.ru.md)** | Begründung der Compiler- und Runtime-Korrekturen in JQML v3 | Wartung |
| **[Komponenten-Tutorials](QML_Component_Tutorials.md)** | Praktische Tutorials für häufige UI-Muster | Praxis-Beispiele |

---

## 🏗️ Architektur-Übersicht

Die QML-Welt von ImtCore ist in mehrere Schichten organisiert:

```
┌─────────────────────────────────────────────────────────────┐
│                    Domain-spezifische Module                 │
│  imtauthgui · imtchatgui · imtdeskgui · imtlicgui · ...    │
├─────────────────────────────────────────────────────────────┤
│              GraphQL-Integration (imtguigql)                 │
│  GqlRequest · SubscriptionClient · CollectionRepresentation │
├─────────────────────────────────────────────────────────────┤
│           Applikations-Framework (imtgui)                    │
│  ApplicationMain · PagesManager · Settings · Params          │
├─────────────────────────────────────────────────────────────┤
│            UI-Basiskomponenten (imtcontrols)                 │
│  Buttons · Inputs · Views · Dialogs · Models · Actions       │
├─────────────────────────────────────────────────────────────┤
│              JQML – Web Runtime & Compiler                   │
│           v1 (PureQML) · v2 (JQML2) · v3 (JQ)              │
└─────────────────────────────────────────────────────────────┘
```

### Abhängigkeitskette

```
imtcontrols (Basis-UI-Framework)
    ↓
imtgui (Applikations-Framework, nutzt imtcontrols)
    ↓
imtguigql (GraphQL-Integration, nutzt imtgui)
    ↓
Domain-spezifische Module:
├── imtauthgui  (Authentifizierung & Benutzer)
├── imtchatgui  (Chat-System)
├── imtcolgui   (Collection-Verwaltung)
├── imtdeskgui  (Ticket-System)
├── imtdocgui   (Dokument-Management)
├── imtlicgui   (Lizenz-Verwaltung)
├── imtgeogui   (Geo-Visualisierung)
├── imt3dgui    (3D-Visualisierung)
└── web         (Web-spezifische Komponenten)
```

---

## 🌐 JQML — QML für das Web

JQML ist ein Compiler-Toolchain, das QML-Code in JavaScript transpiliert, sodass dieselbe QML-Codebasis sowohl als native Qt-Desktop-Anwendung als auch als Web-Applikation im Browser ausgeführt werden kann.

| Version | Name | Beschreibung |
|---------|------|-------------|
| **v1** | PureQML | Erste Generation des Web-Compilers |
| **v2** | JQML2 | Erweiterte Version mit 3D-Unterstützung (Three.js), 130+ Komponenten |
| **v3** | JQ | Aktuelle Generation mit eigenem Parser, Source Maps, 60+ QtQml + 50+ QtQuick Typen |

➡️ Details: **[JQML Guide](QML_JQML_Guide.md)**

---

## 📦 Module auf einen Blick

### Kern-UI-Module

| Modul | Komponenten | Beschreibung |
|-------|------------|-------------|
| `imtcontrols` | ~160 | Basis-UI-Framework: Buttons, Inputs, Views, Dialogs, Models, Actions, GraphicsView |
| `imtgui` | ~90 | Applikations-Framework: ApplicationMain, Panels, Params, Settings, Views |
| `imtguigql` | ~35 | GraphQL-Integration: GqlRequest, Subscriptions, Collection-Management |

### Domain-spezifische Module

| Modul | Beschreibung |
|-------|-------------|
| `imtauthgui` | Login, Registration, Tenant-Management, User-Panel |
| `imtchatgui` | Chat-Oberfläche und Messaging |
| `imtcolgui` | Collection-Verwaltung und -Darstellung |
| `imtdeskgui` | Ticket-/Desk-System UI |
| `imtdocgui` | Dokumenten-Management und -Editor |
| `imtlicgui` | Lizenz-Management-Oberfläche |
| `imtgeogui` | Geo-Daten-Visualisierung |
| `imt3dgui` | 3D-Szenen und -Visualisierung |

### Web-spezifisch

| Modul | Beschreibung |
|-------|-------------|
| `web` | Web-spezifische QML-Komponenten für JQML-Builds |

➡️ Details: **[Modul-Referenz](QML_Module_Reference.md)**

---

## 🚀 Schnellstart

```qml
import imtcontrols 1.0
import imtgui 1.0

ApplicationMain {
    PagesManager {
        // Seiten-Definitionen hier
    }
}
```

➡️ Vollständiges Tutorial: **[Getting Started](QML_Getting_Started.md)**

---

## 🔗 Weiterführende Links

- [Include/README.md — User Interface](../../Include/README.md#user-interface-16-libraries-total-7-core--9-domain-specific) — C++ Backend-Bibliotheken
- [Tools/JQML/v2/README-3D.md](../../Tools/JQML/v2/README-3D.md) — JQML2 3D-Dokumentation
- [Tools/JQML/v3/compiler/README.md](../../Tools/JQML/v3/compiler/README.md) — JQML v3 Compiler-Architektur
- [Docs/Tutorials](../Tutorials/) — Bestehende Tutorials und Demos
