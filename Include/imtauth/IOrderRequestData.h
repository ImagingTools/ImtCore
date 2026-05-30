// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtauth/IOrderRequest.h>


namespace imtauth
{


/**
	Serializable data holder for a single order request.
	Used as the stored representation of an OrderRequestInfo inside an
	object collection (e.g. an SQL-backed repository).
*/
class IOrderRequestData: virtual public iser::IObject
{
public:
	/**
		Get the full order request description.
	*/
	virtual OrderRequestInfo GetOrderRequestInfo() const = 0;

	/**
		Set the full order request description.
	*/
	virtual void SetOrderRequestInfo(const OrderRequestInfo& orderRequestInfo) = 0;

	/**
		Convenience accessor for the order request ID, which is used as the
		collection element key.
	*/
	virtual QByteArray GetOrderRequestId() const = 0;
};


typedef istd::TUniqueInterfacePtr<IOrderRequestData> IOrderRequestDataUniquePtr;
typedef istd::TSharedInterfacePtr<IOrderRequestData> IOrderRequestDataSharedPtr;


} // namespace imtauth
