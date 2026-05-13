# imtlic Module

## Overview

The **imtlic** module is the core licensing domain model for ImtCore, implementing a comprehensive feature-based licensing system for software and hardware products. It provides clean separation between product definitions and deployed instances, enabling flexible licensing models with temporal control, hardware binding, and dependency management.

## Key Features

- **Feature-Based Licensing**: Atomic units of functionality that can be unlocked through licenses
- **Product-Instance Separation**: Clear distinction between product definitions and customer deployments
- **Hierarchical Organization**: Features can form trees, products can be in families
- **Dependency Management**: Features and licenses can depend on each other
- **Temporal Control**: License instances with expiration dates and goodwill periods
- **Hardware Binding**: Support for hardware-locked licenses
- **Access Control**: Permission-based features for application control
- **Extensible Architecture**: ACF component-based design with provider patterns

## Core Concepts

### Products (IProductInfo)
Products represent software or hardware offerings. A product is fundamentally a container of Features.

```cpp
// Example: Office Suite Product
Product "Office Suite"
├─ Feature: Document Editing (Mandatory)
├─ Feature: Spreadsheet (Optional)
└─ Feature: Presentations (Optional)
```

### Features (IFeatureInfo)
Features are atomic units of functionality. They can be:
- **Optional** (sold separately) or **Mandatory** (always included)
- **Capabilities** (sellable features) or **Permissions** (access control)
- Organized in hierarchies with parent-child relationships
- Dependent on other features

```cpp
// Example: Feature Hierarchy
Document Editing
├─ Basic Formatting (Mandatory)
├─ Advanced Formatting (Optional)
└─ Track Changes (Optional)
```

### License Definitions (ILicenseDefinition)
License Definitions specify which features are unlocked when a license is activated.

```cpp
// Example: License Tiers
Basic License:       [Document Editing + Basic Formatting]
Professional:        [Basic Features + Advanced Formatting + Spreadsheet]
Enterprise:          [All Features]
```

### Product Instances (IProductInstanceInfo)
When a product is deployed to a customer, a Product Instance is created with:
- Customer information
- Hardware binding data
- Collection of activated License Instances
- Multi-product support for bulk licensing
- Parent instance reference for license distribution hierarchies

### License Instances (ILicenseInstance)
License Instances are activated licenses based on License Definitions, adding:
- Expiration date and goodwill period
- Cryptographic keys
- Validation status

## Architecture

### Design Patterns

**Definition-Instance Pattern:**
- Separates static definitions (Product, License Definition, Feature) from runtime instances
- Enables centralized definition management with per-customer customization

**Container Pattern:**
- Products contain Features
- License Definitions contain Features (subsets)
- Product Instances contain License Instances

**Provider Pattern:**
- IFeatureInfoProvider - Feature catalog access
- ILicenseInfoProvider - License definition access
- ILicenseInstanceProvider - Activated license access
- IProductInstanceInfoProvider - Product instance access

**Component Pattern (ACF):**
- All major classes are ACF components
- Factory-based instantiation
- Interface-based programming
- Dependency injection support

### Key Components

| Component | Purpose |
|-----------|---------|
| `CLicenseControllerComp` | Client-side license validation & import |
| `CProductControllerComp` | Product data persistence control |
| `CFeatureInfoProviderComp` | Feature catalog access |
| `CLicenseInfoProviderComp` | License definition access |
| `CLicenseBasedRightsProviderComp` | Feature-based access control |
| `CProductInfoComp` | Product information implementation |
| `CProductInstanceInfoComp` | Product instance implementation |
| `CProductLicensingInfoComp` | Complete licensing catalog |

### Meta-Information Creators

Each domain object has a corresponding meta-information creator component for serialization:
- `CFeatureMetaInfoCreatorComp`
- `CLicenseMetaInfoCreatorComp`
- `CProductMetaInfoCreatorComp`
- `CProductInstanceMetaInfoCreatorComp`
- `CProductLicensingInfoMetaInfoCreatorComp`

