#include <imtauth/CContactInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public static methods

QByteArray CContactInfo::GetTypeId()
{
	return "PersonInfo";
}


// public methods

CContactInfo::CContactInfo()
	:m_genderType(GT_DIVERSE), // :)
	m_firstName("DefaultFirstName"),
	m_lastName("DefaultLastName"),
	m_nickName("DefaultNickname"),
	m_modelUpdateBridge(this, imod::CModelUpdateBridge::UF_SOURCE)
{
	m_addresses.AttachObserver(&m_modelUpdateBridge);
}


// reimplemented (IContactInfo)

QString CContactInfo::GetMail() const
{
	return m_email;
}


void CContactInfo::SetEmail(const QString& email)
{
	if (m_email!= email){
		istd::CChangeNotifier changeNotifier(this);

		m_email = email;
	}
}


IContactInfo::GenderType CContactInfo::GetGenderType() const
{
	return m_genderType;
}


void CContactInfo::SetGenderType(GenderType genderType)
{
	if (m_genderType != genderType){
		istd::CChangeNotifier changeNotifier(this);

		m_genderType = genderType;
	}
}


QDate CContactInfo::GetBirthday() const
{
	return m_birthday;
}


void CContactInfo::SetBirthday(const QDate& birthday)
{
	if (m_birthday != birthday){
		istd::CChangeNotifier changeNotifier(this);

		m_birthday = birthday;
	}
}


QString CContactInfo::GetNameField(NameFieldType fieldType) const
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


void CContactInfo::SetNameField(NameFieldType fieldType, const QString& value)
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


const IAddressProvider* CContactInfo::GetAddresses() const
{
	return &m_addresses;
}


// reimplemented (iser::IObject)

QByteArray CContactInfo::GetFactoryId() const
{
	return GetTypeId();
}


// reimplemented (iser::ISerializable)

bool CContactInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag emailTag("EMail", "EMail", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(emailTag);
	retVal = retVal && archive.Process(m_email);
	retVal = retVal && archive.EndTag(emailTag);

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

	static iser::CArchiveTag addressesTag("Addresses", "Addresses", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(addressesTag);
	retVal = retVal && m_addresses.Serialize(archive);
	retVal = retVal && archive.EndTag(addressesTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CContactInfo::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_RESET;
}


bool CContactInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CContactInfo* sourcePtr = dynamic_cast<const CContactInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_email = sourcePtr->m_email;
		m_genderType = sourcePtr->m_genderType;
		m_birthday = sourcePtr->m_birthday;
		m_firstName = sourcePtr->m_firstName;
		m_lastName = sourcePtr->m_lastName;
		m_nickName = sourcePtr->m_nickName;

		return true;
	}

	return false;
}


istd::IChangeable* CContactInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CContactInfo> clonePtr(new CContactInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CContactInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_email.clear();
	m_genderType = GT_DIVERSE;
	m_birthday = QDate();
	m_firstName.clear();
	m_lastName.clear();
	m_nickName.clear();

	return true;
}


} // namespace imtauth


