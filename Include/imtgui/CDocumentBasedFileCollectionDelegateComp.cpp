#include <imtgui/CDocumentBasedFileCollectionDelegateComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QDir>
#include <QtWidgets/QMessageBox>

// ACF includes
#include <istd/CChangeGroup.h>
#include <istd/CSystem.h>
#include <ifilegui/CFileDialogLoaderComp.h>

// ImtCore includes
#include <imtbase/IFileObjectCollection.h>


namespace imtgui
{


CDocumentBasedFileCollectionDelegateComp::CDocumentBasedFileCollectionDelegateComp()
	:m_collectionPersistence(*this),
	m_documentManagerObserver(*this),
	m_editContentsCommand("Edit", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_DOCUMENT_MANAGER)
{
}


// reimplemented (ICollectionViewDelegate)

QByteArray CDocumentBasedFileCollectionDelegateComp::GetSupportedTypeId() const
{
	return *m_objectTypeIdAttrPtr;
}


QByteArray CDocumentBasedFileCollectionDelegateComp::CreateNewObject(const QByteArray& typeId, const istd::IChangeable* /*defaultDataPtr*/) const
{
	if (typeId != GetSupportedTypeId()){
		return QByteArray();
	}

	if (m_documentManagerCompPtr.IsValid()){
		ObjectInfo* objectInfoPtr = new ObjectInfo;
		objectInfoPtr->typeId = typeId;

		if (m_documentManagerCompPtr->InsertNewDocument(typeId, true, "", &objectInfoPtr->objectPtr)){
			m_workingObjects.PushBack(objectInfoPtr);

			return "Default";
		}
		else{
			QMessageBox::critical(NULL, tr("Collection Manager"), tr("The file could not be created"));
		}
	}

	return QByteArray();
}


void CDocumentBasedFileCollectionDelegateComp::UpdateItemSelection(int viewStateFlags, const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId)
{
	BaseClass2::UpdateItemSelection(viewStateFlags, selectedItems, selectedTypeId);

	m_editContentsCommand.setEnabled(!selectedItems.isEmpty());
}


// protected methods

// reimplemented (CObjectCollectionViewDelegate)

void CDocumentBasedFileCollectionDelegateComp::SetupCommands()
{
	BaseClass2::SetupCommands();

	imtbase::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtbase::IFileObjectCollection*>(m_collectionPtr);
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
	m_workingObjects.Reset();

	if (m_documentManagerModelCompPtr.IsValid()){
		m_documentManagerModelCompPtr->DetachObserver(&m_documentManagerObserver);
	}

	BaseClass::OnComponentDestroyed();
}


// protected slots

void CDocumentBasedFileCollectionDelegateComp::OnEdit()
{
	imtbase::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtbase::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	if (m_documentManagerCompPtr.IsValid()){
		for (const QByteArray& objectId : m_selectedItemIds){
			bool isAlreadyOpened = false;
			for (int i = 0; i < m_workingObjects.GetCount(); ++i){
				if (m_workingObjects.GetAt(i)->uuid == objectId){
					isAlreadyOpened = true;
					break;
				}
			}

			if (isAlreadyOpened){
				continue;
			}

			ObjectInfo* objectInfoPtr = new ObjectInfo;
			objectInfoPtr->typeId = m_collectionPtr->GetObjectTypeId(objectId);
			objectInfoPtr->name = m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();

			imtbase::IFileObjectCollection::FileInfo fileInfo = fileCollectionPtr->GetFileInfo(objectId);

			objectInfoPtr->uuid = objectId;

			QString tempPath = QDir::tempPath() + "/ImtCore/" + QUuid::createUuid().toString();

			istd::CSystem::EnsurePathExists(tempPath);

			QString tempFilePath = tempPath + "/ " + objectInfoPtr->name + "." + QFileInfo(fileInfo.fileName).suffix();

			QString targetFilePath = fileCollectionPtr->GetFile(objectId, tempFilePath);
			if (!targetFilePath.isEmpty()){
				if (m_documentManagerCompPtr->OpenDocument(&objectInfoPtr->typeId, &targetFilePath, true, "", &objectInfoPtr->objectPtr)){
					m_workingObjects.PushBack(objectInfoPtr);
				}
			}
		}
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
				int retVal = m_parent.m_filePersistenceCompPtr->LoadFromFile(data, filePath, progressManagerPtr);
				if (retVal == ifile::IFilePersistence::OS_OK){
					for (int documentIndex = 0; documentIndex < m_parent.m_workingObjects.GetCount(); ++documentIndex){
						ICollectionViewDelegate::ObjectInfo* objectInfoPtr = m_parent.m_workingObjects.GetAt(documentIndex);
						Q_ASSERT(objectInfoPtr != NULL);

						if (filePath == objectInfoPtr->tempFilePath){
							idoc::IDocumentMetaInfo* documentMetaInfoPtr = CompCastPtr<idoc::IDocumentMetaInfo>(&data);
							if (documentMetaInfoPtr != NULL){
								documentMetaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION, objectInfoPtr->description);
								documentMetaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE, objectInfoPtr->name);
							}
						}
					}

					return retVal;
				}
			}
		}
	}

	return OS_FAILED;
}


