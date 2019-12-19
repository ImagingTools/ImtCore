#include <imtgui/CObjectCollectionViewComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QDir>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QInputDialog>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonArray>

// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iqtgui/CCommandTools.h>
#include <iqtgui/CHierarchicalCommand.h>


namespace imtgui
{


// protected methods

CObjectCollectionViewComp::CObjectCollectionViewComp()
	:m_blockColumnsSettingsSynchronize(false)
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
	if (!settings.contains("ObjectCollectionViewColumns/Data")){
		return;
	}

	QVariant varData = settings.value("ObjectCollectionViewColumns/Data");
	if (varData.type() != QVariant::ByteArray){
		return;
	}

	QByteArray data = varData.toByteArray();
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
			for (QString key : keys) {
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
	EnsureColumnsSettingsSynchronized();

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

	settings.beginGroup("ObjectCollectionViewColumns");
	settings.setValue("Data", data);
	settings.endGroup();
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CObjectCollectionViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	const imtbase::IObjectCollection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	m_blockColumnsSettingsSynchronize = true;

	QByteArray lastTypeId = m_currentTypeId;

	TypeList->clear();
	m_itemModel.clear();
	m_itemModel.setColumnCount(1);
	m_itemModel.setHorizontalHeaderLabels(QStringList() << tr("Name"));

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

		for (int typeIndex = 0; typeIndex < typesCount; ++typeIndex){
			QByteArray typeId = objectTypeInfoPtr->GetOptionId(typeIndex);
			QString typeName = objectTypeInfoPtr->GetOptionName(typeIndex);

			QTreeWidgetItem* typeItemPtr = new QTreeWidgetItem;
			typeItemPtr->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			typeItemPtr->setData(0, Qt::DisplayRole, typeName);
			typeItemPtr->setData(0, Qt::EditRole, typeName);
			typeItemPtr->setData(0, DR_TYPE_ID, typeId);
			TypeList->addTopLevelItem(typeItemPtr);

			if (lastTypeId == typeId){
				TypeList->setItemSelected(typeItemPtr, true);
			}

			if (lastTypeId.isEmpty() && (typeIndex == 0)){
				TypeList->setItemSelected(typeItemPtr, true);
			}
		}

		QSet<QByteArray> foundedTypeIds;

		for (const QByteArray& itemId : collectionItemIds){
			QByteArray itemTypeId = objectPtr->GetObjectTypeId(itemId);

			QStandardItem* objectItemPtr = new QStandardItem;
			QString objectName = objectPtr->GetElementInfo(itemId, imtbase::IObjectCollectionInfo::EIT_NAME).toString();
			objectItemPtr->setData(objectName, Qt::DisplayRole);
			objectItemPtr->setData(objectName, Qt::EditRole);
			objectItemPtr->setData(itemId, DR_OBJECT_ID);
			objectItemPtr->setData(objectPtr->GetElementInfo(itemId, imtbase::IObjectCollectionInfo::EIT_TYPE_ID), DR_TYPE_ID);

			Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

			if (objectPtr->GetSupportedOperations() & imtbase::IObjectCollection::OF_SUPPORT_RENAME){
				flags |= Qt::ItemIsEditable;
			}		
			
			objectItemPtr->setFlags(flags);
			
			foundedTypeIds += itemTypeId;

			QList<QStandardItem*> columns;
			QStringList metaInfo = GetObjectMetaInfo(itemId, itemTypeId);

			columns += objectItemPtr;
			for(QString infoItem : metaInfo){
				columns += new QStandardItem(infoItem);
			}

			m_itemModel.appendRow(columns);
		}
	}

	UpdateCommands();

	on_TypeList_itemSelectionChanged();

	m_blockColumnsSettingsSynchronize = false;
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
	m_customProxyModelPtr = new QCustomSortFilterProxyModel(this);
	m_customProxyModelPtr->setSourceModel(&m_itemModel);

	m_proxyModelPtr = new QSortFilterProxyModel(this);
	m_proxyModelPtr->setSourceModel(m_customProxyModelPtr);
	m_proxyModelPtr->setFilterKeyColumn(0);
	m_proxyModelPtr->setFilterRole(DR_TYPE_ID);

	m_searchShortCutPtr = new QShortcut(GetQtWidget());
	m_searchShortCutPtr->setKey(Qt::CTRL + Qt::Key_F);
	m_escShortCutPtr = new QShortcut(GetQtWidget());
	m_escShortCutPtr->setKey(Qt::Key_Escape);

	m_focusDecoratorPtr = new iwidgets::CFocusDecorator(this);
	m_focusDecoratorPtr->RegisterWidget(FilterEdit, &m_graphicsEffectFactory);

