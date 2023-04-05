#include "imtbase/IObjectCollection.h"
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


QByteArray CUserInfo::GetUserId() const
{
	return m_userId;
}


void CUserInfo::SetUserId(const QByteArray& userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = userId;
	}
}


QByteArray CUserInfo::GetPasswordHash() const
{
	return m_passwordHash;
}


void CUserInfo::SetPasswordHash(const QByteArray& passwordHash)
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


void CUserInfo::SetMail(const QString& mail)
{
	if (m_mail != mail){
		istd::CChangeNotifier changeNotifier(this);

		m_mail = mail;
	}
}


bool CUserInfo::IsAdmin() const
{
	return false;
}


bool CUserInfo::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);
	bool retVal = true;

	BaseClass::Serialize(archive);

	static iser::CArchiveTag usernameTag("Username", "Username of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(usernameTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(usernameTag);

	static iser::CArchiveTag mailTag("Mail", "Mail of user", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(mailTag);
	retVal = retVal && archive.Process(m_mail);
	retVal = retVal && archive.EndTag(mailTag);

	return retVal;
}


bool CUserInfo::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CUserInfo* sourcePtr = dynamic_cast<const CUserInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		BaseClass::CopyFrom(object);

		m_userId = sourcePtr->m_userId;
		m_passwordHash = sourcePtr->m_passwordHash;
		m_mail = sourcePtr->m_mail;

		return true;
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

	BaseClass::ResetData(mode);

	m_userId.clear();
	m_mail.clear();

	return true;
}


} // namespace imtauth


