// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtgeo/IPosition.h>
#include <imtbase/IIdentifiable.h>


namespace imtgeo
{
/**
	Interface for describing an address element object.
	\ingroup Address
*/
class IAddressElementInfo:
			virtual public iser::IObject,
			virtual public IPosition,
			virtual public imtbase::IIdentifiable
{
public:
	/**
		Get ids of parent the address element.
	*/
	virtual QByteArrayList GetParentIds() const = 0;

	/**
		Set ids of parent the address element.
	*/
	virtual void SetParentIds(const QByteArrayList& parentIds) = 0;
	/**
		Get id address type of element.
	*/
	virtual QByteArray GetAddressTypeId() const = 0;

	/**
		Set id address type of element.
	*/
	virtual void SetAddressTypeId(const QByteArray& typeId) = 0;

	/**
		Get name of the address element.
	*/
	virtual QString GetName() const = 0;

	/**
		Set name of the address element.
	*/
	virtual void SetName(const QString& name) = 0;

	/**
		Get description of the address element.
	*/
	virtual QString GetDescription() const = 0;

	/**
		Set description of the address element.
	*/
	virtual void SetDescription(const QString& description) = 0;

	/**
		Get full address (based on parent id names combined).
	*/
	virtual QString GetFullAddress() const = 0;

	/**
		Set full address (based on parent id names combined).
	*/
	virtual void SetFullAddress(const QString& fullAddress) = 0;
};


using IAddressElementInfoUniquePtr = istd::TUniqueInterfacePtr<IAddressElementInfo>;
using IAddressElementInfoSharedPtr = istd::TSharedInterfacePtr<IAddressElementInfo>;


} // namespace imtgeo