	FilterPanel->setMaximumHeight(0);
	m_filterPanelAnimationPtr = new QPropertyAnimation(FilterPanel, "maximumHeight", this);

	ItemList->setModel(m_proxyModelPtr);

	QItemSelectionModel* selectionModelPtr = ItemList->selectionModel();
	if (selectionModelPtr != nullptr){
		connect(
					selectionModelPtr,
					SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
					this, SLOT(OnSelectionChanged(const QItemSelection&, const QItemSelection&)));
	}

	connect(&m_itemModel, &QStandardItemModel::itemChanged, this, &CObjectCollectionViewComp::OnItemChanged);
	connect(ItemList, &QTreeView::doubleClicked, this, &CObjectCollectionViewComp::OnItemDoubleClick);
	connect(ItemList, &QTreeView::customContextMenuRequested, this, &CObjectCollectionViewComp::OnCustomContextMenuRequested);
	connect(FilterEdit, &QLineEdit::textChanged, this, &CObjectCollectionViewComp::OnFilterChanged);
	connect(CloseButton, &QToolButton::clicked, this, &CObjectCollectionViewComp::OnEscShortCut);
	connect(m_searchShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnSearchShortCut);
	connect(m_escShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnEscShortCut);

	ItemList->setContextMenuPolicy(Qt::CustomContextMenu);
	ItemList->setEditTriggers(QAbstractItemView::NoEditTriggers);

	BaseClass::OnGuiCreated();
}


void CObjectCollectionViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	UpdateGui(istd::IChangeable::GetAnyChange());
}


// reimplemented (icomp::CComponentBase)

void CObjectCollectionViewComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int delegatesCount = qMin(m_viewDelegatesCompPtr.GetCount(), m_objectTypeIdsAttrPtr.GetCount());
	for (int i = 0; i < delegatesCount; ++i){
		ICollectionViewDelegate* delegatePtr = m_viewDelegatesCompPtr[i];
		if (delegatePtr != nullptr){
			QByteArray typeId = m_objectTypeIdsAttrPtr[i];
			Q_ASSERT(!typeId.isEmpty());

			m_viewDelegateMap[typeId] = delegatePtr;
		}
	}
}


void CObjectCollectionViewComp::OnComponentDestroyed()
{
	m_viewDelegateMap.clear();

	BaseClass::OnComponentDestroyed();
}


bool CObjectCollectionViewComp::QCustomSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	if (m_filter.isEmpty()){
		return true;
	}

	for (int i = 0; i < columnCount(); i++){
		QString value = sourceModel()->index(source_row, i).data(Qt::DisplayRole).toString();
		if (value.toLower().contains(m_filter.toLower())){
			return true;
		}
	}	

	return false;
}


QGraphicsEffect* CObjectCollectionViewComp::DefaultFocusDecorationFactory::CreateInstance(const QByteArray& keyId) const
{
	QGraphicsDropShadowEffect* shadowPtr = new QGraphicsDropShadowEffect;
	shadowPtr->setXOffset(0);
	shadowPtr->setYOffset(0);
	shadowPtr->setBlurRadius(6);
	shadowPtr->setColor(qRgba(74, 149, 217, 128));

	return shadowPtr;
}


istd::IFactoryInfo::KeyList CObjectCollectionViewComp::DefaultFocusDecorationFactory::GetFactoryKeys() const
{
	istd::IFactoryInfo::KeyList retVal;

	return retVal;
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
			QModelIndex mappedIndex1 = m_proxyModelPtr->mapToSource(selectedIndexes[i]);
			QModelIndex mappedIndex2 = m_customProxyModelPtr->mapToSource(mappedIndex1);
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(mappedIndex2);
			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				if (!itemId.isEmpty()){
					itemIds.push_back(itemPtr->data(DR_OBJECT_ID).toByteArray());
				}
			}
		}
	}

	int stateFlags = imtgui::ICollectionViewDelegate::VS_NONE;

	if (!itemIds.isEmpty()){
		stateFlags |= imtgui::ICollectionViewDelegate::VS_SELECTED;
		if (itemIds.count() == 1){
			stateFlags |= imtgui::ICollectionViewDelegate::VS_SINGLE_SELECTION;
		}
	}

	GetViewDelegateRef(m_currentTypeId).UpdateItemSelection(stateFlags, itemIds, m_currentTypeId);
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


