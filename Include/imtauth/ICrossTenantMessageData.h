// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtauth/ICrossTenantMessage.h>


namespace imtauth
{


/**
	Serializable data holder for a single cross-tenant message.
	Used as the stored representation of a CrossTenantMessageInfo inside an
	object collection (e.g. an SQL-backed repository).
*/
class ICrossTenantMessageData: virtual public iser::IObject
{
public:
	/**
		Get the full message description.
	*/
	virtual CrossTenantMessageInfo GetMessageInfo() const = 0;

	/**
		Set the full message description.
	*/
	virtual void SetMessageInfo(const CrossTenantMessageInfo& messageInfo) = 0;

	/**
		Convenience accessor for the message ID, which is used as the
		collection element key.
	*/
	virtual QByteArray GetMessageId() const = 0;
};


typedef istd::TUniqueInterfacePtr<ICrossTenantMessageData> ICrossTenantMessageDataUniquePtr;
typedef istd::TSharedInterfacePtr<ICrossTenantMessageData> ICrossTenantMessageDataSharedPtr;


} // namespace imtauth
