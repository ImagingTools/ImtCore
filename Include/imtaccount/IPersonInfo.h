// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDate>

// ACF includes
#include <istd/TPointerVector.h>

// ImtCore includes
#include <imtaccount/IParty.h>


namespace imtaccount
{


/**
	A natural person.
	\ingroup Account
*/
class IPersonInfo: virtual public IParty
{
public:
	enum GenderType
	{
		GT_DIVERSE, // :)
		GT_FEMALE,
		GT_MALE
	};

	I_DECLARE_ENUM(GenderType, GT_DIVERSE, GT_FEMALE, GT_MALE);

	enum NameFieldType
	{
		NFT_FIRST_NAME,
		NFT_LAST_NAME,
		NFT_NICKNAME,
		NFT_SALUTATION,
		NFT_TITLE
	};

	I_DECLARE_ENUM(NameFieldType, NFT_FIRST_NAME, NFT_LAST_NAME, NFT_NICKNAME, NFT_SALUTATION, NFT_TITLE);

	enum MetaInfoTypes
	{
		/**
			Gender type given as GenderType.
		*/
		MIT_GENDER_TYPE = IParty::MIT_DESCRIPTION + 1,

		/**
			Birthday given as QDate.
		*/
		MIT_BIRTHDAY,

		/**
			First name given as QString.
		*/
		MIT_FIRST_NAME,

		/**
			Last name given as QString.
		*/
		MIT_LAST_NAME,

		/**
			Nickname given as QString.
		*/
		MIT_NICKNAME
	};

	virtual GenderType GetGenderType() const = 0;
	virtual void SetGenderType(GenderType genderType) = 0;
	virtual QDate GetBirthday() const = 0;
	virtual void SetBirthday(const QDate& birthday) = 0;
	virtual QString GetNameField(NameFieldType fieldType) const = 0;
	virtual void SetNameField(NameFieldType fieldType, const QString& value) = 0;
};


} // namespace imtaccount


