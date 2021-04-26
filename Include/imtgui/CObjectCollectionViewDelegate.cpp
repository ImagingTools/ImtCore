#include <imtgui/CObjectCollectionViewDelegate.h>


// Qt includes
#include <QtGui/QRegExpValidator>
#include <QtWidgets/QMessageBox>

// ACF includes
#include <istd/CChangeGroup.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtbase/IRevisionController.h>


namespace imtgui
{


CObjectCollectionViewDelegate::CObjectCollectionViewDelegate()
	:m_editCommands(tr("&Edit"), 100),
	m_insertCommand(tr("New"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT),
	m_duplicateCommand(tr("Duplicate"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT),
	m_removeCommand(tr("Remove"), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT),
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


void CObjectCollectionViewDelegate::UpdateItemSelection(
			const imtbase::ICollectionInfo::Ids& selectedItems,
			const QByteArray& selectedTypeId)
{
	bool isAddEnabled = false;
	bool isRemoveEnabled = false;

	if (m_collectionPtr != nullptr){
		isAddEnabled = m_collectionPtr->GetOperationFlags() & imtbase::IObjectCollection::OF_SUPPORT_INSERT;
		isRemoveEnabled = false;

		if (!selectedItems.isEmpty()){
			int flags = m_collectionPtr->GetOperationFlags(selectedItems[0]);
			if (flags & imtbase::IObjectCollection::OF_SUPPORT_DELETE){
				isRemoveEnabled = true;
			}
		}
	}

	m_insertCommand.setEnabled(isAddEnabled);
	m_duplicateCommand.setEnabled(isAddEnabled && !selectedItems.isEmpty());
	m_removeCommand.setEnabled(isRemoveEnabled);

	m_selectedItemIds = selectedItems;
	m_selectedTypeId = selectedTypeId;
}


QByteArray CObjectCollectionViewDelegate::CreateNewObject(const QByteArray& typeId, const istd::IChangeable* defaultDataPtr) const
{
	if (m_collectionPtr != nullptr){
		return m_collectionPtr->InsertNewObject(typeId, tr("New"), QString(), defaultDataPtr);
	}

	return QByteArray();
}


QByteArray CObjectCollectionViewDelegate::ImportObject(const QByteArray& /*typeId*/, const QString& /*sourcePath*/) const
{
	return QByteArray();
}


bool CObjectCollectionViewDelegate::ExportObject(const QByteArray& /*objectId*/, const QString& /*targetPath*/) const
{
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
		else if (informationId == QByteArray("Type")){
			result.text = m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_TYPE_NAME).toByteArray();
			result.sortValue = result.text;
		}
		else if (informationId == QByteArray("Description")){
			result.text = m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
			result.sortValue = result.text;
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

	m_summaryInformationTypes.InsertItem("Type", tr("Type"), "");
	m_summaryInformationHeaders["Type"] = HeaderInfo(false);

	m_summaryInformationTypes.InsertItem("Description", tr("Description"), "");
	m_summaryInformationHeaders["Description"] = HeaderInfo(false);
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
			if (typesCount > 1){
				for (int i = 0; i < typesCount; ++i){
					const QString typeName = typesPtr->GetOptionName(i);

					QAction* action = m_insertNewDocumentMenuPtr->addAction(typeName);
					action->setData(typesPtr->GetOptionId(i));
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


// reimplemented (ibase::TLocalizableWrap)

void CObjectCollectionViewDelegate::OnLanguageChanged()
{
	SetupSummaryInformation();

	m_insertCommand.SetVisuals(tr("Insert"), tr("New"), tr("Insert new document into the collection"), QIcon(":/Icons/Add"));
	m_duplicateCommand.SetVisuals(tr("Duplicate"), tr("Duplicate"), tr("Duplicate selected objects"), QIcon(":/Icons/Duplicate"));
	m_removeCommand.SetVisuals(tr("Remove"), tr("Remove"), tr("Remove selected document from the collection"), QIcon(":/Icons/Delete"));
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


