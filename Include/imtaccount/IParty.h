// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iimg/IBitmap.h>
#include <iser/IObject.h>
#include <istd/TPointerVector.h>

// ImtCore includes
#include <imtaccount/IContactInfoProvider.h>


namespace imtaccount
{


/**
	A subject able to take part in business relations.
	Describes who the subject is; the role it plays (customer, supplier, ...) is modelled separately.
	Implemented by IPersonInfo and ICompanyInfo.
	\ingroup Account
*/
class IParty:
	virtual public IContactInfoProvider,
	virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		/**
			Display name given as QString.
		*/
		MIT_DISPLAY_NAME = idoc::IDocumentMetaInfo::MIT_USER + 1,

		/**
			Description given as QString.
		*/
		MIT_DESCRIPTION
	};

	/**
		Get name used to present this party.
		Composed by the implementation from the party specific name fields.
	*/
	virtual QString GetDisplayName() const = 0;

	virtual QString GetDescription() const = 0;
	virtual void SetDescription(const QString& description) = 0;
	virtual const iimg::IBitmap& GetPicture() const = 0;
	virtual void SetPicture(const iimg::IBitmap& picture) = 0;
};


typedef istd::TPointerVector<const IParty> PartyList;


} // namespace imtaccount
