#include <imtrepogui/CFileObjectCollectionViewDelegate.h>


// Qt includes
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

// ACF includes
#include <istd/CChangeGroup.h>
#include <ifilegui/CFileDialogLoaderComp.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtrepo/IFileObjectCollection.h>
#include <imtrepo/IRevisionController.h>
#include <imtrepogui/CFileObjectCollectionRevisionDialog.h>


namespace imtrepogui
{


CFileObjectCollectionViewDelegate::CFileObjectCollectionViewDelegate()
	:m_importCommand("Import", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT),
	m_exportCommand("Export", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT),
	m_restoreCommand("Restore", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT)
{
	m_summaryInformationTypes.ResetData();
	m_summaryInformationHeaders.clear();

	m_summaryInformationTypes.InsertItem("Name", "Name", "");
	m_summaryInformationHeaders["Name"] = HeaderInfo(true);

	m_summaryInformationTypes.InsertItem(QByteArray("TypeId"), tr("Type"), "");
	m_summaryInformationHeaders["TypeId"] = HeaderInfo(false);

	m_summaryInformationTypes.InsertItem(QByteArray("Description"), tr("Description"), "");
	m_summaryInformationHeaders["Description"] = HeaderInfo(false);

	m_summaryInformationTypes.InsertItem(QByteArray("Added"), tr("Added"), "");
	m_summaryInformationHeaders["Added"] = HeaderInfo(false);

	m_summaryInformationTypes.InsertItem(QByteArray("ModificationTime"), tr("Last Modified"), "");
	m_summaryInformationHeaders["ModificationTime"] = HeaderInfo(false);
}


// reimplemented (ICollectionViewDelegate)

void CFileObjectCollectionViewDelegate::UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId)
{
	BaseClass::UpdateItemSelection(selectedItems, selectedTypeId);

	m_exportCommand.setEnabled(selectedItems.count() == 1);
	m_restoreCommand.setEnabled(false);

	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	imtrepo::IRevisionController* revisionControllerPtr = dynamic_cast<imtrepo::IRevisionController*>(fileCollectionPtr);
	if (revisionControllerPtr != nullptr){
		if (m_selectedItemIds.count() == 1){
			if (revisionControllerPtr->GetRevisionInfoList(m_selectedItemIds[0]).count() > 1){
				m_restoreCommand.setEnabled(true);
			}
		}
	}

}


QByteArray CFileObjectCollectionViewDelegate::ImportObject(const QByteArray& typeId, const QString& sourcePath) const
{
	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	return fileCollectionPtr->InsertFile(sourcePath, typeId);
}


bool CFileObjectCollectionViewDelegate::ExportObject(const QByteArray& objectId, const QString& targetPath) const
{
	if (targetPath.isEmpty()){
		return false;
	}

	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	QString resultPath = fileCollectionPtr->GetFile(objectId, targetPath);

	return (resultPath == targetPath);
}


imtgui::ICollectionViewDelegate::SummaryInformation CFileObjectCollectionViewDelegate::GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const
{
	SummaryInformation result;

	if (m_collectionPtr != nullptr){
		idoc::CStandardDocumentMetaInfo metaInfo;
		if (m_collectionPtr->GetCollectionItemMetaInfo(objectId, metaInfo)){
			if (informationId == QByteArray("Name")){
				result.text = m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("TypeId")){
				result.text = m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_TYPE_ID).toByteArray();
				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("Description")){
				result.text = m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("Added")){
				result.sortValue = metaInfo.GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME);
				result.text = result.sortValue.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
			}
			else if (informationId == QByteArray("ModificationTime")){
				result.sortValue = metaInfo.GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME);
				result.text = result.sortValue.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
			}
		}
	}

	return result;
}


bool CFileObjectCollectionViewDelegate::IsCommandSupported(int commandId) const
{
	return BaseClass::IsCommandSupported(commandId);
}


// protected methods

