#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IFileMetaInfoProvider.h>
#include <idoc/IDocumentManager.h>
#include <iqtgui/IIconProvider.h>

// ImtCore includes
#include <imtrepogui/CFileObjectCollectionViewDelegate.h>


namespace imtrepogui
{


/**
	Implementation of the view delegate for a file collection based on idoc::IDocumentManager.
	Such operations as 'New' 'Edit' (Update) on the collection objects will be done via registered document manager.
	\ingroup Collection
*/
class CDocumentCollectionViewDelegateComp:
			public CFileObjectCollectionViewDelegate,
			public icomp::CComponentBase
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CFileObjectCollectionViewDelegate BaseClass2;

	I_BEGIN_COMPONENT(CDocumentCollectionViewDelegateComp);
		I_REGISTER_INTERFACE(imtgui::ICollectionViewDelegate);
		I_REGISTER_SUBELEMENT(CollectionPersistence);
		I_REGISTER_SUBELEMENT_INTERFACE(CollectionPersistence, ifile::IFilePersistence, ExtractCollectionPersistence);
		I_REGISTER_SUBELEMENT_INTERFACE(CollectionPersistence, ifile::IFileTypeInfo, ExtractCollectionPersistence);
		I_ASSIGN(m_objectTypeIdAttrPtr, "ObjectTypeId", "Type-ID supported by this delegate", true, "ObjectTypeId");
		I_ASSIGN(m_filePersistenceCompPtr, "FilePersistence", "Persistence for editable objects", true, "FilePersistence");
		I_ASSIGN(m_objectImportPersistenceCompPtr, "ImportPersistence", "Persistence used for importing documents", false, "ImportPersistence");
		I_ASSIGN(m_objectExportPersistenceCompPtr, "ExportPersistence", "Persistence used for exporting documents", false, "ExportPersistence");
		I_ASSIGN(m_statusIconsProviderCompPtr, "StatusIcons", "Icons for delegate visual status", false, "StatusIcons");
		I_ASSIGN(m_documentManagerCompPtr, "DocumentManager", "Document manager", true, "DocumentManager");
		I_ASSIGN_TO(m_documentManagerModelCompPtr, m_documentManagerCompPtr, true);
		I_ASSIGN(m_informationViewCompPtr, "InformationView", "Document summary information view", false, "InformationView");
	I_END_COMPONENT;

	enum CommandGroup
	{
		CG_DOCUMENT_MANAGER = 2100
	};

	CDocumentCollectionViewDelegateComp();

	// reimplemented (imtgui::ICollectionViewDelegate)
	virtual QByteArray GetSupportedTypeId() const override;
	virtual bool InitializeDelegate(imtbase::IObjectCollection* collectionPtr, iqtgui::IGuiObject* parentGuiPtr) override;
	virtual QByteArray CreateNewObject(const QByteArray& typeId, const istd::IChangeable* defaultDataPtr = nullptr) const override;
	virtual QByteArray ImportObject(const QByteArray& typeId, const QString& sourcePath = QString()) const override;
	virtual bool ExportObject(const QByteArray& objectId, const QString& targetPath = QString()) const override;
	virtual void RemoveObjects(const imtbase::ICollectionInfo::Ids& objectIds)  const override;
	virtual void UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId) override;
	virtual bool OpenDocumentEditor(const QByteArray& objectId, const QByteArray& viewTypeId = QByteArray()) const override;
	virtual iqtgui::IGuiObject* GetInformationView() const override;

protected:
	/**
		The persistence proxy is needed for the document manager for saving resources,
		but not to the file system rather to the collection:
	*/
	class ObjectPersistenceProxy: virtual public ifile::IFilePersistence
	{
	public:
		ObjectPersistenceProxy(CDocumentCollectionViewDelegateComp& parent);

		// reimplemented (ifile::IFilePersistence)
		virtual bool IsOperationSupported(
					const istd::IChangeable* dataObjectPtr,
					const QString* filePathPtr = NULL,
					int flags = -1,
					bool beQuiet = true) const ;
		virtual int LoadFromFile(
					istd::IChangeable& data,
					const QString& filePath = QString(),
					ibase::IProgressManager* progressManagerPtr = NULL) const;
		virtual int SaveToFile(
					const istd::IChangeable& data,
					const QString& filePath = QString(),
					ibase::IProgressManager* progressManagerPtr = NULL) const;

		// reimplemented (ifile::IFileTypeInfo)
		virtual bool GetFileExtensions(QStringList& result, const istd::IChangeable* dataObjectPtr = NULL, int flags = -1, bool doAppend = false) const;
		virtual QString GetTypeDescription(const QString* extensionPtr = NULL) const;

	private:
		void CreateBackup(const imtbase::IObjectCollection& collection, const QByteArray& objectId) const;

	private:
		CDocumentCollectionViewDelegateComp& m_parent;
	};


	class DocumentManagerObserver: public imod::TSingleModelObserverBase<idoc::IDocumentManager>
	{
	public:
		DocumentManagerObserver(CDocumentCollectionViewDelegateComp& parent);

		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet);

	private:
		CDocumentCollectionViewDelegateComp& m_parent;
	};

protected:
	// reimplemented (imtrepogui::CFileObjectCollectionViewDelegate)
	virtual bool IsRestoreAllowed(const QByteArray& objectId) override;
	virtual void AfterRestore(const QByteArray& objectId, bool isRestoreSuccessful) override;
	virtual const ifile::IFileTypeInfo* FindFileInfo(const QByteArray& typeId, FileOperationType operationType) const;

	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual void SetupCommands() override;
	virtual void SetupInsertCommand() override;

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected Q_SLOTS:
	virtual void OnEdit();

private:
	template <class InterfaceType>
	static InterfaceType* ExtractCollectionPersistence(CDocumentCollectionViewDelegateComp& component)
	{
		return &component.m_collectionPersistence;
	}

	void InitializeVisualStatus();

private:
	/**
		Type-ID of the related document/object.
	*/
	I_ATTR(QByteArray, m_objectTypeIdAttrPtr);

	/**
		Provider of status icons.
	*/
	I_REF(iqtgui::IIconProvider, m_statusIconsProviderCompPtr);

	/**
		Persistence for the related object.
	*/
	I_REF(ifile::IFilePersistence, m_filePersistenceCompPtr);

	/**
		Persistence used for import of objects.
	*/
	I_REF(ifile::IFilePersistence, m_objectImportPersistenceCompPtr);

	/**
		Persistence used for export of objects.
	*/
	I_REF(ifile::IFilePersistence, m_objectExportPersistenceCompPtr);

	/**
		Underlaying document manager used for object operations.
	*/
	I_REF(idoc::IDocumentManager, m_documentManagerCompPtr);
	I_REF(imod::IModel, m_documentManagerModelCompPtr);

	/**
		View used for showing the information about the currently selected item.
	*/
	I_REF(iqtgui::IGuiObject, m_informationViewCompPtr);

private:
	ifile::IFileMetaInfoProvider::MetaInfoPtr m_selectedMetaInfoPtr;

	iqtgui::CHierarchicalCommand m_editContentsCommand;

	ObjectPersistenceProxy m_collectionPersistence;

	DocumentManagerObserver m_documentManagerObserver;

	typedef istd::TPointerVector<ICollectionViewDelegate::ObjectInfo> OpenedDocuments;
	mutable OpenedDocuments m_openedDocuments;

	QByteArray m_closedForRestoreId;
};


} // namespace imtrepogui


