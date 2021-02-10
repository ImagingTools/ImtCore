#include <imtrepogui/CDocumentBasedFileCollectionDelegateComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QDir>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>

// ACF includes
#include <istd/CChangeGroup.h>
#include <istd/CSystem.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <ifilegui/CFileDialogLoaderComp.h>

// ImtCore includes
#include <imtbase/IRevisionController.h>
#include <imtrepo/IFileObjectCollection.h>


namespace imtrepogui
{


CDocumentBasedFileCollectionDelegateComp::CDocumentBasedFileCollectionDelegateComp()
	:m_collectionPersistence(*this),
	m_documentManagerObserver(*this),
	m_editContentsCommand(tr("Edit"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_DOCUMENT_MANAGER)
{
}


// reimplemented (ICollectionViewDelegate)

QByteArray CDocumentBasedFileCollectionDelegateComp::GetSupportedTypeId() const
{
	return *m_objectTypeIdAttrPtr;
}


bool CDocumentBasedFileCollectionDelegateComp::InitializeDelegate(
			imtbase::IObjectCollection* collectionPtr,
			iqtgui::IGuiObject* parentGuiPtr)
{
	if (!BaseClass2::InitializeDelegate(collectionPtr, parentGuiPtr)){
		return false;
	}

	InitializeVisualStatus();

	return true;
}


QByteArray CDocumentBasedFileCollectionDelegateComp::CreateNewObject(
			const QByteArray& typeId,
			const istd::IChangeable* /*defaultDataPtr*/) const
{
	if (typeId != GetSupportedTypeId()){
		return QByteArray();
	}

	if (m_documentManagerCompPtr.IsValid()){
		ObjectInfo* objectInfoPtr = new ObjectInfo;
		objectInfoPtr->typeId = typeId;

		if (m_documentManagerCompPtr->InsertNewDocument(typeId, true, "", &objectInfoPtr->objectPtr)){
			m_openedDocuments.PushBack(objectInfoPtr);

			return "Default";
		}
		else{
			QMessageBox::critical(NULL, tr("Collection Manager"), tr("The file could not be created"));
		}
	}

	return QByteArray();
}


void CDocumentBasedFileCollectionDelegateComp::RemoveObjects(const imtbase::ICollectionInfo::Ids& objectIds)  const
{
	if (objectIds.isEmpty()){
		return;
	}

	if (QMessageBox::question(NULL, tr("Remove"), tr("Remove selected document from the collection"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes){
		return;
	}

	if (m_collectionPtr != nullptr){
		istd::CChangeGroup changeGroup(m_collectionPtr);

		for (const QByteArray& id : objectIds){
			bool isRemoveAccepted = true;

			for (int i = 0; i < m_openedDocuments.GetCount(); i++){
				ObjectInfo* objectInfoPtr = m_openedDocuments.GetAt(i);
				bool isFound = false;

				if (id == objectInfoPtr->uuid){
					for (int docIndex = 0; m_documentManagerCompPtr->GetDocumentsCount(); docIndex++){
						idoc::IDocumentManager::DocumentInfo documentInfo;
						if (objectInfoPtr->objectPtr == &m_documentManagerCompPtr->GetDocumentFromIndex(docIndex, &documentInfo)){
							if (documentInfo.isDirty){
								QString message = tr("Document \"%1\" is currently being edited and cannot be deleted").arg(objectInfoPtr->name);
								QMessageBox::warning(NULL, "", message, QMessageBox::Ok);
								isRemoveAccepted = false;
							}
							else{
								m_documentManagerCompPtr->CloseDocument(docIndex, true);
							}

							isFound = true;
							break;
						}
					}
				}

				if (isFound){
					break;
				}
			}

			if (isRemoveAccepted){
				m_collectionPtr->RemoveObject(id);
			}
		}
	}
}


void CDocumentBasedFileCollectionDelegateComp::UpdateItemSelection(
			const imtbase::ICollectionInfo::Ids& selectedItems,
			const QByteArray& selectedTypeId)
{
	BaseClass2::UpdateItemSelection(selectedItems, selectedTypeId);

	m_editContentsCommand.setEnabled(!selectedItems.isEmpty());

	if (m_selectedMetaInfoPtr.IsValid()){
		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_selectedMetaInfoPtr.GetPtr());
		if (modelPtr != nullptr){
			modelPtr->DetachAllObservers();
		}

		m_selectedMetaInfoPtr.Reset();
	}

	if (m_informationViewCompPtr.IsValid() && (selectedItems.count() == 1)){
		imod::IObserver* observerPtr = dynamic_cast<imod::IObserver*>(m_informationViewCompPtr.GetPtr());
		if (observerPtr != nullptr){
			imtbase::IObjectCollection* collectionPtr = dynamic_cast<imtbase::IObjectCollection*>(m_collectionPtr);
			if (collectionPtr->GetDataMetaInfo(selectedItems[0], m_selectedMetaInfoPtr)){
				imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_selectedMetaInfoPtr.GetPtr());
				if (modelPtr != nullptr){
					modelPtr->AttachObserver(observerPtr);
				}
			}
		}
	}
}


bool CDocumentBasedFileCollectionDelegateComp::OpenDocumentEditor(
			const QByteArray& objectId,
			const QByteArray& viewTypeId) const
{
	if (!m_documentManagerCompPtr.IsValid()){
		return false;
	}

	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	ObjectInfo* objectInfoPtr = nullptr;
	bool isAlreadyOpened = false;
	for (int i = 0; i < m_openedDocuments.GetCount(); ++i){
		if (m_openedDocuments.GetAt(i)->uuid == objectId){
			isAlreadyOpened = true;
			objectInfoPtr = m_openedDocuments.GetAt(i);
			break;
		}
	}

	if (isAlreadyOpened){
		int count = m_documentManagerCompPtr->GetDocumentsCount();
		for (int i = 0; i < count; i++){
			if (objectInfoPtr->objectPtr == &m_documentManagerCompPtr->GetDocumentFromIndex(i)){
				istd::IPolymorphic* viewPtr = m_documentManagerCompPtr->GetViewFromIndex(i, 0);
				if (viewPtr != nullptr){
					m_documentManagerCompPtr->SetActiveView(viewPtr);
				}
			}
		}

		return false;
	}

	objectInfoPtr = new ObjectInfo;
	objectInfoPtr->typeId = m_collectionPtr->GetObjectTypeId(objectId);
	objectInfoPtr->name = m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();

	imtrepo::IFileObjectCollection::FileInfo fileInfo = fileCollectionPtr->GetFileInfo(objectId);

	objectInfoPtr->uuid = objectId;

	QString tempPath = QDir::tempPath() + "/ImtCore/" + QUuid::createUuid().toString();

	istd::CSystem::EnsurePathExists(tempPath);

	QString tempFilePath = tempPath + "/" + objectInfoPtr->name + "." + QFileInfo(fileInfo.fileName).suffix();

	QString targetFilePath = fileCollectionPtr->GetFile(objectId, tempFilePath);
	if (!targetFilePath.isEmpty()){
		if (m_documentManagerCompPtr->OpenDocument(&objectInfoPtr->typeId, &targetFilePath, true, viewTypeId, &objectInfoPtr->objectPtr)){
			m_openedDocuments.PushBack(objectInfoPtr);

			QDir tempDir(tempPath);
			tempDir.removeRecursively();

			return true;
		}
	}

	QDir tempDir(tempPath);
	tempDir.removeRecursively();

	if (m_collectionPtr != nullptr){
		QVariant documentName = m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME);
		if (documentName.isValid()){
			QMessageBox::critical(nullptr, "", tr("Document \"%1\"could not be opened").arg(documentName.toString()));
		}
	}

	return false;
}


iqtgui::IGuiObject* CDocumentBasedFileCollectionDelegateComp::GetInformationView() const
{
	return m_informationViewCompPtr.GetPtr();
}


// protected methods

// reimplemented (imtrepogui::CFileObjectCollectionViewDelegate)

bool CDocumentBasedFileCollectionDelegateComp::IsRestoreAllowed(const QByteArray& objectId)
{
	for (int i = 0; i < m_openedDocuments.GetCount(); i++){
		ObjectInfo* objectInfoPtr = m_openedDocuments.GetAt(i);
		if (objectInfoPtr->uuid == objectId){
			for (int docIndex = 0; m_documentManagerCompPtr->GetDocumentsCount(); docIndex++){
				idoc::IDocumentManager::DocumentInfo documentInfo;
				if (objectInfoPtr->objectPtr == &m_documentManagerCompPtr->GetDocumentFromIndex(docIndex, &documentInfo)){
					bool retVal = m_documentManagerCompPtr->CloseDocument(docIndex, false);
					if (retVal){
						m_closedForRestoreId = objectId;
					}

					return retVal;
				}
			}
		}
	}

	return true;
}


void CDocumentBasedFileCollectionDelegateComp::AfterRestore(const QByteArray& objectId, bool /*isRestoreSuccessful*/)
{
	if (!m_closedForRestoreId.isEmpty()){
		OpenDocumentEditor(objectId);
	}

	m_closedForRestoreId.clear();
}


// reimplemented (imtgui::CObjectCollectionViewDelegate)

void CDocumentBasedFileCollectionDelegateComp::SetupCommands()
{
	BaseClass2::SetupCommands();

	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	if (fileCollectionPtr != nullptr){
		connect(&m_editContentsCommand, SIGNAL(triggered()), this, SLOT(OnEdit()));
		m_editCommands.InsertChild(&m_editContentsCommand);
	}
}


void CDocumentBasedFileCollectionDelegateComp::SetupInsertCommand()
{
}


// reimplemented (ibase::TLocalizableWrap)

void CDocumentBasedFileCollectionDelegateComp::OnLanguageChanged()
{
	BaseClass2::OnLanguageChanged();

	m_editContentsCommand.SetVisuals(tr("Edit"), tr("Edit"), tr("Edit existing object"), QIcon(":/Icons/Edit"));
}


// reimplemented (icomp::CComponentBase)

void CDocumentBasedFileCollectionDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_documentManagerModelCompPtr.IsValid()){
		m_documentManagerModelCompPtr->AttachObserver(&m_documentManagerObserver);
	}
}


