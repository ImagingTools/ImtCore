// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>


namespace imtgeo
{


/**
	Common interface for an address type info instance.
	\ingroup AddressType
*/
class IAddressTypeInfo: virtual public iser::IObject
{
public:
	/**
		Get id of the address type.
	*/
	virtual QByteArray GetId() const = 0;

	/**
		Set id of the address type.
	*/
	virtual void SetId(QByteArray id) = 0;

	/**
		Get name of type the address type.
	*/
	virtual QString GetName() const = 0;

	/**
		Set name of type the address type.
	*/
	virtual void SetName(QString name) = 0;

	/**
		Get short name of the address type.
	*/
	virtual QString GetShortName() const = 0;

	/**
		Get short name of the address type.
	*/
	virtual void SetShortName(QString sname) = 0;

	/**
		Get description of the address type.
	*/
	virtual QString GetDescription() const = 0;

	/**
		Set description of the address type.	m_name.clear();

	*/
	virtual void SetDescription(QString des) = 0;
};


typedef istd::TUniqueInterfacePtr<IAddressTypeInfo> IAddressTypeInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IAddressTypeInfo> IAddressTypeInfoSharedPtr;


} // namespace imtgeo