## Usage Examples

### Creating a Product with Features

```cpp
#include <imtlic/IProductInfo.h>
#include <imtlic/IFeatureInfo.h>

// Create product
IProductInfoSharedPtr product = ...; // From factory
product->SetProductId("product-office-suite");
product->SetName("Office Suite");
product->SetProductDescription("Complete office productivity suite");
product->SetCategoryId("productivity-software");

// Create and add features
IFeatureInfoSharedPtr coreFeature = ...; // From factory
coreFeature->SetFeatureId("feature-document-editing");
coreFeature->SetFeatureName("Document Editing");
coreFeature->SetIsOptional(false);  // Mandatory
product->AddFeature(coreFeature->GetFeatureId(), *coreFeature);

IFeatureInfoSharedPtr optionalFeature = ...; // From factory
optionalFeature->SetFeatureId("feature-spreadsheet");
optionalFeature->SetFeatureName("Spreadsheet Module");
optionalFeature->SetIsOptional(true);  // Optional
product->AddFeature(optionalFeature->GetFeatureId(), *optionalFeature);
```

### Creating a License Definition

```cpp
#include <imtlic/ILicenseDefinition.h>

// Create license
ILicenseDefinitionSharedPtr license = ...; // From factory
license->SetLicenseId("license-professional");
license->SetLicenseName("Professional License");
license->SetLicenseDescription("Full-featured professional edition");
license->SetProductId("product-office-suite");

// Add features to unlock
license->AddFeature("feature-document-editing", "Document Editing");
license->AddFeature("feature-spreadsheet", "Spreadsheet Module");
license->AddFeature("feature-advanced-formatting", "Advanced Formatting");

// Set dependencies (requires basic license first)
QByteArrayList deps;
deps << "license-basic";
license->SetDependsOnLicenseIds(deps);
```

### Creating a Product Instance

```cpp
#include <imtlic/IProductInstanceInfo.h>

// Create product instance for customer deployment
IProductInstanceInfoSharedPtr instance = ...; // From factory
instance->SetProductInstanceId("instance-customer-123");
instance->SetCustomerId("customer-acme-corp");
instance->SetSerialNumber("SN-2024-001");
instance->SetProjectId("project-office-deployment");

// Set parent product
instance->SetProductId("product-office-suite");

// Bind to hardware
instance->SetHardwareId("MAC-00-11-22-33-44-55");
```

### Multi-Product Instances and License Distribution

Multi-product instances allow a single product instance to represent multiple units,
enabling license distribution to other accounts:

```cpp
#include <imtlic/IProductInstanceInfo.h>

// Create a multi-product instance (e.g., 10 licenses purchased)
IProductInstanceInfoSharedPtr parentInstance = ...; // From factory/component system
parentInstance->SetProductInstanceId("instance-bulk-001");
parentInstance->SetCustomerId("customer-reseller");
parentInstance->SetMultiProduct(true);
parentInstance->SetProductCount(10);  // 10 units available

// Distribute some licenses to another account (child instance)
IProductInstanceInfoSharedPtr childInstance = ...; // From factory/component system
childInstance->SetProductInstanceId("instance-customer-456");
childInstance->SetCustomerId("customer-end-user");
childInstance->SetParentInstanceId("instance-bulk-001");  // Link to parent
childInstance->SetProductCount(3);  // 3 units from parent pool

// Parent instance retains remaining units (7 in this case)
// This enables license pool management and tracking
```

**Use Cases for Parent-Child Instances:**
- **Reseller Distribution**: Bulk licenses distributed to end customers
- **Enterprise Licensing**: Department-level license allocation from corporate pool
- **Multi-Site Deployments**: Licenses shared across multiple locations
- **License Pooling**: Centralized license management with distributed usage


### Activating a License Instance

