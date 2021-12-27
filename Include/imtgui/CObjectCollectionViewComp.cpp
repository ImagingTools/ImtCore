#include <imtgui/CObjectCollectionViewComp.h>


// Qt includes
#include <QtCore/QMetaType>
#include <QtCore/QUuid>
#include <QtCore/QQueue>
#include <QtCore/QDir>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonArray>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QInputDialog>
#include <QtGui/QResizeEvent>

// ACF includes
#include <imod/IModel.h>
#include <idoc/IDocumentMetaInfo.h>
#include <iqtgui/CCommandTools.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtbase/IObjectCollectionEvent.h>


namespace imtgui
{


// protected methods

CObjectCollectionViewComp::CObjectCollectionViewComp()
	:m_semaphoreCounter(0),
	m_currentInformationViewPtr(nullptr),
	m_readCollectionThread(this),
	m_readCollectionThreadState(RCTS_IDLE),
	m_itemModelPtr(&m_itemModel1),
	m_eventBasedUpdateEnabled(false)
{
	m_commands.SetParent(this);
}


void CObjectCollectionViewComp::SetFilterString(const QString &text)
{
	OnFilterChanged(text);
}


// reimplemented (imtbase::IObjectCollectionEventHandler)

void CObjectCollectionViewComp::OnCollectionConnected(const imtbase::IObjectCollection* /*objectCollectionPtr*/)
{
	m_eventBasedUpdateEnabled = true;
}


// reimplemented (imtbase::IMultiSelection)

const iprm::IOptionsList* CObjectCollectionViewComp::GetSelectionConstraints() const
{
	return nullptr;
}


imtbase::IMultiSelection::Ids CObjectCollectionViewComp::GetSelectedIds() const
{
	return m_itemsSelection[m_currentTypeId].toVector();
}


bool CObjectCollectionViewComp::SetSelectedIds(const Ids& /*selectedIds*/)
{
	return false;
}


// reimplemented (ibase::IProgressManager)

int CObjectCollectionViewComp::BeginProgressSession(
			const QByteArray& /*progressId*/,
			const QString& /*description*/,
			bool /*isCancelable*/)
{
	if (IsGuiCreated()){
		Message->setText(tr("Reading collection..."));
		Progress->setValue(0);
		Message->show();
		Progress->show();
	}
	return 0;
}


void CObjectCollectionViewComp::EndProgressSession(int /*sessionId*/)
{
	if (IsGuiCreated()){
		Progress->setValue(0);
		Message->hide();
		Progress->hide();
	}
}


void CObjectCollectionViewComp::OnProgress(int /*sessionId*/, double currentProgress)
{
	if (IsGuiCreated()){
		Message->setText(tr("Reading collection..."));
		Progress->setValue(currentProgress);
		Message->show();
		Progress->show();
	}
}

bool CObjectCollectionViewComp::IsCanceled(int /*sessionId*/) const
{
	return false;
}


// reimplemented (iser::ISerialize)

bool CObjectCollectionViewComp::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


// protected methods

ICollectionViewDelegate & CObjectCollectionViewComp::GetViewDelegateRef(const QByteArray& typeId)
{
	if (m_viewDelegateMap.contains(typeId)){
		return *m_viewDelegateMap[typeId];
	}

	return m_defaultViewDelegate;
}


const ICollectionViewDelegate& CObjectCollectionViewComp::GetViewDelegate(const QByteArray& typeId) const
{
	return (const_cast<CObjectCollectionViewComp*>(this))->GetViewDelegateRef(typeId);
}


// reimplemented (imtbase::TObjectCollectionEventHandlerCompWrap)

void CObjectCollectionViewComp::ProcessObjectCollectionEvent(
			const imtbase::IObjectCollection* objectCollectionPtr,
			const imtbase::IObjectCollectionEvent* eventPtr)
{
	istd::TSmartPtr<const imtbase::IObjectCollectionEvent> clonePtr(dynamic_cast<const imtbase::IObjectCollectionEvent*>(eventPtr->CloneMe()));
	Q_ASSERT(clonePtr.IsValid());

	QMetaObject::invokeMethod(
			this,
			"ProcessObjectCollectionEventSync",
			Q_ARG(ObjectCollectionPtr, objectCollectionPtr),
			Q_ARG(ObjectCollectionEventPtr, clonePtr));
}


// reimplemented (iqtgui::TRestorableGuiWrap)

void CObjectCollectionViewComp::OnRestoreSettings(const QSettings& settings)
{
	if ((*m_columnSettingsKeyAttrPtr).isEmpty()){
		return;
	}

	QByteArray settingsKey("ObjectCollectionViewColumnsSettings/" + *m_columnSettingsKeyAttrPtr);
	if (!settings.contains(settingsKey)){
		return;
	}
	
	QVariant settingsValue = settings.value(settingsKey);
	if (settingsValue.type() != QVariant::ByteArray){
		return;
	}

	QByteArray data = settingsValue.toByteArray();

	QJsonParseError error;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(data, &error);
	if (jsonDocument.isNull()){
		return;
	}

	TypeIdColumnsSettings typeIdColumnsSettings;
	
	QJsonArray jsonTypeIds = jsonDocument.array();
	if (jsonTypeIds.isEmpty()){
		return;
	}

	for (int typeIdIndex = 0; typeIdIndex < jsonTypeIds.count(); typeIdIndex++){
		QJsonObject jsonTypeId = jsonTypeIds[typeIdIndex].toObject();
		if (jsonTypeId.isEmpty()){
			return;
		}

		QByteArray typeId = jsonTypeId.value("TypeId").toVariant().toByteArray();
		if (typeId.isEmpty()){
			return;
		}

		ColumnsList columnsList;

		QJsonArray jsonColumns = jsonTypeId.value("Columns").toArray();
		if (jsonColumns.isEmpty()){
			return;
		}

		for (int columnIndex = 0; columnIndex < jsonColumns.count(); columnIndex++){
			QJsonObject jsonColumn = jsonColumns[columnIndex].toObject();
			if (jsonColumn.isEmpty()){
				return;
			}

			ColumnSettings columnSettings;

			QStringList keys = jsonColumn.keys();
			for (QString key : keys){
				QVariant variant = jsonColumn[key].toVariant();
				if (!variant.isValid()){
					return;
				}

				columnSettings[key] = variant;
			}

			columnsList.append(columnSettings);
		}

		typeIdColumnsSettings[typeId] = columnsList;
	}

	m_typeIdColumnsSettings = typeIdColumnsSettings;
}


void CObjectCollectionViewComp::OnSaveSettings(QSettings& settings) const
{
	if ((*m_columnSettingsKeyAttrPtr).isEmpty()){
		return;
	}

	QJsonDocument jsonDocument;
	QJsonArray jsonTypeIds;

	for (QString typeId : m_typeIdColumnsSettings.keys()){
		ColumnsList columnsList = m_typeIdColumnsSettings[typeId];

		QJsonObject jsonTypeId;
		jsonTypeId.insert("TypeId", QJsonValue::fromVariant(typeId));

		QJsonArray jsonColumns;
		for (int columnIndex = 0; columnIndex < columnsList.count(); columnIndex++){
			ColumnSettings columnSettings = columnsList[columnIndex];
			QStringList columnSettingsKeys = columnSettings.keys();

			QJsonObject jsonColumnSettings;
			for (QString columnSettingsKey : columnSettingsKeys){
				jsonColumnSettings.insert(columnSettingsKey, QJsonValue::fromVariant(columnSettings[columnSettingsKey]));
			}
			
			jsonColumns.append(QJsonValue::fromVariant(jsonColumnSettings));
		}
		jsonTypeId.insert("Columns", QJsonValue::fromVariant(jsonColumns));
		jsonTypeIds.append(QJsonValue::fromVariant(jsonTypeId));
	}

	jsonDocument.setArray(jsonTypeIds);

	QByteArray data = jsonDocument.toJson(QJsonDocument::Compact);

	settings.beginGroup("ObjectCollectionViewColumnsSettings");

	settings.setValue(*m_columnSettingsKeyAttrPtr, data);

	settings.endGroup();
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CObjectCollectionViewComp::UpdateGui(const istd::IChangeable::ChangeSet& changeSet)
{
	if (changeSet.Contains(istd::IChangeable::CF_ALL_DATA) || !m_eventBasedUpdateEnabled){
		if (*m_useAsyncReadAttrPtr){
			StartCollectionRead();
		}
		else{
			ReadCollection(&m_typeModel, m_itemModelPtr);
			OnCollectionReadFinished();
		}
	}
}


void CObjectCollectionViewComp::OnGuiModelAttached()
{
	imtbase::IObjectCollection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	if (m_viewDelegateMap.isEmpty()){
		m_defaultViewDelegate.InitializeDelegate(objectPtr, this);
	}

	for (ViewDelegateMap::Iterator iter = m_viewDelegateMap.begin(); iter != m_viewDelegateMap.end(); ++iter){
		iter.value()->InitializeDelegate(objectPtr, this);
	}

	BaseClass::OnGuiModelAttached();
}


// iqtgui::TDesignSchemaHandlerWrap

void CObjectCollectionViewComp::OnDesignSchemaChanged()
{
	BaseClass::OnDesignSchemaChanged();

	if (IsGuiCreated()){
		iqtgui::SetStyleSheetFromFile(*GetWidget(), ":/Styles/ObjectCollectionView");
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CObjectCollectionViewComp::OnGuiCreated()
{
	Message->hide();
	Progress->hide();

	if (m_filterParamsGuiCompPtr.IsValid()){
		m_filterParamsGuiCompPtr->CreateGui(CustomFilterParamsFrame);
	}

	m_itemModelPtr = &m_itemModel1;
	m_readCollectionThread.SetModels(&m_typeModel, &m_itemModel2);

	ItemList->setProperty("ItemView", true);

	m_proxyModelPtr = new ItemProxyModel(this);
	m_proxyModelPtr->setSourceModel(m_itemModelPtr);
	m_proxyModelPtr->setFilterKeyColumn(0);
	m_proxyModelPtr->setFilterRole(DR_TYPE_ID);

	m_searchShortCutPtr = new QShortcut(GetQtWidget());
	m_searchShortCutPtr->setKey(Qt::CTRL + Qt::Key_F);
	m_escShortCutPtr = new QShortcut(GetQtWidget());
	m_escShortCutPtr->setKey(Qt::Key_Escape);
	m_delShortCutPtr = new QShortcut(GetQtWidget());
	m_delShortCutPtr->setKey(Qt::Key_Delete);
	m_renameShortCutPtr = new QShortcut(GetQtWidget());
	m_renameShortCutPtr->setKey(Qt::Key_F2);

	m_focusDecoratorPtr = new iwidgets::CFocusDecorator(this);
	m_focusDecoratorPtr->RegisterWidget(FilterEdit, &m_graphicsEffectFactory);

	FilterPanel->setMaximumHeight(0);
	m_filterPanelAnimationPtr = new QPropertyAnimation(FilterPanel, "maximumHeight", this);

	ItemList->setModel(m_proxyModelPtr);

	connect(FilterEdit, &QLineEdit::textChanged, this, &CObjectCollectionViewComp::OnFilterChanged);
	connect(CloseButton, &QToolButton::clicked, this, &CObjectCollectionViewComp::OnEscShortCut);
	if (*m_useSearchWidgetAttrPtr == true) {
		connect(m_searchShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnSearchShortCut);
	}
	connect(m_escShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnEscShortCut);
	connect(m_delShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnDelShortCut);
	connect(m_renameShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnRenameShortCut);

	ItemList->setContextMenuPolicy(Qt::CustomContextMenu);
//	ItemList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ItemList->header()->setFirstSectionMovable(true);
	ItemList->installEventFilter(this);

	connect(TypeList, &QTreeWidget::itemSelectionChanged, this, &CObjectCollectionViewComp::OnTypeChanged);
	connect(ItemList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CObjectCollectionViewComp::OnSelectionChanged);
	connect(ItemList, &QTreeView::doubleClicked, this, &CObjectCollectionViewComp::OnItemDoubleClick);
	connect(ItemList, &QTreeView::customContextMenuRequested, this, &CObjectCollectionViewComp::OnCustomContextMenuRequested);
	connect(ItemList->header(), &QHeaderView::sectionResized, this, &CObjectCollectionViewComp::OnSectionResized);
	connect(ItemList->header(), &QHeaderView::sectionMoved, this, &CObjectCollectionViewComp::OnSectionMoved);

	BaseClass::OnGuiCreated();

	if (m_currentInformationViewPtr == nullptr){
		RightPanel->setVisible(false);
	}

	iqtgui::SetStyleSheetFromFile(*GetWidget(), ":/Styles/ObjectCollectionView");
}


void CObjectCollectionViewComp::OnGuiDestroyed()
{
	m_readCollectionThread.requestInterruption();
	m_readCollectionThread.wait();

	if (m_filterParamsGuiCompPtr.IsValid() && m_filterParamsGuiCompPtr->IsGuiCreated()){
		m_filterParamsGuiCompPtr->DestroyGui();
	}

	if ((m_currentInformationViewPtr != nullptr) && m_currentInformationViewPtr->IsGuiCreated()){
		m_currentInformationViewPtr->DestroyGui();
	}

	BaseClass::OnGuiDestroyed();
}


void CObjectCollectionViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	UpdateGui(istd::IChangeable::GetAnyChange());
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CObjectCollectionViewComp::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if ((modelId >= MI_DOCUMENT_TYPE_VISUAL_STATUS_BASE) && (modelId < MI_LAST)){
		if (IsGuiCreated()){
			UpdateTypeStatus();
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CObjectCollectionViewComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	for (int i = 0; i < m_viewDelegatesCompPtr.GetCount(); ++i){
		ICollectionViewDelegate* delegatePtr = m_viewDelegatesCompPtr[i];
		if (delegatePtr != nullptr){
			QByteArray typeId = delegatePtr->GetSupportedTypeId();
			Q_ASSERT(!typeId.isEmpty());

			m_viewDelegateMap[typeId] = delegatePtr;

			imod::IModel* documentTypeVisualStatusModelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(&delegatePtr->GetDocumentTypeStatus()));
			if (documentTypeVisualStatusModelPtr != nullptr){
				BaseClass2::RegisterModel(documentTypeVisualStatusModelPtr, MI_DOCUMENT_TYPE_VISUAL_STATUS_BASE + i);
			}
		}
	}
}


void CObjectCollectionViewComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	m_viewDelegateMap.clear();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CObjectCollectionViewComp::UpdateCommands()
{
	istd::IChangeable::ChangeSet changes(ibase::ICommandsProvider::CF_COMMANDS);
	istd::CChangeNotifier changeNotifier(&m_commands, &changes);

	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();

	imtbase::ICollectionInfo::Ids itemIds;

	QSet<QByteArray> selectedTypes;

	if (!selectedIndexes.isEmpty()){
		const imtbase::IObjectCollection* collectionPtr = GetObservedObject();
		Q_ASSERT(collectionPtr != nullptr);
		for (int i = 0; i < selectedIndexes.count(); ++i){
			QModelIndex mappedIndex = m_proxyModelPtr->mapToSource(selectedIndexes[i]);
			QStandardItem* itemPtr = m_itemModelPtr->itemFromIndex(mappedIndex);
			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				if (!itemId.isEmpty()){
					itemIds.push_back(itemPtr->data(DR_OBJECT_ID).toByteArray());
				}
			}
		}
	}

	GetViewDelegateRef(m_currentTypeId).UpdateItemSelection(itemIds, m_currentTypeId);
}


QVector<QByteArray> CObjectCollectionViewComp::GetMetaInfoIds(const QByteArray& typeId) const
{
	const ICollectionViewDelegate& viewDelegate = GetViewDelegate(typeId);
	const imtbase::ICollectionInfo& fieldCollection = viewDelegate.GetSummaryInformationTypes();
	return fieldCollection.GetElementIds();
}


QStringList CObjectCollectionViewComp::GetMetaInfoHeaders(const QByteArray& typeId) const
{
	const ICollectionViewDelegate& viewDelegate = GetViewDelegate(typeId);
	const imtbase::ICollectionInfo& fieldCollection = viewDelegate.GetSummaryInformationTypes();
	QVector<QByteArray> fieldIds = fieldCollection.GetElementIds();

	QStringList headers;
	for (QByteArray fieldId : fieldIds){
		headers += fieldCollection.GetElementInfo(fieldId, imtbase::ICollectionInfo::EIT_NAME).toString();
	}

	return headers;
}


CObjectCollectionViewComp::ObjectMetaInfo CObjectCollectionViewComp::GetMetaInfo(const QByteArray& itemId, const QByteArray& typeId) const
{
	const ICollectionViewDelegate& viewDelegate = GetViewDelegate(typeId);
	const imtbase::ICollectionInfo& fieldCollection = viewDelegate.GetSummaryInformationTypes();
	QVector<QByteArray> fieldIds = fieldCollection.GetElementIds();

	ObjectMetaInfo result;

	for (QByteArray fieldId : fieldIds){
		ICollectionViewDelegate::SummaryInformation summaryInfo = viewDelegate.GetSummaryInformation(itemId, fieldId);
		result.append(summaryInfo);
	}

	return result;
}


void CObjectCollectionViewComp::UpdateItem(const imtbase::IObjectCollectionInfo::Id& objectId, QStandardItemModel* modelPtr)
{
	imtbase::IObjectCollection* objectCollectionPtr = GetObservedObject();
	Q_ASSERT(objectCollectionPtr != nullptr);

	int row = -1;
	for (int i = 0; i < modelPtr->rowCount(); i++){
		if (modelPtr->item(i)->data(DR_OBJECT_ID) == objectId){
			row = i;
			break;
		}
	}

	QByteArray itemTypeId = objectCollectionPtr->GetObjectTypeId(objectId);

	ObjectMetaInfo metaInfo = GetMetaInfo(objectId, itemTypeId);
	if (metaInfo.isEmpty()){
		return;
	}

	QList<QStandardItem*> columns;

	//for (ICollectionViewDelegate::SummaryInformation metaInfoItem : metaInfo){
	for (int i = 0; i < metaInfo.count(); i++){
		if (row == -1){
			QStandardItem* column = new QStandardItem(metaInfo[i].text);

			column->setData(metaInfo[i].sortValue, DR_SORT_VALUE);
			if (!metaInfo[i].icon.isNull()){
				column->setIcon(metaInfo[i].icon);
			}

			columns.append(column);
		}
		else{
			QStandardItem* column = modelPtr->item(row, i);

			column->setText(metaInfo[i].text);
			column->setData(metaInfo[i].sortValue, DR_SORT_VALUE);
			if (!metaInfo[i].icon.isNull()){
				column->setIcon(metaInfo[i].icon);
			}
		}
	}

	if (row == -1){
		Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
		if (objectCollectionPtr->GetOperationFlags(objectId) & imtbase::IObjectCollection::OF_SUPPORT_EDIT){
			flags |= Qt::ItemIsEditable;
		}

		columns[0]->setData(objectId, DR_OBJECT_ID);
		columns[0]->setData(objectCollectionPtr->GetElementInfo(objectId, imtbase::IObjectCollectionInfo::EIT_TYPE_ID), DR_TYPE_ID);

		for (int i = 0; i < columns.count(); i++){
			columns[i]->setFlags(flags);
		}

		modelPtr->appendRow(columns);
	}
}


void CObjectCollectionViewComp::RemoveItem(const imtbase::IObjectCollectionInfo::Id& objectId)
{
	for (int i = 0; i < m_itemModelPtr->rowCount(); i++){
		if (m_itemModelPtr->item(i)->data(DR_OBJECT_ID) == objectId){
			m_itemModelPtr->removeRow(i);
			break;
		}
	}
}


void CObjectCollectionViewComp::EnsureColumnsSettingsSynchronized() const
{
	ColumnsList columnsList;
	ColumnSettings columnSettings;
	QVector<QByteArray> ids = GetMetaInfoIds(m_currentTypeId);
	QStringList headers = GetMetaInfoHeaders(m_currentTypeId);

	int totalWidth = 0;
	
	for (int i = 0; i < ids.count(); i++){
		int columndIndex = ItemList->header()->logicalIndex(i);
		totalWidth += ItemList->columnWidth(columndIndex);
	}

	for (int i = 0; i < ids.count(); i++){
		int columndIndex = ItemList->header()->logicalIndex(i);
		int fieldIndex = columndIndex;

		ColumnSettings settings;
		settings["FieldId"] = QString(ids[fieldIndex]);
		settings["Width"] = (double)ItemList->columnWidth(columndIndex) / totalWidth;
		columnsList.append(settings);
	}

	m_typeIdColumnsSettings[m_currentTypeId] = columnsList;
}


void CObjectCollectionViewComp::RestoreColumnsSettings()
{
	SignalSemaphore semaphore(m_semaphoreCounter);

	// Restore visual cloumn position by model
	for (int i = 0; i < m_itemModelPtr->columnCount(); i++){
		ItemList->header()->moveSection(ItemList->header()->visualIndex(i), i);
	}

	QVector<QByteArray> tempFieldIds = GetMetaInfoIds(m_currentTypeId);
	QStringList fieldIds;
	for (QByteArray tempFieldId : tempFieldIds){
		fieldIds.append(tempFieldId);
	}
	QSet<QString> fieldSet = fieldIds.toSet();

	// Compare restored column set with actual column set:
	bool compareOk = true;

	if (m_typeIdColumnsSettings.contains(m_currentTypeId)){
		ColumnsList columnsList = m_typeIdColumnsSettings[m_currentTypeId];
	
		for (ColumnSettings settings : columnsList){
			QVariant varFieldId = settings["FieldId"];
			QVariant varWidth = settings["Width"];

			if (varFieldId.type() != QVariant::String){
				compareOk = false;
				break;
			}

			if (varWidth.type() != QVariant::Double){
				compareOk = false;
				continue;
			}

			QString fieldId = varFieldId.toString();
			if (!fieldSet.contains(fieldId)){
				compareOk = false;
				break;
			}

			fieldSet -= fieldId;
		}
	}

	if (!fieldSet.isEmpty()){
		compareOk = false;
	}

	// Set column order and sizes:
	if (compareOk && m_typeIdColumnsSettings.contains(m_currentTypeId)){
		ColumnsList columnsList = m_typeIdColumnsSettings[m_currentTypeId];

		int currentIndex = 0;

		for (int i = 0; i < columnsList.count(); i++){
			ColumnSettings columnSettings = columnsList[i];
			
			QVariant varFieldId = columnSettings["FieldId"];
			QVariant varWidth = columnSettings["Width"];

			if (varFieldId.type() != QVariant::String){
				continue;
			}

			if (varWidth.type() != QVariant::Double){
				continue;
			}

			QString fieldId;
			fieldId = varFieldId.toString();

			bool ok;
			double width;
			width = varWidth.toDouble(&ok);

			if (fieldId.isEmpty()){
				if (ok){
					ItemList->setColumnWidth(0, width * ItemList->width());
				}
				else{
					ItemList->resizeColumnToContents(0);
				}
				
				currentIndex++;
				continue;
			}

			int logicIndex = fieldIds.indexOf(fieldId);
			ItemList->header()->moveSection(ItemList->header()->visualIndex(logicIndex), currentIndex);
			fieldSet.remove(fieldId);
				
			if (ok){
				int size = width * ItemList->width();
				ItemList->setColumnWidth(logicIndex, size);
				ValidateSectionSize(logicIndex, size);
			}
			else{
				int size = ItemList->width() / columnsList.count();
				ItemList->setColumnWidth(logicIndex, size);
				ValidateSectionSize(logicIndex, size);
			}

			currentIndex++;
		}
	}
	else{
		int columnCount = m_itemModelPtr->columnCount();
		for (int i = 0; i < columnCount; i++){
			int currentVisualIndex = ItemList->header()->visualIndex(i);
			ItemList->header()->moveSection(currentVisualIndex, i);
			ItemList->setColumnWidth(currentVisualIndex, ItemList->width() / columnCount);
		}
	}
}


void CObjectCollectionViewComp::ValidateSectionSize(int logicalIndex, int newSize)
{
	if (m_currentTypeId.isEmpty()){
		return;
	}

	const ICollectionViewDelegate& viewDelegate = GetViewDelegate(m_currentTypeId);
	QVector<QByteArray> ids = GetMetaInfoIds(m_currentTypeId);

	Q_ASSERT(logicalIndex < ids.count());

	ICollectionViewDelegate::HeaderInfo headerInfo = viewDelegate.GetSummaryInformationHeaderInfo(ids[logicalIndex]);

	if (newSize < headerInfo.minWidth){
		ItemList->setColumnWidth(logicalIndex, headerInfo.minWidth);
	}

	if (newSize > headerInfo.maxWidth){
		ItemList->setColumnWidth(logicalIndex, headerInfo.maxWidth);
	}
}


void CObjectCollectionViewComp::SaveItemsSelection()
{
	m_itemsSelection[m_currentTypeId].clear();

	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();
	if (!selectedIndexes.isEmpty()){
		for (int i = 0; i < selectedIndexes.count(); i++){
			QModelIndex mappedIndex = m_proxyModelPtr->mapToSource(selectedIndexes[i]);
			QStandardItem* itemPtr = m_itemModelPtr->itemFromIndex(mappedIndex);
			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				m_itemsSelection[m_currentTypeId].append(itemId);
			}
		}
	}
}


void CObjectCollectionViewComp::RestoreItemsSelection()
{
	QItemSelection selection;
	for (int i = 0; i < m_proxyModelPtr->rowCount(); i++){
		QModelIndex mappedIndex = m_proxyModelPtr->mapToSource(m_proxyModelPtr->index(i, 0));
		QStandardItem* itemPtr = m_itemModelPtr->itemFromIndex(mappedIndex);
		QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
		if (m_itemsSelection[m_currentTypeId].contains(itemId)){
			selection.append(QItemSelectionRange(m_proxyModelPtr->index(i, 0)));
		}
	}

	ItemList->selectionModel()->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}


void CObjectCollectionViewComp::ReadCollection(QStandardItemModel* typeModelPtr, QStandardItemModel* itemModelPtr)
{
	imtbase::IObjectCollection* objectCollectionPtr = GetObservedObject();
	if (objectCollectionPtr != nullptr){
		Q_ASSERT(typeModelPtr != nullptr);
		Q_ASSERT(itemModelPtr != nullptr);

		typeModelPtr->clear();
		itemModelPtr->clear();
		itemModelPtr->setColumnCount(0);

		const iprm::IOptionsList* objectTypeInfoPtr = objectCollectionPtr->GetObjectTypesInfo();
		if (objectTypeInfoPtr != nullptr){
			int typesCount = objectTypeInfoPtr->GetOptionsCount();
			imtbase::IObjectCollectionInfo::Ids collectionItemIds = objectCollectionPtr->GetElementIds();

			for (int typeIndex = 0; typeIndex < typesCount; ++typeIndex){
				if (objectTypeInfoPtr->IsOptionEnabled(typeIndex)){
					QByteArray typeId = objectTypeInfoPtr->GetOptionId(typeIndex);
					QString typeName = objectTypeInfoPtr->GetOptionName(typeIndex);

					QStandardItem* typeItemPtr = new QStandardItem();
					typeItemPtr->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
					typeItemPtr->setData(typeName, Qt::DisplayRole);
					typeItemPtr->setData(typeName, Qt::EditRole);
					typeItemPtr->setData(typeId, DR_TYPE_ID);
					typeModelPtr->appendRow(typeItemPtr);
				}
			}

			int progress = 0;
			int count = collectionItemIds.count();

			for (int i = 0; i < count; i++){
				UpdateItem(collectionItemIds[i], itemModelPtr);

				if (*m_useAsyncReadAttrPtr && *m_viewProgressAttrPtr){
					int currentProgress = i * 100 / count;
					if (progress != currentProgress){
						progress = currentProgress;
						QMetaObject::invokeMethod(this, "OnUpdateProgress", Qt::QueuedConnection, Q_ARG(int, progress));
					}
				}
			}
		}
	}
}


void CObjectCollectionViewComp::StartCollectionRead()
{
	if (m_readCollectionThreadState == RCTS_IDLE){
		if (m_proxyModelPtr->sourceModel() == &m_itemModel1){
			m_readCollectionThread.SetModels(&m_typeModel, &m_itemModel2);
		}
		else{
			m_readCollectionThread.SetModels(&m_typeModel, &m_itemModel1);
		}

		m_readCollectionThreadState = RCTS_RUNNING;
		m_readCollectionThread.start();
	}
	else{
		m_readCollectionThreadState = RCTS_PENDING;
	}
}


void CObjectCollectionViewComp::OnUpdateProgress(int progress)
{
	Message->setText(tr("Update..."));
	Progress->setValue(progress);
	Message->show();
	Progress->show();
}


void CObjectCollectionViewComp::OnCollectionReadFinished()
{
	Progress->setValue(0);
	Message->hide();
	Progress->hide();

	{
		SignalSemaphore semaphore(m_semaphoreCounter);

		if (m_typeModel.rowCount() > 1){
			TypeList->show();
		}
		else {
			TypeList->hide();
		}

		QByteArray lastTypeId = m_currentTypeId;

		TypeList->clear();
		QTreeWidgetItem* activeTypeItemPtr = nullptr;

		for (int i = 0; i < m_typeModel.rowCount(); i++){
			QStandardItem* itemPtr = m_typeModel.item(i, 0);

			QTreeWidgetItem* typeItemPtr = new QTreeWidgetItem(TypeList);
			typeItemPtr->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			typeItemPtr->setData(0, Qt::DisplayRole, itemPtr->data(Qt::DisplayRole));
			typeItemPtr->setData(0, Qt::EditRole, itemPtr->data(Qt::EditRole));
			typeItemPtr->setData(0, DR_TYPE_ID, itemPtr->data(DR_TYPE_ID));
			TypeList->addTopLevelItem(typeItemPtr);

			if (lastTypeId == itemPtr->data(DR_TYPE_ID)){
				activeTypeItemPtr = typeItemPtr;
			}

			if (lastTypeId.isEmpty() && (i == 0)){
				activeTypeItemPtr = typeItemPtr;
			}
		}

		if (*m_useAsyncReadAttrPtr){
			if (m_proxyModelPtr->sourceModel() == &m_itemModel1){
				m_itemModelPtr = &m_itemModel2;
			}
			else{
				m_itemModelPtr = &m_itemModel1;
			}
		}

		m_proxyModelPtr->setSortRole(DR_SORT_VALUE);
		m_proxyModelPtr->setSourceModel(m_itemModelPtr);

		if (activeTypeItemPtr != nullptr){
			activeTypeItemPtr->setSelected(true);
			const ICollectionViewDelegate &delegate = GetViewDelegateRef(m_currentTypeId);
			const imtbase::ICollectionInfo &collectionInfo = delegate.GetSummaryInformationTypes();

			imtbase::ICollectionInfo::Ids ids = collectionInfo.GetElementIds();
			for (int i = 0; i < ids.count(); ++i){
				QByteArray id = ids[i];
				ItemList->itemDelegateForColumn(i)->deleteLater();
				QAbstractItemDelegate *itemDelegate = delegate.GetColumnItemDelegate(id);
				ItemList->setItemDelegateForColumn(i, itemDelegate);
			}
		}
	}

	UpdateCommands();

	RestoreItemsSelection();

	UpdateTypeStatus();

	if (m_readCollectionThreadState == RCTS_PENDING){
		m_readCollectionThreadState = RCTS_IDLE;
		StartCollectionRead();
	}
	else{
		m_readCollectionThreadState = RCTS_IDLE;
	}
}


void CObjectCollectionViewComp::ProcessObjectCollectionEventSync(
			ObjectCollectionPtr /*objectCollectionPtr*/,
			ObjectCollectionEventPtr eventPtr)
{
	switch (eventPtr->GetEventType()){
	case imtbase::IObjectCollectionEvent::ET_INSERT:
		UpdateItem(eventPtr->GetItemId(), m_itemModelPtr);
		if (!m_itemsSelection.isEmpty()){
			UpdateCommands();
		}
		break;
	case imtbase::IObjectCollectionEvent::ET_UPDATE:
		UpdateItem(eventPtr->GetItemId(), m_itemModelPtr);
		if (!m_itemsSelection.isEmpty()){
			UpdateCommands();
		}
		break;
	case imtbase::IObjectCollectionEvent::ET_REMOVE:
		RemoveItem(eventPtr->GetItemId());
		break;

	default:
		break;
	}
}


void CObjectCollectionViewComp::UpdateTypeStatus()
{
	for (int delegateIndex = 0; delegateIndex < m_viewDelegatesCompPtr.GetCount(); ++delegateIndex){
		ICollectionViewDelegate* delegatePtr = m_viewDelegatesCompPtr[delegateIndex];
		if (delegatePtr != nullptr){
			QByteArray delegateTypeId = delegatePtr->GetSupportedTypeId();

			for (int itemIndex = 0; itemIndex < TypeList->topLevelItemCount(); itemIndex++){
				QByteArray itemTypeId = TypeList->topLevelItem(itemIndex)->data(0, DR_TYPE_ID).toByteArray();
				if (delegateTypeId == itemTypeId){
					TypeList->topLevelItem(itemIndex)->setIcon(0, delegatePtr->GetDocumentTypeStatus().GetStatusIcon());
					TypeList->topLevelItem(itemIndex)->setToolTip(0, delegatePtr->GetDocumentTypeStatus().GetStatusText());
					break;
				}
			}
		}
	}
}


bool CObjectCollectionViewComp::eventFilter(QObject *object, QEvent *event)
{
	if (object == ItemList){
		if (event->type() == QEvent::Resize){
			QResizeEvent *resizeEvent = (QResizeEvent*)event;
			if (resizeEvent->size() != resizeEvent->oldSize()){
				RestoreColumnsSettings();
			}
		}
	}

	return BaseClass::eventFilter(object, event);
}


// private slots

void CObjectCollectionViewComp::OnSelectionChanged(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/)
{
	if (m_semaphoreCounter > 0){
		return;
	}

	istd::CChangeNotifier notifier(this);

	SaveItemsSelection();
	UpdateCommands();

	if (ItemList->selectionModel()->selectedRows().isEmpty()){
		ItemList->setCurrentIndex(QModelIndex());
	}
}


void CObjectCollectionViewComp::OnItemDoubleClick(const QModelIndex &item)
{
	int sourceRow = m_proxyModelPtr->mapToSource(item).row();
	QByteArray itemId = m_itemModelPtr->item(sourceRow, 0)->data(DR_OBJECT_ID).toByteArray();

	const ICollectionViewDelegate &delegate = GetViewDelegateRef(m_currentTypeId);
	const imtbase::ICollectionInfo &collectionInfo = delegate.GetSummaryInformationTypes();

	imtbase::ICollectionInfo::Ids ids = collectionInfo.GetElementIds();
	int column = item.column();
	if (column < ids.count()) {
		if (delegate.IsEditorEnabled(ids[column])){
			delegate.OpenDocumentEditor(itemId);
		}
	}
}


void CObjectCollectionViewComp::OnCustomContextMenuRequested(const QPoint &point)
{
	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();
	if (selectedIndexes.isEmpty()){
		return;
	}

	QAction* actionRename;
	QAction* actionEditDescription;
	QAction* actionEditDocument;
	QAction* actionRemove;
	QMenu menu(ItemList);

	if (GetViewDelegate(m_currentTypeId).IsCommandSupported(imtgui::CObjectCollectionViewDelegate::CI_EDIT)){
		actionEditDocument = menu.addAction(QIcon(":/Icons/Edit"), tr("Edit..."));
		connect(actionEditDocument, &QAction::triggered, this, &CObjectCollectionViewComp::OnContextMenuEditDocument);
	}

	if (GetViewDelegate(m_currentTypeId).IsCommandSupported(imtgui::CObjectCollectionViewDelegate::CI_REMOVE)){
		actionRemove = menu.addAction(QIcon(":/Icons/Remove"), tr("Remove"));
		connect(actionRemove, &QAction::triggered, this, &CObjectCollectionViewComp::OnContextMenuRemove);
	}

	if (selectedIndexes.count() == 1){
		if (menu.actions().count() > 0){
			menu.addSeparator();
		}

		if (GetViewDelegate(m_currentTypeId).IsCommandSupported(imtgui::CObjectCollectionViewDelegate::CI_EDIT_DESCRIPTION)){
			actionEditDescription = menu.addAction(tr("Set Description..."));
			connect(actionEditDescription, &QAction::triggered, this, &CObjectCollectionViewComp::OnContextMenuEditDescription);
		}
		if (GetViewDelegate(m_currentTypeId).IsCommandSupported(imtgui::CObjectCollectionViewDelegate::CI_RENAME)){
			actionRename = menu.addAction(tr("Rename..."));
			connect(actionRename, &QAction::triggered, this, &CObjectCollectionViewComp::OnContextMenuRename);
		}
	}

	if (menu.actions().count() > 0){
		menu.exec(ItemList->viewport()->mapToGlobal(point));
	}
}


void CObjectCollectionViewComp::OnSectionResized(int logicalIndex, int /*oldSize*/, int newSize)
{
	if (m_semaphoreCounter > 0){
		return;
	}

	ValidateSectionSize(logicalIndex, newSize);

	EnsureColumnsSettingsSynchronized();
}


void CObjectCollectionViewComp::OnSectionMoved(int logicalIndex, int /*oldVisualIndex*/, int /*newVisualIndex*/)
{
	if (m_semaphoreCounter > 0){
		return;
	}

	SignalSemaphore semaphore(m_semaphoreCounter);

	if (!m_currentTypeId.isEmpty()){
		const ICollectionViewDelegate& viewDelegate = GetViewDelegate(m_currentTypeId);
		QVector<QByteArray> ids = GetMetaInfoIds(m_currentTypeId);

		Q_ASSERT(logicalIndex < ids.count());

		QList<ICollectionViewDelegate::HeaderInfo> infos;

		for (int i = 0; i < ids.count(); i++){
			ICollectionViewDelegate::HeaderInfo headerInfo = viewDelegate.GetSummaryInformationHeaderInfo(ids[i]);
			infos.append(headerInfo);
		}

		QQueue<int> movable;
		QHeaderView *headerPtr = ItemList->header();

		for (int i = 0; i < ids.count(); i++){
			int index = headerPtr->logicalIndex(i);

			if (!infos[index].isFixed){
				movable.enqueue(index);
			}
		}

		for (int i = 0; i < ids.count(); i++){
			if (infos[i].isFixed){
				int from = headerPtr->visualIndex(i);
				headerPtr->moveSection(from, i);
			}
			else{
				int from = headerPtr->visualIndex(movable.dequeue());
				headerPtr->moveSection(from, i);
			}
		}
	}

	EnsureColumnsSettingsSynchronized();
}


void CObjectCollectionViewComp::OnTypeChanged()
{
	SignalSemaphore semaphore(m_semaphoreCounter);

	SaveItemsSelection();

	m_currentTypeId.clear();

	QList<QTreeWidgetItem*> selectedItems = TypeList->selectedItems();
	if (!selectedItems.isEmpty()){
		m_currentTypeId = selectedItems[0]->data(0, DR_TYPE_ID).toByteArray();
	}

	m_proxyModelPtr->setFilterFixedString(m_currentTypeId);
	
	QStringList headerLabels;
	headerLabels.append(GetMetaInfoHeaders(m_currentTypeId));
	m_itemModelPtr->setHorizontalHeaderLabels(headerLabels);

	for (int i = 0; i < m_itemModelPtr->columnCount(); i++){
		if (i < headerLabels.count()){
			ItemList->showColumn(i);
		}
		else {
			ItemList->hideColumn(i);
		}
	}

	if (m_semaphoreCounter == 1){
		RestoreItemsSelection();
		RestoreColumnsSettings();
	}
	else{
		if (!m_currentTypeId.isEmpty() && m_typeIdColumnsSettings.contains(m_currentTypeId)){
			RestoreItemsSelection();
			RestoreColumnsSettings();
		}
	}

	if ((m_currentInformationViewPtr != nullptr) && m_currentInformationViewPtr->IsGuiCreated()){
		m_currentInformationViewPtr->DestroyGui();
	}

	if (!m_currentTypeId.isEmpty()){
		m_currentInformationViewPtr = GetViewDelegateRef(m_currentTypeId).GetInformationView();
		if (m_currentInformationViewPtr != nullptr){
			m_currentInformationViewPtr->CreateGui(RightPanel);
			if (*m_viewRightPanelAttrPtr){
				RightPanel->setVisible(true);
			}
		}
	}

	disconnect(m_renameShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnRenameShortCut);
	if (GetViewDelegate(m_currentTypeId).IsCommandSupported(imtgui::CObjectCollectionViewDelegate::CI_RENAME)){
		connect(m_renameShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnRenameShortCut);
	}

	disconnect(m_delShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnDelShortCut);
	if (GetViewDelegate(m_currentTypeId).IsCommandSupported(imtgui::CObjectCollectionViewDelegate::CI_REMOVE)){
		connect(m_delShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnDelShortCut);
	}

	UpdateCommands();
}


void CObjectCollectionViewComp::OnContextMenuRename(bool /*checked*/)
{
	ICollectionViewDelegate & delegate = GetViewDelegateRef(m_currentTypeId);
	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();

	if (selectedIndexes.count() != 1){
		return;
	}

	if (!selectedIndexes.isEmpty()){
		for (int i = 0; i < selectedIndexes.count(); ++i){
			QModelIndex mappedIndex = m_proxyModelPtr->mapToSource(selectedIndexes[i]);
			QStandardItem* itemPtr = m_itemModelPtr->itemFromIndex(mappedIndex);
			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				if (!itemId.isEmpty()){
					delegate.RenameObject(itemId, "");
				}
			}
		}
	}
}


void CObjectCollectionViewComp::OnContextMenuEditDescription(bool /*checked*/)
{
	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();

	if (selectedIndexes.count() != 1){
		return;
	}

	if (!selectedIndexes.isEmpty()){
		for (int i = 0; i < selectedIndexes.count(); ++i){
			QModelIndex mappedIndex = m_proxyModelPtr->mapToSource(selectedIndexes[i]);
			QStandardItem* itemPtr = m_itemModelPtr->itemFromIndex(mappedIndex);
			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				if (!itemId.isEmpty()){
					imtbase::IObjectCollection* objectPtr = GetObservedObject();
					QString description = objectPtr->GetElementInfo(itemId, imtbase::IObjectCollectionInfo::EIT_DESCRIPTION).toString();

					bool ok;
					QString newDescription = QInputDialog::getText(ItemList, tr("Enter object description"), tr("Description"), QLineEdit::Normal, description, &ok);
					if (ok){
						objectPtr->SetObjectDescription(itemId, newDescription);
					}
				}
			}
		}
	}
}