void CFileObjectCollectionViewDelegate::OnImportObject(const QByteArray& objectId)
{
}


// reimplemented (CFileObjectCollectionViewDelegate)

void CFileObjectCollectionViewDelegate::SetupCommands()
{
	BaseClass::SetupCommands();

	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	if (fileCollectionPtr != nullptr){
		connect(&m_importCommand, SIGNAL(triggered()), this, SLOT(OnImport()));
		connect(&m_exportCommand, SIGNAL(triggered()), this, SLOT(OnExport()));
		connect(&m_restoreCommand, SIGNAL(triggered()), this, SLOT(OnRestore()));

		if (IsCommandSupported(CI_IMPORT)){
			m_editCommands.InsertChild(&m_importCommand);
		}

		if (IsCommandSupported(CI_EXPORT)){
			m_editCommands.InsertChild(&m_exportCommand);
		}

		if (IsCommandSupported(CI_RESTORE)){
			m_editCommands.InsertChild(&m_restoreCommand);
		}
	}
}


// reimplemented (ibase::TLocalizableWrap)

void CFileObjectCollectionViewDelegate::OnLanguageChanged()
{
	BaseClass::OnLanguageChanged();

	m_importCommand.SetVisuals(tr("Import from File..."), tr("Import"), tr("Import existing file into the collection"), QIcon(":/Icons/Load"));
	m_exportCommand.SetVisuals(tr("Export to File..."), tr("Export"), tr("Export data from the collection to a file"), QIcon(":/Icons/Export"));
	m_restoreCommand.SetVisuals(tr("Restore revision..."), tr("Restore"), tr("Restore data in collection to revision"), QIcon(":/Icons/Undo"));
}


// protected slots

