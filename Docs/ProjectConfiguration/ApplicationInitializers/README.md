# Application Initializers

This section documents the new modular application initialization model for ImtCore.

## Language Versions

- Russian (RU): [README.RU.md](README.RU.md)
- English (EN): [README.EN.md](README.EN.md)

## What Is Covered

- Why legacy static initialization was replaced with composable initializers
- Initialization architecture and responsibilities
- Aggregating and domain-specific initializer classes
- Usage patterns in applications
- Extension rules for adding new initializers

## Quick Start

Use `imtcore::CDefaultImtCoreQmlInitializer` when you need full default startup behavior.
Use domain aggregators (for example `imtcore::CImtCoreAuthInitializer`) when only a subsystem should be initialized.

See full details in the language-specific documents above.