void CObjectCollectionViewComp::OnContextMenuEditDocument(bool /*checked*/)
{
	ICollectionViewDelegate & delegate = GetViewDelegateRef(m_currentTypeId);
	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();

	if (!selectedIndexes.isEmpty()){
		for (int i = 0; i < selectedIndexes.count(); ++i){
			QModelIndex mappedIndex = m_proxyModelPtr->mapToSource(selectedIndexes[i]);
			QStandardItem* itemPtr = m_itemModelPtr->itemFromIndex(mappedIndex);
			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				if (!itemId.isEmpty()){
					delegate.OpenDocumentEditor(itemId);
				}
			}
		}
	}
}


void CObjectCollectionViewComp::OnContextMenuRemove(bool /*checked*/)
{
	ICollectionViewDelegate & delegate = GetViewDelegateRef(m_currentTypeId);
	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();

	QVector<QByteArray> itemIds;
	if (!selectedIndexes.isEmpty()){
		for (int i = 0; i < selectedIndexes.count(); ++i){
			QModelIndex mappedIndex = m_proxyModelPtr->mapToSource(selectedIndexes[i]);
			QStandardItem* itemPtr = m_itemModelPtr->itemFromIndex(mappedIndex);
			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				if (!itemId.isEmpty()){
					itemIds.append(itemId);
				}
			}
		}
	}

	delegate.RemoveObjects(itemIds);
}


