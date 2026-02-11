// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtlic
{


/**
	\interface ILicenseDefinition
	\brief Interface for license definition information.
	
	A License Definition represents a specific license type that can be purchased
	and activated for a product. It serves as a "container of Features" - defining
	which features will be unlocked when this license is activated.
	
	\section license_concept Conceptual Model
	
	**License Definition as Feature Container:**
	```
	Product "Office Suite"
	├─ License "Basic"
	│  └─ Unlocks: [Document Editing, Basic Formatting]
	├─ License "Professional"
	│  └─ Unlocks: [Document Editing, Advanced Formatting, Spreadsheet]
	└─ License "Enterprise"
	   └─ Unlocks: [All Features]
	```
	
	**Relationship Model:**
	```
	Product (definition)
	  ├─ Contains: Features (all capabilities)
	  └─ Contains: License Definitions (feature subsets)
	       └─ Each License Definition → unlocks specific Features
	
	Therefore: Product = Container of License Definitions = Container of Features
	```
	
	\section license_lifecycle License Lifecycle
	
	**1. Definition Phase:**
	- Create License Definition with unique ID
	- Assign to specific Product
	- Define which Features this license unlocks
	- Set license dependencies (if required)
	- Add to product's license catalog
	
	**2. Sales Phase:**
	- Customer browses available licenses
	- Compares features included in each license
	- Selects and purchases appropriate license
	- Receives license file or activation key
	
	**3. Activation Phase:**
	- License Instance created based on License Definition
	- Expiration date added (if time-limited)
	- Cryptographic validation applied
	- Installed on Product Instance
	- Features unlocked for use
	
	**4. Runtime Phase:**
	- Application checks which licenses are active
	- Collects all features from active licenses
	- Enables/disables features accordingly
	- Validates license status (expiration, goodwill)
	
	\section license_types License Type Patterns
	
	**Tiered Licensing:**
	```
	Basic License:
	  └─ Core features only
	
	Professional License:
	  └─ Core + Advanced features
	
	Enterprise License:
	  └─ All features + Premium support
	```
	
	**Feature-Based Licensing:**
	```
	Analytics Module License:
	  └─ Unlocks: Analytics features only
	
	Export Module License:
	  └─ Unlocks: Export features only
	
	Combined License:
	  └─ Unlocks: Analytics + Export
	```
	
	**Hardware-Bound Licensing:**
	```
	Single-Machine License:
	  └─ Tied to specific hardware ID
	
	Floating License:
	  └─ Can be moved between machines
	
	Site License:
	  └─ Unlimited machines at location
	```
	
	\section license_dependencies License Dependencies
	
	Licenses can depend on other licenses:
	```
	Enterprise License
	  └─ Depends on: Professional License
	     └─ Depends on: Basic License
	```
	
	**Dependency Validation:**
	1. Check if license is in active licenses list
	2. Check if all dependent licenses are also active
	3. Recursively validate transitive dependencies
	4. License valid only if all dependencies satisfied
	
	**Use Cases:**
	- Add-on licenses requiring base license
	- Upgrade paths (Pro → Enterprise)
	- Module dependencies (Analytics requires Data Import)
	
	\section license_features Feature Management
	
	**Adding Features to License:**
	```cpp
	ILicenseDefinitionSharedPtr license = ...;
	license->AddFeature("feature-analytics-id", "Advanced Analytics");
	license->AddFeature("feature-export-id", "PDF Export");
	```
	
	**Querying License Features:**
	```cpp
	QVector<FeatureInfo> features = license->GetFeatures();
	for (const FeatureInfo& feature : features) {
	    qDebug() << "Feature:" << feature.name << "ID:" << feature.id;
	}
	```
	
	**Checking Feature Inclusion:**
	```cpp
	bool hasAnalytics = license->HasFeature("feature-analytics-id");
	if (hasAnalytics) {
	    // Enable analytics functionality
	}
	```
	
	\section license_metainfo Meta-Information
	
	License Definitions support meta-information:
	- MIT_LICENSE_NAME: QString - Human-readable license name
	- MIT_LICENSE_ID: QByteArray - Unique license identifier
	- MIT_LICENSE_DESCRIPTION: QString - Detailed description
	- MIT_PRODUCT_ID: QByteArray - Parent product identifier
	
	\section license_validation License Validation
	
	**Validation Steps:**
	1. Parse license file or activation key
	2. Verify cryptographic signature
	3. Check license ID against catalog
	4. Validate product ID matches
	5. Check expiration date (for instances)
	6. Verify hardware binding (if applicable)
	7. Validate dependencies are satisfied
	8. Unlock features if valid
	
	**License Status:**
	- **Valid**: All checks passed, features unlocked
	- **Expired**: Past expiration date, features locked
	- **Grace Period**: Expired but within goodwill period
	- **Invalid**: Signature or validation failed
	- **Missing Dependencies**: Required licenses not active
	
	\section license_usage Usage Examples
	
	**Creating a License Definition:**
	```cpp
	ILicenseDefinitionSharedPtr license = ...;  // From factory
	license->SetLicenseId("license-professional");
	license->SetLicenseName("Professional License");
	license->SetLicenseDescription("Full-featured professional edition");
	license->SetProductId("product-office-suite");
	
	// Add features
	license->AddFeature("feature-core-editing", "Core Editing");
	license->AddFeature("feature-advanced-format", "Advanced Formatting");
	license->AddFeature("feature-spreadsheet", "Spreadsheet Module");
	
	// Set dependencies
	QByteArrayList deps;
	deps << "license-basic";  // Requires basic license
	license->SetDependsOnLicenseIds(deps);
	```
	
	**Checking License Features:**
	```cpp
	// Check if specific feature is included
	if (license->HasFeature("feature-advanced-format")) {
	    qDebug() << "Advanced formatting included";
	}
	
	// Get all features
	QVector<FeatureInfo> features = license->GetFeatures();
	qDebug() << "License includes" << features.size() << "features";
	
	// Check dependencies
	QByteArrayList deps = license->GetDependsOnLicenseIds();
	for (const QByteArray& depId : deps) {
	    // Validate dependency license is available
	}
	```
	
	**Creating License Instance:**
	```cpp
	// License Definition → License Instance
	ILicenseDefinitionSharedPtr definition = ...;  // Purchased license type
	ILicenseInstanceSharedPtr instance = ...;  // From factory
	
	// Copy definition properties
	instance->SetLicenseId(definition->GetLicenseId());
	instance->SetLicenseName(definition->GetLicenseName());
	// Add temporal properties
	instance->SetExpirationDate(QDate::currentDate().addYears(1));
	instance->SetGoodwillPeriodDays(30);
	```
	
	\section license_best_practices Best Practices
	
	**License Design:**
	- Keep license structure simple and intuitive
	- Clearly communicate which features each license includes
	- Use meaningful license names (Basic, Pro, Enterprise)
	- Document license dependencies clearly
	- Consider upgrade paths between licenses
	
	**Feature Assignment:**
	- Group related features together
	- Mandatory features in all licenses
	- Optional features in higher-tier licenses
	- Permission features separate from capabilities
	- Avoid feature fragmentation
	
	**Dependency Management:**
	- Minimize dependency chains
	- Document why dependencies exist
	- Test dependency validation thoroughly
	- Consider circular dependency prevention
	
	**Product Integration:**
	- All licenses must reference valid product
	- All features must exist in product catalog
	- Validate feature IDs during license creation
	- Keep product-license relationship consistent
	
	\section license_relationships Related Interfaces
	
	- **IProductInfo**: Product this license belongs to
	- **IFeatureInfo**: Features unlocked by this license
	- **ILicenseInstance**: Runtime instance based on this definition
	- **IProductLicensingInfo**: Complete license catalog
	- **ILicenseInfoProvider**: Access to license definitions
	- **CLicenseControllerComp**: License validation and import
	
	\sa IFeatureInfo, IProductInfo, ILicenseInstance, IProductLicensingInfo
	\ingroup LicenseManagement
*/
class ILicenseDefinition: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		/**
			License Name given as QString.
		*/
		MIT_LICENSE_NAME = idoc::IDocumentMetaInfo::MIT_USER + 1,

		/**
			License-ID given as QByteArray.
		*/
		MIT_LICENSE_ID,

		/**
			License Description given as QString.
		*/
		MIT_LICENSE_DESCRIPTION,
		
		/**
			Product-ID given as QByteArray.
		*/
		MIT_PRODUCT_ID
	};

	struct FeatureInfo
	{
		QByteArray id;
		QString name;

		bool operator==(const FeatureInfo& other) const
		{
			return (id == other.id) && (name == other.name);
		}

		bool operator!=(const FeatureInfo& other) const
		{
			return !operator==(other);
		}
	};

	typedef QVector<FeatureInfo> FeatureInfos;

	/**
		Get the human-readable name of the license.
		\return Display name of this license type
	*/
	virtual QString GetLicenseName() const = 0;

	/**
		Set the name of the license.
		\param licenseName Human-readable name for this license
	*/
	virtual void SetLicenseName(const QString& licenseName) = 0;

	/**
		Get the license ID used for identification during license validation.
		This ID is used by the rights provider system to check license validity.
		\return Unique identifier for this license type
		\sa iauth::IRightsProvider
	*/
	virtual QByteArray GetLicenseId() const = 0;
	
	/**
		Set the unique ID of the license.
		\param licenseId Unique identifier for license validation
	*/
	virtual void SetLicenseId(const QByteArray& licenseId) = 0;

	/**
		Get the detailed description of the license.
		\return Description explaining what this license provides
	*/
	virtual QString GetLicenseDescription() const = 0;

	/**
		Set the description of the license.
		\param licenseDescription Detailed description of the license
	*/
	virtual void SetLicenseDescription(const QString& licenseDescription) = 0;

	/**
		Get the product ID that this license belongs to.
		\return ID of the product this license is associated with
	*/
	virtual QByteArray GetProductId() const = 0;

	/**
		Set the product ID that this license belongs to.
		\param productId ID of the associated product
	*/
	virtual void SetProductId(const QByteArray& productId) = 0;

	/**
		Get the list of licenses that this license depends on.
		Dependent licenses must be activated before this license can be used.
		\return List of license IDs that this license depends on
	*/
	virtual QByteArrayList GetDependencies() const = 0;

	/**
		Set the license dependencies.
		\param dependencies List of license IDs that must be active for this license to work
	*/
	virtual void SetDependencies(QByteArrayList dependencies) = 0;

	/**
		Get the list of features that this license unlocks.
		This defines which product capabilities are enabled when this license is activated.
		\return List of feature information (ID and name) enabled by this license
	*/
	virtual FeatureInfos GetFeatureInfos() const = 0;

	/**
		Set the list of features that this license unlocks.
		\param featureInfos List of features enabled by this license
	*/
	virtual void SetFeatureInfos(const FeatureInfos& featureInfos) = 0;
};


typedef istd::TUniqueInterfacePtr<ILicenseDefinition> ILicenseDefinitionUniquePtr;
typedef istd::TSharedInterfacePtr<ILicenseDefinition> ILicenseDefinitionSharedPtr;


} // namespace imtlic


