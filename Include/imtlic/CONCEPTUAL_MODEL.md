# ImtLic Module - Conceptual Model and Documentation

## Overview

The `imtlic` module implements a comprehensive licensing system for software and hardware products. This document explains the conceptual model and the relationships between the main abstractions.

## Core Concepts

### 1. Product (IProductInfo, CProductInfo)

A **Product** represents a software or hardware offering from the company. Products are fundamentally defined as collections of **Features**.

**Key Properties:**
- Unique product ID
- Human-readable name and description
- Category classification
- Collection of Features that define capabilities

**Conceptual Role:**
- Container of Features
- Through licenses, also a container of License Definitions
- Part of a potential product family hierarchy

### 2. Feature (IFeatureInfo, CFeatureInfo)

A **Feature** represents an individual capability or functionality within a product. Features are the atomic units that define what a product can do.

**Key Properties:**
- Unique feature ID
- Human-readable name and description
- Optional vs. mandatory flag
- Permission flag (for access control)
- Dependencies on other features
- Hierarchical parent-child relationships

**Conceptual Role:**
- Atomic unit of product functionality
- Can be organized in hierarchies (feature trees)
- "Unlocked" through licenses

**Important Distinctions:**
- **Optional Features** - functionality that can be sold optionally. They are NOT mandatory for the core functionality of the product and can be purchased separately by customers based on their needs. This allows flexible product configurations.
- **Permission Features** - these are NOT sellable features. They represent access control rights or operational permissions within the application, not product capabilities. Example: a "Close" permission controls whether the application can be exited, which is an operational right, not a sellable product feature.

### 3. License Definition (ILicenseDefinition, CLicenseDefinition)

A **License Definition** represents a license type that can be purchased. It is essentially a **container of Features** - it specifies which features will be unlocked when this license is activated.

**Key Properties:**
- Unique license ID
- Human-readable name and description
- Associated product ID
- List of Features it unlocks
- Dependencies on other licenses

**Conceptual Role:**
- Defines what a customer gets when purchasing this license
- Maps purchases to feature availability
- Can require other licenses as prerequisites

**Key Insight:**
```
Product → contains License Definitions → contain Features
Therefore: Product → contains Features
```

### 4. Product Instance (IProductInstanceInfo, CProductInstanceInfo)

A **Product Instance** is created when a product is sold, installed, or deployed to a customer. It represents a specific installation of a product.

**Key Properties:**
- Unique instance ID (e.g., MAC address, hardware ID)
- Link to product definition (product ID)
- Customer information (customer ID)
- Serial number and project assignment
- Collection of activated License Instances
- Usage status and flags

**Conceptual Role:**
- Represents a specific product installation
- Manages which licenses are activated
- Provides access to available licenses for this installation
- Links product definitions to actual deployments

### 5. License Instance (ILicenseInstance, CLicenseInstance)

A **License Instance** is an activated license based on a License Definition. When a customer purchases a license, it becomes a License Instance on their Product Instance.

**Key Properties:**
- Inherits all properties from License Definition
- Expiration date/time (or perpetual)
- Instance-specific activation data

**Conceptual Role:**
- Represents an actual, activated license (not just a license type)
- Adds temporal information to license definitions
- Managed within a Product Instance

### 6. Product Licensing Info (IProductLicensingInfo, CProductLicensingInfo)

**Product Licensing Info** provides access to all available License Definitions for a specific product. It serves as the complete licensing catalog.

**Key Properties:**
- Collection of all License Definitions for a product
- Product identification

**Conceptual Role:**
- Complete catalog of licensing options for a product
- Used to browse available license types
- Source for license activation

## Typical Workflow

1. **Product Definition Phase:**
   - Define a Product with its Features
   - Create License Definitions specifying which Features each license unlocks
   - Establish dependencies between features and licenses

2. **Sales/Deployment Phase:**
   - Customer purchases specific licenses
   - Product Instance is created for the installation
   - License Instances are activated based on purchases

3. **Runtime/Validation Phase:**
   - System validates available features based on active License Instances
   - Feature availability determines application capabilities
   - Expiration dates are checked for time-limited licenses

## Supporting Interfaces

### Provider Interfaces

- **IFeatureInfoProvider**: Provides access to feature catalogs
- **ILicenseInfoProvider**: Provides access to license definition catalogs  
- **ILicenseInstanceProvider**: Provides access to activated licenses
- **IProductInstanceInfoProvider**: Provides access to all product instances

### Hardware-Specific Interfaces

- **IHardwareInstanceInfo**: Represents hardware product instances
- **ILicensedHardwareInstanceInfo**: Hardware instances linked to software licensing

### Validation and Control

- **ILicenseStatus**: License validation status and expiration information
- **ILicenseController**: Manages license file import and validation

## Relationships Diagram

```
Product
  ├── Contains: Features (1:N)
  └── Through: License Definitions (1:N)
       └── Unlock: Features (N:M)

Product Instance (created when product is sold)
  ├── References: Product (N:1)
  ├── References: Customer (N:1)
  └── Contains: License Instances (1:N)
       └── Based on: License Definition (N:1)
            └── Unlocks: Features (N:M)

License Instance extends License Definition
  └── Adds: Expiration Date
```

## Key Design Principles

1. **Separation of Concerns:**
   - Product definitions are separate from product instances
   - License definitions are separate from license instances
   - Features are independent units

2. **Container Pattern:**
   - Products contain Features
   - License Definitions contain Features
   - Product Instances contain License Instances

3. **Flexibility:**
   - Features can be hierarchical
   - Licenses can depend on other licenses
   - Features can depend on other features

4. **Temporal Control:**
   - License Instances can have expiration dates
   - Goodwill periods allow grace time after expiration

## Implementation Notes

All interfaces follow ACF (Application Component Framework) patterns:
- Virtual inheritance for interface composition
- Smart pointer typedefs (Unique/Shared)
- Serialization support through IArchive
- Change tracking through IChangeable
- Meta-information through enum constants

## Documentation Improvements

This documentation update includes:
- Comprehensive namespace documentation in `imtlic.h`
- Detailed interface documentation for all core concepts
- Improved method documentation with parameter descriptions
- Cross-references between related interfaces
- Clarification of the conceptual model throughout
- Explicit emphasis on the distinction between optional features and permission features

---

**Note:** This document is synchronized with the Russian version (CONCEPTUAL_MODEL_RU.md) and German version (DOKUMENTATION_ZUSAMMENFASSUNG.md). When making changes to one version, all language versions must be updated accordingly.
