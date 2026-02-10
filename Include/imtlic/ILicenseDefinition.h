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
	Interface for license definition information.
	
	A License Definition represents a specific license type that can be purchased
	and activated for a product. It serves as a "container of Features" - defining
	which features will be unlocked when this license is activated.
	
	From a licensing perspective:
	- A Product contains all available License Definitions
	- Each License Definition specifies which Features it enables
	- Therefore, a Product (through its licenses) is ultimately a container of Features
	
	When a customer purchases a license, a License Instance is created based on
	this License Definition, which may include activation date and expiration date.
	
	Key characteristics:
	- Belongs to a specific Product
	- Contains a list of Features it unlocks
	- Can have dependencies on other licenses
	- Identified by a unique license ID used during license validation
	
	\sa IFeatureInfo, IProductInfo, ILicenseInstance
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


