# Application Initializers

This section documents the static function based initialization model for ImtCore.

## Language Versions

- Russian (RU): [README.RU.md](README.RU.md)
- English (EN): [README.EN.md](README.EN.md)

## What Is Covered

- Static initialization entry points and responsibilities
- Macro profiles for common app types:
	- server-auth
- Domain split into core and UI functions: Base/Auth/Desk/Lic
- Explicit Core/UI calls in client profiles (without intermediate wrappers)
- Tenant extension rules

## Quick Start

- Use server profile (`InitializeImtCoreServerAuth`) for headless startup.
- For client apps, create a product initializer and compose the required domain helper calls explicitly.

See full details in the language-specific documents above.
