// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtgeo/IPosition.h>


namespace imtgeo
{
/**
	Interface for describing an address element object.
	\ingroup Address
*/
class IAddressElementInfo: virtual public iser::IObject, virtual public IPosition
{
public:

	/**
		Get ids of parent the address element.
	*/
	virtual QList<QByteArray> GetParentIds() const = 0;

	/**
		Set ids of parent the address element.
	*/
	virtual void SetParentIds(const QList<QByteArray>& parentId) = 0;
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
		Get string address.
	*/
	virtual QString GetAddress() const = 0;

	/**
		Set string full address.
	*/
	virtual void SetAddress(const QString& fullAddress) = 0;

	/**
		Get has children.
	*/
	virtual bool GetHasChildren() const = 0;

	/**
		Set has children.
	*/
	virtual void SetHasChildren(const bool& hasChildren) = 0;
};

typedef istd::TUniqueInterfacePtr<IAddressElementInfo> IAddressElementInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IAddressElementInfo> IAddressElementInfoSharedPtr;

} // namespace imtgeo

