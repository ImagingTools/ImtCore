# Application Initializers

This section documents the static function based initialization model for ImtCore.

## Language Versions

- Russian (RU): [README.RU.md](README.RU.md)
- English (EN): [README.EN.md](README.EN.md)

## What Is Covered

- Static initialization entry points and responsibilities
- Macro profiles for common app types:
	- authorizable-server
- Domain split into core and UI functions: Base/Auth/Desk/Lic
- Product-level composition from domain helper functions
- Tenant extension rules

## Quick Start

- Use server profile (`InitializeImtCoreAuthorizableServer`) for headless startup.
- For client apps, create a product initializer and compose the required domain helper calls explicitly.

See full details in the language-specific documents above.
