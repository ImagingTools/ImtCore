// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CSessionInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtcore/Version.h>


namespace imtauth
{


// reimplemented (iser::ISession)

QByteArray CSessionInfo::GetToken() const
{
	return m_token;
}


void CSessionInfo::SetToken(const QByteArray &token)
{
	if (m_token != token){
		istd::CChangeNotifier changeNotifier(this);

		m_token = token;
	}
}


QByteArray CSessionInfo::GetUserId() const
{
	return m_userId;
}


void CSessionInfo::SetUserId(const QByteArray &userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = userId;
	}
}


QDateTime CSessionInfo::GetCreationDate() const
{
	return m_creationDate;
}


void CSessionInfo::SetCreationDate(const QDateTime& creationDate)
{
	if (m_creationDate != creationDate){
		istd::CChangeNotifier changeNotifier(this);

		m_creationDate = creationDate;
	}
}


QDateTime CSessionInfo::GetExpirationDate() const
{
	return m_expirationDate;
}


void CSessionInfo::SetExpirationDate(const QDateTime& expirationDate)
{
	if (m_expirationDate != expirationDate){
		istd::CChangeNotifier changeNotifier(this);

		m_expirationDate = expirationDate;
	}
}


// reimplemented (iser::ISerializable)

bool CSessionInfo::Serialize(iser::IArchive &archive)
{
	bool retVal = true;

	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();

	quint32 imtCoreVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion)){
		imtCoreVersion = 0;
	}

	iser::CArchiveTag tokenTag("Token", "User token", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tokenTag);
	retVal = retVal && archive.Process(m_token);
	retVal = retVal && archive.EndTag(tokenTag);

	iser::CArchiveTag userIdTag("UserId", "User-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	if (imtCoreVersion >= 12860){
		iser::CArchiveTag creationDateTag("CreationDate", "Creation Date", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(creationDateTag);
		retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_creationDate);
		retVal = retVal && archive.EndTag(creationDateTag);

		iser::CArchiveTag expirationDateTag("ExpirationDate", "Expiration Date", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(expirationDateTag);
		retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_expirationDate);
		retVal = retVal && archive.EndTag(expirationDateTag);
	}

	return retVal;
}


// reimplemented (iser::IChangeable)

bool CSessionInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CSessionInfo* sourcePtr = dynamic_cast<const CSessionInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_token = sourcePtr->m_token;
		m_userId = sourcePtr->m_userId;
		m_creationDate = sourcePtr->m_creationDate;
		m_expirationDate = sourcePtr->m_expirationDate;

		return true;
	}

	return false;
}


bool CSessionInfo::IsEqual(const IChangeable& object) const
{
	bool retVal = true;

	const CSessionInfo* sourcePtr = dynamic_cast<const CSessionInfo*>(&object);
	if (retVal && sourcePtr != nullptr){
		retVal = retVal && m_token == sourcePtr->m_token;
		retVal = retVal && m_userId == sourcePtr->m_userId;
		retVal = retVal && m_creationDate == sourcePtr->m_creationDate;
		retVal = retVal && m_expirationDate == sourcePtr->m_expirationDate;

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CSessionInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CSessionInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CSessionInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_token.clear();
	m_userId.clear();

	return true;
}


} // namespace imtauth


