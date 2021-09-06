#include <imtgui/CObjectCollectionViewDelegate.h>


// Qt includes
#include <QtGui/QRegExpValidator>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

// ACF includes
#include <istd/CChangeGroup.h>
#include <istd/CChangeNotifier.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <ifilegui/CFileDialogLoaderComp.h>

// ImtCore includes
#include <imtbase/IRevisionController.h>
#include <imtbase/ICollectionDataController.h>
#include <imtgui/CObjectCollectionRevisionDialog.h>
#include <imtwidgets/CNoEditableItemDelegate.h>


namespace
{


QString FindSelectedFilter(const QString& filters, const QString& filePath)
{
	QStringList filterList = filters.split(";;");
	QString ext = filePath.isEmpty() ? QString() : QFileInfo(filePath).suffix();
	if (ext.isEmpty())
		return QString();

	for (auto& filter : filterList)
		if (filter.contains(ext))
			return filter;

	return QString();
}

QString GetExportFileName(const imtbase::IObjectCollection& collection, const QByteArray& objectId)
{
	QVariant title = collection.GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME);

	return title.isValid() && !title.toString().isEmpty() ? title.toString() : QString("export");
}

QString ComposeExportFilePath(const QString olderPath, const QString& projectName)
{
	if (olderPath.isEmpty())
		return projectName;

	QFileInfo info(olderPath);
	return info.absoluteDir().absolutePath() + QDir::separator() + projectName + "." + info.suffix();
}


} // namespace


