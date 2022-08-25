#include <imtauth/CUserInfo.h>


// Qt includes
#include <QtCore/QByteArrayList>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods


QByteArray CUserInfo::GetId() const
{
	return m_userId;
}


void CUserInfo::SetId(QByteArray id)
{
	if (m_userId != id){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = id;
	}
}


QString CUserInfo::GetUsername() const
{
	return m_username;
}


void CUserInfo::SetUsername(QString username)
{
	if (m_username != username){
		istd::CChangeNotifier changeNotifier(this);

		m_username = username;
	}
}


QString CUserInfo::GetName() const
{
	return m_name;
}


void CUserInfo::SetName(QString name)
{
	if (m_name != name){
		istd::CChangeNotifier changeNotifier(this);

		m_name = name;
	}
}


QByteArray CUserInfo::GetPasswordHash() const
{
	return m_passwordHash;
}


void CUserInfo::SetPasswordHash(QByteArray passwordHash)
{
	if (m_passwordHash != passwordHash){
		istd::CChangeNotifier changeNotifier(this);

		m_passwordHash = passwordHash;
	}
}


QString CUserInfo::GetMail() const
{
	return m_mail;
}


void CUserInfo::SetMail(QString mail)
{
	if (m_mail != mail){
		istd::CChangeNotifier changeNotifier(this);

		m_mail = mail;
	}
}


bool CUserInfo::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);
	bool retVal = BaseClass::Serialize(archive);

	static iser::CArchiveTag userIdTag("UserId", "ID of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	static iser::CArchiveTag usernameTag("Username", "Username of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(usernameTag);
	retVal = retVal && archive.Process(m_username);
	retVal = retVal && archive.EndTag(usernameTag);

	static iser::CArchiveTag nameTag("Name", "Name of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	static iser::CArchiveTag passwordHashTag("PasswordHash", "Password hash of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(passwordHashTag);
	retVal = retVal && archive.Process(m_passwordHash);
	retVal = retVal && archive.EndTag(passwordHashTag);

	static iser::CArchiveTag mailTag("Mail", "Mail of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(mailTag);
	retVal = retVal && archive.Process(m_mail);
	retVal = retVal && archive.EndTag(mailTag);

	return retVal;
}


int CUserInfo::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_RESET;
}


bool CUserInfo::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CUserInfo* sourcePtr = dynamic_cast<const CUserInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);
		if (BaseClass::CopyFrom(object)){
			m_userId = sourcePtr->m_userId;
			m_username = sourcePtr->m_username;
			m_name = sourcePtr->m_name;
			m_passwordHash = sourcePtr->m_passwordHash;
			m_mail = sourcePtr->m_mail;
			return true;
		}
	}

	return false;
}


istd::IChangeable *CUserInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CUserInfo> clonePtr(new CUserInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CUserInfo::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);
	if (BaseClass::ResetData(mode)){
		m_userId.clear();
		m_username.clear();
		m_name.clear();
		m_passwordHash.clear();
		m_mail.clear();
		return true;
	}
	return false;
}



} // namespace imtauth


