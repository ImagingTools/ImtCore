#pragma once


// ImtCore includes
#include <imtbase/IRevisionController.h>
#include <imtbase/TFilterableCollectionWrap.h>
#include <imtrepo/IRepositoryFileTransformationStepsProvider.h>
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
			public imtbase::TFilterableCollectionWrap<CFileCollectionCompBase>,
			virtual protected imtbase::IRevisionController
{
	Q_OBJECT
public:
	typedef imtbase::TFilterableCollectionWrap <CFileCollectionCompBase> BaseClass;

	I_BEGIN_COMPONENT(CFileCollectionComp)
		I_ASSIGN(m_isEnableRevisionHistoryAttrPtr, "IsEnableRevisionHistory", "Allow saving item revisions", true, false);
		I_ASSIGN(m_restoreRevisionRightIdAttrPtr, "RestoreRevisionRightId", "Restore to revision right id", true, "RestoreObject");
		I_ASSIGN(m_transformationControllerCompPtr, "TransformationController", "Controller for down- and upgrade of the repository data", false, "TransformationController");
		I_ASSIGN_TO(m_transformationStepsProviderCompPtr, m_transformationControllerCompPtr, true);
	I_END_COMPONENT;

	CFileCollectionComp();

	// reimplemented (imtbase::IRevisionController)
	virtual RevisionInfoList GetRevisionInfoList(const imtbase::IObjectCollection& collection, const QByteArray& objectId) const override;
	virtual int BackupObject(const imtbase::IObjectCollection& collection, const Id& objectId, const QString& userComment = QString()) const override;
	virtual bool RestoreObject(imtbase::IObjectCollection& collection, const Id& objectId, int revision) const override;
	virtual bool ExportObject(const imtbase::IObjectCollection& collection, const Id& objectId, int revision, const QString& filePath) const override;

	// reimplemented (IFileCollectionInfo)
	virtual FileCollectionLayout GetCollectionFileLayout() const override;

	// reimplemented (IObjectCollection)
	virtual const imtbase::IRevisionController* GetRevisionController() const override;
	virtual bool RemoveElement(const Id& elementId) override;

	// reimplemented (ICollectionInfo)
	virtual bool SetElementName(const Id& elementId, const QString& name) override;

	// reimplemented (ICollectionDataController)
	virtual bool ExportFile(const imtbase::IObjectCollection& collection, const Id& objectId, const QString& targetFilePath = QString()) const override;
	virtual QByteArray ImportFile(imtbase::IObjectCollection& collection, const QByteArray& typeId, const QString& sourceFilePath = QString(), const ICollectionInfo::Id& parentId = ICollectionInfo::Id()) const override;

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
				ilog::IMessageConsumer* messageConsumerPtr) const override;

	virtual QString CalculateTargetFilePath(
				const QString& filePath,
				const QString& objectName,
				const QByteArray& typeId) const override;

	virtual QString CalculateTargetFilePath(
				const QString& targetFolderPath,
				const QString& localFilePath) const override;

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
	/**
		Enable items changing history
	*/
	I_ATTR(bool, m_isEnableRevisionHistoryAttrPtr);

	/**
		Right-ID for accessing revision controller.
	*/
	I_ATTR(QByteArray, m_restoreRevisionRightIdAttrPtr);

	/**
		Upgrade and downgrade controller for the file repository.
	*/
	I_REF(IRepositoryTransformationController, m_transformationControllerCompPtr);

	/**
		Transformation steps provider for the file in the repository.
	*/
	I_REF(IRepositoryFileTransformationStepsProvider, m_transformationStepsProviderCompPtr);
};


} // namespace imtrepo


