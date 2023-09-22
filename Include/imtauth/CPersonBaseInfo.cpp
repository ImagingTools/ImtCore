#include <imtauth/CPersonBaseInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// reimplemented (IPersonInfo)

IPersonBaseInfo::GenderType CPersonBaseInfo::GetGenderType() const
{
	return m_genderType;
}


void CPersonBaseInfo::SetGenderType(GenderType genderType)
{
	if (m_genderType != genderType){
		istd::CChangeNotifier notifier(this);

		m_genderType = genderType;
	}
}


QDate CPersonBaseInfo::GetBirthday() const
{
	return m_birthday;
}


void CPersonBaseInfo::SetBirthday(const QDate& birthday)
{
	if (m_birthday != birthday){
		istd::CChangeNotifier notifier(this);

		m_birthday = birthday;
	}
}


QString CPersonBaseInfo::GetNameField(NameFieldType /*fieldType*/) const
{
	return QString();
}


void CPersonBaseInfo::SetNameField(NameFieldType /*fieldType*/, const QString& /*value*/)
{

}


// reimplemented (iser::ISerializable)

bool CPersonBaseInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass::Serialize(archive);

	iser::CArchiveTag lastNameTag("LastName", "LastName", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(lastNameTag);
	retVal = retVal && archive.Process(m_lastName);
	retVal = retVal && archive.EndTag(lastNameTag);

	iser::CArchiveTag nickNameTag("NickName", "NickName", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nickNameTag);
	retVal = retVal && archive.Process(m_nick);
	retVal = retVal && archive.EndTag(nickNameTag);

	iser::CArchiveTag birthdayTag("Birthday", "Birthday", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(birthdayTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDate(archive, m_birthday);
	retVal = retVal && archive.EndTag(birthdayTag);

	iser::CArchiveTag genderTypeTag("GenderType", "Gender type", iser::CArchiveTag::TT_LEAF);
	retVal= archive.BeginTag(genderTypeTag);
	retVal = retVal && I_SERIALIZE_ENUM(GenderType, archive, m_genderType);
	retVal = retVal && archive.EndTag(genderTypeTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CPersonBaseInfo::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_RESET;
}


bool CPersonBaseInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CPersonBaseInfo* sourcePtr = dynamic_cast<const CPersonBaseInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		bool retVal = BaseClass::CopyFrom(object);

		m_genderType = sourcePtr->m_genderType;
		m_birthday = sourcePtr->m_birthday;
		m_lastName = sourcePtr->m_lastName;
		m_nick = sourcePtr->m_nick;

		return retVal;
	}

	return false;
}


istd::IChangeable* CPersonBaseInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CPersonBaseInfo> clonePtr(new CPersonBaseInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CPersonBaseInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	bool retVal = BaseClass::ResetData();

	m_lastName.clear();
	m_genderType = GT_DIVERSE;
	m_birthday = QDate();
	m_nick.clear();

	return retVal;
}


}


