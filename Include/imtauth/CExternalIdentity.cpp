// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CExternalIdentity.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IExternalIdentity)

QByteArray CExternalIdentity::GetId() const
{
	return m_id;
}


void CExternalIdentity::SetId(const QByteArray& id)
{
	if (m_id != id){
		istd::CChangeNotifier changeNotifier(this);

		m_id = id;
	}
}


QByteArray CExternalIdentity::GetUserId() const
{
	return m_userId;
}


void CExternalIdentity::SetUserId(const QByteArray& userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = userId;
	}
}


QByteArray CExternalIdentity::GetProvider() const
{
	return m_provider;
}


void CExternalIdentity::SetProvider(const QByteArray& provider)
{
	if (m_provider != provider){
		istd::CChangeNotifier changeNotifier(this);

		m_provider = provider;
	}
}


QString CExternalIdentity::GetExternalSubject() const
{
	return m_externalSubject;
}


void CExternalIdentity::SetExternalSubject(const QString& subject)
{
	if (m_externalSubject != subject){
		istd::CChangeNotifier changeNotifier(this);

		m_externalSubject = subject;
	}
}


QString CExternalIdentity::GetExternalEmail() const
{
	return m_externalEmail;
}


void CExternalIdentity::SetExternalEmail(const QString& email)
{
	if (m_externalEmail != email){
		istd::CChangeNotifier changeNotifier(this);

		m_externalEmail = email;
	}
}


QDateTime CExternalIdentity::GetLinkedAt() const
{
	return m_linkedAt;
}


void CExternalIdentity::SetLinkedAt(const QDateTime& linkedAt)
{
	if (m_linkedAt != linkedAt){
		istd::CChangeNotifier changeNotifier(this);

		m_linkedAt = linkedAt;
	}
}


QDateTime CExternalIdentity::GetLastAuthAt() const
{
	return m_lastAuthAt;
}


void CExternalIdentity::SetLastAuthAt(const QDateTime& lastAuthAt)
{
	if (m_lastAuthAt != lastAuthAt){
		istd::CChangeNotifier changeNotifier(this);

		m_lastAuthAt = lastAuthAt;
	}
}


// reimplemented (iser::ISerializable)

bool CExternalIdentity::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag idTag("Id", "External identity ID");
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag userIdTag("UserId", "Local user ID");
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	static iser::CArchiveTag providerTag("Provider", "External provider name");
	retVal = retVal && archive.BeginTag(providerTag);
	retVal = retVal && archive.Process(m_provider);
	retVal = retVal && archive.EndTag(providerTag);

	static iser::CArchiveTag subjectTag("ExternalSubject", "External subject claim");
	retVal = retVal && archive.BeginTag(subjectTag);
	retVal = retVal && archive.Process(m_externalSubject);
	retVal = retVal && archive.EndTag(subjectTag);

	static iser::CArchiveTag emailTag("ExternalEmail", "External email");
	retVal = retVal && archive.BeginTag(emailTag);
	retVal = retVal && archive.Process(m_externalEmail);
	retVal = retVal && archive.EndTag(emailTag);

	static iser::CArchiveTag linkedAtTag("LinkedAt", "Link creation timestamp");
	retVal = retVal && archive.BeginTag(linkedAtTag);
	iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_linkedAt);
	retVal = retVal && archive.EndTag(linkedAtTag);

	static iser::CArchiveTag lastAuthAtTag("LastAuthAt", "Last authentication timestamp");
	retVal = retVal && archive.BeginTag(lastAuthAtTag);
	iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_lastAuthAt);
	retVal = retVal && archive.EndTag(lastAuthAtTag);

	return retVal;
}


// reimplemented (iser::IChangeable)

bool CExternalIdentity::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CExternalIdentity* sourcePtr = dynamic_cast<const CExternalIdentity*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	m_id = sourcePtr->m_id;
	m_userId = sourcePtr->m_userId;
	m_provider = sourcePtr->m_provider;
	m_externalSubject = sourcePtr->m_externalSubject;
	m_externalEmail = sourcePtr->m_externalEmail;
	m_linkedAt = sourcePtr->m_linkedAt;
	m_lastAuthAt = sourcePtr->m_lastAuthAt;

	return true;
}


bool CExternalIdentity::IsEqual(const IChangeable& object) const
{
	const CExternalIdentity* sourcePtr = dynamic_cast<const CExternalIdentity*>(&object);
	if (sourcePtr != nullptr){
		bool retVal = true;

		retVal = retVal && (m_id == sourcePtr->m_id);
		retVal = retVal && (m_userId == sourcePtr->m_userId);
		retVal = retVal && (m_provider == sourcePtr->m_provider);
		retVal = retVal && (m_externalSubject == sourcePtr->m_externalSubject);
		retVal = retVal && (m_externalEmail == sourcePtr->m_externalEmail);
		retVal = retVal && (m_linkedAt == sourcePtr->m_linkedAt);
		retVal = retVal && (m_lastAuthAt == sourcePtr->m_lastAuthAt);

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CExternalIdentity::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CExternalIdentity);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CExternalIdentity::ResetData(CompatibilityMode /*mode*/)
{
	m_id.clear();
	m_userId.clear();
	m_provider.clear();
	m_externalSubject.clear();
	m_externalEmail.clear();
	m_linkedAt = QDateTime();
	m_lastAuthAt = QDateTime();

	return true;
}


} // namespace imtauth
