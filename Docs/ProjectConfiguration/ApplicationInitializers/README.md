# Application Initializers

This section documents the static function based initialization model for ImtCore.

## Language Versions

- Russian (RU): [README.RU.md](README.RU.md)
- English (EN): [README.EN.md](README.EN.md)

## What Is Covered

- Static initialization entry points and responsibilities
- Macro profiles for common app types: server-auth, server-lic, server-auth-lic, client
- Domain-level resource initialization functions
- Usage patterns in applications

## Quick Start

Use `imtcore::InitializeDefaultImtCoreQml()` when you need full default startup behavior.
Use domain functions (for example `imtcore::InitializeImtCoreAuth()`) when only a subsystem should be initialized.

See full details in the language-specific documents above.
