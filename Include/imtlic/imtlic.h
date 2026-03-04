// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


/**
	\namespace imtlic
	\brief Core licensing domain model providing feature-based product licensing and instance management.
	
	\section overview Overview
	
	The imtlic module implements a comprehensive, feature-based licensing system for software and hardware
	products. It provides a clean separation between product definitions and deployed instances, enabling
	flexible licensing models with temporal control, hardware binding, and dependency management.
	
	\section architecture Architecture
	
	The module uses a three-level abstraction hierarchy:
	
	**Level 1: Definitions** (What can be sold)
	- Products: Container of all available features
	- Features: Atomic units of functionality
	- License Definitions: Specification of which features a license unlocks
	
	**Level 2: Instances** (What was sold and deployed)
	- Product Instances: Deployed product at customer site
	- License Instances: Activated licenses with expiration dates
	
	**Level 3: Management** (How it's controlled)
	- Controllers: Import, validation, and management
	- Providers: Data access and dependency resolution
	- Meta-info Creators: Metadata generation
	
	\section conceptual_model Conceptual Model
	
	\subsection products Products
	A Product (IProductInfo, CProductInfo) represents a software or hardware offering.
	
	**Key Characteristics:**
	- Unique product ID and human-readable name/description
	- Category classification for organization
	- Container of Features (all capabilities the product offers)
	- Part of potential product families
	- Template for creating Product Instances
	
	**Design Pattern:** Container pattern - Products contain Features
	
	\subsection features Features
	A Feature (IFeatureInfo, CFeatureInfo) represents an individual capability or functionality.
	
	**Key Characteristics:**
	- Atomic unit of product functionality
	- Unique feature ID and name
	- Optional vs. Mandatory classification
	- Permission flag for access control features
	- Hierarchical organization (parent-child relationships)
	- Dependency management (feature A requires feature B)
	
	**Feature Types:**
	- **Optional Features**: Functionality that can be sold separately, not required for core operation
	- **Mandatory Features**: Core functionality included in all licenses
	- **Permission Features**: Access control rights (NOT sellable features, but operational permissions)
	
	**Design Pattern:** Composite pattern - Features can form hierarchies
	
	\subsection license_definitions License Definitions
	A License Definition (ILicenseDefinition, CLicenseDefinition) specifies a license type available for purchase.
	
	**Key Characteristics:**
	- Container of Features that this license unlocks
	- Unique license ID and name
	- Belongs to a specific Product
	- Can have dependencies on other licenses
	- Template for creating License Instances
	
	**Conceptual Model:**
	```
	Product = Container of Features
	License Definition = Container of Features (subset of Product features)
	Therefore: Product = Container of License Definitions
	```
	
	**Design Pattern:** Container pattern - License Definitions contain Features
	
	\subsection product_instances Product Instances
	A Product Instance (IProductInstanceInfo, CProductInstanceInfo) represents a deployed product installation.
	
	**Key Characteristics:**
	- References parent Product (N:1 relationship)
	- Unique instance ID (hardware ID, MAC address, etc.)
	- Customer/account association
	- Serial number and project assignment
	- Container of activated License Instances
	- Hardware binding support
	
	**Lifecycle:** Created when product is sold/installed, persists throughout deployment
	
	**Design Pattern:** Definition-Instance pattern separating static definitions from runtime instances
	
	\subsection license_instances License Instances
	A License Instance (ILicenseInstance, CLicenseInstance) represents an activated license.
	
	**Key Characteristics:**
	- Based on License Definition (extends it)
	- Expiration date with goodwill period
	- Cryptographic license keys
	- Validation status (Active, Expired, Grace Period)
	- Instance-specific activation data
	
	**Temporal Control:**
	- Expiration Date: Primary validity period
	- Goodwill Period: Grace period after expiration
	- Unlimited Licenses: Special case with no expiration
	
	**Design Pattern:** Decorator pattern - extends License Definition with temporal data
	
	\subsection hardware_instances Hardware Instances
	Hardware binding support through hardware instance abstractions.
	
	**IHardwareInstanceInfo / CHardwareInstanceInfo:**
	- Physical hardware identification (MAC, serial numbers)
	- Production tracking information
	- Part manufacturing data
	
	**ILicensedHardwareInstanceInfo / CLicensedHardwareInstanceInfo:**
	- Links software licensing to physical hardware
	- Prevents license transfer between hardware units
	- Hardware-locked licensing support
	
	\subsection product_licensing Product Licensing Info
	Product Licensing Info (IProductLicensingInfo, CProductLicensingInfo) provides the complete licensing catalog.
	
	**Key Characteristics:**
	- All available licenses for a product
	- Complete feature catalog
	- License dependencies and relationships
	- Feature dependencies and hierarchies
	
	**Purpose:** Single access point for all licensing information about a product
	
	\section design_patterns Design Patterns Used
	
	\subsection pattern_definition_instance Definition-Instance Pattern
	Separates static definitions from runtime instances:
	- Product → Product Instance (1:N)
	- License Definition → License Instance (1:N)
	- One definition can spawn many instances
	- Centralized definition management
	- Instance-specific customization
	
	\subsection pattern_container Container Pattern
	Products and License Definitions are containers:
	- Product contains Features
	- License Definition contains Features (subset)
	- Hierarchical feature organization
	- Dependency management between contained items
	
	\subsection pattern_provider Provider Pattern
	Data access through provider interfaces:
	- IFeatureInfoProvider - Feature catalog access
	- ILicenseInfoProvider - License definition access
	- ILicenseInstanceProvider - Activated license access
	- IProductInstanceInfoProvider - Product instance access
	- Decouples consumers from data sources
	- Enables dependency injection
	
	\subsection pattern_component Component Pattern (ACF)
	All major classes are ACF components:
	- Factory-based instantiation
	- Interface-based programming
	- Configuration-driven composition
	- Dependency injection support
	
	\section workflow Typical Workflows
	
	\subsection workflow_product_definition Product Definition Workflow
	1. Create Features (atomic capabilities)
	2. Organize Features in hierarchies (if needed)
	3. Define Feature dependencies
	4. Create Product containing all Features
	5. Create License Definitions specifying Feature subsets
	6. Define License dependencies
	7. Create Product Licensing Info (complete catalog)
	
	\subsection workflow_product_deployment Product Deployment Workflow
	1. Customer purchases Product with selected licenses
	2. Create Product Instance with customer info
	3. Generate hardware binding data (if needed)
	4. Activate License Instances on Product Instance
	5. Generate license files with cryptographic keys
	6. Deploy license files to customer site
	7. System validates licenses and enables features
	
	\subsection workflow_license_validation License Validation Workflow
	1. Application starts, loads license files
	2. CLicenseControllerComp imports license files
	3. Cryptographic validation of license keys
	4. Check expiration dates and goodwill periods
	5. Build list of unlocked Features
	6. CLicenseBasedRightsProviderComp provides access control
	7. Application enables/disables features accordingly
	
	\subsection workflow_feature_check Feature Availability Check
	1. Application needs to use a Feature
	2. Query CLicenseBasedRightsProviderComp (implements IRightsProvider)
	3. Check if feature is unlocked by active licenses
	4. Check feature dependencies (if feature A needs feature B)
	5. Enable/disable functionality based on result
	
	\section components Key Components
	
	\subsection comp_controllers Controllers
	- **CLicenseControllerComp**: Client-side license file import, validation, and status checking
	- **CProductControllerComp**: Product data persistence and management
	
	\subsection comp_providers Providers
	- **CFeatureInfoProviderComp**: Access to feature catalog
	- **CLicenseInfoProviderComp**: Access to license definitions
	- **CLicenseBasedRightsProviderComp**: Feature-based access control
	
	\subsection comp_implementations Core Implementations
	- **CProductInfoComp**: Product information implementation
	- **CProductInstanceInfoComp**: Product instance implementation
	- **CProductLicensingInfoComp**: Complete licensing catalog
	
	\subsection comp_metainfo Meta-Information Creators
	- **CFeatureMetaInfoCreatorComp**: Feature metadata
	- **CLicenseMetaInfoCreatorComp**: License metadata
	- **CProductMetaInfoCreatorComp**: Product metadata
	- **CProductInstanceMetaInfoCreatorComp**: Product instance metadata
	- **CProductLicensingInfoMetaInfoCreatorComp**: Licensing catalog metadata
	
	\section interfaces Key Interfaces
	
	All interfaces follow ACF conventions with virtual inheritance and smart pointers.
	
	**Domain Model Interfaces:**
	- IProductInfo, IFeatureInfo, ILicenseDefinition
	- IProductInstanceInfo, ILicenseInstance
	- IProductLicensingInfo
	
	**Provider Interfaces:**
	- IFeatureInfoProvider, ILicenseInfoProvider
	- ILicenseInstanceProvider, IProductInstanceInfoProvider
	- IProductLicensingInfoProvider
	
	**Dependency Management Interfaces:**
	- IFeatureDependenciesManager, IFeatureDependenciesProvider
	- ILicenseDependenciesManager, ILicenseDependenciesProvider
	
	**Control Interfaces:**
	- ILicenseController, ILicenseStatus
	
	**Hardware Interfaces:**
	- IHardwareInstanceInfo, ILicensedHardwareInstanceInfo
	
	**Security Interfaces:**
	- IKeyRing, IEncyptedLicenseKeyProvider, IPartProductionInfo
	
	\section integration Integration Points
	
	**With imtlicgql Module:**
	- Provides GraphQL API layer over imtlic domain model
	- Collection controllers for CRUD operations
	- Metadata delegates for JSON serialization
	
	**With imtlicdb Module:**
	- Database persistence layer
	- SQL/JSON storage in PostgreSQL
	- Query and filtering capabilities
	
	**With Client Applications:**
	- License file import and validation
	- Feature availability queries
	- Access control enforcement
	
	\section documentation Additional Documentation
	
	- **CONCEPTUAL_MODEL.md**: Detailed English conceptual overview
	- **CONCEPTUAL_MODEL_RU.md**: Russian translation
	- **DOKUMENTATION_ZUSAMMENFASSUNG.md**: German documentation summary
	- **LICENSING_ARCHITECTURE.md**: Complete three-module architecture
	
	\see imtlicgql For GraphQL API layer
	\see imtlicdb For database persistence layer
	\ingroup LicenseManagement
*/
namespace imtlic
{


} // namespace imtlic