void CObjectCollectionViewComp::OnFilterChanged(const QString &text)
{
	m_proxyModelPtr->setFilter(text);
	m_itemModelPtr->dataChanged(m_itemModelPtr->index(0,0), m_itemModelPtr->index(m_itemModelPtr->rowCount() - 1, m_itemModelPtr->columnCount() - 1));
}


void CObjectCollectionViewComp::OnSearchShortCut()
{
	if (m_filterPanelAnimationPtr->state() != QAbstractAnimation::State::Stopped){
		return;
	}

	if (FilterPanel->maximumHeight() == FilterPanel->sizeHint().height()){
		FilterEdit->setFocus();
	}

	if (FilterPanel->maximumHeight() != 0){
		return;
	}

	m_filterPanelAnimationPtr->setStartValue(0);
	m_filterPanelAnimationPtr->setEndValue(FilterPanel->sizeHint().height());
	m_filterPanelAnimationPtr->setDuration(300);
	m_filterPanelAnimationPtr->start();
	FilterEdit->setFocus();
}


void CObjectCollectionViewComp::OnEscShortCut()
{
	if (m_filterPanelAnimationPtr->state() != QAbstractAnimation::State::Stopped){
		return;
	}

	if (FilterPanel->maximumHeight() == 0){
		return;
	}

	m_filterPanelAnimationPtr->setStartValue(FilterPanel->height());
	m_filterPanelAnimationPtr->setEndValue(0);
	m_filterPanelAnimationPtr->setDuration(300);
	m_filterPanelAnimationPtr->start();
	GetQtWidget()->setFocus();
	FilterEdit->clear();

	OnFilterChanged("");
}


