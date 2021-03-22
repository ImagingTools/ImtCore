#include <imtauth/CPersonInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public static methods

QByteArray CPersonInfo::GetTypeId()
{
	return "PersonInfo";
}


// public methods

CPersonInfo::CPersonInfo()
	:m_genderType(GT_DIVERSE), // :)
	m_firstName("DefaultFirstName"),
	m_lastName("DefaultLastName"),
	m_nickName("DefaultNickname")
{
	m_birthday = QDate(1, 1, 2020);
}


// reimplemented (IAddress)

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
	switch(fieldType){
	case NFT_FIRST_NAME:
		return m_firstName;
	case NFT_LAST_NAME:
		return m_lastName;
	case NFT_NICKNAME:
		return m_nickName;
	default:
		return QString();
	};
}


void CPersonInfo::SetNameField(NameFieldType fieldType, const QString& value)
{
	switch (fieldType){
	case NFT_FIRST_NAME:
		if (m_firstName != value){
			istd::CChangeNotifier changeNotifier(this);

			m_firstName = value;
		}
		break;
	case NFT_LAST_NAME:
		if (m_lastName != value){
			istd::CChangeNotifier changeNotifier(this);

			m_lastName = value;
		}
		break;
	case NFT_NICKNAME:
		if (m_nickName != value){
			istd::CChangeNotifier changeNotifier(this);

			m_nickName = value;
		}
		break;
	};
}


const IAddressProvider* CPersonInfo::GetAddresses() const
{
	return &m_addresses;
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

	int genderType = m_genderType;

	static iser::CArchiveTag genderTypeTag("GenderType", "Gender type", iser::CArchiveTag::TT_LEAF);
	retVal= archive.BeginTag(genderTypeTag);
	retVal = retVal && archive.Process(genderType);
	retVal = retVal && archive.EndTag(genderTypeTag);

	if (!archive.IsStoring()){
		m_genderType = GT_DIVERSE;

		if (genderType >= 0 && genderType <= GT_MALE){
			m_genderType = (GenderType)genderType;
		}
	}

	static iser::CArchiveTag birthdayTag("Birthday", "Birthday", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(birthdayTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDate(archive, m_birthday);
	retVal = retVal && archive.EndTag(birthdayTag);

	static iser::CArchiveTag firstNameTag("FirstName", "First name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(firstNameTag);
	retVal = retVal && archive.Process(m_firstName);
	retVal = retVal && archive.EndTag(firstNameTag);

	static iser::CArchiveTag lastNameTag("LastName", "Last name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(lastNameTag);
	retVal = retVal && archive.Process(m_lastName);
	retVal = retVal && archive.EndTag(lastNameTag);

	static iser::CArchiveTag nickNameTag("NickName", "Nick name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nickNameTag);
	retVal = retVal && archive.Process(m_nickName);
	retVal = retVal && archive.EndTag(nickNameTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CPersonInfo::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_RESET;
}


bool CPersonInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IPersonInfo* sourcePtr = dynamic_cast<const IPersonInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_genderType = sourcePtr->GetGenderType();
		m_birthday = sourcePtr->GetBirthday();
		m_firstName = sourcePtr->GetNameField(NFT_FIRST_NAME);
		m_lastName = sourcePtr->GetNameField(NFT_LAST_NAME);
		m_nickName = sourcePtr->GetNameField(NFT_NICKNAME);

		return true;
	}

	return false;
}


istd::IChangeable* CPersonInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CPersonInfo> clonePtr(new CPersonInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CPersonInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_genderType = GT_DIVERSE;
	m_birthday = QDate();
	m_firstName.clear();
	m_lastName.clear();
	m_nickName.clear();

	return true;
}


} // namespace imtauth