void CDocumentBasedFileCollectionDelegateComp::OnComponentDestroyed()
{
	m_openedDocuments.Reset();

	if (m_documentManagerModelCompPtr.IsValid()){
		m_documentManagerModelCompPtr->DetachObserver(&m_documentManagerObserver);
	}

	BaseClass::OnComponentDestroyed();
}


// private methods

void CDocumentBasedFileCollectionDelegateComp::InitializeVisualStatus()
{
	m_visualStatus.SetStatusIcon(m_statusIconsProviderCompPtr->GetIcon(0));

	const iprm::IOptionsList* typeListPtr = m_collectionPtr->GetObjectTypesInfo();
	if (typeListPtr != nullptr){
		for (int i = 0; i < typeListPtr->GetOptionsCount(); i++){
			if (typeListPtr->GetOptionId(i) == *m_objectTypeIdAttrPtr){
				m_visualStatus.SetStatusText(typeListPtr->GetOptionDescription(i));
				break;
			}
		}
	}
}


// protected slots

void CDocumentBasedFileCollectionDelegateComp::OnEdit()
{
	for (const QByteArray& objectId : m_selectedItemIds){
		OpenDocumentEditor(objectId);
	}
}


// public methods of the embedded class ObjectPersistenceProxy

CDocumentBasedFileCollectionDelegateComp::ObjectPersistenceProxy::ObjectPersistenceProxy(CDocumentBasedFileCollectionDelegateComp& parent)
	:m_parent(parent)
{
}


