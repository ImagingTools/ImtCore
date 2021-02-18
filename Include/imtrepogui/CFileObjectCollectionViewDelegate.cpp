#include <imtrepogui/CFileObjectCollectionViewDelegate.h>


// Qt includes
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QInputDialog>

// ACF includes
#include <istd/CChangeGroup.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <ifilegui/CFileDialogLoaderComp.h>

// ImtCore includes
#include <imtbase/IRevisionController.h>
#include <imtrepo/IFileObjectCollection.h>
#include <imtrepogui/CFileObjectCollectionRevisionDialog.h>


namespace imtrepogui
{


CFileObjectCollectionViewDelegate::CFileObjectCollectionViewDelegate()
	:m_importCommand(tr("Import"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT),
	m_exportCommand(tr("Export"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT),
	m_restoreCommand(tr("Restore"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT)
{
	SetupSummaryInformation();
}


// reimplemented (ICollectionViewDelegate)

void CFileObjectCollectionViewDelegate::UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId)
{
	BaseClass::UpdateItemSelection(selectedItems, selectedTypeId);

	m_exportCommand.setEnabled(selectedItems.count() == 1);
	m_restoreCommand.setEnabled(false);

	const imtbase::IRevisionController* revisionControllerPtr = m_collectionPtr->GetRevisionController();
	if (revisionControllerPtr != nullptr){
		if (m_selectedItemIds.count() == 1){
			int revision = -1;
			idoc::CStandardDocumentMetaInfo metaInfo;

			QByteArray objectId = m_selectedItemIds[0];

			if (m_collectionPtr->GetCollectionItemMetaInfo(objectId, metaInfo)){
				QVariant variant = metaInfo.GetMetaInfo(imtrepo::IFileObjectCollection::MIT_REVISION);
				if (variant.isValid()){
					revision = variant.toInt();
				}
			}

			imtbase::IRevisionController::RevisionInfoList revisionList = revisionControllerPtr->GetRevisionInfoList(*m_collectionPtr, objectId);
			for (int i = 0; i < revisionList.count(); i++){
				if (revisionList[i].revision == revision){
					revisionList.removeAt(i);
					break;
				}
			}

			if (revisionList.count() > 0){
				m_restoreCommand.setEnabled(true);
			}
		}
	}
}


QByteArray CFileObjectCollectionViewDelegate::ImportObject(const QByteArray& typeId, const QString& sourcePath) const
{
	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	QByteArray objectId = fileCollectionPtr->InsertFile(sourcePath, typeId);
	if (!objectId.isEmpty()){
		const imtbase::IRevisionController* revisionControllerPtr = fileCollectionPtr->GetRevisionController();
		if (revisionControllerPtr != nullptr){
			revisionControllerPtr->BackupObject(*fileCollectionPtr, objectId, tr("Initial Revision"));
		}
	}

	return objectId;
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


// protected methods

void CFileObjectCollectionViewDelegate::OnImportObject(const QByteArray& /*objectId*/)
{
}


bool CFileObjectCollectionViewDelegate::IsRestoreAllowed(const QByteArray& objectId)
{
	return true;
}


void CFileObjectCollectionViewDelegate::BeforeRestore(const QByteArray& objectId)
{
}


void CFileObjectCollectionViewDelegate::AfterRestore(const QByteArray& objectId, bool isRestoreSuccessful)
{
}


// reimplemented (imtgui::CObjectCollectionViewDelegate)

void CFileObjectCollectionViewDelegate::SetupSummaryInformation()
{
	m_summaryInformationTypes.ResetData();
	m_summaryInformationHeaders.clear();

	m_summaryInformationTypes.InsertItem("Name", tr("Name"), "");
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

	m_importCommand.SetVisuals(tr("Import from File..."), tr("Import"), tr("Import existing file into the collection"), QIcon(":/Icons/Import"));
	m_exportCommand.SetVisuals(tr("Export to File..."), tr("Export"), tr("Export data from the collection to a file"), QIcon(":/Icons/Export"));
	m_restoreCommand.SetVisuals(tr("Restore revision..."), tr("Restore"), tr("Restore data from backup"), QIcon(":/Icons/Undo"));
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
				CreateFileImportFilter());

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

	if (!m_selectedItemIds.isEmpty()){
		QByteArray objectId = m_selectedItemIds[0];

		QString filePath = QFileDialog::getSaveFileName(
					(m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr,
					tr("Export File"),
					QString(),
					CreateFileExportFilter(objectId));

		if (!filePath.isEmpty()){
			if (!ExportObject(objectId, filePath)){
				QMessageBox::critical((m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr, tr("Collection"), tr("Document could not be exported"));
			}
		}
	}
}


void CFileObjectCollectionViewDelegate::OnRestore()
{
	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);
	Q_ASSERT(m_selectedItemIds.count() > 0);

	const imtbase::IRevisionController* revisionControllerPtr = m_collectionPtr->GetRevisionController();
	if (revisionControllerPtr != nullptr){
		idoc::CStandardDocumentMetaInfo metaInfo;
		int currentRevision = -1;

		QByteArray objectId = m_selectedItemIds[0];

		if (m_collectionPtr->GetCollectionItemMetaInfo(objectId, metaInfo)){
			QVariant revision = metaInfo.GetMetaInfo(imtrepo::IFileObjectCollection::MIT_REVISION);
			if (revision.isValid()){
				currentRevision = revision.toInt();
			}
		}

		imtbase::IRevisionController::RevisionInfoList revisionList = revisionControllerPtr->GetRevisionInfoList(*m_collectionPtr, objectId);

		QString fileName = fileCollectionPtr->GetElementInfo(objectId, imtbase::IObjectCollectionInfo::EIT_NAME).toString();

		CFileObjectCollectionRevisionDialog dialog;

		dialog.SetParams(
					revisionList,
					currentRevision,
					m_collectionPtr,
					revisionControllerPtr,
					objectId,
					fileName,
					CreateFileExportFilter(objectId));

		if (dialog.exec() == QDialog::Accepted){
			int revision = dialog.GetSelectedRevision();
			if (revision != -1 && revision != currentRevision){
				if (IsRestoreAllowed(objectId)){
					BeforeRestore(objectId);
					bool isRestored = revisionControllerPtr->RestoreObject(*m_collectionPtr, objectId, revision);
					AfterRestore(objectId, isRestored);
				}
			}
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


QString CFileObjectCollectionViewDelegate::CreateFileImportFilter() const
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

	if (allExt.size() > 1){
		filters.prepend(tr("All known documents (%1)").arg("*." + allExt.join(" *.")));
	}

	return filters.join(";;");
}


QString CFileObjectCollectionViewDelegate::CreateFileExportFilter(const QByteArray& objectId) const
{
	imtrepo::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtrepo::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	QStringList filters;
	QStringList allExt;

	QByteArray typeId = fileCollectionPtr->GetObjectTypeId(objectId);
	const ifile::IFileTypeInfo* fileInfoPtr = FindFileInfo(typeId);

	if (fileInfoPtr != nullptr){
		ifilegui::CFileDialogLoaderComp::AppendLoaderFilterList(*fileInfoPtr, nullptr, -1, allExt, filters, false);
	}

	return filters.join(";;");
}


} // namespace imtrepogui