```cpp
#include <imtlic/ILicenseInstance.h>

// Create license instance based on definition
ILicenseInstanceSharedPtr instance = ...; // From factory
instance->SetLicenseId("license-professional");
instance->SetLicenseName("Professional License");

// Set temporal properties
QDate expirationDate = QDate::currentDate().addYears(1);
instance->SetExpirationDate(expirationDate);
instance->SetGoodwillPeriodDays(30);

// Set cryptographic data
instance->SetLicenseKey(cryptographicKey);

// Validate
if (instance->IsValid()) {
    // License is active, unlock features
    QVector<FeatureInfo> features = instance->GetFeatures();
    // Enable each feature in the application
}
```

### Checking Feature Availability

```cpp
#include <imtlic/CLicenseBasedRightsProviderComp.h>

// Get rights provider
CLicenseBasedRightsProviderCompSharedPtr rightsProvider = ...;

// Check if feature is available
QByteArray featureId = "feature-advanced-formatting";
if (rightsProvider->HasFeature(featureId)) {
    // Feature is unlocked by active licenses
    // Enable advanced formatting in UI
} else {
    // Feature not available
    // Show upgrade prompt or disable feature
}
```

## Integration Points

### With imtlicgql Module
The imtlicgql module provides GraphQL API access to imtlic functionality:
- Exposes CRUD operations on Products, Features, Licenses
- Converts between GraphQL representations and imtlic objects
- Provides web-based license management interface

### With imtlicdb Module
The imtlicdb module provides database persistence for imtlic objects:
- Stores Products, Features, Licenses in PostgreSQL
- JSON document-based storage with SQL queries
- Implements provider interfaces for data access

### With Client Applications
Applications integrate imtlic for license enforcement:
- Import and validate license files
- Check feature availability before enabling functionality
- Enforce license-based access control
- Display license status to users

## Best Practices

### Feature Design
- Keep features atomic and well-defined
- Use meaningful, descriptive feature names
- Organize complex features in hierarchies
- Document feature dependencies clearly
- Separate permission features from capability features

### License Design
- Create intuitive license tiers (Basic, Pro, Enterprise)
- Clearly document which features each license includes
- Design upgrade paths between license tiers
- Minimize license dependencies
- Test validation logic thoroughly

### Product Organization
- Use categories to organize related products
- Create product families for product lines
- Keep mandatory features minimal
- Offer flexible optional features
- Document product structure clearly

### Security
- Always validate cryptographic signatures
- Check expiration dates on every use
- Implement goodwill periods appropriately
- Log license validation attempts
- Protect license keys from extraction

## Documentation

- **imtlic.h**: Comprehensive namespace documentation with architecture overview
- **CONCEPTUAL_MODEL.md**: Detailed conceptual model in English
- **CONCEPTUAL_MODEL_RU.md**: Russian translation of conceptual model
- **DOKUMENTATION_ZUSAMMENFASSUNG.md**: German documentation summary
- **LICENSING_ARCHITECTURE.md**: Complete three-module architecture (root directory)

## Building

### CMake
```bash
cd Include/imtlic/CMake
cmake .
make
```

### QMake
```bash
cd Include/imtlic/QMake
qmake
make
```

## Testing

Tests are located in `Include/imtlic/Test/`:
```bash
cd Include/imtlic/Test/CMake
cmake .
make
./imtlicTest
```

## Related Modules

- **imtlicgql**: GraphQL API layer for web-based licensing
- **imtlicdb**: Database persistence layer for PostgreSQL
- **imtbase**: Base collection and object infrastructure
- **imtdb**: Database framework with SQL/JSON support

## License

See LICENSE file in repository root. ImtCore uses multi-licensing:
- LGPL-2.1-or-later
- GPL-2.0-or-later
- GPL-3.0-or-later
- LicenseRef-ImtCore-Commercial

All source files include SPDX license identifier headers.
