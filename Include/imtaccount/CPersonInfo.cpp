// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtaccount/CPersonInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtaccount
{


// public static methods

QByteArray CPersonInfo::GetTypeId()
{
	return QByteArrayLiteral("PersonInfo");
}


// public methods

CPersonInfo::CPersonInfo()
	:m_genderType(GT_MALE)
{
}


// reimplemented (IPersonInfo)

IPersonInfo::GenderType CPersonInfo::GetGenderType() const
{
	return m_genderType;
}


void CPersonInfo::SetGenderType(GenderType genderType)
{
	if (m_genderType != genderType){
		istd::CChangeNotifier changeNotifier(this);

		m_genderType = genderType;
	}
}


QDate CPersonInfo::GetBirthday() const
{
	return m_birthday;
}


void CPersonInfo::SetBirthday(const QDate& birthday)
{
	if (m_birthday != birthday){
		istd::CChangeNotifier changeNotifier(this);

		m_birthday = birthday;
	}
}


QString CPersonInfo::GetNameField(NameFieldType fieldType) const
{
	switch (fieldType){
	case NFT_FIRST_NAME:
		return m_firstName;
	case NFT_LAST_NAME:
		return m_lastName;
	case NFT_NICKNAME:
		return m_nickName;
	case NFT_SALUTATION:
		return m_salutation;
	case NFT_TITLE:
		return m_title;
	default:
		return QString();
	}
}


void CPersonInfo::SetNameField(NameFieldType fieldType, const QString& value)
{
	if (GetNameField(fieldType) == value){
		return;
	}

	istd::CChangeNotifier changeNotifier(this);

	switch (fieldType){
	case NFT_FIRST_NAME:
		m_firstName = value;
		break;
	case NFT_LAST_NAME:
		m_lastName = value;
		break;
	case NFT_NICKNAME:
		m_nickName = value;
		break;
	case NFT_SALUTATION:
		m_salutation = value;
		break;
	case NFT_TITLE:
		m_title = value;
		break;
	default:
		Q_ASSERT(false);
		break;
	}
}


// reimplemented (iser::IObject)

QByteArray CPersonInfo::GetFactoryId() const
{
	return GetTypeId();
}


// reimplemented (iser::ISerializable)

bool CPersonInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag genderTypeTag(QByteArrayLiteral("GenderType"), QByteArrayLiteral("Gender type"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(genderTypeTag);
	retVal = retVal && I_SERIALIZE_ENUM(GenderType, archive, m_genderType);
	retVal = retVal && archive.EndTag(genderTypeTag);

	static iser::CArchiveTag birthdayTag(QByteArrayLiteral("Birthday"), QByteArrayLiteral("Birthday"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(birthdayTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDate(archive, m_birthday);
	retVal = retVal && archive.EndTag(birthdayTag);

	static iser::CArchiveTag firstNameTag(QByteArrayLiteral("FirstName"), QByteArrayLiteral("First name"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(firstNameTag);
	retVal = retVal && archive.Process(m_firstName);
	retVal = retVal && archive.EndTag(firstNameTag);

	static iser::CArchiveTag lastNameTag(QByteArrayLiteral("LastName"), QByteArrayLiteral("Last name"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(lastNameTag);
	retVal = retVal && archive.Process(m_lastName);
	retVal = retVal && archive.EndTag(lastNameTag);

	static iser::CArchiveTag nickNameTag(QByteArrayLiteral("NickName"), QByteArrayLiteral("Nickname"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nickNameTag);
	retVal = retVal && archive.Process(m_nickName);
	retVal = retVal && archive.EndTag(nickNameTag);

	static iser::CArchiveTag salutationTag(QByteArrayLiteral("Salutation"), QByteArrayLiteral("Salutation"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(salutationTag);
	retVal = retVal && archive.Process(m_salutation);
	retVal = retVal && archive.EndTag(salutationTag);

	static iser::CArchiveTag titleTag(QByteArrayLiteral("Title"), QByteArrayLiteral("Title"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(titleTag);
	retVal = retVal && archive.Process(m_title);
	retVal = retVal && archive.EndTag(titleTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CPersonInfo::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_CLONE | SO_RESET;
}


bool CPersonInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CPersonInfo* sourcePtr = dynamic_cast<const CPersonInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_genderType = sourcePtr->m_genderType;
	m_birthday	 = sourcePtr->m_birthday;
	m_firstName	 = sourcePtr->m_firstName;
	m_lastName	 = sourcePtr->m_lastName;
	m_nickName	 = sourcePtr->m_nickName;
	m_salutation = sourcePtr->m_salutation;
	m_title		 = sourcePtr->m_title;

	return true;
}


bool CPersonInfo::IsEqual(const IChangeable& object) const
{
	const CPersonInfo* sourcePtr = dynamic_cast<const CPersonInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	bool retVal = m_genderType == sourcePtr->m_genderType;
	retVal = retVal && m_birthday == sourcePtr->m_birthday;
	retVal = retVal && m_firstName == sourcePtr->m_firstName;
	retVal = retVal && m_lastName == sourcePtr->m_lastName;
	retVal = retVal && m_nickName == sourcePtr->m_nickName;
	retVal = retVal && m_salutation == sourcePtr->m_salutation;
	retVal = retVal && m_title == sourcePtr->m_title;

	return retVal;
}


istd::IChangeableUniquePtr CPersonInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CPersonInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CPersonInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_genderType = GT_MALE;
	m_birthday = QDate();
	m_firstName.clear();
	m_lastName.clear();
	m_nickName.clear();
	m_salutation.clear();
	m_title.clear();

	return true;
}


} // namespace imtaccount

