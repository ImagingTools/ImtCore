// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/INameParam.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureInfoProvider.h>


namespace imtlic
{


/**
	\interface IProductInfo
	\brief Interface for accessing information about a product (software or hardware).
	
	A Product represents a software or hardware offering and is fundamentally defined as
	a collection of Features. From a licensing perspective, a Product also contains all
	available License Definitions, where each license "unlocks" a specific set of features.
	
	\section product_concept Conceptual Model
	
	**Product as Feature Container:**
	```
	Product
	├─ Feature 1 (Mandatory)
	├─ Feature 2 (Optional)
	├─ Feature 3 (Optional)
	│  ├─ Child Feature 3.1
	│  └─ Child Feature 3.2
	└─ Feature 4 (Permission)
	```
	
	**Product as License Container:**
	```
	Product
	├─ License Definition "Basic" → unlocks Features {1}
	├─ License Definition "Professional" → unlocks Features {1, 2, 3}
	└─ License Definition "Enterprise" → unlocks Features {1, 2, 3, 4}
	```
	
	\section product_lifecycle Product Lifecycle
	
	**1. Definition Phase:**
	- Create Product with unique ID, name, description
	- Assign to category for organization
	- Add Features that define capabilities
	- Create License Definitions that unlock feature subsets
	
	**2. Catalog Phase:**
	- Product becomes part of product catalog
	- Available for sale to customers
	- Marketing materials reference product features
	- Pricing based on license types
	
	**3. Deployment Phase:**
	- Customer purchases product with selected licenses
	- Product Instance created (see IProductInstanceInfo)
	- License Instances activated on Product Instance
	- Features enabled based on active licenses
	
	\section product_hierarchy Product Families
	
	Products can be organized in families:
	- Parent product defines common features
	- Child products inherit or extend features
	- Simplifies product management
	- Example: "Office Suite" → "Office Home", "Office Professional", "Office Enterprise"
	
	\section product_responsibilities Key Responsibilities
	
	- **Maintains product identification:**
	  - Unique Product ID (UUID or custom identifier)
	  - Human-readable name and description
	  - Category classification for organization
	
	- **Manages feature collection:**
	  - Container of all product Features
	  - Feature addition and removal
	  - Feature hierarchy management
	  - Feature dependency tracking (via IFeatureInfo)
	
	- **Supports product families:**
	  - Optional parent-child relationships
	  - Feature inheritance
	  - Hierarchical organization
	
	\section product_metainfo Meta-Information
	
	Products support meta-information for persistence and serialization:
	- MIT_PRODUCT_NAME: QString - Human-readable product name
	- MIT_PRODUCT_ID: QByteArray - Unique product identifier
	- MIT_PRODUCT_DESCRIPTION: QString - Detailed product description
	- MIT_PRODUCT_CATEGORY: QByteArray - Category classification
	
	\section product_usage Usage Example
	
	```cpp
	// Create a product
	IProductInfoSharedPtr product = ...;  // From factory
	product->SetProductId("product-enterprise-suite");
	product->SetName("Enterprise Suite");
	product->SetProductDescription("Complete enterprise solution");
	product->SetCategoryId("software");
	
	// Add features
	IFeatureInfoSharedPtr featureCore = ...;
	product->AddFeature("feature-core", *featureCore);
	
	IFeatureInfoSharedPtr featureAdvanced = ...;
	product->AddFeature("feature-advanced", *featureAdvanced);
	
	// Access features
	imtbase::IObjectCollection* features = product->GetFeatures();
	int featureCount = features->GetCount();
	```
	
	\section product_relationships Related Interfaces
	
	- **IFeatureInfo**: Individual capabilities contained in this product
	- **ILicenseDefinition**: License types that unlock feature subsets
	- **IProductInstanceInfo**: Deployed instance of this product
	- **IProductLicensingInfo**: Complete licensing catalog for this product
	- **IProductInfoProvider**: Access to product catalog
	
	\sa IFeatureInfo, ILicenseDefinition, IProductInstanceInfo, IProductLicensingInfo
	\ingroup LicenseManagement
*/
class IProductInfo: virtual public iprm::INameParam
{
public:
	enum MetaInfoTypes
	{
		/**
			Product Name given as QString.
		*/
		MIT_PRODUCT_NAME = idoc::IDocumentMetaInfo::MIT_USER,

		/**
			Product-ID given as QByteArray.
		*/
		MIT_PRODUCT_ID,

		/**
			Product Description given as QString.
		*/
		MIT_PRODUCT_DESCRIPTION,

		/**
			Product category given as QByteArray.
		*/
		MIT_PRODUCT_CATEGORY
	};

	/**
		Get information about the related product family of this product, if available.
	*/
	virtual const IProductInfo* GetProductFamilyInfo() const = 0;

	/**
		Get a unique ID of this product.
	*/
	virtual QByteArray GetProductId() const = 0;

	/**
		Set product-ID.
	*/
	virtual void SetProductId(const QByteArray& productId) = 0;

	/**
		Get a description of this product.
	*/
	virtual QString GetProductDescription() const = 0;

	/**
		Set product description.
	*/
	virtual void SetProductDescription(const QString& description) = 0;

	/**
		Get a category ID of this product.
	*/
	virtual QByteArray GetCategoryId() const = 0;

	/**
		Set category-ID.
	*/
	virtual void SetCategoryId(const QByteArray& categoryId) = 0;

	/**
		Get the collection of features that define this product's capabilities.
		A Product is essentially a container of Features.
	*/
	virtual imtbase::IObjectCollection* GetFeatures() = 0;

	/**
		Add a feature to this product's feature collection.
		\param featureId Unique identifier for the feature
		\param featureInfo Feature information to be added
		\return true if the feature was successfully added, false otherwise
	*/
	virtual bool AddFeature(const QByteArray& featureId, const IFeatureInfo& featureInfo) = 0;

	/**
		Remove a feature from this product's feature collection.
		\param featureId Unique identifier of the feature to remove
		\return true if the feature was successfully removed, false otherwise
	*/
	virtual bool RemoveFeature(const QByteArray& featureId) = 0;
};


typedef istd::TUniqueInterfacePtr<IProductInfo> IProductInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IProductInfo> IProductInfoSharedPtr;


} // namespace imtlic


