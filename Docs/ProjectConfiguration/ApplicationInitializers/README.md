# Application Initializers

This section documents the static function based initialization model for ImtCore.

## Language Versions

- Russian (RU): [README.RU.md](README.RU.md)
- English (EN): [README.EN.md](README.EN.md)

## What Is Covered

- Static initialization entry points and responsibilities
- Macro profiles for common app types: server-auth, server-lic, server-auth-lic, client-auth, client-lic, client-auth-lic, full-client
- Domain split into core and UI functions: Base/Auth/Desk/Lic
- Dedicated client standard initializers in separate `*UiInitializer.h` files
- Usage patterns in applications

## Quick Start

- Use `imtcore::InitializeImtCoreClientApp()` when you need full default startup behavior.
- Use server profiles (`InitializeImtCoreServerAuth`, `InitializeImtCoreServerLic`, `InitializeImtCoreServerAuthLic`) for headless startup.
- Use split client profiles (`InitializeImtCoreClientAuth`, `InitializeImtCoreClientLic`, `InitializeImtCoreClientAuthLic`) for partial UI startup.
- Use full client profile (`InitializeImtCoreClientApp`) for full UI startup including Desk.

See full details in the language-specific documents above.