void CObjectCollectionViewComp::OnDelShortCut()
{
	OnContextMenuRemove(false);
}


void CObjectCollectionViewComp::OnRenameShortCut()
{
	OnContextMenuRename(false);
}


// public methods of the embedded class Commands

CObjectCollectionViewComp::Commands::Commands()
	:m_parentPtr(nullptr)
{
}


void CObjectCollectionViewComp::Commands::SetParent(CObjectCollectionViewComp* parentPtr)
{
	Q_ASSERT(parentPtr != nullptr);

	m_parentPtr = parentPtr;
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CObjectCollectionViewComp::Commands::GetCommands() const
{
	Q_ASSERT(m_parentPtr != nullptr);

	return m_parentPtr->GetViewDelegate(m_parentPtr->m_currentTypeId).GetCommands();
}


// protected methods of the embedded class ItemProxyModel


// reimplemented (QSortFilterProxyModel)

bool CObjectCollectionViewComp::ItemProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& /*sourceParent*/) const
{
	QRegExp internalFilterRegExp = filterRegExp();
	int internalFilterColumn = filterKeyColumn();
	int internalFilterRole = filterRole();

	QString internalFilterData = sourceModel()->index(sourceRow, internalFilterColumn).data(internalFilterRole).toString();

	if (!internalFilterRegExp.exactMatch(internalFilterData)){
		return false;
	}

	if (m_filter.isEmpty()){
		return true;
	}

	for (int i = 0; i < columnCount(); i++){
		QString value = sourceModel()->index(sourceRow, i).data(Qt::DisplayRole).toString();
		if (value.contains(m_filter, Qt::CaseInsensitive)){
			return true;
		}
	}	

	return false;
}


