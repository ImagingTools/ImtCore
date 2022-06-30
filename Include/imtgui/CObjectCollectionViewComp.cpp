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
#include <istd/CChangeGroup.h>
#include <imod/IModel.h>
#include <iprm/CParamsSet.h>
#include <idoc/IDocumentMetaInfo.h>
#include <iqtgui/CCommandTools.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iwidgets/iwidgets.h>

// ImtCore includes
#include <imtbase/IObjectCollectionEvent.h>


namespace imtgui
{


// protected methods

CObjectCollectionViewComp::CObjectCollectionViewComp()
	:m_semaphoreCounter(0),
	m_currentInformationViewPtr(nullptr),
	m_eventBasedUpdateEnabled(false),
	m_tableModel(*this)
{
	m_pageSelection.SetParent(this);
	m_commands.SetParent(this);

	qRegisterMetaType<istd::IChangeable::ChangeSet>("istd::IChangeable::ChangeSet");
}


void CObjectCollectionViewComp::SetFilterString(const QString &text)
{
	OnFilterChanged(text);
}


// reimplemented (imtbase::ISelection)

const imtbase::ICollectionInfo* CObjectCollectionViewComp::GetSelectionConstraints() const
{
	return nullptr;
}


imtbase::ISelection::SelectionMode CObjectCollectionViewComp::GetSelectionMode() const
{
	return SelectionMode::SM_MULTI;
}


imtbase::ISelection::Ids CObjectCollectionViewComp::GetSelectedIds() const
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

ICollectionViewDelegate& CObjectCollectionViewComp::GetViewDelegateRef(const QByteArray& typeId)
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


void CObjectCollectionViewComp::OnPageSelectionUpdated()
{
	imtbase::IObjectCollection* collectionPtr = GetObservedObject();
	if (collectionPtr != nullptr){
		m_tableModel.UpdateFromData(*collectionPtr, istd::IChangeable::GetAnyChange());
	}
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
	QCoreApplication* applicationPtr = QCoreApplication::instance();
	bool isMainThread = (applicationPtr == NULL) || (QThread::currentThread() == applicationPtr->thread());
	if (!isMainThread){
		Q_EMIT EmitUpdateGui(changeSet);

		return;
	}

	DoUpdateGui(changeSet);
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

	connect(this, &CObjectCollectionViewComp::EmitUpdateGui, this, &CObjectCollectionViewComp::DoUpdateGui, Qt::QueuedConnection);

	if (m_filterParamsGuiCompPtr.IsValid()){
		m_filterParamsGuiCompPtr->CreateGui(CustomFilterParamsFrame);
	}

	ItemList->setProperty("ItemView", true);
	ItemList->setAlternatingRowColors(true);

	ItemList->setModel(&m_tableModel);

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

	if(*m_viewFilterAttrPtr){
		FilterPanel->setMaximumHeight(FilterPanel->sizeHint().height());
	}
	else{
		FilterPanel->setMaximumHeight(0);
	}
	m_filterPanelAnimationPtr = new QPropertyAnimation(FilterPanel, "maximumHeight", this);

	connect(FilterEdit, &QLineEdit::textChanged, this, &CObjectCollectionViewComp::OnFilterChanged);
	connect(CloseButton, &QToolButton::clicked, this, &CObjectCollectionViewComp::OnEscShortCut);
	if (*m_useSearchWidgetAttrPtr == true){
		connect(m_searchShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnSearchShortCut);
	}
	connect(m_escShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnEscShortCut);
	connect(m_delShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnDelShortCut);
	connect(m_renameShortCutPtr, &QShortcut::activated, this, &CObjectCollectionViewComp::OnRenameShortCut);

	ItemList->setContextMenuPolicy(Qt::CustomContextMenu);
	ItemList->header()->setFirstSectionMovable(true);
	ItemList->installEventFilter(this);

	connect(TypeList, &QTreeWidget::itemSelectionChanged, this, &CObjectCollectionViewComp::OnTypeChanged);
	connect(ItemList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CObjectCollectionViewComp::OnSelectionChanged);
	connect(ItemList, &QTreeView::doubleClicked, this, &CObjectCollectionViewComp::OnItemDoubleClick);
	connect(ItemList, &QTreeView::customContextMenuRequested, this, &CObjectCollectionViewComp::OnCustomContextMenuRequested);
	connect(ItemList->header(), &QHeaderView::sectionResized, this, &CObjectCollectionViewComp::OnSectionResized);
	connect(ItemList->header(), &QHeaderView::sectionMoved, this, &CObjectCollectionViewComp::OnSectionMoved);
	connect(ItemList->header(), &QHeaderView::sortIndicatorChanged, this, &CObjectCollectionViewComp::OnSortingChanged);

	PaginationFrame->setVisible(m_paginationGuiCompPtr.IsValid());
	if (m_paginationGuiCompPtr.IsValid()){
		if (m_paginationGuiCompPtr->CreateGui(PaginationFrame)){
			if (m_paginationGuiObserverCompPtr.IsValid()){
				if (!m_pageSelection.IsAttached(m_paginationGuiObserverCompPtr.GetPtr())){
				m_pageSelection.AttachObserver(m_paginationGuiObserverCompPtr.GetPtr());
			}
		}
	}
	}

	BaseClass::OnGuiCreated();

	if (m_currentInformationViewPtr == nullptr){
		RightPanel->setVisible(false);
	}

	iqtgui::SetStyleSheetFromFile(*GetWidget(), ":/Styles/ObjectCollectionView");
}


void CObjectCollectionViewComp::OnGuiDestroyed()
{
	if (m_paginationGuiObserverCompPtr.IsValid() && m_paginationGuiObserverCompPtr->IsModelAttached(&m_pageSelection)){
		m_pageSelection.DetachObserver(m_paginationGuiObserverCompPtr.GetPtr());
	}

	if (m_paginationGuiCompPtr.IsValid() && m_paginationGuiCompPtr->IsGuiCreated()){
		m_paginationGuiCompPtr->DestroyGui();
	}

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
			QModelIndex mappedIndex = selectedIndexes[i]; //m_proxyModelPtr->mapToSource(selectedIndexes[i]);
			QByteArray itemId = m_tableModel.data(mappedIndex, DR_OBJECT_ID).toByteArray();
			if (!itemId.isEmpty()){
				itemIds.push_back(itemId);
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

	// Restore visual column position by model
	for (int i = 0; i < m_tableModel.columnCount(); i++){
		ItemList->header()->moveSection(ItemList->header()->visualIndex(i), i);
	}

	QVector<QByteArray> tempFieldIds = GetMetaInfoIds(m_currentTypeId);
	QStringList fieldIds;
	for (QByteArray tempFieldId : tempFieldIds){
		fieldIds.append(tempFieldId);
	}

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
	QSet<QString> fieldSet(fieldIds.begin(), fieldIds.end());
#else
	QSet<QString> fieldSet = fieldIds.toSet();
#endif

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
		int columnCount = m_tableModel.columnCount();
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
			QModelIndex mappedIndex = selectedIndexes[i];
			QByteArray itemId = m_tableModel.data(mappedIndex, DR_OBJECT_ID).toByteArray();
			
			m_itemsSelection[m_currentTypeId].append(itemId);
		}
	}
}