QStringList CObjectCollectionViewComp::GetObjectMetaInfo(const QByteArray &itemId, const QByteArray &typeId) const
{
	const ICollectionViewDelegate& viewDelegate = GetViewDelegate(typeId);
	const imtbase::ICollectionInfo& fieldCollection = viewDelegate.GetSummaryInformationTypes();
	QVector<QByteArray> fieldIds = fieldCollection.GetElementIds();

	QStringList objectMetaInfo;

	for (QByteArray fieldId : fieldIds){
		QVariant info = viewDelegate.GetSummaryInformation(itemId, fieldId);

		switch (info.type()){
		case QVariant::ByteArray:
			objectMetaInfo.append(QString(info.toByteArray()));
			break;
		case QVariant::String:
			objectMetaInfo.append(info.toString());
			break;
		case QVariant::DateTime:
			objectMetaInfo.append(info.toDateTime().toString("dd.MM.yyyy hh:mm:ss"));
			break;
		default:
			objectMetaInfo.append(QString());
		}
	}

	return objectMetaInfo;
}


void CObjectCollectionViewComp::EnsureColumnsSettingsSynchronized() const
{
	if (m_blockColumnsSettingsSynchronize){
		return;
	}

	if (m_proxyModelPtr->rowCount() == 0){
		return;
	}

	ColumnsList columnsList;

	ColumnSettings columnSettings;
	columnSettings["FieldId"] = QString();
	columnSettings["Width"] = ItemList->columnWidth(0);
	columnsList.append(columnSettings);

	QVector<QByteArray> ids = GetMetaInfoIds(m_currentTypeId);
	QStringList headers = GetMetaInfoHeaders(m_currentTypeId);
	
	for (int i = 0; i < ids.count(); i++){
		int columndIndex = ItemList->header()->logicalIndex(i + 1);
		int fieldIndex = columndIndex - 1;

		ColumnSettings columnSettings;
		columnSettings["FieldId"] = QString(ids[fieldIndex]);
		columnSettings["Width"] = ItemList->columnWidth(columndIndex);
		columnsList.append(columnSettings);
	}

	m_typeIdColumnsSettings[m_currentTypeId] = columnsList;
}


void CObjectCollectionViewComp::RestoreColumnsSettings()
{
	QVector<QByteArray> tempFieldIds = GetMetaInfoIds(m_currentTypeId);
	QStringList fieldIds;
	for (QByteArray tempFieldId : tempFieldIds){
		fieldIds.append(tempFieldId);
	}
	QSet<QString> fieldSet = fieldIds.toSet();
		
	if (m_typeIdColumnsSettings.contains(m_currentTypeId)){
		ColumnsList columnsList = m_typeIdColumnsSettings[m_currentTypeId];

		int currentIndex = 0;

		for (int i = 0; i < columnsList.count(); i++){
			ColumnSettings columnSettings = columnsList[i];
			
			QVariant varFieldId = columnSettings["FieldId"];
			QVariant varWidth = columnSettings["Width"];

			QString fieldId;
			int width;

			if (varFieldId.type() != QVariant::String){
				continue;
			}

			fieldId = varFieldId.toString();

			bool ok;
			width = varWidth.toInt(&ok);

			if (fieldId.isEmpty()){
				if (ok){
						ItemList->setColumnWidth(0, width);
				}
				else{
					ItemList->resizeColumnToContents(0);
				}
				
				currentIndex++;
				continue;
			}

			if (fieldSet.contains(fieldId)) {
				int logicIndex = fieldIds.indexOf(fieldId) + 1;
				ItemList->header()->moveSection(ItemList->header()->visualIndex(logicIndex), currentIndex);
				fieldSet.remove(fieldId);
				
				if (ok){
					ItemList->setColumnWidth(logicIndex, width);
				}
				else{
					ItemList->resizeColumnToContents(currentIndex);
				}

				currentIndex++;
			}
		}

		for (int i = 0; i < fieldIds.count(); i++){
			if (!fieldSet.contains(fieldIds[i])){
				fieldIds[i].clear();
			}
		}

		for (QString fieldId : fieldIds){
			if (fieldId.isEmpty()){
				continue;
			}

			int fieldLogicIndex = fieldIds.indexOf(fieldId) + 1;
			ItemList->header()->moveSection(fieldLogicIndex, currentIndex++);
			ItemList->resizeColumnToContents(fieldLogicIndex);
		}
	}
	else{
		for (int i = 0; i < m_itemModel.columnCount(); i++){
			int currentVisualIndex = ItemList->header()->visualIndex(i);
			ItemList->header()->moveSection(currentVisualIndex, i);
			ItemList->resizeColumnToContents(i);
		}
	}
}


// private slots

void CObjectCollectionViewComp::OnSelectionChanged(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/)
{
	UpdateCommands();
}


void CObjectCollectionViewComp::OnItemChanged(QStandardItem* itemPtr)
{
	imtbase::IObjectCollection* collectionPtr = GetObservedObject();
	Q_ASSERT(collectionPtr != nullptr);

	QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
	if (!itemId.isEmpty()){
		QString currentName = collectionPtr->GetElementInfo(itemId, imtbase::ICollectionInfo::EIT_NAME).toString();
		QString newName = itemPtr->text();

		collectionPtr->SetObjectName(itemId, newName);
	}
}


