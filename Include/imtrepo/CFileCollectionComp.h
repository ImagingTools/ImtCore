#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QDateTime>
#include <QtCore/QThread>
#include <QtCore/QReadWriteLock>
#include <QtCore/QFileInfo>
#include <QtCore/QTimer>

// ACF includes
#include <ibase/IProgressManager.h>
#include <istd/TSmartPtr.h>
#include <iser/ISerializable.h>
#include <imod/TModelWrap.h>
#include <ilog/TLoggerCompWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <ifile/IFileNameParam.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IFileResourceTypeConstraints.h>
#include <iauth/ILogin.h>
#include <iauth/IRightsProvider.h>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>
#include <imtbase/IRevisionController.h>
#include <imtfile/IFileCompression.h>
#include <imtrepo/IFileCollectionInfo.h>
#include <imtrepo/IFileObjectCollection.h>
#include <imtrepo/IRepositoryFileTransformationStepsProvider.h>
#include <imtrepo/IRepositoryItemInfoProvider.h>
#include <imtrepo/IRepositoryTransformationController.h>
#include <imtrepo/CFileCollectionCompBase.h>


namespace imtrepo
{


/**
	Implementation of a file-based data object collection.
	Each object will be placed in the file system according to the configured settings:
	*<b>Plain</b>:In this mode the data file and related description files will be simply placed into the file collection folder.
	*<b>Subfolder</b>: Each file will be placed into a separate folder. This folder contains the data file itself, file collection description file, and the meta-info file.
	\ingroup Collection
	\ingroup FileRepository
*/

class CFileCollectionComp:
			public CFileCollectionCompBase2,
			virtual public IRepositoryItemInfoProvider,
			virtual protected imtbase::IRevisionController
{
	Q_OBJECT
public:
	typedef CFileCollectionCompBase2 BaseClass;

	I_BEGIN_COMPONENT(CFileCollectionComp)
		I_ASSIGN(m_isEnableRevisionHistoryAttrPtr, "IsEnableRevisionHistory", "Allow saving item revisions", true, false);
		I_ASSIGN(m_transformationControllerCompPtr, "TransformationController", "Controller for down- and upgrade of the repository data", false, "TransformationController");
		I_ASSIGN(m_transformationStepsProviderCompPtr, "FileTranformationStepsProvider", "Transformation steps provider for the file in the repository", false, "FileTranformationStepsProvider");
	I_END_COMPONENT;

	CFileCollectionComp();

	// reimplemented (IRepositoryItemInfoProvider)
	virtual const imtbase::ICollectionInfo& GetRepositoryItems() override;
	virtual const IRepositoryItemInfo* GetRepositoryItemInfo(const QByteArray& itemId) const override;

	// reimplemented (imtbase::IRevisionController)
	virtual RevisionInfoList GetRevisionInfoList(const imtbase::IObjectCollection& collection, const QByteArray& objectId) const override;
	virtual int BackupObject(imtbase::IObjectCollection& collection, const QByteArray& objectId, const QString& userComment = QString()) const override;
	virtual bool RestoreObject(imtbase::IObjectCollection& collection, const QByteArray& objectId, int revision) const override;
	virtual bool ExportObject(const imtbase::IObjectCollection& collection, const QByteArray& objectId, int revision, const QString& filePath) const override;

	// reimplemented (IFileObjectCollection)
	virtual QByteArray InsertFile(
				const QString& localFilePath,
				const QByteArray& typeId = QByteArray(),
				const QString& resourceName = QString(),
				const QString& resourceDescription = QString(),
				const QByteArray& resourceId = QByteArray()) override;
	virtual QByteArray ImportFile(const QByteArray& typeId, const QString& sourceFilePath = QString()) override;

	// reimplemented (IFileCollectionInfo)
	virtual FileCollectionLayout GetCollectionFileLayout() const override;

	// reimplemented (IObjectCollection)
	virtual const imtbase::IRevisionController* GetRevisionController() const override;
	virtual bool RemoveObject(const QByteArray& objectId) override;
	virtual void SetObjectName(const QByteArray& objectId, const QString& objectName) override;


	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	struct RevisionsContentsItem: public imtbase::IRevisionController::RevisionInfo
	{
		RevisionsContentsItem()
			:repositoryRevision(0)
		{
		}

		/**
			Revision of the whole collection at time of the backup creation.
		*/
		int repositoryRevision;
		QString path;
	};

	typedef QMap<int, RevisionsContentsItem> RevisionsContentsMap;

	class RevisionsContents: public RevisionsContentsMap, virtual public iser::ISerializable
	{
	public:
		// reimplement (iser::ISerializable)
		virtual bool Serialize(iser::IArchive& archive) override;
	};

	/**
		Calculate path in file collection for the local file path
	*/
	virtual QString CalculateFolderPathInRepository(
				const QString& localFilePath,
				const QString& resourceName,
				const QByteArray& typeId,
				ilog::IMessageConsumer* messageConsumerPtr) const;

	/**
		Calculate full file path for the item in the file collection.
	*/
	virtual QString CalculateTargetFilePath(
				const QString& targetFolderPath,
				const QString& localFilePath) const;

	virtual bool LoadRevisionsContents(const IFileObjectCollection& collection, const QByteArray& objectId, RevisionsContents& revisionsContents) const;
	virtual bool SaveRevisionsContents(const IFileObjectCollection& collection, const QByteArray& objectId, RevisionsContents& revisionsContents) const;

	/**
		Update the version of the repository if necessary.
		\return \c true if the revision was changed, or \c false otherwise.
	*/
	virtual bool UpdateRepositoryFormat();
	virtual bool TransformRepositoryItem(const RepositoryItemInfo& repositoryItemInfo, int fromRepositoryRevision, int toRepositoryRevision) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	class ResourceLocker
	{
	public:
		explicit ResourceLocker(
					CFileCollectionComp& collection,
					const QByteArray& resourceId,
					const QString& resourceName);
		~ResourceLocker();

	private:
		CFileCollectionComp& m_collection;
		QByteArray m_resourceId;
		QString m_resourceName;
	};

private:
	bool IsObjectIdLocked(const QByteArray& resourceId);
	bool IsObjectNameLocked(const QString& resourceName);

	bool IsObjectIdUsed(const QByteArray& objectId);
	QString CalculateTargetFilePath(
				const QString& filePath,
				const QString& objectName,
				const QByteArray& typeId) const;
	bool FinishInsertFileTransaction(
				const QString& workingPath,
				const QString& repositoryPath,
				const QByteArray& fileId,
				const CollectionItem& collectionItem);

private:
	I_ATTR(bool, m_isEnableRevisionHistoryAttrPtr);

	/**
		Upgrade and downgrade controller for the file repository.
	*/
	I_REF(IRepositoryTransformationController, m_transformationControllerCompPtr);

	/**
		Transformation steps provider for the file in the repository.
	*/
	I_REF(IRepositoryFileTransformationStepsProvider, m_transformationStepsProviderCompPtr);

	QList<QByteArray> m_lockedObjectIds;
	QList<QString> m_lockedObjectNames;
	QMutex m_lockedObjectInfoMutex;

	RepositoryItemInfoProvider m_itemInfoProvider;
};


} // namespace imtrepo