void CObjectCollectionViewComp::RestoreItemsSelection()
{
	QItemSelection selection;

	for (int i = 0; i < m_tableModel.rowCount(); i++){
		QModelIndex mappedIndex = m_tableModel.index(i, 0);
		QByteArray itemId = m_tableModel.data(mappedIndex, DR_OBJECT_ID).toByteArray();
		if (m_itemsSelection[m_currentTypeId].contains(itemId)){
			selection.append(QItemSelectionRange(m_tableModel.index(i, 0)));
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

	istd::CChangeNotifier notifier(this);

	SaveItemsSelection();
	UpdateCommands();

	if (ItemList->selectionModel()->selectedRows().isEmpty()){
		ItemList->setCurrentIndex(QModelIndex());
	}
}


void CObjectCollectionViewComp::OnItemDoubleClick(const QModelIndex &item)
{
	int sourceRow = item.row(); //m_proxyModelPtr->mapToSource(item).row();
	QByteArray itemId = m_tableModel.data(m_tableModel.index(sourceRow, 0), DR_OBJECT_ID).toByteArray();

	const ICollectionViewDelegate &delegate = GetViewDelegateRef(m_currentTypeId);
	const imtbase::ICollectionInfo &collectionInfo = delegate.GetSummaryInformationTypes();

	imtbase::ICollectionInfo::Ids ids = collectionInfo.GetElementIds();
	int column = item.column();
	if (column < ids.count()){
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


void CObjectCollectionViewComp::OnSortingChanged(int logicalIndex, Qt::SortOrder order)
{
	m_tableModel.SetSorting(logicalIndex, order);

	RestoreColumnsSettings();
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
	
	QStringList headerLabels;
	headerLabels.append(GetMetaInfoHeaders(m_currentTypeId));

	for(int i = 0; i < headerLabels.count(); ++i){
		m_tableModel.setHeaderData(i, Qt::Horizontal, headerLabels[i], Qt::DisplayRole);
	}

	for (int i = 0; i < m_tableModel.columnCount(); i++){
		if (i < headerLabels.count()){
			ItemList->showColumn(i);
		}
		else {
			ItemList->hideColumn(i);
		}
	}

	ICollectionViewDelegate& viewDelegate = GetViewDelegateRef(m_currentTypeId);

	imtbase::ICollectionInfo::Ids informationIds = viewDelegate.GetSummaryInformationTypes().GetElementIds();
	for (int i = 0; i < informationIds.count(); ++i){
		ICollectionViewDelegate::HeaderInfo headerInfo = viewDelegate.GetSummaryInformationHeaderInfo(informationIds[i]);
		if (headerInfo.flags & ICollectionViewDelegate::HeaderInfo::IF_SORT_BY_DEFAULT){
			ItemList->header()->setSortIndicator(i, headerInfo.defaultSortOrder);
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

		iwidgets::ClearLayout(RightPanel->layout());
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

	const imtbase::IObjectCollection* collectionPtr = GetObservedObject();
	Q_ASSERT(collectionPtr != nullptr);

	m_tableModel.SetCurrentTypeId(m_currentTypeId);

	m_tableModel.UpdateFromData(*collectionPtr, istd::IChangeable::GetAnyChange());

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
			QModelIndex mappedIndex = selectedIndexes[i];
			QByteArray itemId = m_tableModel.data(mappedIndex, DR_OBJECT_ID).toByteArray();
			if (!itemId.isEmpty()){
				delegate.RenameObject(itemId, "");
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
			QModelIndex mappedIndex = selectedIndexes[i];
			QByteArray itemId = m_tableModel.data(mappedIndex, DR_OBJECT_ID).toByteArray();
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


void CObjectCollectionViewComp::OnContextMenuEditDocument(bool /*checked*/)
{
	ICollectionViewDelegate & delegate = GetViewDelegateRef(m_currentTypeId);
	QModelIndexList selectedIndexes = ItemList->selectionModel()->selectedRows();

	if (!selectedIndexes.isEmpty()){
		for (int i = 0; i < selectedIndexes.count(); ++i){
			QModelIndex mappedIndex = selectedIndexes[i];
			QByteArray itemId = m_tableModel.data(mappedIndex, DR_OBJECT_ID).toByteArray();
			if (!itemId.isEmpty()){
				delegate.OpenDocumentEditor(itemId);
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
			QModelIndex mappedIndex = selectedIndexes[i];
			QByteArray itemId = m_tableModel.data(mappedIndex, DR_OBJECT_ID).toByteArray();
			if (!itemId.isEmpty()){
				itemIds.append(itemId);
			}
		}
	}

	delegate.RemoveObjects(itemIds);
}


void CObjectCollectionViewComp::OnFilterChanged(const QString& text)
{
	m_tableModel.SetFilter(text);

	RestoreColumnsSettings();
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

	if (m_filterSelectionCompPtr.IsValid()){
		m_filterSelectionCompPtr->SetSelectedOptionIndex(0);
	}
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

	OnFilterChanged(QString());

	if (m_filterSelectionCompPtr.IsValid()){
		m_filterSelectionCompPtr->SetSelectedOptionIndex(-1);
	}
}


void CObjectCollectionViewComp::OnDelShortCut()
{
	OnContextMenuRemove(false);
}


void CObjectCollectionViewComp::OnRenameShortCut()
{
	OnContextMenuRename(false);
}


void CObjectCollectionViewComp::DoUpdateGui(const istd::IChangeable::ChangeSet& changeSet)
{
	const imtbase::IObjectCollection* collectionPtr = GetObservedObject();
	Q_ASSERT(collectionPtr != nullptr);

	{
		SignalSemaphore semaphore(m_semaphoreCounter);

		const iprm::IOptionsList* objectTypeInfoPtr = collectionPtr->GetObjectTypesInfo();
		if (objectTypeInfoPtr != nullptr){
			int typesCount = objectTypeInfoPtr->GetOptionsCount();

			QByteArray lastTypeId = m_currentTypeId;

			istd::IChangeable::ChangeInfoMap changeInfoMap = changeSet.GetChangeInfoMap();

			if (changeInfoMap.isEmpty()){
				TypeList->clear();

				QTreeWidgetItem* activeTypeItemPtr = nullptr;

				for (int typeIndex = 0; typeIndex < typesCount; ++typeIndex){
					if (objectTypeInfoPtr->IsOptionEnabled(typeIndex)){
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
				}

				if (activeTypeItemPtr != nullptr){
					activeTypeItemPtr->setSelected(true);
					const ICollectionViewDelegate& delegate = GetViewDelegateRef(m_currentTypeId);
					const imtbase::ICollectionInfo& collectionInfo = delegate.GetSummaryInformationTypes();

					imtbase::ICollectionInfo::Ids ids = collectionInfo.GetElementIds();
					for (int i = 0; i < ids.count(); ++i){
						QByteArray id = ids[i];
						ItemList->itemDelegateForColumn(i)->deleteLater();
						QAbstractItemDelegate* itemDelegate = delegate.GetColumnItemDelegate(id);
						ItemList->setItemDelegateForColumn(i, itemDelegate);
					}
				}
			}

			if (typesCount > 1){
				TypeList->show();
			}
			else{
				TypeList->hide();
			}

		}
	}

	m_tableModel.SetSorting(ItemList->header()->sortIndicatorSection(), ItemList->header()->sortIndicatorOrder());

	m_tableModel.UpdateFromData(*collectionPtr, changeSet);

	RestoreColumnsSettings();
}


// public methods of the embedded class PageSelection

CObjectCollectionViewComp::PageSelection::PageSelection()
	:m_parentPtr(nullptr),
	m_pageCount(0),
	m_selectedPageIndex(-1)
{
}


void CObjectCollectionViewComp::PageSelection::SetParent(CObjectCollectionViewComp* parentPtr)
{
	m_parentPtr = parentPtr;
}


void CObjectCollectionViewComp::PageSelection::SetPageCount(int pageCount)
{
	if (pageCount >= 0){
		if (m_pageCount != pageCount){
			istd::IChangeable::ChangeSet optionsChangeSet(iprm::IOptionsList::CF_OPTIONS_CHANGED);
			istd::CChangeNotifier notifier(this, &optionsChangeSet);

			m_pageCount = pageCount;

			if (m_selectedPageIndex >= m_pageCount){
				SetSelectedOptionIndex(m_pageCount - 1);
			}
		}
	}
}


// reimplemented (iprm::ISelectionParam)

const iprm::IOptionsList* CObjectCollectionViewComp::PageSelection::GetSelectionConstraints() const
{
	return this;
}


int CObjectCollectionViewComp::PageSelection::GetSelectedOptionIndex() const
{
	return m_selectedPageIndex;
}


bool CObjectCollectionViewComp::PageSelection::SetSelectedOptionIndex(int index)
{
	if (index >= -1 && index < m_pageCount){
		if (index != m_selectedPageIndex){
			istd::IChangeable::ChangeSet selectionChangeSet(iprm::ISelectionParam::CF_SELECTION_CHANGED);
			istd::CChangeNotifier notifier(this, &selectionChangeSet);

			m_selectedPageIndex = index;

			if (m_parentPtr != nullptr){
				m_parentPtr->OnPageSelectionUpdated();
			}
		}

		return true;
	}

	return false;
}


iprm::ISelectionParam* CObjectCollectionViewComp::PageSelection::GetSubselection(int /*index*/) const
{
	return nullptr;
}


// reimplemented (iprm::IOptionsList)

int CObjectCollectionViewComp::PageSelection::GetOptionsFlags() const
{
	return SCF_SUPPORT_UNIQUE_ID;
}


int CObjectCollectionViewComp::PageSelection::GetOptionsCount() const
{
	return m_pageCount;
}


QString CObjectCollectionViewComp::PageSelection::GetOptionName(int index) const
{
	QString retVal;

	if (index >= 0 && index < m_pageCount){
		retVal = QString::number(index);
	}

	return retVal;
}


QString CObjectCollectionViewComp::PageSelection::GetOptionDescription(int /*index*/) const
{
	return QString();
}


QByteArray CObjectCollectionViewComp::PageSelection::GetOptionId(int index) const
{
	QByteArray retVal;

	if (index >= 0 && index < m_pageCount){
		retVal = QString::number(index).toUtf8();
	}

	return retVal;
}


bool CObjectCollectionViewComp::PageSelection::IsOptionEnabled(int /*index*/) const
{
	return true;
}


// reimplemented (iser::ISerializable)

bool CObjectCollectionViewComp::PageSelection::Serialize(iser::IArchive& /*archive*/)
{
	return false;
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


CObjectCollectionViewComp::TableModel::TableModel(CObjectCollectionViewComp& parent)
	:m_fetchedRowCount(0),
	m_totalRowCount(0),
	m_parent(parent),
	m_batchSize(50)
{
}


void CObjectCollectionViewComp::TableModel::UpdateFromData(const imtbase::IObjectCollection& collection, const istd::IChangeable::ChangeSet& /*changes*/)
{
	m_isPageMode = m_parent.m_paginationGuiCompPtr.IsValid();
	m_batchSize = m_isPageMode ? 25 : 50;

	beginResetModel();

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("Filter", &m_filter);

	m_totalRowCount = collection.GetElementsCount(&filterParams);
	m_fetchedRowCount = 0;
	m_ids.clear();

	m_metaInfo.clear();
	m_metaInfoMap.clear();

	if (m_totalRowCount > 0){
		imtbase::IObjectCollection::Ids elementIds = collection.GetElementIds(0, 1, &filterParams);
		if (!elementIds.isEmpty()){
			QByteArray itemTypeId = collection.GetObjectTypeId(elementIds.front());

			m_metaInfo = m_parent.GetMetaInfo(elementIds.front(), itemTypeId);
		}
	}

	if (m_isPageMode){
		int lastSelectedPageIndex = m_parent.m_pageSelection.GetSelectedOptionIndex();

		istd::CChangeGroup changeGroup(&m_parent.m_pageSelection);

		int pageCount = m_totalRowCount / m_batchSize + (((m_totalRowCount % m_batchSize) != 0) ? 1 : 0);

		m_parent.m_pageSelection.SetPageCount(pageCount);

		if (lastSelectedPageIndex < 0){
			m_parent.m_pageSelection.SetSelectedOptionIndex(pageCount > 0 ? 0 : -1);
		}
		else{
			m_parent.m_pageSelection.SetSelectedOptionIndex(pageCount > lastSelectedPageIndex ? lastSelectedPageIndex : -1);
		}

		if (m_totalRowCount > 0){
			int pageIndex = m_parent.m_pageSelection.GetSelectedOptionIndex();

			beginInsertRows(QModelIndex(), 0, m_batchSize - 1);

			imtbase::ICollectionInfo::Ids fetchedIds = collection.GetElementIds(pageIndex * m_batchSize, m_batchSize, &filterParams);
			if (!fetchedIds.isEmpty()){
				m_fetchedRowCount = fetchedIds.count();

				m_ids = fetchedIds;
			}

			Q_ASSERT(m_ids.count() == m_fetchedRowCount);

			endInsertRows();
		}
	}

	endResetModel();
}


void CObjectCollectionViewComp::TableModel::AddItem(const imtbase::IObjectCollectionInfo::Id& objectId)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount());

	m_totalRowCount++;
	m_fetchedRowCount++;

	m_ids.push_back(objectId);

	endInsertRows();
}


void CObjectCollectionViewComp::TableModel::UpdateItem(const imtbase::IObjectCollectionInfo::Id& objectId)
{
	imtbase::IObjectCollection* objectCollectionPtr = m_parent.GetObservedObject();
	Q_ASSERT(objectCollectionPtr != nullptr);

	int row = -1;
	for (int i = 0; i < rowCount(); i++){
		if (data(index(i, 0), DR_OBJECT_ID) == objectId){
			row = i;
			break;
		}
	}

	QByteArray itemTypeId = objectCollectionPtr->GetObjectTypeId(objectId);

	ObjectMetaInfo metaInfo = m_parent.GetMetaInfo(objectId, itemTypeId);
	if (metaInfo.isEmpty()){
		return;
	}
	else{
		m_metaInfo = metaInfo;
		m_metaInfoMap[objectId] = m_metaInfo;
	}
}


void CObjectCollectionViewComp::TableModel::RemoveItem(const imtbase::IObjectCollectionInfo::Id& objectId)
{
	for (int i = 0; i < rowCount(); i++){
		if (data(index(i, 0), DR_OBJECT_ID) == objectId){
			beginRemoveRows(QModelIndex(), i, i);

			m_totalRowCount -=1;
			m_fetchedRowCount -=1;
			m_ids.removeOne(objectId);
			m_metaInfoMap.remove(objectId);

			endRemoveRows();

			break;
		}
	}
}


void CObjectCollectionViewComp::TableModel::SetSorting(int logicalIndex, Qt::SortOrder order)
{
	imtbase::IObjectCollection* objectCollectionPtr = m_parent.GetObservedObject();
	Q_ASSERT(objectCollectionPtr != nullptr);

	m_filter.SetSortingOrder(order == Qt::AscendingOrder ? imtbase::ICollectionFilter::SO_ASC : imtbase::ICollectionFilter::SO_DESC);

	imtbase::ICollectionInfo::Ids informationIds = m_parent.GetMetaInfoIds(m_parent.m_currentTypeId);

	Q_ASSERT(logicalIndex < informationIds.count());
	Q_ASSERT(!informationIds[logicalIndex].isEmpty());

	m_filter.SetSortingInfoIds(QByteArrayList() << informationIds[logicalIndex]);

	UpdateFromData(*objectCollectionPtr, istd::IChangeable::GetAnyChange());
}


void CObjectCollectionViewComp::TableModel::SetCurrentTypeId(const QByteArray & typeId)
{
	m_filter.SetObjectTypeId(typeId);
}


void CObjectCollectionViewComp::TableModel::SetFilter(const QString& textFilter)
{
	imtbase::IObjectCollection* objectCollectionPtr = m_parent.GetObservedObject();
	Q_ASSERT(objectCollectionPtr != nullptr);

	imtbase::ICollectionInfo::Ids informationIds = m_parent.GetMetaInfoIds(m_parent.m_currentTypeId);
	QByteArrayList filterableInfoIds;
	for (const QByteArray& infoId : informationIds){
		ICollectionViewDelegate::HeaderInfo headerInfo = m_parent.GetViewDelegate(m_parent.m_currentTypeId).GetSummaryInformationHeaderInfo(infoId);
		if (headerInfo.flags & ICollectionViewDelegate::HeaderInfo::IF_FILTERABLE){
			filterableInfoIds.push_back(infoId);
		}
	}

	m_filter.SetFilteringInfoIds(filterableInfoIds);

	m_filter.SetTextFilter(textFilter);

	UpdateFromData(*objectCollectionPtr, istd::IChangeable::GetAnyChange());
}


// reimplemented (QAbstractTableModel)

int CObjectCollectionViewComp::TableModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid()){
		return 0;
	}

	return m_fetchedRowCount;
}


int CObjectCollectionViewComp::TableModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid()){
		return 0;
	}

	return m_metaInfo.count();
}


QVariant CObjectCollectionViewComp::TableModel::data(const QModelIndex& index, int role) const
{
	int rowIndex = index.row();
	if (rowIndex < 0 || rowIndex >= m_ids.count()){
		return QVariant();
	}

	imtbase::IObjectCollection* collectionPtr = m_parent.GetObservedObject();
	if (collectionPtr == nullptr){
		return QVariant();
	}

	QByteArray objectId = m_ids[index.row()];

	QByteArray itemTypeId = collectionPtr->GetObjectTypeId(objectId);

	ObjectMetaInfo metaInfo;
	if (m_metaInfoMap.contains(objectId)){
		metaInfo = m_metaInfoMap[objectId];
	}
	else{
		metaInfo = m_parent.GetMetaInfo(objectId, itemTypeId);
		m_metaInfoMap[objectId] = metaInfo;
	}

	//	qDebug() << QString("CObjectCollectionViewComp::TableModel::data --> Index(%1, %2) Role(%3)").arg(index.row()).arg(index.column()).arg(role);

	switch (role){
	case Qt::DisplayRole:
		return metaInfo[index.column()].text;
	case Qt::DecorationRole:
		return metaInfo[index.column()].icon;
	case DR_TYPE_ID:
		return itemTypeId;
	case DR_OBJECT_ID:
		return objectId;
	case DR_SORT_VALUE:
		return metaInfo[index.column()].sortValue;
	}

	return QVariant();
}


QVariant CObjectCollectionViewComp::TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (section < 0){
		return QVariant();
	}

	if (orientation == Qt::Horizontal){
		switch (role){
		case Qt::DisplayRole:
			return m_parent.GetMetaInfoHeaders(m_parent.m_currentTypeId)[section];
		}
	}

	return QVariant();
}


Qt::ItemFlags CObjectCollectionViewComp::TableModel::flags(const QModelIndex& index) const
{
	imtbase::IObjectCollection* collectionPtr = m_parent.GetObservedObject();
	if (collectionPtr == nullptr){
		return Qt::NoItemFlags;
	}

	int rowIndex = index.row();
	if (rowIndex  < 0){
		return Qt::NoItemFlags;
	}

	Q_ASSERT(rowIndex < m_ids.count());

	QByteArray objectId = m_ids[rowIndex];

	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	if (collectionPtr->GetOperationFlags(objectId) & imtbase::IObjectCollection::OF_SUPPORT_EDIT){
		flags |= Qt::ItemIsEditable;
	}

	return flags;
}


// protected methods

// reimplemented (QAbstractTableModel)

bool CObjectCollectionViewComp::TableModel::canFetchMore(const QModelIndex& parent) const
{
	if (parent.isValid()){
		return false;
	}

	if (m_isPageMode){
		return false;
	}

	return (m_fetchedRowCount < m_totalRowCount);
}


void CObjectCollectionViewComp::TableModel::fetchMore(const QModelIndex& parent)
{
	if (parent.isValid()){
		return;
	}

	if (m_isPageMode){
		return;
	}

	imtbase::IObjectCollection* collectionPtr = m_parent.GetObservedObject();
	if (collectionPtr == nullptr){
		return;
	}

	const int start = m_fetchedRowCount;
	const int remainder = m_totalRowCount - start;
	const int itemsToFetch = qMin(m_batchSize, remainder);

	if (itemsToFetch <= 0){
		return;
	}

	beginInsertRows(QModelIndex(), start, start + itemsToFetch - 1);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("Filter", &m_filter);

	imtbase::ICollectionInfo::Ids fetchedIds = collectionPtr->GetElementIds(start, itemsToFetch, &filterParams);
	if (!fetchedIds.isEmpty()){
		m_fetchedRowCount += fetchedIds.count();

		m_ids += fetchedIds;
	}

	Q_ASSERT(m_ids.count() == m_fetchedRowCount);

	endInsertRows();
}


} // namespace imtgui