// reimplemented (ifile::IFilePersistence)

bool CDocumentBasedFileCollectionDelegateComp::ObjectPersistenceProxy::IsOperationSupported(
			const istd::IChangeable* /*dataObjectPtr*/,
			const QString* /*filePathPtr*/,
			int /*flags*/,
			bool /*beQuiet*/) const
{
	return true;
}


int CDocumentBasedFileCollectionDelegateComp::ObjectPersistenceProxy::LoadFromFile(
			istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* progressManagerPtr) const
{
	if (m_parent.m_filePersistenceCompPtr.IsValid()){
		QString fileExtension = QFileInfo(filePath).suffix();
		QStringList extensions;
		if (m_parent.m_filePersistenceCompPtr->GetFileExtensions(extensions, &data, ifile::IFileTypeInfo::QF_LOAD)){
			if (extensions.contains(fileExtension, Qt::CaseInsensitive)){
				return m_parent.m_filePersistenceCompPtr->LoadFromFile(data, filePath, progressManagerPtr);
			}
		}
	}

	return OS_FAILED;
}


int CDocumentBasedFileCollectionDelegateComp::ObjectPersistenceProxy::SaveToFile(
			const istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	if (m_parent.m_collectionPtr != NULL){
		for (int i = 0; i < m_parent.m_openedDocuments.GetCount(); ++i){
			ICollectionViewDelegate::ObjectInfo* objectInfoPtr = m_parent.m_openedDocuments.GetAt(i);
			Q_ASSERT(objectInfoPtr != nullptr);

			if (objectInfoPtr->objectPtr == &data){
				QString fileExtension = QFileInfo(filePath).suffix();
				QByteArray typeId = objectInfoPtr->typeId;

				QString objectName = QFileInfo(filePath).completeBaseName();

				// If the object-ID is empty, we have to insert a new instance to the collection:
				if (objectInfoPtr->uuid.isEmpty()){
					QString description;
					const idoc::IDocumentMetaInfo* documentMetaInfoPtr = CompCastPtr<idoc::IDocumentMetaInfo>(&data);
					if (documentMetaInfoPtr != NULL){
						description = documentMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString();
					}

					QByteArray objectId = m_parent.m_collectionPtr->InsertNewObject(typeId, objectName, description, &data);
					if (!objectId.isEmpty()){
						objectInfoPtr->uuid = objectId;
						objectInfoPtr->name = objectName;
						objectInfoPtr->description = description;

						CreateBackup(*m_parent.m_collectionPtr, objectInfoPtr->uuid);

						return OS_OK;
					}
				}
				// An existing object in the collection should be updated:
				else{
					if (m_parent.UpdateObject(objectInfoPtr->uuid, data)){
						CreateBackup(*m_parent.m_collectionPtr, objectInfoPtr->uuid);

						return OS_OK;
					}
				}
			}
		}
	}

	return OS_FAILED;
}


