// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CRevisionObjectCollectionComp.h"


namespace DocumentServiceIntegrationTestPck
{


// public methods

// reimplemented (imtbase::IObjectCollection)

const imtbase::IRevisionController* CRevisionObjectCollectionComp::GetRevisionController() const
{
	return this;
}


// reimplemented (imtbase::IRevisionController)

imtbase::IRevisionController::RevisionInfoList CRevisionObjectCollectionComp::GetRevisionInfoList(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Id& objectId) const
{
	QMutexLocker locker(&m_revisionsMutex);

	RevisionInfoList retVal;

	const RevisionMap revisions = m_revisions.value(objectId);
	for (auto revisionIt = revisions.constBegin(); revisionIt != revisions.constEnd(); ++revisionIt){
		RevisionInfo info;
		info.revision = revisionIt.key();
		info.comment = revisionIt.value().comment;
		info.isRevisionAvailable = true;

		retVal.append(info);
	}

	return retVal;
}


int CRevisionObjectCollectionComp::BackupRevision(
			const imtbase::IObjectCollection& collection,
			const imtbase::ICollectionInfo::Id& objectId,
			const QString& userComment) const
{
	DataPtr dataPtr;
	if (!collection.GetObjectData(objectId, dataPtr) || !dataPtr.IsValid()){
		return -1;
	}

	Revision revision;
	revision.dataPtr.FromUnique(dataPtr->CloneMe());
	revision.comment = userComment;
	if (!revision.dataPtr.IsValid()){
		return -1;
	}

	QMutexLocker locker(&m_revisionsMutex);

	RevisionMap& revisions = m_revisions[objectId];
	const int revisionNumber = revisions.isEmpty() ? 1 : revisions.lastKey() + 1;
	revisions.insert(revisionNumber, revision);

	return revisionNumber;
}


bool CRevisionObjectCollectionComp::RestoreRevision(
			imtbase::IObjectCollection& collection,
			const imtbase::ICollectionInfo::Id& objectId,
			int revision) const
{
	istd::IChangeableSharedPtr dataPtr;
	idoc::MetaInfoPtr metaInfoPtr;
	if (!GetRevisionData(collection, objectId, revision, dataPtr, metaInfoPtr)){
		return false;
	}

	return collection.SetObjectData(objectId, *dataPtr);
}


bool CRevisionObjectCollectionComp::ExportRevision(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Id& /*objectId*/,
			int /*revision*/,
			const QString& /*filePath*/) const
{
	return false;
}


bool CRevisionObjectCollectionComp::GetRevisionData(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Id& objectId,
			int revision,
			istd::IChangeableSharedPtr& dataPtr,
			idoc::MetaInfoPtr& dataMetaInfoPtr) const
{
	dataPtr.Reset();
	dataMetaInfoPtr.Reset();

	QMutexLocker locker(&m_revisionsMutex);

	const RevisionMap revisions = m_revisions.value(objectId);
	auto revisionIt = revisions.constFind(revision);
	if (revisionIt == revisions.constEnd()){
		return false;
	}

	dataPtr.FromUnique(revisionIt.value().dataPtr->CloneMe());

	return dataPtr.IsValid();
}


bool CRevisionObjectCollectionComp::DeleteRevision(
			imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Id& objectId,
			int revision) const
{
	QMutexLocker locker(&m_revisionsMutex);

	auto objectIt = m_revisions.find(objectId);
	if (objectIt == m_revisions.end()){
		return false;
	}

	return objectIt.value().remove(revision) > 0;
}


} // namespace DocumentServiceIntegrationTestPck