// reimplemented (QSortFilterProxyModel)

bool CObjectCollectionViewComp::ItemProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	QVariant leftValue = sourceModel()->data(left);
	QVariant rightValue = sourceModel()->data(right);

	bool result;

	if (TryDateTime(leftValue, rightValue, result)){
		return result;
	}
	else if (TryInt(leftValue, rightValue, result)){
		return result;
	}
	else{
		// if neither DateTime, nor Int or convertible to Int, then sort it as QString
		QString leftString = leftValue.toString();
		QString rightString = rightValue.toString();
		return  QString::localeAwareCompare(leftString, rightString) < 0;
	}
}


// private methods of embedded class ItemProxyModel

bool CObjectCollectionViewComp::ItemProxyModel::TryDateTime(QVariant left, QVariant right, bool& result) const
{
	QString leftString = left.toString();
	QString rightString = right.toString();

	QDateTime leftDate = left.toDateTime();
	if (leftDate.isNull()) {
		leftDate = QDateTime::fromString(leftString, CObjectCollectionViewDelegate::s_dateTimeFormat);
	}
	QDateTime rightDate = right.toDateTime();
	if (rightDate.isNull()) {
		rightDate = QDateTime::fromString(rightString, CObjectCollectionViewDelegate::s_dateTimeFormat);
	}

	if (leftDate.isValid() && rightDate.isValid()){
		result = (leftDate < rightDate);
		return true;
	}

	return false;
}


