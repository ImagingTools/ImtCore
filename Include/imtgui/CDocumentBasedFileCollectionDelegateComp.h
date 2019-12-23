#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>
#include <ifile/IFilePersistence.h>
#include <idoc/IDocumentManager.h>
#include <iqtgui/IIconProvider.h>

// ImtCore includes
#include <imtgui/CFileObjectCollectionViewDelegate.h>


namespace imtgui
{


/**
	Implementation of the view delegate for a file collection based on idoc::IDocumentManager.
	Such operations as 'New' 'Edit' (Update) on the collection objects will be done via registered document manager.
	\ingroup Collection
*/
class CDocumentBasedFileCollectionDelegateComp:
			public CFileObjectCollectionViewDelegate,
			public icomp::CComponentBase
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CFileObjectCollectionViewDelegate BaseClass2;

	I_BEGIN_COMPONENT(CDocumentBasedFileCollectionDelegateComp);
		I_REGISTER_INTERFACE(imtgui::ICollectionViewDelegate);
		I_REGISTER_SUBELEMENT(CollectionPersistence);
		I_REGISTER_SUBELEMENT_INTERFACE(CollectionPersistence, ifile::IFilePersistence, ExtractCollectionPersistence);
		I_REGISTER_SUBELEMENT_INTERFACE(CollectionPersistence, ifile::IFileTypeInfo, ExtractCollectionPersistence);
		I_ASSIGN(m_objectTypeIdAttrPtr, "ObjectTypeId", "Type-ID supported by this delegate", true, "ObjectTypeId");
		I_ASSIGN(m_filePersistenceCompPtr, "FilePersistence", "Persistence for editable objects", true, "FilePersistence");
		I_ASSIGN(m_statusIconsProviderCompPtr, "StatusIcons", "Icons for delegate visual status", false, "StatusIcons");
		I_ASSIGN(m_documentManagerCompPtr, "DocumentManager", "Document manager", true, "DocumentManager");
		I_ASSIGN_TO(m_documentManagerModelCompPtr, m_documentManagerCompPtr, true);
	I_END_COMPONENT;

	enum CommandGroup
	{
		CG_DOCUMENT_MANAGER = 2100
	};

	CDocumentBasedFileCollectionDelegateComp();

	// reimplemented (ICollectionViewDelegate)
	virtual QByteArray GetSupportedTypeId() const override;
	virtual QByteArray CreateNewObject(const QByteArray& typeId, const istd::IChangeable* defaultDataPtr = nullptr) const override;
	virtual void UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId) override;
	virtual bool OpenDocumentEditor(const QByteArray& objectId, const QByteArray& viewTypeId = QByteArray()) const override;

protected:
	/**
		The persistence proxy is needed for the document manager for saving resources,
		but not to the file system rather to the collection:
	*/
	class ObjectPersistenceProxy: virtual public ifile::IFilePersistence
	{
	public:
		ObjectPersistenceProxy(CDocumentBasedFileCollectionDelegateComp& parent);

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
		CDocumentBasedFileCollectionDelegateComp& m_parent;
	};


	class DocumentManagerObserver: public imod::TSingleModelObserverBase<idoc::IDocumentManager>
	{
	public:
		DocumentManagerObserver(CDocumentBasedFileCollectionDelegateComp& parent);

		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet);

	private:
		CDocumentBasedFileCollectionDelegateComp& m_parent;
	};

protected:
	// reimplemented (CObjectCollectionViewDelegate)
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
	static InterfaceType* ExtractCollectionPersistence(CDocumentBasedFileCollectionDelegateComp& component)
	{
		return &component.m_collectionPersistence;
	}

	void InitializeVisualStatus();

private:
	I_ATTR(QByteArray, m_objectTypeIdAttrPtr);
	I_REF(iqtgui::IIconProvider, m_statusIconsProviderCompPtr);

	/**
		Persistence for the related object.
	*/
	I_REF(ifile::IFilePersistence, m_filePersistenceCompPtr);

	/**
		Underlaying document manager used for object operations.
	*/
	I_REF(idoc::IDocumentManager, m_documentManagerCompPtr);
	I_REF(imod::IModel, m_documentManagerModelCompPtr);

	typedef istd::TPointerVector<ICollectionViewDelegate::ObjectInfo> WorkingObjects;
	mutable WorkingObjects m_workingObjects;

	ObjectPersistenceProxy m_collectionPersistence;
	DocumentManagerObserver m_documentManagerObserver;

	iqtgui::CHierarchicalCommand m_editContentsCommand;
};


} // namespace imtgui