int CDocumentBasedFileCollectionDelegateComp::ObjectPersistenceProxy::SaveToFile(
			const istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* progressManagerPtr) const
{
	if (m_parent.m_collectionPtr != NULL){
		for (int i = 0; i < m_parent.m_workingObjects.GetCount(); ++i){
			ICollectionViewDelegate::ObjectInfo* objectInfoPtr = m_parent.m_workingObjects.GetAt(i);
			Q_ASSERT(objectInfoPtr != nullptr);

			if (objectInfoPtr->objectPtr == &data){
				QString fileExtension = QFileInfo(filePath).suffix();
				QByteArray typeId = objectInfoPtr->typeId;

				QString tempFilePath = QDir::tempPath() + "/" + QUuid::createUuid().toString() + "." + fileExtension;
				objectInfoPtr->tempFilePath = tempFilePath;

				int saveState = m_parent.m_filePersistenceCompPtr->SaveToFile(data, tempFilePath, progressManagerPtr);
				if (saveState == ifile::IFilePersistence::OS_OK) {
					QString objectName = QFileInfo(filePath).completeBaseName();

					// If the object-ID is empty, we have to insert a new instance to the collection:
					if (objectInfoPtr->uuid.isEmpty()) {
						QString description;
						const idoc::IDocumentMetaInfo* documentMetaInfoPtr = CompCastPtr<idoc::IDocumentMetaInfo>(&data);
						if (documentMetaInfoPtr != NULL) {
							description = documentMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString();
						}

						QByteArray objectId = m_parent.m_collectionPtr->InsertNewObject(typeId, objectName, description, &data);
						if (!objectId.isEmpty()) {
							objectInfoPtr->uuid = objectId;
							objectInfoPtr->name = objectName;
							objectInfoPtr->description = description;

							return OS_OK;
						}
					}
					// An existing object in the collection should be updated:
					else{
						return m_parent.UpdateObject(objectInfoPtr->uuid, data) ? OS_OK : OS_FAILED;
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
	for (int i = 0; i < m_parent.m_workingObjects.GetCount(); ++i){
		const ICollectionViewDelegate::ObjectInfo* documentInfoPtr = m_parent.m_workingObjects.GetAt(i);
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

		for (int i = 0; i < m_parent.m_workingObjects.GetCount(); ++i){
			const istd::IChangeable* dataPtr = m_parent.m_workingObjects.GetAt(i)->objectPtr;

			bool wasFound = false;
			for (int documentIndex = 0; documentIndex < documentsCount; ++documentIndex){
				const istd::IChangeable& documentData = m_parent.m_documentManagerCompPtr->GetDocumentFromIndex(documentIndex);
				if (dataPtr == &documentData){
					wasFound = true;
					break;
				}
			}

			if (!wasFound){
				m_parent.m_workingObjects.RemoveAt(i);
			}
		}
	}
}


} // namespace imtgui


