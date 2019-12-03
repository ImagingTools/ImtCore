#include <imtgui/CDocumentBasedFileCollectionDelegateComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QDir>
#include <QtWidgets/QMessageBox>

// ACF includes
#include <istd/CChangeGroup.h>
#include <ifilegui/CFileDialogLoaderComp.h>

// ImtCore includes
#include <imtbase/IFileObjectCollection.h>


namespace imtgui
{


CDocumentBasedFileCollectionDelegateComp::CDocumentBasedFileCollectionDelegateComp()
	:m_collectionPersistence(*this),
	m_editContentsCommand("Edit", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT)
{
}


// reimplemented (ICollectionViewDelegate)

QByteArray CDocumentBasedFileCollectionDelegateComp::CreateNewObject(const QByteArray& typeId) const
{
	if (m_documentManagerCompPtr.IsValid()){
		ObjectInfo* objectInfoPtr = new ObjectInfo;
		objectInfoPtr->typeId = GetSupportedTypeId();
		objectInfoPtr->uuid = QUuid::createUuid().toByteArray();

		if (m_documentManagerCompPtr->InsertNewDocument(typeId, true, "", &objectInfoPtr->objectPtr)){
			m_workingObjects.PushBack(objectInfoPtr);

			return objectInfoPtr->uuid;
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

	m_editCommands.InsertChild(&m_editContentsCommand);
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
}


void CDocumentBasedFileCollectionDelegateComp::OnComponentDestroyed()
{
	m_workingObjects.Reset();

	BaseClass::OnComponentDestroyed();
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
					else {
						return m_parent.UpdateObject(objectInfoPtr->uuid, data);
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

} // namespace imtgui