bool CObjectCollectionViewComp::ItemProxyModel::TryInt(QVariant left, QVariant right, bool& result) const
{
	if (left.userType() == QMetaType::Int && right.userType() == QMetaType::Int){
		result = left.toInt() < right.toInt();
		return true;
	}

	bool ok;
	int valueLeft = left.toInt(&ok);
	if (ok){
		int valueRight = right.toInt(&ok);
		if (ok){
			result = (valueLeft < valueRight);
			return true;
		}
	}

	return false;
}



// protected methods of the embedded class FocusDecorationFactory

// reimplemented (IGraphicsEffectFactory)

QGraphicsEffect* CObjectCollectionViewComp::FocusDecorationFactory::CreateInstance(const QByteArray& /*keyId*/) const
{
	QGraphicsDropShadowEffect* shadowPtr = new QGraphicsDropShadowEffect;
	shadowPtr->setXOffset(0);
	shadowPtr->setYOffset(0);
	shadowPtr->setBlurRadius(6);
	shadowPtr->setColor(qRgba(74, 149, 217, 128));

	return shadowPtr;
}


istd::IFactoryInfo::KeyList CObjectCollectionViewComp::FocusDecorationFactory::GetFactoryKeys() const
{
	istd::IFactoryInfo::KeyList retVal;

	return retVal;
}


// public methods of embedded class UpdateThread

CObjectCollectionViewComp::ReadCollectionThread::ReadCollectionThread(CObjectCollectionViewComp* parentPtr)
	:m_parentPtr(parentPtr),
	m_typeModelPtr(nullptr),
	m_itemModelPtr(nullptr)
{
}


void CObjectCollectionViewComp::ReadCollectionThread::SetModels(QStandardItemModel* typeModelPtr, QStandardItemModel* itemModelPtr)
{
	m_typeModelPtr = typeModelPtr;
	m_itemModelPtr = itemModelPtr;
}


// private methods of embedded class UpdateThread

// reimplemented (QThread)

void CObjectCollectionViewComp::ReadCollectionThread::run()
{
	m_parentPtr->ReadCollection(m_typeModelPtr, m_itemModelPtr);

	QMetaObject::invokeMethod(m_parentPtr, "OnCollectionReadFinished", Qt::QueuedConnection);
}


} // namespace imtgui


