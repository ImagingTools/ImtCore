// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtaccount/IAddressProvider.h>


namespace imtaccount
{


/**
	Communication channels of an entity, including its addresses.
	Holds no data about the entity itself: name, picture and everything else belong to IParty.
	\ingroup Account
*/
class IContactInfo: virtual public iser::ISerializable
{
public:
	enum MetaInfoTypes
	{
		/**
			Contact email given as QString.
		*/
		MIT_EMAIL = idoc::IDocumentMetaInfo::MIT_USER + 1,

		/**
			Contact phone number given as QString.
		*/
		MIT_PHONE
	};

	virtual QString GetEmail() const = 0;
	virtual void SetEmail(const QString& email) = 0;
	virtual QString GetPhone() const = 0;
	virtual void SetPhone(const QString& phone) = 0;
	virtual QString GetWebPage() const = 0;
	virtual void SetWebPage(const QString& webPage) = 0;
	virtual IAddressProvider* GetAddresses() const = 0;
};


} // namespace imtaccount


