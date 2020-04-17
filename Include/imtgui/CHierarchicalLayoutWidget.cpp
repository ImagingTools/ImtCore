#include <imtgui/CHierarchicalLayoutWidget.h>

// Acf includes
#include <iser/IArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iser/CArchiveTag.h>
#include <istd/CChangeNotifier.h>
#include <iser/ISerializable.h>
#include <iser/CXmlStringWriteArchive.h>
#include <iser/CXmlStringReadArchive.h>

// Qt includes
#include <QtCore/QMimeData>
#include <QtCore/QUuid>

#include <QtGui/QPixmap>

#include <QtWidgets/QAction>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>

// remove later
#include <QDebug>


namespace imtgui
{


// public methods

CHierarchicalLayoutWidget::CHierarchicalLayoutWidget(QWidget* parentPtr)
	:QWidget(parentPtr),
	m_viewMode(VM_UNDEFINED)
{
	setAutoFillBackground(true);
	setLayout(new QVBoxLayout(this));
	layout()->setMargin(0);

	m_rootId = QUuid::createUuid().toByteArray();


	SetViewMode(VM_NORMAL);
}


void CHierarchicalLayoutWidget::SetViewMode(ViewMode viewMode)
{
	m_viewMode = viewMode;

	IdsList keys = m_customWidgetMap.keys();
	for (QByteArray& key : keys){
		CCustomLayoutWidget* customWidgetPtr = m_customWidgetMap[key];
		InternalItemData* internalItemDataPtr = GetInternalItem(key);
		customWidgetPtr->SetEditMode(viewMode == VM_EDIT);
		if (internalItemDataPtr != NULL){
			customWidgetPtr->setAcceptDrops((viewMode == VM_EDIT) && (internalItemDataPtr->layoutType != LT_HORIZONTAL_SPLITTER) && (internalItemDataPtr->layoutType != LT_VERTICAL_SPLITTER));
			if ((customWidgetPtr->layout() != NULL) && (customWidgetPtr->layout()->count() > 0)){
				QLayoutItem* layoutItemPtr = customWidgetPtr->layout()->itemAt(0);
				if (layoutItemPtr != NULL){
					QSplitter* splitterPtr = dynamic_cast<QSplitter*>(layoutItemPtr->widget());
					if (splitterPtr != NULL){
						splitterPtr->setHandleWidth(viewMode == VM_NORMAL ? 0 : 3);
						for (int i = 0; i < splitterPtr->count(); ++i){
							QSplitterHandle* splitterHandlePtr = splitterPtr->handle(i);
							if (splitterHandlePtr != NULL){
								setEnabled(viewMode == VM_EDIT);
							}
						}
					}
				}
			}
		}
	}
	update();
}


bool CHierarchicalLayoutWidget::SetRootItemId(const QByteArray& id)
{
	CustomWidgetMap::iterator iter = m_customWidgetMap.find(m_rootId);
	if (iter != m_customWidgetMap.end()){
		CCustomLayoutWidget* widgetPtr = iter.value();
		m_customWidgetMap.remove(m_rootId);
		m_customWidgetMap.insert(id, widgetPtr);

		//for (int i = 0; i < m_internalItemList.count(); ++i){
		//	if (m_internalItemList[i].id == m_rootId){
		//		m_internalItemList[i].id == id;
		//	}
		//}
	}

	m_rootId = id;

	return true;
}


bool CHierarchicalLayoutWidget::SetLayoutToItem(const QByteArray& id, LayoutType type, int count, IdsList *idsListPtr)
{
	InternalItemData* internalItemDataPtr = GetInternalItem(id);
	if (internalItemDataPtr != NULL){
		if (	(internalItemDataPtr->layoutType == LT_NONE) &&
				((type == LT_HORIZONTAL_SPLITTER) ||
				(type == LT_VERTICAL_SPLITTER))){
			SetSplitterLayout(id, type == LT_HORIZONTAL_SPLITTER ? Qt::Horizontal : Qt::Vertical, count, idsListPtr);

			return true;
		}
	}

	return false;
}


bool CHierarchicalLayoutWidget::SetItemSizes(const QByteArray& id, const SizeList &sizeList)
{
	//InternalItemData* internalItemDataPtr = GetInternalItem(id);
	//if (internalItemDataPtr != NULL){
	//	internalItemDataPtr->sizeList = sizeList;
	//	return true;
	//}

	return false;
}


bool CHierarchicalLayoutWidget::SetWidgetToItem(const QByteArray& id, const QByteArray& viewId, QWidget* widgetPtr)
{
	//InternalItemData* internalItemDataPtr = GetInternalItem(id);
	//if (internalItemDataPtr != NULL){
		//if (((internalItemDataPtr->layoutType == LT_NONE) && (widgetPtr != NULL)) ||
		//	(widgetPtr == NULL)){
			if ((m_customWidgetMap.contains(id)) && m_customWidgetMap[id]->SetWidget(widgetPtr)){
				m_customWidgetMap[id]->SetViewId(viewId);
				//internalItemDataPtr->layoutType = (widgetPtr != NULL) ? LT_OBJECT : LT_NONE;
				return true;
			}
		//}
	//}

	return false;
}


QByteArray CHierarchicalLayoutWidget::GetRootItemId()
{
	return m_rootId;
}


CHierarchicalLayoutWidget::IdsList CHierarchicalLayoutWidget::GetItemChildIdList(const QByteArray& id)
{
	IdsList retVal;
	//InternalItemData* internalItemDataPtr = GetInternalItem(id);
	//if (internalItemDataPtr != NULL){
	//	retVal.append(internalItemDataPtr->childItems);
	//}

	return retVal;
}


CHierarchicalLayoutWidget::LayoutType CHierarchicalLayoutWidget::GetItemLayoutType(const QByteArray& id)
{
	CHierarchicalLayoutWidget::LayoutType retVal;
	//InternalItemData* internalItemDataPtr = GetInternalItem(id);
	//if (internalItemDataPtr != NULL){
	//	retVal = internalItemDataPtr->layoutType;
	//}

	return retVal;
}


CHierarchicalLayoutWidget::SizeList CHierarchicalLayoutWidget::GetItemSizes(const QByteArray& id)
{
	CHierarchicalLayoutWidget::SizeList retVal;
	//InternalItemData* internalItemDataPtr = GetInternalItem(id);
	//if (internalItemDataPtr != NULL){
	//	retVal = internalItemDataPtr->sizeList;
	//}

	return retVal;
}


void CHierarchicalLayoutWidget::ClearAll()
{
	CleanLayoutRecursive(layout());
	m_customWidgetMap.clear();
	//m_internalItemList.clear();

	m_rootId = QUuid::createUuid().toByteArray();
	CCustomLayoutWidget* customLayoutWidgetPtr = new CCustomLayoutWidget(m_rootId, *this, NULL, this);
	if (customLayoutWidgetPtr != NULL){
		layout()->addWidget(customLayoutWidgetPtr);
		m_customWidgetMap.insert(m_rootId, customLayoutWidgetPtr);
		//m_internalItemList.push_back(InternalItemData(m_rootId));
	}

}


void CHierarchicalLayoutWidget::ResetWidget()
{

}


// protected methods

void CHierarchicalLayoutWidget::OnDropEvent(const QByteArray& inputId, QDropEvent* eventPtr)
{
	return;

	QByteArray id = inputId;
	Q_ASSERT(m_customWidgetMap.contains(id));
	Q_ASSERT(GetInternalItem(id) != NULL);

	InternalItemData* internalItemDataPtr = GetInternalItem(id);
	if (m_customWidgetMap.contains(id) && (internalItemDataPtr != NULL)){
		if (eventPtr->mimeData()->hasFormat("layout-item")){
			eventPtr->accept();
			eventPtr->setDropAction(Qt::MoveAction);

			QByteArray data = eventPtr->mimeData()->data("layout-item");
			qDebug() << "Drop " << data;
			if ((internalItemDataPtr->layoutType == LT_NONE) && (data == "Horizontal2")){
				SetSplitterLayout(id, Qt::Horizontal, 2);
				Q_EMIT EmitLayoutChanged(id, LT_HORIZONTAL_SPLITTER, 2);
			}
			else if ((internalItemDataPtr->layoutType == LT_NONE) && (data == "Horizontal3")){
				SetSplitterLayout(id, Qt::Horizontal, 3);
				Q_EMIT EmitLayoutChanged(id, LT_HORIZONTAL_SPLITTER, 3);
			}
			else if ((internalItemDataPtr->layoutType == LT_NONE) && (data == "Vertical2")){
				SetSplitterLayout(id, Qt::Vertical, 2);
				Q_EMIT EmitLayoutChanged(id, LT_VERTICAL_SPLITTER, 2);
			}
			else if ((internalItemDataPtr->layoutType == LT_NONE) && (data == "Vertical3")){
				SetSplitterLayout(id, Qt::Vertical, 3);
				Q_EMIT EmitLayoutChanged(id, LT_VERTICAL_SPLITTER, 3);
			}
			else if ((internalItemDataPtr->layoutType == LT_OBJECT) && (data == "CleanWidget")){
	//			SetWidgetToItem(id, NULL);
				Q_EMIT EmitClearEvent(id);
			}
			else if (data == "MergeLayout"){
				MergeLayout(id);
				Q_EMIT EmitLayoutChanged(id, LT_MERGE, 1);
			}
		}
		else if (eventPtr->mimeData()->hasFormat("widget-item")){
			Q_EMIT EmitDropEvent(id, eventPtr);
		}
		else{
			eventPtr->ignore();
		}
	}
}


void CHierarchicalLayoutWidget::OnMouseReleaseEvent(const QByteArray& id, QMouseEvent* eventPtr)
{
	//Q_EMIT EmitOpenMenuEvent(id, eventPtr);
}


void CHierarchicalLayoutWidget::OnSplitterMoved(const QByteArray& id, SizeList sizeList)
{
	//InternalItemData* internalItemDataPtr = GetInternalItem(id);
	//if (m_customWidgetMap.contains(id) && (internalItemDataPtr != NULL)){
	//	internalItemDataPtr->sizeList.clear();
	//	internalItemDataPtr->sizeList << sizeList;
	//}
}


void CHierarchicalLayoutWidget::SetSplitterLayout(const QByteArray& id, Qt::Orientation orientation, int count, IdsList *idsListPtr)
{
	IdsList localIdsList;
	//InternalItemData* internalItemDataPtr = GetInternalItem(id);
	QList<int> sizes;

	if (m_customWidgetMap.contains(id) ){
		QRect rectParent = m_customWidgetMap[id]->geometry();
		QSplitter* splitterParentPtr = qobject_cast<QSplitter*> (m_customWidgetMap[id]->parent());
		if (splitterParentPtr == NULL){
			splitterParentPtr = new QSplitter(this);
			splitterParentPtr->setStyleSheet("QSplitter{background-color: transparent;}");
			splitterParentPtr->setOrientation(orientation);
			this->layout()->removeWidget(m_customWidgetMap[id]);		
			this->layout()->addWidget(splitterParentPtr);
			splitterParentPtr->addWidget(m_customWidgetMap[id]);
		}
		if (splitterParentPtr->orientation() != orientation){
			sizes = splitterParentPtr->sizes();
			QSplitter* splitterPtr = new QSplitter(this);
			splitterPtr->setStyleSheet("QSplitter{background-color: transparent;}");
			splitterPtr->setOrientation(orientation);
			int index = splitterParentPtr->indexOf(m_customWidgetMap[id]);
			m_customWidgetMap[id]->setParent(NULL);
			splitterParentPtr->insertWidget(index, splitterPtr);
			splitterParentPtr->setSizes(sizes);
			splitterParentPtr = splitterPtr;
			splitterParentPtr->insertWidget(index, m_customWidgetMap[id]);
		}

		int index = splitterParentPtr->indexOf(m_customWidgetMap[id]);
		if (index < 0) {
			index = 0;
		}
		QByteArray newId;
		if ((idsListPtr != NULL) && (0 < idsListPtr->count()) && !idsListPtr->at(0).isEmpty()){
			newId = idsListPtr->at(0);
		}
		else{
			newId = QUuid::createUuid().toByteArray();
		}
		localIdsList.push_back(newId);
		CCustomLayoutWidget* newItemPtr = new CCustomLayoutWidget(newId, *this, m_customWidgetMap[id], splitterParentPtr);
		splitterParentPtr->insertWidget(index, newItemPtr);
		int size = 0;
		
		if (orientation == Qt::Horizontal){
			size = rectParent.width() / splitterParentPtr->count();
		}
		else{
			size = rectParent.height() / splitterParentPtr->count();
		}
		for (int i = 0; i < splitterParentPtr->count(); i++){
			sizes << size;
		}
		splitterParentPtr->setSizes(sizes);
		qDebug() << newItemPtr->geometry() << splitterParentPtr->geometry() << splitterParentPtr->sizes();
		m_customWidgetMap.insert(newId, newItemPtr);
		//internalItemDataPtr->childItems << newId;
		//m_internalItemList << InternalItemData(newId);
	}

}


void CHierarchicalLayoutWidget::MergeLayout(const QByteArray& id)
{
	//InternalItemData* internalItemDataPtr = GetInternalItem(id);
	//if ((internalItemDataPtr != NULL) && (internalItemDataPtr->layoutType != LT_OBJECT) && m_customWidgetMap.contains(id)){
	//	if (internalItemDataPtr->layoutType == LT_NONE){
	//		// transfer to parent
	//		QByteArray parentId = GetParentId(id);
	//		if (!parentId.isEmpty()){
	//			MergeLayout(parentId);
	//			InternalItemData* parentInternalItemDataPtr = GetInternalItem(parentId);
	//			if (parentInternalItemDataPtr != NULL){
	//				parentInternalItemDataPtr->layoutType = LT_NONE;
	//			}
	//			return;
	//		}
	//	}

	//	QLayout* layoutPtr = m_customWidgetMap[id]->layout();
	//	if (layoutPtr != NULL){
	//		IdsList childIdList;
	//		IdsListCollectChildIdsRecursive(id, childIdList);
	//		for (QByteArray& childId : childIdList){
	//			m_customWidgetMap[childId]->deleteLater();
	//			m_customWidgetMap.remove(childId);
	//			//m_internalItemList.removeAll(InternalItemData(childId));
	//		}
	//		CleanLayoutRecursive(layoutPtr);
	//		m_customWidgetMap[id]->setAcceptDrops(true);
	//		delete layoutPtr;
	//	}
	//}
}


void CHierarchicalLayoutWidget::RemoveLayout(const QByteArray& id)
{

	//InternalItemData* internalItemDataPtr = GetInternalItem(id);
	if (m_customWidgetMap.contains(id)){
		QSplitter* splitterPtr = qobject_cast<QSplitter*> (m_customWidgetMap[id]->parent());
		if (splitterPtr != NULL){
			m_customWidgetMap[id]->setParent(NULL);
			if (splitterPtr->count() == 1){
				QSplitter* splitterParentPtr = qobject_cast<QSplitter*> (splitterPtr->parent());
				if (splitterParentPtr != NULL){
					int index = splitterParentPtr->indexOf(splitterPtr);
					QWidget *widget = splitterPtr->widget(0);
					QSize size = widget->size();
					splitterParentPtr->insertWidget(index,widget);
					
				}
				else{
					QWidget *widget = splitterPtr->widget(0);
					widget->setParent(NULL);
				//	CleanLayoutRecursive(this->layout());
					this->layout()->addWidget(widget);
				}
				delete splitterPtr;
			}
			CleanLayoutRecursive(m_customWidgetMap[id]->layout());
			m_customWidgetMap[id]->deleteLater();
			m_customWidgetMap.remove(id);
			//m_internalItemList.removeAll(InternalItemData(id));
		}

	}

}


void CHierarchicalLayoutWidget::SetName(const QByteArray& id, QString &name)
{
	if (m_customWidgetMap.contains(id)){
		m_customWidgetMap[id]->SetName(name);
	}
}


QString CHierarchicalLayoutWidget::GetName(const QByteArray& id)
{
	QString name;
	if (m_customWidgetMap.contains(id)){
		name = m_customWidgetMap[id]->GetName();
	}
	return name;
}


void CHierarchicalLayoutWidget::SetAdditionalNames(QStringList& additionalNames)
{
	m_additionalNames = additionalNames;
	if (layout()->count() == 0){
		CCustomLayoutWidget* customLayoutWidgetPtr = new CCustomLayoutWidget(m_rootId, *this, NULL, this);
		if (customLayoutWidgetPtr != NULL) {
			layout()->addWidget(customLayoutWidgetPtr);
			m_customWidgetMap.insert(m_rootId, customLayoutWidgetPtr);
			//m_internalItemList.push_back(InternalItemData(m_rootId));
		}
	}
}


bool CHierarchicalLayoutWidget::Serialize(iser::IArchive& archive)
{
	QLayout *layoutPtr = layout();
	QLayoutItem* itemPtr = layoutPtr->itemAt(0);
	QWidget *widget = itemPtr->widget();
	if (!archive.IsStoring()) {
		CleanLayoutRecursive(layout());
		m_customWidgetMap.clear();
		//m_internalItemList.clear();
		widget = NULL;
	}

	bool retVal = SerializeRecursive(archive, &widget);
	if (!archive.IsStoring()) {
		layoutPtr->addWidget(widget);
	}

	return retVal;
}


bool CHierarchicalLayoutWidget::SerializeRecursive(iser::IArchive& archive, QWidget** widget)
{
	bool retVal = true;
	LayoutType layoutType = LT_NONE;

	QSplitter* splitter = dynamic_cast<QSplitter*> (*widget);


	static iser::CArchiveTag layoutItemTag("LayoutItem", "Layout item");
	retVal = retVal && archive.BeginTag(layoutItemTag);

	static iser::CArchiveTag layoutItemTypeTag("LayoutItemType", "Layout item type");
	retVal = retVal && archive.BeginTag(layoutItemTypeTag);

	if (splitter != NULL) {
		layoutType = splitter->orientation() == Qt::Horizontal ? LT_HORIZONTAL_SPLITTER : LT_VERTICAL_SPLITTER;
	}
	
	retVal = retVal && I_SERIALIZE_ENUM(LayoutType, archive, layoutType);
	retVal = retVal && archive.EndTag(layoutItemTypeTag);
	QByteArray sizeListAsByteArray;
	if ((layoutType == LayoutType::LT_HORIZONTAL_SPLITTER) || (layoutType == LayoutType::LT_VERTICAL_SPLITTER)) {
		static iser::CArchiveTag layoutSizeListTag("LayoutSizeList", "Layout item size list");
		retVal = retVal && archive.BeginTag(layoutSizeListTag);
		if (archive.IsStoring()) {
			QDataStream stream(&sizeListAsByteArray, QIODevice::WriteOnly);
			SizeList sizeList = splitter->sizes();
			stream << sizeList;
		}
		else {
			splitter = new QSplitter();
			splitter->setOrientation(layoutType == LayoutType::LT_HORIZONTAL_SPLITTER ? Qt::Horizontal : Qt::Vertical);
			*widget = splitter;
		}

		retVal = retVal && archive.Process(sizeListAsByteArray);
		retVal = retVal && archive.EndTag(layoutSizeListTag);


		int childCount = splitter->count();

		static iser::CArchiveTag childItemGroupTag("ChildItems", "Child items");
		retVal = retVal && archive.BeginMultiTag(childItemGroupTag, layoutItemTag, childCount);

		for (int i = 0; i < childCount; i++) {
			QWidget *widget = NULL;
			if (archive.IsStoring()) {
				widget = splitter->widget(i);
			}
			retVal = retVal && SerializeRecursive(archive, &widget);
			if (!archive.IsStoring()) {
				splitter->addWidget(widget);
			}
		}

		if (!archive.IsStoring()) {
			QDataStream stream(&sizeListAsByteArray, QIODevice::ReadWrite);
			SizeList sizeList;
			stream >> sizeList;
			splitter->setSizes(sizeList);
		}

		retVal = retVal && archive.EndTag(childItemGroupTag);

	}
	else {
		CCustomLayoutWidget* customWidget = dynamic_cast<CCustomLayoutWidget*>(*widget);
		QByteArray id;
		QPixmap icon;
		QString name;
		QByteArray iconAsByteArray;
		QByteArray viewId;
		AlignType titleAlign = AT_LEFT;

		if (customWidget != NULL) {
			id = customWidget->GetId();
			name = customWidget->GetName();
			viewId = customWidget->GetViewId();
			titleAlign = customWidget->GetTitleAlign();
			icon = customWidget->GetIcon();
			QDataStream stream(&iconAsByteArray, QIODevice::WriteOnly);
			stream << icon;
		}
		static iser::CArchiveTag herrachicalWidget("HerarchicalWidget", "Herarchical widget");
		retVal = retVal && archive.BeginTag(herrachicalWidget);

		static iser::CArchiveTag herrachicalId("Id", "Herarchical id");
		retVal = retVal && archive.BeginTag(herrachicalId);
		retVal = retVal && archive.Process(id);
		retVal = retVal && archive.EndTag(herrachicalId);

		static iser::CArchiveTag herrachicalName("Name", "Herarchical name");
		retVal = retVal && archive.BeginTag(herrachicalName);
		retVal = retVal && archive.Process(name);
		retVal = retVal && archive.EndTag(herrachicalName);

		static iser::CArchiveTag herrachicalViewId("ViewId", "Herarchical viewId");
		retVal = retVal && archive.BeginTag(herrachicalViewId);
		retVal = retVal && archive.Process(viewId);
		retVal = retVal && archive.EndTag(herrachicalViewId);

		static iser::CArchiveTag herrachicalTitleAlign("Title Align", "Herarchical title align");
		retVal = retVal && archive.BeginTag(herrachicalTitleAlign);
		retVal = retVal && I_SERIALIZE_ENUM(AlignType, archive, titleAlign);
		retVal = retVal && archive.EndTag(herrachicalTitleAlign);

		static iser::CArchiveTag herrachicalIcon("Icon", "Herarchical icon");
		retVal = retVal && archive.BeginTag(herrachicalIcon);
		retVal = retVal && archive.Process(iconAsByteArray);
		retVal = retVal && archive.EndTag(herrachicalIcon);

		retVal = retVal && archive.EndTag(herrachicalWidget);

		if (!archive.IsStoring()) {
			customWidget = new CCustomLayoutWidget(id, *this, NULL, NULL);
			m_customWidgetMap.insert(id, customWidget);
			customWidget->SetName(name);
			customWidget->SetTitleAlign(titleAlign);
			QDataStream stream(&iconAsByteArray, QIODevice::ReadWrite);
			stream >> icon;
			customWidget->SetIcon(icon);
			customWidget->SetViewId(viewId);
			if (viewId.isEmpty() == false){
				EmitAddWidgetByViewId(id, viewId);
			}

			*widget = customWidget;
		}

	}

	retVal = retVal && archive.EndTag(layoutItemTag);

	return retVal;
	
}


void CHierarchicalLayoutWidget::CleanLayoutRecursive(QLayout* layoutPtr)
{
	if (layoutPtr != NULL){
		while (QLayoutItem* itemPtr = layoutPtr->takeAt(0)){
			QWidget* widgetPtr = itemPtr->widget();
			if (widgetPtr != NULL) {
				delete widgetPtr;
			}

			QLayout* childLayoutPtr = itemPtr->layout();
			if (childLayoutPtr != NULL){
				CleanLayoutRecursive(childLayoutPtr);
			}

			delete itemPtr;
		}
	}
}


void CHierarchicalLayoutWidget::IdsListCollectChildIdsRecursive(const QByteArray& id, IdsList& idList)
{
	//InternalItemData* internalItemDataPtr = GetInternalItem(id);
	//if (internalItemDataPtr != NULL){
	//	idList.append(internalItemDataPtr->childItems);
	//	for (QByteArray& childId : internalItemDataPtr->childItems){
	//		IdsListCollectChildIdsRecursive(childId, idList);
	//	}
	//}
}


//QByteArray CHierarchicalLayoutWidget::GetParentId(const QByteArray& id)
//{
//	//for (int i = 0; i < m_internalItemList.count(); ++i){
//	//	if (m_internalItemList[i].childItems.contains(id)){
//	//		return m_internalItemList[i].id;
//	//	}
//	//}
//
//	return QByteArray();
//}


CHierarchicalLayoutWidget::InternalItemData* CHierarchicalLayoutWidget::GetInternalItem(const QByteArray& id)
{
	//for (int i = 0; i < m_internalItemList.count(); ++i){
	//	if (m_internalItemList[i].id == id){
	//		return &m_internalItemList[i];
	//	}
	//}

	return NULL;
}


// public methods of class CCustomLayoutWidget

CCustomLayoutWidget::CCustomLayoutWidget(const QByteArray& id, CHierarchicalLayoutWidget& hierarchicalLayoutWidget, CCustomLayoutWidget* parentCustomWidgetPtr, QWidget* parentPtr)
	:QWidget(parentPtr),
	m_hierarchicalLayoutWidget(hierarchicalLayoutWidget),
	//m_parentCustomWidgetPtr(parentCustomWidgetPtr),
	m_parentCustomWidgetPtr(NULL),
	m_id(id),
	m_externalWidgetPtr(NULL),
	m_name("No name"),
	m_titleSize(30),
	m_isHaveChilds(false),
	m_titleAlign(CHierarchicalLayoutWidget::AT_LEFT)
{
	setupUi(this);
//	TitlePanel->setProperty("ImtTopFrame", QVariant(true));
//	EditPanel->setProperty("ImtTopFrame", QVariant(true));
	ChangeNameButton->setVisible(false);
	ChangeIconButton1->setVisible(false);
	SetEditMode(m_hierarchicalLayoutWidget.m_viewMode == CHierarchicalLayoutWidget::VM_EDIT);
	//EditPanel->setVisible(m_hierarchicalLayoutWidget.m_viewMode == CHierarchicalLayoutWidget::VM_EDIT);

	connect(SplitVerticaly, &QToolButton::clicked, this, &CCustomLayoutWidget::OnSplitVertical);
	connect(SplitHorizontaly, &QToolButton::clicked, this, &CCustomLayoutWidget::OnSplitHorizontal);
	connect(DeleteButton, &QToolButton::clicked, this, &CCustomLayoutWidget::OnDeleteWidget);
	//connect(ChangeNameButton, &QToolButton::clicked, this, &CCustomLayoutWidget::OnChangeName);
	connect(ChangeIconButton, &QToolButton::clicked, this, &CCustomLayoutWidget::OnChangeIcon);

	//ButtonsPanel->setProperty("ImtTopFrame", QVariant(true));
	QAction *action;
	action = new QAction(tr("Left"), this);
	NamePosition->addAction(action);
	connect(action, &QAction::triggered, this, &CCustomLayoutWidget::OnNamePosition);
	action = new QAction(tr("Right"), this);
	NamePosition->addAction(action);
	connect(action, &QAction::triggered, this, &CCustomLayoutWidget::OnNamePosition);
	action = new QAction(tr("Center"), this);
	NamePosition->addAction(action);
	connect(action, &QAction::triggered, this, &CCustomLayoutWidget::OnNamePosition);

	for (int i = -1; i < m_hierarchicalLayoutWidget.m_additionalNames.count(); i++)
	{
//		QAction *action = addWidgets->addAction(m_hierarchicalLayoutWidget.m_additionalNames[i], this, &CCustomLayoutWidget::OnAddWidget);
		if (i < 0){
			action = new QAction(tr("Empty"), this);
		}
		else{
			action = new QAction(m_hierarchicalLayoutWidget.m_additionalNames[i], this);
		}
		connect(action, &QAction::triggered, this, &CCustomLayoutWidget::OnAddWidget);
		AddWidgets->addAction(action);
	}

	setAutoFillBackground(true);
	SetDefaultPalette();

	//setAcceptDrops(true);
	setMinimumSize(m_titleSize * 2, m_titleSize * 2);

	setLayout(new QVBoxLayout(this));
}


void CCustomLayoutWidget::SetDefaultPalette()
{
	QPalette pal = palette();
	pal.setColor(QPalette::Background, QColor("transparent"));
	setPalette(pal);

	//if (m_parentCustomWidgetPtr != NULL){
	//	m_parentCustomWidgetPtr->SetDefaultPalette();
	//}
}


void CCustomLayoutWidget::SetHighLightPalette()
{
	QPalette pal = palette();
	pal.setColor(QPalette::Background, QColor("lightblue"));
	setPalette(pal);
}


bool CCustomLayoutWidget::SetWidget(QWidget* widgetPtr)
{
	if (widgetPtr != NULL){
		//if (m_externalWidgetPtr != NULL){
		//	return false;
		//}

		if (WidgetPanel->layout() == NULL){
			WidgetPanel->setLayout(new QVBoxLayout(this));
		}

		if (WidgetPanel->layout()->count() == 0){
			//QWidget *widget = new QWidget();
			//widget->setFixedSize(m_titleSize, m_titleSize);
			//layout()->addWidget(widget);
			WidgetPanel->layout()->addWidget(widgetPtr);
			m_externalWidgetPtr = widgetPtr;
			return true;
		}
	}

	if (m_externalWidgetPtr == NULL){
		return true;
	}

	// take care about external widget
	if ((WidgetPanel->layout() != NULL) && (WidgetPanel->layout()->count() == 1) && (WidgetPanel->layout()->itemAt(0)->widget() != NULL)){
		QWidget* widgetPtr = WidgetPanel->layout()->takeAt(0)->widget();
		widgetPtr->setParent(NULL);
	}

	delete WidgetPanel->layout();
	m_externalWidgetPtr = NULL;

	return true;
}


QWidget* CCustomLayoutWidget::GetWidget()
{
	return m_externalWidgetPtr;
}


void CCustomLayoutWidget::SetIsHaveChilds(bool source)
{
	m_isHaveChilds = source;
}


void CCustomLayoutWidget::SetName(QString name)
{
	CCustomLayoutWidgetForm::titleName->setText(name);
	CCustomLayoutWidgetForm::TitleNameEdit->setText(name);
}


QString CCustomLayoutWidget::GetName()
{
	QString retVal;
	if (m_hierarchicalLayoutWidget.m_viewMode == CHierarchicalLayoutWidget::VM_EDIT){
		retVal = CCustomLayoutWidgetForm::TitleNameEdit->text();
	}
	else{
		retVal = CCustomLayoutWidgetForm::titleName->text();
	}
	return retVal;
}


void CCustomLayoutWidget::SetEditMode(bool isEditMode)
{
	if (isEditMode == true){
		EditPanel->setVisible(true);
		ChangeIconButton->setVisible(true);
		TitleNameEdit->setVisible(true);
		TitleNameEdit->setText(titleName->text());
		titleIcon->setVisible(false);
		titleName->setVisible(false);
	}
	else{
		EditPanel->setVisible(false);
		ChangeIconButton->setVisible(false);
		TitleNameEdit->setVisible(false);
		titleIcon->setVisible(true);
		titleName->setVisible(true);
		titleName->setText(TitleNameEdit->text());
	}
}


void CCustomLayoutWidget::SetId(QByteArray& id)
{
	m_id = id;
}


QByteArray CCustomLayoutWidget::GetId()
{
	return m_id;
}


QPixmap CCustomLayoutWidget::GetIcon()
{
//	QIcon icon = ChangeIconButton->icon();
//	QPixmap pixmap = icon.pixmap(ChangeIconButton->iconSize(), QIcon::Normal);
	QPixmap pixmap = *titleIcon->pixmap();
	return pixmap;
}


void CCustomLayoutWidget::SetIcon(QPixmap &icon)
{
	titleIcon->setPixmap(icon);
	ChangeIconButton->setIcon(icon);
}

QByteArray CCustomLayoutWidget::GetViewId()
{
	return m_viewId;
}


void CCustomLayoutWidget::SetViewId(const QByteArray &viewId)
{
	m_viewId = viewId;
}


void CCustomLayoutWidget::SetTitleAlign(const CHierarchicalLayoutWidget::AlignType &align)
{
	m_titleAlign = align;
	if (m_titleAlign == CHierarchicalLayoutWidget::AT_LEFT){
		TitleSpacerLeft->changeSize(2, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
		TitleSpacerRight->changeSize(2, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	}
	if (m_titleAlign == CHierarchicalLayoutWidget::AT_RIGHT) {
		TitleSpacerLeft->changeSize(2, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		TitleSpacerRight->changeSize(2, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
	}
	if (m_titleAlign == CHierarchicalLayoutWidget::AT_H_CENTER) {
		TitleSpacerLeft->changeSize(2, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		TitleSpacerRight->changeSize(2, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	}
	bool isEditMode = m_hierarchicalLayoutWidget.m_viewMode == CHierarchicalLayoutWidget::VM_EDIT;
	SetEditMode(!isEditMode);
	SetEditMode(isEditMode);
}


CHierarchicalLayoutWidget::AlignType CCustomLayoutWidget::GetTitleAlign()
{
	return m_titleAlign;
}


// protected methods

// reimplemented (QWidget)

void CCustomLayoutWidget::paintEvent(QPaintEvent* eventPtr)
{
	if (m_isHaveChilds == true)
		return;
	QPainter painter(this);
	painter.save();
	painter.setOpacity(0.7);
	//		painter.setPen(QPen(QColor("green")));
	QPen pen(QColor("gray"));
	if (m_hierarchicalLayoutWidget.m_viewMode == CHierarchicalLayoutWidget::VM_EDIT) {
		pen.setStyle(Qt::DashLine);
	}
	painter.setPen(pen);
	//	painter.setBrush(palette().color(QPalette::Background));
	painter.drawRect(rect().adjusted(0, 0, -1, -1));

	if (m_hierarchicalLayoutWidget.m_viewMode == CHierarchicalLayoutWidget::VM_EDIT) {

		//QRect r;
		//r.setWidth(rect().width());
		//r.setHeight(m_titleSize);
		//painter.drawRect(r);
		//r.setLeft(rect().right() - m_titleSize);
		//r.setTop(rect().top());
		//r.setWidth(m_titleSize);
		//r.setHeight(m_titleSize);
		//painter.drawRect(r);
		//painter.restore();
	}
	//QFont font = painter.font();
	//font.setPixelSize(m_titleSize * 0.8);
	//painter.setFont(font);

	//const QRect rectangle = QRect(0, 0, rect().width(), 50);
	//QRect boundingRect;
	//painter.drawText(rectangle, Qt::AlignHCenter, m_name, &boundingRect);
}


void CCustomLayoutWidget::dragEnterEvent(QDragEnterEvent *eventPtr)
{
	return;
	if (eventPtr->mimeData()->hasFormat("layout-item") || eventPtr->mimeData()->hasFormat("widget-item")){
		if ((eventPtr->mimeData()->data("layout-item") == "MergeLayout") && (layout() == NULL)){
			if (m_parentCustomWidgetPtr != NULL){
				m_parentCustomWidgetPtr->SetHighLightPalette();
				m_parentCustomWidgetPtr->update();
			}
		}
		else{
			SetHighLightPalette();
		}

		eventPtr->acceptProposedAction();
	}
}


void CCustomLayoutWidget::dragLeaveEvent(QDragLeaveEvent * /*eventPtr*/)
{
	return;
	SetDefaultPalette();
}


void CCustomLayoutWidget::dropEvent(QDropEvent *eventPtr)
{
	return;
	SetDefaultPalette();
	if (eventPtr->mimeData()->hasFormat("layout-item") || eventPtr->mimeData()->hasFormat("widget-item")){
		eventPtr->accept();
		eventPtr->setDropAction(Qt::MoveAction);

		m_hierarchicalLayoutWidget.OnDropEvent(m_id, eventPtr);
	}
	else{
		eventPtr->ignore();
	}
}


void CCustomLayoutWidget::mouseReleaseEvent(QMouseEvent *eventPtr)
{
	//m_hierarchicalLayoutWidget.OnMouseReleaseEvent(m_id, eventPtr);
	if (eventPtr->x() > rect().width() - m_titleSize && eventPtr->y() < m_titleSize && eventPtr->button() == Qt::RightButton) {
		m_hierarchicalLayoutWidget.EmitOpenMenuEvent(m_id, eventPtr);
	}
}


void CCustomLayoutWidget::OnSplitterMoved(int /*pos*/, int /*index*/)
{
	QSplitter* splitterPtr = dynamic_cast<QSplitter*>(sender());
	if (splitterPtr != NULL){
		m_hierarchicalLayoutWidget.OnSplitterMoved(m_id, splitterPtr->sizes());
	}
}


void CCustomLayoutWidget::OnNamePosition()
{
	QAction *action = dynamic_cast<QAction*>(sender());
	if (action) {
		QList<QAction*> actions = NamePosition->actions();
		int index = actions.indexOf(action);
		if (index == 0){
			SetTitleAlign(CHierarchicalLayoutWidget::AT_LEFT);
			//titleName->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			//TitleNameEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		}
		if (index == 1){
			SetTitleAlign(CHierarchicalLayoutWidget::AT_RIGHT);
			//titleName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			//TitleNameEdit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		}
		if (index == 2){
			SetTitleAlign(CHierarchicalLayoutWidget::AT_H_CENTER);
			//titleName->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//TitleNameEdit->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
		QWidget *widget = dynamic_cast<QWidget*>(parent());
		if (widget != NULL){
			widget->update();
		}
	}
}


void CCustomLayoutWidget::OnAddWidget()
{
	QAction *action = dynamic_cast<QAction*>(sender());
	if (action){
		QList<QAction*> actions = AddWidgets->actions();
		int index = actions.indexOf(action) - 1;
		emit m_hierarchicalLayoutWidget.EmitAddWidget(m_id, index);
	}
}


void CCustomLayoutWidget::OnDeleteWidget()
{
	m_hierarchicalLayoutWidget.RemoveLayout(m_id);
	//emit m_hierarchicalLayoutWidget.EmitDeleteWidget(m_id);
}


void CCustomLayoutWidget::OnSplitVertical()
{
	m_hierarchicalLayoutWidget.SetSplitterLayout(m_id, Qt::Vertical, 2);
	//emit m_hierarchicalLayoutWidget.EmitSplitVertical(m_id);
}


void CCustomLayoutWidget::OnSplitHorizontal()
{
	m_hierarchicalLayoutWidget.SetSplitterLayout(m_id, Qt::Horizontal, 2);
	//emit m_hierarchicalLayoutWidget.EmitSplitHorizontal(m_id);
}


void CCustomLayoutWidget::OnChangeName()
{
	QString name = CCustomLayoutWidgetForm::titleName->text();
	bool ok;
	name = QInputDialog::getText(this, tr("Get Name"),
		tr("View name:"), QLineEdit::Normal,
		name, &ok);
	if (ok && !name.isEmpty()) {
		titleName->setText(name);
	}
}


void CCustomLayoutWidget::OnChangeIcon()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp *.ico *.svg)"));
	if (fileName.isEmpty() == false){
		QPixmap pixmap(fileName);
		pixmap = pixmap.scaled(28, 28);
		titleIcon->setPixmap(pixmap);
		ChangeIconButton->setIcon(pixmap);
	}
}


} // namespace imtgui