void CObjectCollectionViewComp::OnItemDoubleClick(const QModelIndex &item)
{
	QModelIndex mappedIndex1 = m_proxyModelPtr->mapToSource(item);
	int sourceRow = m_customProxyModelPtr->mapToSource(mappedIndex1).row();
	QByteArray itemId = m_itemModel.item(sourceRow, 0)->data(DR_OBJECT_ID).toByteArray();

	const ICollectionViewDelegate &delegate = GetViewDelegateRef(m_currentTypeId);

	delegate.OpenDocumentEditor(itemId);
}


void CObjectCollectionViewComp::OnCustomContextMenuRequested(const QPoint &point)
{
	QAction *actionRename;
	QAction *actionEdit;
	QAction *actionRemove;

	QMenu menu(ItemList);
	actionRename = menu.addAction(QIcon(":/Icons/Rename"), tr("Rename"));
	actionEdit = menu.addAction(QIcon(":/Icons/Edit"), tr("Edit"));
	actionRemove = menu.addAction(QIcon(":/Icons/Remove"), tr("Remove"));

	connect(actionRename, &QAction::triggered, this, &CObjectCollectionViewComp::OnContextMenuRename);
	connect(actionEdit, &QAction::triggered, this, &CObjectCollectionViewComp::OnContextMenuEdit);
	connect(actionRemove, &QAction::triggered, this, &CObjectCollectionViewComp::OnContextMenuRemove);

	menu.exec(ItemList->viewport()->mapToGlobal(point));
}


void CObjectCollectionViewComp::on_TypeList_itemSelectionChanged()
{
	EnsureColumnsSettingsSynchronized();

	m_currentTypeId.clear();

	QList<QTreeWidgetItem*> selectedItems = TypeList->selectedItems();
	if (!selectedItems.isEmpty()){
		m_currentTypeId = selectedItems[0]->data(0, DR_TYPE_ID).toByteArray();
	}

	m_proxyModelPtr->setFilterFixedString(m_currentTypeId);

	UpdateCommands();
	
	QStringList headerLabels;
	headerLabels.append(tr("Name"));
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

	RestoreColumnsSettings();
}


void CObjectCollectionViewComp::OnContextMenuRename(bool checked)
{
	ICollectionViewDelegate & delegate = GetViewDelegateRef(m_currentTypeId);
	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();

	if (selectedIndexes.count() != 1){
		return;
	}	

	if (!selectedIndexes.isEmpty()){
		for (int i = 0; i < selectedIndexes.count(); ++i){
			QModelIndex mappedIndex1 = m_proxyModelPtr->mapToSource(selectedIndexes[i]);
			QModelIndex mappedIndex2 = m_customProxyModelPtr->mapToSource(mappedIndex1);
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(mappedIndex2);

			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				QByteArray typeId = itemPtr->data(DR_TYPE_ID).toByteArray();
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


void CObjectCollectionViewComp::OnContextMenuEdit(bool checked)
{
	ICollectionViewDelegate & delegate = GetViewDelegateRef(m_currentTypeId);
	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();

	if (!selectedIndexes.isEmpty()){
		for (int i = 0; i < selectedIndexes.count(); ++i){
			QModelIndex mappedIndex1 = m_proxyModelPtr->mapToSource(selectedIndexes[i]);
			QModelIndex mappedIndex2 = m_customProxyModelPtr->mapToSource(mappedIndex1);
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(mappedIndex2);

			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				QByteArray typeId = itemPtr->data(DR_TYPE_ID).toByteArray();
				if (!itemId.isEmpty()){
					bool result = delegate.OpenDocumentEditor(itemId);
				}
			}
		}
	}
}


void CObjectCollectionViewComp::OnContextMenuRemove(bool checked)
{
	ICollectionViewDelegate & delegate = GetViewDelegateRef(m_currentTypeId);
	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();

	QVector<QByteArray> itemIds;
	if (!selectedIndexes.isEmpty()){
		for (int i = 0; i < selectedIndexes.count(); ++i){
			QModelIndex mappedIndex1 = m_proxyModelPtr->mapToSource(selectedIndexes[i]);
			QModelIndex mappedIndex2 = m_customProxyModelPtr->mapToSource(mappedIndex1);
			QStandardItem* itemPtr = m_itemModel.itemFromIndex(mappedIndex2);

			if (itemPtr != nullptr){
				QByteArray itemId = itemPtr->data(DR_OBJECT_ID).toByteArray();
				QByteArray typeId = itemPtr->data(DR_TYPE_ID).toByteArray();
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
	m_customProxyModelPtr->setFilter(text);
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
	ItemList->setFocus();
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


} // namespace imtgui


