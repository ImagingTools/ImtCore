// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>

// ImtCore includes
#include <imtbase/CObjectCollectionComp.h>
#include <imtbase/IRevisionController.h>


namespace DocumentServiceIntegrationTestPck
{


/**
	Test object collection with in-memory object revisions.
	A revision is a copy of the object data created by \c BackupRevision.
	Revision numbers of an object start with 1.
*/
class CRevisionObjectCollectionComp:
			public imtbase::CObjectCollectionComp,
			virtual public imtbase::IRevisionController
{
public:
	typedef imtbase::CObjectCollectionComp BaseClass;

	I_BEGIN_COMPONENT(CRevisionObjectCollectionComp)
		I_REGISTER_INTERFACE(imtbase::IRevisionController);
	I_END_COMPONENT

	// reimplemented (imtbase::IObjectCollection)
	virtual const imtbase::IRevisionController* GetRevisionController() const override;

	// reimplemented (imtbase::IRevisionController)
	virtual RevisionInfoList GetRevisionInfoList(
				const imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Id& objectId) const override;
	virtual int BackupRevision(
				const imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Id& objectId,
				const QString& userComment = QString()) const override;
	virtual bool RestoreRevision(
				imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Id& objectId,
				int revision) const override;
	virtual bool ExportRevision(
				const imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Id& objectId,
				int revision,
				const QString& filePath) const override;
	virtual bool GetRevisionData(
				const imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Id& objectId,
				int revision,
				istd::IChangeableSharedPtr& dataPtr,
				idoc::MetaInfoPtr& dataMetaInfoPtr) const override;
	virtual bool DeleteRevision(
				imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Id& objectId,
				int revision) const override;

private:
	struct Revision
	{
		istd::IChangeableSharedPtr dataPtr;
		QString comment;
	};

	typedef QMap<int, Revision> RevisionMap;

	mutable QMutex m_revisionsMutex;
	mutable QMap<QByteArray, RevisionMap> m_revisions;
};


} // namespace DocumentServiceIntegrationTestPck


