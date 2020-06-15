#include <imtgui/CObjectCollectionViewComp.h>


// Qt includes
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


namespace imtgui
{


// protected methods

CObjectCollectionViewComp::CObjectCollectionViewComp()
	:m_semaphoreCounter(0),
	m_currentInformationViewPtr(nullptr)
{
	m_commands.SetParent(this);
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


// reimplemented (iqtgui::TRestorableGuiWrap)

void CObjectCollectionViewComp::OnRestoreSettings(const QSettings& settings)
{
	QByteArray columnSettingsKey = *m_columnSettingsKeyAttrPtr;
	if (columnSettingsKey.isEmpty()){
		return;
	}

	QByteArray key("ObjectCollectionViewColumnsSettings/" + columnSettingsKey);
	if (!settings.contains(key)){
		return;
	}
	
	QVariant settingsValue = settings.value(key);
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
	QByteArray columnSettingsKey = *m_columnSettingsKeyAttrPtr;
	if (columnSettingsKey.isEmpty()){
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

	settings.setValue(columnSettingsKey, data);

	settings.endGroup();
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CObjectCollectionViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	const imtbase::IObjectCollection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	{
		SignalSemaphore semaphore(m_semaphoreCounter);

		QByteArray lastTypeId = m_currentTypeId;

		TypeList->clear();

		m_itemModel.clear();
		m_itemModel.setColumnCount(0);

		const iprm::IOptionsList* objectTypeInfoPtr = objectPtr->GetObjectTypesInfo();
		if (objectTypeInfoPtr != nullptr){
			int typesCount = objectTypeInfoPtr->GetOptionsCount();
			imtbase::IObjectCollectionInfo::Ids collectionItemIds = objectPtr->GetElementIds();

			if (typesCount > 1){
				TypeList->show();
			}
			else {
				TypeList->hide();
			}

			QTreeWidgetItem* activeTypeItemPtr = nullptr;

			for (int typeIndex = 0; typeIndex < typesCount; ++typeIndex){
				QByteArray typeId = objectTypeInfoPtr->GetOptionId(typeIndex);
				QString typeName = objectTypeInfoPtr->GetOptionName(typeIndex);

				QTreeWidgetItem* typeItemPtr = new QTreeWidgetItem(TypeList);
				typeItemPtr->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				typeItemPtr->setData(0, Qt::DisplayRole, typeName);
				typeItemPtr->setData(0, Qt::EditRole, typeName);
				typeItemPtr->setData(0, DR_TYPE_ID, typeId);
				TypeList->addTopLevelItem(typeItemPtr);

				if (lastTypeId == typeId){
					activeTypeItemPtr = typeItemPtr;
				}

				if (lastTypeId.isEmpty() && (typeIndex == 0)){
					activeTypeItemPtr = typeItemPtr;
				}
			}

			for (const QByteArray& itemId : collectionItemIds){
				QByteArray itemTypeId = objectPtr->GetObjectTypeId(itemId);

				QList<QStandardItem*> columns;
				ObjectMetaInfo metaInfo = GetMetaInfo(itemId, itemTypeId);

				if (metaInfo.isEmpty()){
					continue;
				}

				for (ICollectionViewDelegate::SummaryInformation metaInfoItem : metaInfo){
					QStandardItem* column = new QStandardItem(metaInfoItem.text);
					if (!metaInfoItem.icon.isNull()){
						column->setIcon(metaInfoItem.icon);
					}
					column->setData(metaInfoItem.sortValue, DR_SORT_VALUE);

					columns.append(column);
				}

				Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
				if (objectPtr->GetSupportedOperations() & imtbase::IObjectCollection::OF_SUPPORT_RENAME){
					flags |= Qt::ItemIsEditable;
				}

				columns[0]->setData(itemId, DR_OBJECT_ID);
				columns[0]->setData(objectPtr->GetElementInfo(itemId, imtbase::IObjectCollectionInfo::EIT_TYPE_ID), DR_TYPE_ID);
				columns[0]->setFlags(flags);

				m_itemModel.appendRow(columns);
			}

			if (activeTypeItemPtr != nullptr){
				activeTypeItemPtr->setSelected(true);
			}
		}
	}

	m_proxyModelPtr->setSortRole(DR_SORT_VALUE);

	UpdateCommands();

	RestoreItemsSelection();

	UpdateTypeStatus();
}


void CObjectCollectionViewComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	imtbase::IObjectCollection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	if (m_viewDelegateMap.isEmpty()){
		m_defaultViewDelegate.InitializeDelegate(objectPtr, this);
	}

	for (ViewDelegateMap::Iterator iter = m_viewDelegateMap.begin(); iter != m_viewDelegateMap.end(); ++iter){
		iter.value()->InitializeDelegate(objectPtr, this);
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CObjectCollectionViewComp::OnGuiCreated()
{
	ItemList->setProperty("ItemView", true);

	m_proxyModelPtr = new ItemProxyModel(this);
	m_proxyModelPtr->setSourceModel(&m_itemModel);
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

	connect(m_searchShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnSearchShortCut);
	connect(m_escShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnEscShortCut);
	connect(m_delShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnDelShortCut);
	connect(m_renameShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnRenameShortCut);

	ItemList->setContextMenuPolicy(Qt::CustomContextMenu);
	ItemList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ItemList->header()->setFirstSectionMovable(true);
	ItemList->installEventFilter(this);

	connect(TypeList, &QTreeWidget::itemSelectionChanged, this, &CObjectCollectionViewComp::OnTypeChanged);
	connect(ItemList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CObjectCollectionViewComp::OnSelectionChanged);
	connect(ItemList, &QTreeView::doubleClicked, this, &CObjectCollectionViewComp::OnItemDoubleClick);
	connect(ItemList, &QTreeView::customContextMenuRequested, this, &CObjectCollectionViewComp::OnCustomContextMenuRequested);
	connect(ItemList->header(), &QHeaderView::sectionResized, this, &CObjectCollectionViewComp::OnSectionResized);
	connect(ItemList->header(), &QHeaderView::sectionMoved, this, &CObjectCollectionViewComp::OnSectionMoved);

	BaseClass::OnGuiCreated();
}


void CObjectCollectionViewComp::OnGuiDestroyed()
{
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
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(mappedIndex);
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


QVector<QByteArray> CObjectCollectionViewComp::GetMetaInfoIds(const QByteArray &typeId) const
{
	const ICollectionViewDelegate& viewDelegate = GetViewDelegate(typeId);
	const imtbase::ICollectionInfo& fieldCollection = viewDelegate.GetSummaryInformationTypes();
	return fieldCollection.GetElementIds();
}


QStringList CObjectCollectionViewComp::GetMetaInfoHeaders(const QByteArray &typeId) const
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


CObjectCollectionViewComp::ObjectMetaInfo CObjectCollectionViewComp::GetMetaInfo(const QByteArray &itemId, const QByteArray &typeId) const
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
	for (int i = 0; i < m_itemModel.columnCount(); i++){
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
		int columnCount = m_itemModel.columnCount();
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
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(mappedIndex);
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
		QStandardItem* itemPtr = m_itemModel.itemFromIndex(mappedIndex);
		QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
		if (m_itemsSelection[m_currentTypeId].contains(itemId)){
			selection.append(QItemSelectionRange(m_proxyModelPtr->index(i, 0)));
		}
	}

	ItemList->selectionModel()->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
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

	SaveItemsSelection();
	UpdateCommands();

	if (ItemList->selectionModel()->selectedRows().isEmpty()){
		ItemList->setCurrentIndex(QModelIndex());
	}
}


void CObjectCollectionViewComp::OnItemDoubleClick(const QModelIndex &item)
{
	int sourceRow = m_proxyModelPtr->mapToSource(item).row();
	QByteArray itemId = m_itemModel.item(sourceRow, 0)->data(DR_OBJECT_ID).toByteArray();

	const ICollectionViewDelegate &delegate = GetViewDelegateRef(m_currentTypeId);
	delegate.OpenDocumentEditor(itemId);
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

	actionEditDocument = menu.addAction(QIcon(":/Icons/Edit"), tr("Edit..."));
	connect(actionEditDocument, &QAction::triggered, this, &CObjectCollectionViewComp::OnContextMenuEditDocument);

	if (GetViewDelegate(m_currentTypeId).IsCommandSupported(imtgui::CObjectCollectionViewDelegate::CI_REMOVE)){
		actionRemove = menu.addAction(QIcon(":/Icons/Remove"), tr("Remove"));
		connect(actionRemove, &QAction::triggered, this, &CObjectCollectionViewComp::OnContextMenuRemove);
	}

	if (selectedIndexes.count() == 1){
		menu.addSeparator();
		actionEditDescription = menu.addAction(tr("Set Description..."));
		if (GetViewDelegate(m_currentTypeId).IsCommandSupported(imtgui::CObjectCollectionViewDelegate::CI_RENAME)){
			actionRename = menu.addAction(tr("Rename..."));
			connect(actionRename, &QAction::triggered, this, &CObjectCollectionViewComp::OnContextMenuRename);
		}
		connect(actionEditDescription, &QAction::triggered, this, &CObjectCollectionViewComp::OnContextMenuEditDescription);
	}

	menu.exec(ItemList->viewport()->mapToGlobal(point));
}


void CObjectCollectionViewComp::OnSectionResized(int logicalIndex, int /*oldSize*/, int newSize)
{
	if (m_semaphoreCounter > 0){
		return;
	}

	ValidateSectionSize(logicalIndex, newSize);

	EnsureColumnsSettingsSynchronized();
}


void CObjectCollectionViewComp::OnSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
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
			int logicalIndex = headerPtr->logicalIndex(i);

			if (!infos[logicalIndex].isFixed){
				movable.enqueue(logicalIndex);
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

	UpdateCommands();
	
	QStringList headerLabels;
	headerLabels.append(GetMetaInfoHeaders(m_currentTypeId));
	m_itemModel.setHorizontalHeaderLabels(headerLabels);

	for (int i = 0; i < m_itemModel.columnCount(); i++){
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
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(mappedIndex);
			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				if (!itemId.isEmpty()){
					const imtbase::IObjectCollection* objectPtr = GetObservedObject();
					QString name = objectPtr->GetElementInfo(itemId, imtbase::IObjectCollectionInfo::EIT_NAME).toString();

					bool ok;
					const QString newName = QInputDialog::getText(ItemList, tr("Enter new object name"), tr("Name"), QLineEdit::Normal, name, &ok);
					if (ok){
						delegate.RenameObject(itemId, newName);
					}
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
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(mappedIndex);
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
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(mappedIndex);
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
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(mappedIndex);
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
	m_itemModel.dataChanged(m_itemModel.index(0,0), m_itemModel.index(m_itemModel.rowCount() - 1, m_itemModel.columnCount() - 1));
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


} // namespace imtgui


