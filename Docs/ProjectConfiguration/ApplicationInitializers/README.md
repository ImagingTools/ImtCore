# Application Initializers

This section documents the static function based initialization model for ImtCore.

## Language Versions

- Russian (RU): [README.RU.md](README.RU.md)
- English (EN): [README.EN.md](README.EN.md)

## What Is Covered

- Static initialization entry points and responsibilities
- Macro profiles for common app types:
	- server-auth
	- server-auth-tenant
	- client-auth
	- client-lic
	- full-client
- Domain split into core and UI functions: Base/Auth/Desk/Lic
- Explicit Core/UI calls in client profiles (without intermediate wrappers)
- Tenant extension rules

## Quick Start

- Use `InitializeImtCoreClientApp()` when you need full default startup behavior.
- Use server profiles (`InitializeImtCoreServerAuth`, `InitializeImtCoreServerAuthTenant`) for headless startup.
- Use split client profiles (`InitializeImtCoreClientAuth`, `InitializeImtCoreClientLic`) for partial UI startup.

See full details in the language-specific documents above.
