// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserVerification.h>


// Qt includes
#include <QtCore/QDebug>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

// reimplemented (iser::IUserVerification)

QByteArray CUserVerification::GetUserId() const
{
	return m_userId;
}


void CUserVerification::SetUserId(const QByteArray& userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = userId;
	}
}


QByteArray CUserVerification::GetVerificationCode() const
{
	return m_verificationCode;
}


void CUserVerification::SetVerificationCode(const QByteArray& code)
{
	if (m_verificationCode != code){
		istd::CChangeNotifier changeNotifier(this);

		m_verificationCode = code;
	}
}


QDateTime CUserVerification::GetCreationDate() const
{
	return m_creationDate;
}


void CUserVerification::SetCreationDate(const QDateTime& creationDate)
{
	if (m_creationDate != creationDate){
		istd::CChangeNotifier changeNotifier(this);

		m_creationDate = creationDate;
	}
}


QDateTime CUserVerification::GetExpirationDate() const
{
	return m_expirationDate;
}


void CUserVerification::SetExpirationDate(const QDateTime& expirationDate)
{
	if (m_expirationDate != expirationDate){
		istd::CChangeNotifier changeNotifier(this);

		m_expirationDate = expirationDate;
	}
}


bool CUserVerification::IsExpired() const
{
	QDateTime currentDateTime = QDateTime::currentDateTime();
	return m_expirationDate <= currentDateTime;
}


// reimplemented (iser::ISerializable)

bool CUserVerification::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	iser::CArchiveTag userIdTag("UserId", "User-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	iser::CArchiveTag codeTag("VerificationCode", "Verification Code", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(codeTag);
	retVal = retVal && archive.Process(m_verificationCode);
	retVal = retVal && archive.EndTag(codeTag);

	iser::CArchiveTag creationDateTag("CreationDate", "Creation Date", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(creationDateTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_creationDate);
	retVal = retVal && archive.EndTag(creationDateTag);

	iser::CArchiveTag expirationDateTag("ExpirationDate", "Expiration Date", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(expirationDateTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_expirationDate);
	retVal = retVal && archive.EndTag(expirationDateTag);

	return retVal;
}


// reimplemented (iser::IChangeable)

bool CUserVerification::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CUserVerification* sourcePtr = dynamic_cast<const CUserVerification*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = sourcePtr->m_userId;
		m_verificationCode = sourcePtr->m_verificationCode;
		m_creationDate = sourcePtr->m_creationDate;
		m_expirationDate = sourcePtr->m_expirationDate;

		return true;
	}

	return false;
}


bool CUserVerification::IsEqual(const IChangeable& object) const
{
	bool retVal = true;
	const CUserVerification* sourcePtr = dynamic_cast<const CUserVerification*>(&object);
	if (retVal && sourcePtr != nullptr){
		retVal = retVal && m_userId == sourcePtr->m_userId;
		retVal = retVal && m_verificationCode == sourcePtr->m_verificationCode;
		retVal = retVal && m_creationDate == sourcePtr->m_creationDate;
		retVal = retVal && m_expirationDate == sourcePtr->m_expirationDate;

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CUserVerification::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CUserVerification);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CUserVerification::ResetData(CompatibilityMode /*mode*/)
{
	[[maybe_unused]] istd::CChangeNotifier changeNotifier(this, &GetAllChanges());

	m_userId.clear();
	m_verificationCode.clear();

	return true;
}


} // namespace imtauth


