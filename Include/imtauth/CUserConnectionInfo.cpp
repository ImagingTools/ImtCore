// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserConnectionInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IUserConnectionInfo)

QByteArray CUserConnectionInfo::GetUserId() const
{
	return m_userId;
}


void CUserConnectionInfo::SetUserId(const QByteArray& userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = userId;
	}
}


QDateTime CUserConnectionInfo::GetLastConnection() const
{
	return m_lastConnection;
}


void CUserConnectionInfo::SetLastConnection(const QDateTime& lastConnection)
{
	if (m_lastConnection != lastConnection){
		istd::CChangeNotifier changeNotifier(this);

		m_lastConnection = lastConnection;
	}
}


// reimplemented (iser::IObject)

QByteArray CUserConnectionInfo::GetFactoryId() const
{
	return QByteArray("UserConnection");
}


// reimplemented (iser::ISerializable)

bool CUserConnectionInfo::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag userIdTag("UserId", "User-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	iser::CArchiveTag lastConnectionTag("LastConnection", "Last Connection", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(lastConnectionTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_lastConnection);
	retVal = retVal && archive.EndTag(lastConnectionTag);

	return retVal;
}


// reimplemented (iser::IChangeable)

bool CUserConnectionInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CUserConnectionInfo* sourcePtr = dynamic_cast<const CUserConnectionInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = sourcePtr->m_userId;
		m_lastConnection = sourcePtr->m_lastConnection;

		return true;
	}

	return false;
}


bool CUserConnectionInfo::IsEqual(const IChangeable& object) const
{
	const CUserConnectionInfo* sourcePtr = dynamic_cast<const CUserConnectionInfo*>(&object);
	if (sourcePtr != nullptr){
		bool retVal = true;
		retVal = retVal && m_userId == sourcePtr->m_userId;
		retVal = retVal && m_lastConnection == sourcePtr->m_lastConnection;

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CUserConnectionInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CUserConnectionInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CUserConnectionInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_userId.clear();

	return true;
}


} // namespace imtauth


