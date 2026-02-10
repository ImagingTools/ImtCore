// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserGroupFilter.h>


// Qt includes
#include <QtCore/QDebug>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

// reimplemented (prolifedata::IGroupFilter)

QByteArray CUserGroupFilter::GetUserId() const
{
	return m_userId;
}


void CUserGroupFilter::SetUserId(const QByteArray& userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier notifier(this);

		m_userId = userId;
	}
}


QByteArrayList CUserGroupFilter::GetGroupIds() const
{
	return m_groupIds;
}


void CUserGroupFilter::SetGroupIds(const QByteArrayList& groupIds)
{
	if (m_groupIds != groupIds){
		istd::CChangeNotifier notifier(this);

		m_groupIds = groupIds;
	}
}


// reimplemented (iser::ISerializable)

bool CUserGroupFilter::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag userIdTag("UserId", "User-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_groupIds, "GroupIds", "GroupIds");

	return retVal;
}


// reimplemented (istd::IChangeable)

int CUserGroupFilter::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CUserGroupFilter::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CUserGroupFilter* sourcePtr = dynamic_cast<const CUserGroupFilter*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = sourcePtr->m_userId;
		m_groupIds = sourcePtr->m_groupIds;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CUserGroupFilter::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CUserGroupFilter());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CUserGroupFilter::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_userId.clear();
	m_groupIds.clear();

	return true;
}


} // namespace imtauth


