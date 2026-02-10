// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QPointer>
#include <QtCore/QThread>
#include <QtCore/QDir>
#include <QtWidgets/QDialog>

// ACF includes
#include <istd/CSystem.h>
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IFileMetaInfoProvider.h>
#include <idoc/IDocumentManager.h>
#include <iqtgui/IIconProvider.h>

// ImtCore includes
#include <imtgui/CObjectCollectionViewDelegate.h>
#include <iprm/CParamsSet.h>
#include <imtgui/COpenDocumentWorker.h>


namespace imtgui
{


/**
	Implementation of the view delegate for a file collection based on idoc::IDocumentManager.
	Such operations as 'New' 'Edit' (Update) on the collection objects will be done via registered document manager.
	\ingroup Collection
*/
class CDocumentCollectionViewDelegateComp:
			public imtgui::CObjectCollectionViewDelegate,
			public icomp::CComponentBase
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtgui::CObjectCollectionViewDelegate BaseClass2;

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
		I_ASSIGN(m_useCompressorAttrPtr, "UseCompressor", "If enable, use ZIP-compression for import/export of objects", true, true);
		I_ASSIGN(m_editCommandLabelAttrPtr, "EditCommandLabel", "Label of the edit command", true, "Edit");
		I_ASSIGN(m_editCommandDescriptionAttrPtr, "EditCommandDescription", "Description of the edit command", true, "Edit existing object");
	I_END_COMPONENT;

	CDocumentCollectionViewDelegateComp();

	// reimplemented (imtgui::ICollectionViewDelegate)
	virtual QByteArray GetSupportedTypeId() const override;
	virtual bool InitializeDelegate(
				imtbase::IObjectCollection* collectionPtr,
				iqtgui::IGuiObject* parentGuiPtr,
				const iprm::IParamsSet* filterParamsPtr) override;
	virtual QByteArray CreateNewObject(
				const QByteArray& typeId,
				const QString& objectName,
				const QString& description,
				const istd::IChangeable* defaultDataPtr = nullptr) const override;
	virtual QByteArray ImportObject(const QByteArray& typeId, const QString& sourcePath = QString()) const override;
	virtual bool ExportObject(const QByteArray& objectId, const QString& targetPath = QString()) const override;
	virtual void RemoveObjects(const imtbase::ICollectionInfo::Ids& objectIds)  const override;
	virtual QString RenameObject(const QByteArray& objectId, const QString& newName) const override;
	virtual void UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId) override;
	virtual bool OpenDocumentEditor(const QByteArray& objectId, const QByteArray& viewTypeId = QByteArray()) const override;
	virtual iqtgui::IGuiObject* GetInformationView() const override;

protected:
	virtual bool RenameObjectOnSave() const;
	
	/** 
		Callback from the proxy (to reimplement)
	*/
	virtual QString CommentDocumentChanges(int revision) const;

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
		virtual OperationState LoadFromFile(
					istd::IChangeable& data,
					const QString& filePath = QString(),
					ibase::IProgressManager* progressManagerPtr = NULL) const;
		virtual OperationState SaveToFile(
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
	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual bool IsRestoreAllowed(const QByteArray& objectId) override;
	virtual void AfterRestore(const QByteArray& objectId, bool isRestoreSuccessful) override;
	virtual const ifile::IFileTypeInfo* FindFileInfo(const QByteArray& typeId, FileOperationType operationType) const;
	virtual void SetupCommands() override;
	virtual void SetupInsertCommand() override;
	virtual bool IsBundlePersistenceSupported(ifile::IFileTypeInfo::QueryFlags flags) const override;

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

	// reimplemented (iqtgui::TDesignSchemaHandlerWrap)
	virtual void OnDesignSchemaChanged(const QByteArray& themeId) override;

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

protected:
	typedef istd::TPointerVector<ICollectionViewDelegate::ObjectInfo> OpenedDocuments;
	mutable OpenedDocuments m_openedDocuments;
	mutable iprm::CParamsSet m_loadFilterParamSet;

	/**
		Underlaying document manager used for object operations.
	*/
	I_REF(idoc::IDocumentManager, m_documentManagerCompPtr);
	I_REF(imod::IModel, m_documentManagerModelCompPtr);

private:
	/**
		Label for "Edit"-command.
	*/
	I_TEXTATTR(m_editCommandLabelAttrPtr);

	/**
		Description of the "Edit"-command.
	*/
	I_TEXTATTR(m_editCommandDescriptionAttrPtr);

	/**
		Type-ID of the related document/object.
	*/
	I_ATTR(QByteArray, m_objectTypeIdAttrPtr);

	/**
		If enable, use ZIP-compression for import/export of objects.
	*/
	I_ATTR(bool, m_useCompressorAttrPtr);

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
		View used for showing the information about the currently selected item.
	*/
	I_REF(iqtgui::IGuiObject, m_informationViewCompPtr);

private:
	idoc::MetaInfoPtr m_selectedMetaInfoPtr;

	ObjectPersistenceProxy m_collectionPersistence;

	DocumentManagerObserver m_documentManagerObserver;

	QByteArray m_closedForRestoreId;

private:
	friend class COpenDocumentWorker;

	void FinishOpenDocumentAsync(const OpenDocumentResult& result, QPointer<QObject> workerObj, QPointer<QDialog> progressDlg) const;
};


} // namespace imtgui