// reimplemented (ifile::IFileTypeInfo)

bool CDocumentBasedFileCollectionDelegateComp::ObjectPersistenceProxy::GetFileExtensions(QStringList& result, const istd::IChangeable* dataObjectPtr, int flags, bool doAppend) const
{
	for (int i = 0; i < m_parent.m_openedDocuments.GetCount(); ++i){
		const ICollectionViewDelegate::ObjectInfo* documentInfoPtr = m_parent.m_openedDocuments.GetAt(i);
		Q_ASSERT(documentInfoPtr != NULL);

		if ((dataObjectPtr != NULL) && (dataObjectPtr == documentInfoPtr->objectPtr) && m_parent.m_filePersistenceCompPtr.IsValid()){
			return m_parent.m_filePersistenceCompPtr->GetFileExtensions(result, dataObjectPtr, flags, doAppend);
		}
	}

	return false;
}


QString CDocumentBasedFileCollectionDelegateComp::ObjectPersistenceProxy::GetTypeDescription(const QString* /*extensionPtr*/) const
{
	return QString();
}


// private methods of the embedded class ObjectPersistenceProxy

void CDocumentBasedFileCollectionDelegateComp::ObjectPersistenceProxy::CreateBackup(const imtbase::IObjectCollection& collection, const QByteArray& objectId) const
{
	Q_ASSERT(m_parent.m_collectionPtr == &collection);

	const imtbase::IRevisionController* revisionControllerPtr = m_parent.m_collectionPtr->GetRevisionController();
	if (revisionControllerPtr != nullptr){
		idoc::CStandardDocumentMetaInfo metaInfo;
		m_parent.m_collectionPtr->GetCollectionItemMetaInfo(objectId, metaInfo);
		QVariant variant = metaInfo.GetMetaInfo(imtrepo::IFileObjectCollection::MIT_REVISION);
		
		int revision = -1;
		if (variant.isValid()){
			revision = variant.toInt();
		}

		QString comment;
		if (revision == -1){
			comment = tr("Initial Revision");
		}
		else {
			bool isOk;
			comment = QInputDialog::getText(
						nullptr,
						tr("Comment your changes"),
						tr("Please enter comment for your changes"),
						QLineEdit::Normal,
						"",
						&isOk);
		}

		revisionControllerPtr->BackupObject(*m_parent.m_collectionPtr, objectId, comment);
	}
}


// public methods of the embedded class DocumentManagerObserver

CDocumentBasedFileCollectionDelegateComp::DocumentManagerObserver::DocumentManagerObserver(CDocumentBasedFileCollectionDelegateComp& parent)
	:m_parent(parent)
{
}


// reimplemented (imod::CSingleModelObserverBase)

void CDocumentBasedFileCollectionDelegateComp::DocumentManagerObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	Q_ASSERT(m_parent.m_documentManagerCompPtr.IsValid());

	if (changeSet.ContainsExplicit(idoc::IDocumentManager::CF_DOCUMENT_REMOVED)){
		int documentsCount = m_parent.m_documentManagerCompPtr->GetDocumentsCount();

		for (int i = 0; i < m_parent.m_openedDocuments.GetCount(); ++i){
			const istd::IChangeable* dataPtr = m_parent.m_openedDocuments.GetAt(i)->objectPtr;

			bool wasFound = false;
			for (int documentIndex = 0; documentIndex < documentsCount; ++documentIndex){
				const istd::IChangeable& documentData = m_parent.m_documentManagerCompPtr->GetDocumentFromIndex(documentIndex);
				if (dataPtr == &documentData){
					wasFound = true;
					break;
				}
			}

			if (!wasFound){
				m_parent.m_openedDocuments.RemoveAt(i);
			}
		}
	}
}


} // namespace imtrepogui