namespace imtgui
{


CObjectCollectionViewDelegate::CObjectCollectionViewDelegate()
	:m_editCommands(tr("&Edit"), 100),
	m_insertCommand(tr("New"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT),
	m_duplicateCommand(tr("Duplicate"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT),
	m_removeCommand(tr("Remove"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT),
	m_importCommand(tr("Import"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT),
	m_exportCommand(tr("Export"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT),
	m_restoreCommand(tr("Restore"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT),
	m_collectionPtr(nullptr)
{
	SetupSummaryInformation();

	EnableLocalization(true);
	EnableDesignHandler(true);
}


CObjectCollectionViewDelegate::~CObjectCollectionViewDelegate()
{
	EnableDesignHandler(false);
	EnableLocalization(false);
}


// reimplemented (ICollectionViewDelegate)

bool CObjectCollectionViewDelegate::InitializeDelegate(imtbase::IObjectCollection* collectionPtr, iqtgui::IGuiObject* parentGuiPtr)
{
	m_collectionPtr = collectionPtr;
	m_parentGuiPtr = parentGuiPtr;

	if (m_collectionPtr != nullptr){
		SetupCommands();

		OnLanguageChanged();

		return true;
	}

	return false;
}


const iqtgui::IVisualStatus& CObjectCollectionViewDelegate::GetDocumentTypeStatus() const
{
	return m_visualStatus;
}


QByteArray CObjectCollectionViewDelegate::GetSupportedTypeId() const
{
	static QByteArray retVal;

	return retVal;
}


QAbstractItemDelegate* CObjectCollectionViewDelegate::GetColumnItemDelegate(const QByteArray& /*columnId*/) const
{
	return new imtwidgets::CNoEditableItemDelegate();
}


void CObjectCollectionViewDelegate::OnColumnItemDelegateEditFinished(const QByteArray &/*objectId*/, const QByteArray &/*columnId*/, const QVariant &/*newValue*/) const
{

}


bool CObjectCollectionViewDelegate::IsEditorEnabled(const QByteArray &/*columnId*/) const
{
	return true;
}


void CObjectCollectionViewDelegate::UpdateItemSelection(
			const imtbase::ICollectionInfo::Ids& selectedItems,
			const QByteArray& selectedTypeId)
{
	bool isAddEnabled = false;
	bool isRemoveEnabled = false;
	bool isRestoreEnabled = false;
	if (m_collectionPtr != nullptr){
		isAddEnabled = m_collectionPtr->GetOperationFlags() & imtbase::IObjectCollection::OF_SUPPORT_INSERT;
		isRemoveEnabled = false;

		if (!selectedItems.isEmpty()){
			int flags = m_collectionPtr->GetOperationFlags(selectedItems[0]);
			if (flags & imtbase::IObjectCollection::OF_SUPPORT_DELETE){
				isRemoveEnabled = true;
			}
		}
		const imtbase::IRevisionController* revisionControllerPtr = m_collectionPtr->GetRevisionController();
		if (revisionControllerPtr != nullptr){
			if (selectedItems.count() == 1){
				int revision = -1;
				idoc::CStandardDocumentMetaInfo metaInfo;

				QByteArray objectId = selectedItems[0];

				if (m_collectionPtr->GetCollectionItemMetaInfo(objectId, metaInfo)){
					QVariant variant = metaInfo.GetMetaInfo(imtbase::IObjectCollection::MIT_REVISION);
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
					isRestoreEnabled = true;
				}
			}
		}
		else{
			m_restoreCommand.setVisible(false);
		}
	}

	m_insertCommand.setEnabled(isAddEnabled);
	m_duplicateCommand.setEnabled(isAddEnabled && !selectedItems.isEmpty());
	m_removeCommand.setEnabled(isRemoveEnabled);

	m_selectedItemIds = selectedItems;
	m_selectedTypeId = selectedTypeId;

	m_importCommand.setEnabled(m_insertCommand.IsEnabled());
	m_exportCommand.setEnabled(selectedItems.count() == 1);
	m_restoreCommand.setEnabled(isRestoreEnabled);
}


QByteArray CObjectCollectionViewDelegate::CreateNewObject(const QByteArray& typeId, const istd::IChangeable* defaultDataPtr) const
{
	if (m_collectionPtr != nullptr){
		return m_collectionPtr->InsertNewObject(typeId, tr("New"), QString(), defaultDataPtr);
	}

	return QByteArray();
}


QByteArray CObjectCollectionViewDelegate::ImportObject(const QByteArray& typeId, const QString& sourcePath) const
{
	if (sourcePath.isEmpty()){
		return QByteArray();
	}

	Q_ASSERT(m_collectionPtr != nullptr);

	const imtbase::ICollectionDataController* dataControllerPtr = m_collectionPtr->GetDataController();
	if (dataControllerPtr != nullptr){
		return dataControllerPtr->ImportFile(*m_collectionPtr, typeId, sourcePath);
	}

	return QByteArray();
}


bool CObjectCollectionViewDelegate::ExportObject(const QByteArray& objectId, const QString& targetPath) const
{
	if (targetPath.isEmpty()){
		return false;
	}

	Q_ASSERT(m_collectionPtr != nullptr);

	const imtbase::ICollectionDataController* dataControllerPtr = m_collectionPtr->GetDataController();
	if (dataControllerPtr != nullptr){
		return dataControllerPtr->ExportFile(*m_collectionPtr, objectId, targetPath);
	}

	return false;
}


void CObjectCollectionViewDelegate::RemoveObjects(const imtbase::ICollectionInfo::Ids& objectIds)  const
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
			m_collectionPtr->RemoveObject(id);
		}
	}
}


bool CObjectCollectionViewDelegate::UpdateObject(const QByteArray& objectId, const istd::IChangeable& contents) const
{
	if (m_collectionPtr != nullptr){
		return m_collectionPtr->SetObjectData(objectId, contents);
	}

	return false;
}


bool CObjectCollectionViewDelegate::RenameObject(const QByteArray& objectId, const QString& newName) const
{
	if (m_collectionPtr != nullptr){
		QRegExpValidator inputValidator(QRegExp("^[^\\\\/:\\*\\?\"\\<\\>\\|\\+]+$"));
		int pos;
		QString name = newName.trimmed();
		if (inputValidator.validate(name, pos) == QValidator::Acceptable){
			m_collectionPtr->SetObjectName(objectId, name);
			return true;
		}

		QMessageBox::critical(NULL, tr("Error"), tr("The document name contains some not allowed characters"));
	}

	return false;
}


const imtbase::ICollectionInfo& CObjectCollectionViewDelegate::GetSummaryInformationTypes() const
{
	return m_summaryInformationTypes;
}


ICollectionViewDelegate::SummaryInformation CObjectCollectionViewDelegate::GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const
{
	SummaryInformation result;

	if (m_collectionPtr != nullptr){
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

		idoc::CStandardDocumentMetaInfo metaInfo;
		if (m_collectionPtr->GetCollectionItemMetaInfo(objectId, metaInfo)){
			if (informationId == QByteArray("Added")){
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


ICollectionViewDelegate::HeaderInfo CObjectCollectionViewDelegate::GetSummaryInformationHeaderInfo(const QByteArray& informationId) const
{
	if (m_collectionPtr != nullptr){
		if (m_summaryInformationHeaders.contains(informationId)){
			return m_summaryInformationHeaders[informationId];
		}
	}

	return ICollectionViewDelegate::HeaderInfo(false);
}


bool CObjectCollectionViewDelegate::OpenDocumentEditor(const QByteArray& /*objectId*/, const QByteArray& /*viewTypeId*/) const
{
	return false;
}


iqtgui::IGuiObject* CObjectCollectionViewDelegate::GetInformationView() const
{
	return nullptr;
}


bool CObjectCollectionViewDelegate::IsCommandSupported(int /*commandId*/) const
{
	return true;
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CObjectCollectionViewDelegate::GetCommands() const
{
	return &m_rootCommands;
}


// protected methods

bool CObjectCollectionViewDelegate::IsNameUnique(const QString& name) const
{
	Q_ASSERT(m_collectionPtr != nullptr);

	imtbase::ICollectionInfo::Ids ids = m_collectionPtr->GetElementIds();
	for (const QByteArray& id : ids){
		QString itemName = m_collectionPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString();
		if (name == itemName){
			return false;
		}
	}

	return true;
}


QString CObjectCollectionViewDelegate::GetUniqueName(const QString& name) const
{
	QString uniqueName = name;

	int counter = 0;
	while (!IsNameUnique(uniqueName)){
		counter++;
		uniqueName = name + QString(" - %1").arg(counter);
	}

	return uniqueName;
}


void CObjectCollectionViewDelegate::SetupSummaryInformation()
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


void CObjectCollectionViewDelegate::SetupCommands()
{
	SetupInsertCommand();

	disconnect(&m_insertCommand, SIGNAL(triggered()), this, SLOT(OnInsert()));
	disconnect(&m_duplicateCommand, SIGNAL(triggered()), this, SLOT(OnDuplicate()));
	disconnect(&m_removeCommand, SIGNAL(triggered()), this, SLOT(OnRemove()));

	m_rootCommands.ResetChilds();
	m_editCommands.ResetChilds();

	connect(&m_insertCommand, SIGNAL(triggered()), this, SLOT(OnInsert()));
	connect(&m_duplicateCommand, SIGNAL(triggered()), this, SLOT(OnDuplicate()));
	connect(&m_removeCommand, SIGNAL(triggered()), this, SLOT(OnRemove()));

	if (IsCommandSupported(CI_INSERT)){
		m_editCommands.InsertChild(&m_insertCommand);
	}

	if (IsCommandSupported(CI_REMOVE)){
		m_editCommands.InsertChild(&m_removeCommand);
	}

	if (IsCommandSupported(CI_DUPLICATE)){
		m_editCommands.InsertChild(&m_duplicateCommand);
	}

	if (IsCommandSupported(CI_EDIT)){
		m_rootCommands.InsertChild(&m_editCommands);
	}

	if (m_collectionPtr != nullptr){
		const imtbase::ICollectionDataController* dataControllerPtr = m_collectionPtr->GetDataController();
		if (dataControllerPtr != nullptr){
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
}


void CObjectCollectionViewDelegate::SetupInsertCommand()
{
	if (m_collectionPtr != nullptr){
		if (m_insertNewDocumentMenuPtr != nullptr){
			QObject::disconnect(m_insertNewDocumentMenuPtr.GetPtr(), SIGNAL(triggered(QAction*)), this, SLOT(OnAddMenuOptionClicked(QAction*)));
		}

		m_insertNewDocumentMenuPtr.SetPtr(new QMenu);

		const iprm::IOptionsList* typesPtr = m_collectionPtr->GetObjectTypesInfo();
		if (typesPtr != nullptr){
			int typesCount = typesPtr->GetOptionsCount();
			for (int i = 0; i < typesCount; ++i){
				if (!typesPtr->IsOptionEnabled(i)){
					--typesCount;
				}
			}

			if (typesCount > 1){
				for (int i = 0; i < typesCount; ++i){
					if (typesPtr->IsOptionEnabled(i)){
						const QString typeName = typesPtr->GetOptionName(i);

						QAction* action = m_insertNewDocumentMenuPtr->addAction(typeName);
						action->setData(typesPtr->GetOptionId(i));
					}
				}

				m_insertCommand.setMenu(m_insertNewDocumentMenuPtr.GetPtr());

				QObject::connect(m_insertNewDocumentMenuPtr.GetPtr(), SIGNAL(triggered(QAction*)), this, SLOT(OnAddMenuOptionClicked(QAction*)));
			}
		}
	}
}


void CObjectCollectionViewDelegate::OnDuplicateObject(const QByteArray& /*sourceObjectId*/, const QByteArray& /*destinationObjectId*/)
{
}


void CObjectCollectionViewDelegate::OnImportObject(const QByteArray& /*objectId*/)
{
}


bool CObjectCollectionViewDelegate::IsRestoreAllowed(const QByteArray& /*objectId*/)
{
	return true;
}


void CObjectCollectionViewDelegate::BeforeRestore(const QByteArray& /*objectId*/)
{
}


void CObjectCollectionViewDelegate::AfterRestore(const QByteArray& /*objectId*/, bool /*isRestoreSuccessful*/)
{
}


QByteArray CObjectCollectionViewDelegate::FindTypeIdFromFile(const QString& filePath) const
{
	if (m_collectionPtr == nullptr){
		return QByteArray();
	}

	const imtbase::ICollectionDataController* dataControllerPtr = m_collectionPtr->GetDataController();
	if (dataControllerPtr == nullptr){
		return QByteArray();
	}

	const iprm::IOptionsList* objectTypeListPtr = m_collectionPtr->GetObjectTypesInfo();
	if (objectTypeListPtr == nullptr){
		return QByteArray();
	}

	QFileInfo fileInfo(filePath);
	QString extension = fileInfo.suffix();

	for (int typeIndex = 0; typeIndex < objectTypeListPtr->GetOptionsCount(); ++typeIndex){
		QByteArray typeId = objectTypeListPtr->GetOptionId(typeIndex);

		const ifile::IFileTypeInfo* fileTypeInfoPtr = dataControllerPtr->GetPersistenceForObjectType(typeId);
		if (fileTypeInfoPtr != nullptr){
			QStringList extensions;
			fileTypeInfoPtr->GetFileExtensions(extensions);
			if (extensions.contains(extension, Qt::CaseInsensitive)){
				return typeId;
			}
		}
	}

	return QByteArray();
}


const ifile::IFileTypeInfo* CObjectCollectionViewDelegate::FindFileInfo(const QByteArray& typeId, FileOperationType /*operationType*/) const
{
	if (m_collectionPtr == nullptr){
		return nullptr;
	}

	const imtbase::ICollectionDataController* dataControllerPtr = m_collectionPtr->GetDataController();
	if (dataControllerPtr == nullptr){
		return nullptr;
	}

	return dataControllerPtr->GetPersistenceForObjectType(typeId);
}


QString CObjectCollectionViewDelegate::CreateFileImportFilter() const
{
	QStringList filters;
	QStringList allExt;

	const ifile::IFileTypeInfo* fileTypeInfoPtr = FindFileInfo(m_selectedTypeId, FOT_IMPORT);
	if (fileTypeInfoPtr != nullptr){
		ifilegui::CFileDialogLoaderComp::AppendLoaderFilterList(*fileTypeInfoPtr, nullptr, -1, allExt, filters, false);
	}

	filters.append("Compressed item folder (*.zip)");
	allExt.append("zip");

	if (allExt.size() > 1){
		filters.prepend(tr("All known documents (%1)").arg("*." + allExt.join(" *.")));
	}

	return filters.join(";;");
}


QString CObjectCollectionViewDelegate::CreateFileExportFilter(const QByteArray& objectId) const
{
	QStringList filters;
	QStringList allExt;

	QByteArray typeId = m_collectionPtr->GetObjectTypeId(objectId);

	const ifile::IFileTypeInfo* fileInfoPtr = FindFileInfo(typeId, FOT_EXPORT);
	if (fileInfoPtr != nullptr){
		ifilegui::CFileDialogLoaderComp::AppendLoaderFilterList(*fileInfoPtr, nullptr, -1, allExt, filters, false);
	}

	filters.append("Compressed item folder (*.zip)");

	allExt.append("zip");

	return filters.join(";;");
}


// reimplemented (ibase::TLocalizableWrap)

void CObjectCollectionViewDelegate::OnLanguageChanged()
{
	SetupSummaryInformation();

	m_insertCommand.SetVisuals(tr("Insert"), tr("New"), tr("Insert new document into the collection"), QIcon(":/Icons/Add"));
	m_duplicateCommand.SetVisuals(tr("Duplicate"), tr("Duplicate"), tr("Duplicate selected objects"), QIcon(":/Icons/Duplicate"));
	m_removeCommand.SetVisuals(tr("Remove"), tr("Remove"), tr("Remove selected document from the collection"), QIcon(":/Icons/Delete"));
	m_importCommand.SetVisuals(tr("Import from File..."), tr("Import"), tr("Import existing file into the collection"), QIcon(":/Icons/Import"));
	m_exportCommand.SetVisuals(tr("Export to File..."), tr("Export"), tr("Export data from the collection to a file"), QIcon(":/Icons/Export"));
	m_restoreCommand.SetVisuals(tr("Restore revision..."), tr("Restore"), tr("Restore data from backup"), QIcon(":/Icons/Undo"));
}


// reimplemented (imtbase::TPolishableWrap)

void CObjectCollectionViewDelegate::OnDesignSchemaChanged()
{
}


// protected slots

void CObjectCollectionViewDelegate::OnInsert()
{
	Q_ASSERT(m_collectionPtr != nullptr);

	QByteArray objectId = CreateNewObject(m_selectedTypeId);
	if (objectId.isEmpty()){
		QMessageBox::critical((m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr, tr("Collection"), tr("New document could not be created"));
	}
}


void CObjectCollectionViewDelegate::OnDuplicate()
{
	Q_ASSERT(m_collectionPtr != nullptr);

	imtbase::ICollectionInfo::Ids selectedIds(m_selectedItemIds);

	for (const QByteArray& selectedItemId : selectedIds){
		imtbase::IObjectCollection::DataPtr sourceDataPtr;
		if (m_collectionPtr->GetObjectData(selectedItemId, sourceDataPtr)){
			QString sourceName = m_collectionPtr->GetElementInfo(selectedItemId, imtbase::ICollectionInfo::EIT_NAME).toString();
			QString sourceDescription = m_collectionPtr->GetElementInfo(selectedItemId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

			QByteArray objectId = this->CObjectCollectionViewDelegate::CreateNewObject(m_collectionPtr->GetObjectTypeId(selectedItemId), sourceDataPtr.GetPtr());
			if (!objectId.isEmpty()){
				QString copyName = QString("Copy of %1").arg(sourceName);
				QString newName = GetUniqueName(copyName);

				m_collectionPtr->SetObjectName(objectId, newName);
				m_collectionPtr->SetObjectDescription(objectId, sourceDescription);

				OnDuplicateObject(selectedItemId, objectId);

				const imtbase::IRevisionController* revisionControllerPtr = m_collectionPtr->GetRevisionController();
				if (revisionControllerPtr != nullptr){
					revisionControllerPtr->BackupObject(*m_collectionPtr, objectId, tr("Initial Revision"));
				}
			}
		}
	}
}


void CObjectCollectionViewDelegate::OnRemove()
{
	Q_ASSERT(m_collectionPtr != nullptr);

	imtbase::ICollectionInfo::Ids selectedIds(m_selectedItemIds);

	RemoveObjects(selectedIds);
}


void CObjectCollectionViewDelegate::OnAddMenuOptionClicked(QAction* action)
{
	QByteArray objectId = CreateNewObject(action->data().toByteArray());
	if (objectId.isEmpty()){
		QMessageBox::critical((m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr, tr("Collection"), tr("New document could not be created"));
	}
}



void CObjectCollectionViewDelegate::OnImport()
{
	istd::CChangeGroup changeGroup(m_collectionPtr);

	QStringList files = QFileDialog::getOpenFileNames(
				(m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr,
				tr("Import File"),
				QString(),
				CreateFileImportFilter());

	if (!files.isEmpty()){
		for (const QString& filePath : files){
			QByteArray typeId = m_selectedTypeId;
			if (typeId.isEmpty()){
				typeId = FindTypeIdFromFile(filePath);
			}

			QByteArray objectId = ImportObject(typeId, filePath);
			if (objectId.isEmpty()){
				QMessageBox::critical((m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr, tr("Collection"), tr("Document '%1' could not be imported").arg(filePath));
			}
			else{
				const imtbase::IRevisionController* revisionControllerPtr = m_collectionPtr->GetRevisionController();
				if (revisionControllerPtr != nullptr){
					revisionControllerPtr->BackupObject(*m_collectionPtr, objectId, tr("Importing into collection"));
				}

				OnImportObject(objectId);
			}
		}
	}
}


void CObjectCollectionViewDelegate::OnExport()
{
	Q_ASSERT(m_collectionPtr != nullptr);
	if (m_selectedItemIds.isEmpty() || !m_collectionPtr)
		return;

	QByteArray objectId = m_selectedItemIds[0];

	QString filters = CreateFileExportFilter(objectId);
	QString selectedFilter = FindSelectedFilter(filters, m_exportFilePath);
	m_exportFilePath = ComposeExportFilePath(m_exportFilePath, GetExportFileName(*m_collectionPtr, objectId));
	m_exportFilePath = QFileDialog::getSaveFileName(
		m_parentGuiPtr ? m_parentGuiPtr->GetWidget() : nullptr,
		tr("Export File"), m_exportFilePath.isEmpty() ? "export" : m_exportFilePath,
		filters, selectedFilter.isEmpty() ? nullptr : &selectedFilter);

	if (!m_exportFilePath.isEmpty()){
		if (!ExportObject(objectId, m_exportFilePath)){
			QMessageBox::critical(m_parentGuiPtr ? m_parentGuiPtr->GetWidget() : nullptr, tr("Collection"), tr("Document could not be exported"));
		}
	}
}


void CObjectCollectionViewDelegate::OnRestore()
{
	Q_ASSERT(m_selectedItemIds.count() > 0);

	const imtbase::IRevisionController* revisionControllerPtr = m_collectionPtr->GetRevisionController();
	if (revisionControllerPtr != nullptr){
		idoc::CStandardDocumentMetaInfo metaInfo;
		int currentRevision = -1;

		QByteArray objectId = m_selectedItemIds[0];

		if (m_collectionPtr->GetCollectionItemMetaInfo(objectId, metaInfo)){
			QVariant revision = metaInfo.GetMetaInfo(imtbase::IObjectCollection::MIT_REVISION);
			if (revision.isValid()){
				currentRevision = revision.toInt();
			}
		}

		imtbase::IRevisionController::RevisionInfoList revisionList = revisionControllerPtr->GetRevisionInfoList(*m_collectionPtr, objectId);

		QString fileName = m_collectionPtr->GetElementInfo(objectId, imtbase::IObjectCollectionInfo::EIT_NAME).toString();

		CObjectCollectionRevisionDialog dialog;

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



// public methods of the embedded class VisualStatus

// reimplemented (IVisualStatus)

void CObjectCollectionViewDelegate::VisualStatus::SetStatusIcon(const QIcon &statusIcon)
{
	istd::CChangeNotifier changeNotifier(this);

	m_statusIcon = statusIcon;
}


void CObjectCollectionViewDelegate::VisualStatus::SetStatusText(const QString &statusText)
{
	if (m_statusText != statusText){
		istd::CChangeNotifier changeNotifier(this);

		m_statusText = statusText;
	}
}


QIcon CObjectCollectionViewDelegate::VisualStatus::GetStatusIcon() const
{
	return m_statusIcon;
}


QString CObjectCollectionViewDelegate::VisualStatus::GetStatusText() const
{
	return m_statusText;
}


} // namespace imtgui