void CFileObjectCollectionViewDelegate::OnImport()
{
	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	istd::CChangeGroup changeGroup(fileCollectionPtr);

	QStringList files = QFileDialog::getOpenFileNames(
				(m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr,
				tr("Import File"),
				QString(),
				CreateFileFilter(ifile::IFilePersistence::QF_LOAD));

	if (!files.isEmpty()){
		for (const QString& filePath : files){
			QByteArray typeId = m_selectedTypeId;
			if (typeId.isEmpty()){
				FindTypeIdFromFile(filePath);
			}

			QByteArray objectId = ImportObject(typeId, filePath);
			if (objectId.isEmpty()){
				QMessageBox::critical((m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr, tr("Collection"), tr("Document could not be imported"));
			}
			else{
				OnImportObject(objectId);
			}
		}
	}
}


void CFileObjectCollectionViewDelegate::OnExport()
{
	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	QByteArray typeId = fileCollectionPtr->GetObjectTypeId(m_selectedItemIds[0]);
	const ifile::IFileTypeInfo* fileInfoPtr = FindFileInfo(typeId);

	QStringList filters;
	QStringList allExt;

	if (fileInfoPtr != nullptr){
		ifilegui::CFileDialogLoaderComp::AppendLoaderFilterList(*fileInfoPtr, nullptr, -1, allExt, filters, false);
		if (allExt.size() > 1){
			filters.prepend(tr("All known documents (%1)").arg("*." + allExt.join(" *.")));
		}
	}

	istd::CChangeGroup changeGroup(fileCollectionPtr);

	QString filePath = QFileDialog::getSaveFileName(
				(m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr,
				tr("Export File"),
				QString(),
				filters.join(";;"));

	if (!filePath.isEmpty()){
		if (!ExportObject(m_selectedItemIds[0], filePath)){
			QMessageBox::critical((m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr, tr("Collection"), tr("Document could not be exported"));
		}
	}
}


void CFileObjectCollectionViewDelegate::OnRestore()
{
	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	imtrepo::IRevisionController* revisionControllerPtr = dynamic_cast<imtrepo::IRevisionController*>(m_collectionPtr);
	Q_ASSERT(revisionControllerPtr != nullptr);

	idoc::CStandardDocumentMetaInfo metaInfo;
	int currentRevision = -1;

	if (fileCollectionPtr->GetCollectionItemMetaInfo(m_selectedItemIds[0], metaInfo)){
		QVariant revision = metaInfo.GetMetaInfo(imtrepo::IFileObjectCollection::MIT_REVISION);
		if (revision.isValid()){
			currentRevision = revision.toInt();
		}
	}

	imtrepo::IRevisionController::RevisionInfoList revisionList = revisionControllerPtr->GetRevisionInfoList(m_selectedItemIds[0]);
	for (int i = 0; i < revisionList.count(); i++){
		if (revisionList[i].revision == currentRevision){
			revisionList.removeAt(i);
			break;
		}
	}

	CFileObjectCollectionRevisionDialog dialog;

	dialog.SetRevisionList(revisionList);
	if (dialog.exec() == QDialog::Accepted){
		int revision = dialog.GetSelectedRevision();
		if (revision != -1){
			revisionControllerPtr->RestoreObject(m_selectedItemIds[0], revision);
		}
	}
}


// protected methods

QByteArray CFileObjectCollectionViewDelegate::FindTypeIdFromFile(const QString& filePath) const
{
	QFileInfo fileInfo(filePath);
	QString extension = fileInfo.suffix();

	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	const ifile::IFileResourceTypeConstraints* fileConstraintsPtr = fileCollectionPtr->GetFileTypeConstraints();

	for (int typeIndex = 0; typeIndex < fileConstraintsPtr->GetOptionsCount(); ++typeIndex){
		const ifile::IFileTypeInfo* fileTypeInfoPtr = fileConstraintsPtr->GetFileTypeInfo(typeIndex);
		if (fileTypeInfoPtr != nullptr){
			QStringList extensions;
			fileTypeInfoPtr->GetFileExtensions(extensions);
			if (extensions.contains(extension, Qt::CaseInsensitive)){
				return fileConstraintsPtr->GetOptionId(typeIndex);
			}
		}
	}

	return QByteArray();
}


const ifile::IFileTypeInfo* CFileObjectCollectionViewDelegate::FindFileInfo(const QByteArray& typeId) const
{
	const imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<const imtrepo::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	const ifile::IFileResourceTypeConstraints* fileConstraintsPtr = fileCollectionPtr->GetFileTypeConstraints();

	for (int typeIndex = 0; typeIndex < fileConstraintsPtr->GetOptionsCount(); ++typeIndex){
		if (typeId == fileConstraintsPtr->GetOptionId(typeIndex)){
			return fileConstraintsPtr->GetFileTypeInfo(typeIndex);
		}
	}

	return nullptr;
}


QString CFileObjectCollectionViewDelegate::CreateFileFilter(int flags) const
{
	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	QStringList filters;
	QStringList allExt;

	const ifile::IFileResourceTypeConstraints* fileConstraintsPtr = fileCollectionPtr->GetFileTypeConstraints();
	for (int typeIndex = 0; typeIndex < fileConstraintsPtr->GetOptionsCount(); ++typeIndex){
		QByteArray typeId = fileConstraintsPtr->GetOptionId(typeIndex);

		if (m_selectedTypeId.isEmpty() || (m_selectedTypeId == typeId)){
			const ifile::IFileTypeInfo* fileTypeInfoPtr = fileConstraintsPtr->GetFileTypeInfo(typeIndex);
			if (fileTypeInfoPtr != nullptr){
				ifilegui::CFileDialogLoaderComp::AppendLoaderFilterList(*fileTypeInfoPtr, nullptr, -1, allExt, filters, false);
			}
		}
	}

	if ((allExt.size() > 1) && ((flags & ifile::IFilePersistence::QF_SAVE) == 0)){
		filters.prepend(tr("All known documents (%1)").arg("*." + allExt.join(" *.")));
	}

	return filters.join(";;");
}


} // namespace imtrepogui


