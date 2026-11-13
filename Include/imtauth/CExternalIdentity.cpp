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


QByteArray CExternalIdentity::GetProviderId() const
{
	return m_providerId;
}


void CExternalIdentity::SetProviderId(const QByteArray& providerId)
{
	if (m_providerId != providerId){
		istd::CChangeNotifier changeNotifier(this);

		m_providerId = providerId;
	}
}


QString CExternalIdentity::GetSubject() const
{
	return m_subject;
}


void CExternalIdentity::SetSubject(const QString& subject)
{
	if (m_subject != subject){
		istd::CChangeNotifier changeNotifier(this);

		m_subject = subject;
	}
}


QString CExternalIdentity::GetEmail() const
{
	return m_email;
}


void CExternalIdentity::SetEmail(const QString& email)
{
	if (m_email != email){
		istd::CChangeNotifier changeNotifier(this);

		m_email = email;
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


QDateTime CExternalIdentity::GetLastSignInAt() const
{
	return m_lastSignInAt;
}


void CExternalIdentity::SetLastSignInAt(const QDateTime& lastSignInAt)
{
	if (m_lastSignInAt != lastSignInAt){
		istd::CChangeNotifier changeNotifier(this);

		m_lastSignInAt = lastSignInAt;
	}
}


// reimplemented (iser::ISerializable)

bool CExternalIdentity::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag idTag("Id", "Identity link identifier");
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag userIdTag("UserId", "Local user identifier");
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	static iser::CArchiveTag providerIdTag("ProviderId", "External provider identifier");
	retVal = retVal && archive.BeginTag(providerIdTag);
	retVal = retVal && archive.Process(m_providerId);
	retVal = retVal && archive.EndTag(providerIdTag);

	static iser::CArchiveTag subjectTag("Subject", "Account identifier issued by the provider");
	retVal = retVal && archive.BeginTag(subjectTag);
	retVal = retVal && archive.Process(m_subject);
	retVal = retVal && archive.EndTag(subjectTag);

	static iser::CArchiveTag emailTag("Email", "E-mail reported by the provider");
	retVal = retVal && archive.BeginTag(emailTag);
	retVal = retVal && archive.Process(m_email);
	retVal = retVal && archive.EndTag(emailTag);

	static iser::CArchiveTag linkedAtTag("LinkedAt", "Link timestamp");
	retVal = retVal && archive.BeginTag(linkedAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_linkedAt);
	retVal = retVal && archive.EndTag(linkedAtTag);

	static iser::CArchiveTag lastSignInAtTag("LastSignInAt", "Last sign-in timestamp");
	retVal = retVal && archive.BeginTag(lastSignInAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_lastSignInAt);
	retVal = retVal && archive.EndTag(lastSignInAtTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CExternalIdentity::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CExternalIdentity* sourcePtr = dynamic_cast<const CExternalIdentity*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_id = sourcePtr->m_id;
	m_userId = sourcePtr->m_userId;
	m_providerId = sourcePtr->m_providerId;
	m_subject = sourcePtr->m_subject;
	m_email = sourcePtr->m_email;
	m_linkedAt = sourcePtr->m_linkedAt;
	m_lastSignInAt = sourcePtr->m_lastSignInAt;

	return true;
}


bool CExternalIdentity::IsEqual(const IChangeable& object) const
{
	const CExternalIdentity* sourcePtr = dynamic_cast<const CExternalIdentity*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	return
				(m_id == sourcePtr->m_id) &&
				(m_userId == sourcePtr->m_userId) &&
				(m_providerId == sourcePtr->m_providerId) &&
				(m_subject == sourcePtr->m_subject) &&
				(m_email == sourcePtr->m_email) &&
				(m_linkedAt == sourcePtr->m_linkedAt) &&
				(m_lastSignInAt == sourcePtr->m_lastSignInAt);
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
	istd::CChangeNotifier changeNotifier(this);

	m_id.clear();
	m_userId.clear();
	m_providerId.clear();
	m_subject.clear();
	m_email.clear();
	m_linkedAt = QDateTime();
	m_lastSignInAt = QDateTime();

	return true;
}


} // namespace imtauth


