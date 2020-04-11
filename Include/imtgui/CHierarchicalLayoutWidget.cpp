#include <imtgui/CHierarchicalLayoutWidget.h>


// Qt includes
#include <QMimeData>
#include <QSplitter>
#include <QVBoxLayout>
#include <QUuid>

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
	CCustomLayoutWidget* customLayoutWidgetPtr = new CCustomLayoutWidget(m_rootId, *this, NULL, this);
	if (customLayoutWidgetPtr != NULL){
		layout()->addWidget(customLayoutWidgetPtr);	
		m_customWidgetMap.insert(m_rootId, customLayoutWidgetPtr);
		m_internalItemList.push_back(InternalItemData(m_rootId));
	}

	SetViewMode(VM_NORMAL);
}


void CHierarchicalLayoutWidget::SetViewMode(ViewMode viewMode)
{
	m_viewMode = viewMode;

	IdsList keys = m_customWidgetMap.keys();
	for (QByteArray& key : keys){
		CCustomLayoutWidget* customWidgetPtr = m_customWidgetMap[key];
		InternalItemData* internalItemDataPtr = GetInternalItem(key);
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

		for (int i = 0; i < m_internalItemList.count(); ++i){
			if (m_internalItemList[i].id == m_rootId){
				m_internalItemList[i].id == id;
			}
		}
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
	InternalItemData* internalItemDataPtr = GetInternalItem(id);
	if (internalItemDataPtr != NULL){
		internalItemDataPtr->sizeList = sizeList;
		return true;
	}

	return false;
}


bool CHierarchicalLayoutWidget::SetWidgetToItem(const QByteArray& id, QWidget* widgetPtr)
{
	InternalItemData* internalItemDataPtr = GetInternalItem(id);
	if (internalItemDataPtr != NULL){
		//if (((internalItemDataPtr->layoutType == LT_NONE) && (widgetPtr != NULL)) ||
		//	(widgetPtr == NULL)){
			if ((m_customWidgetMap.contains(id)) && m_customWidgetMap[id]->SetWidget(widgetPtr)){
				internalItemDataPtr->layoutType = (widgetPtr != NULL) ? LT_OBJECT : LT_NONE;
				return true;
			}
		//}
	}

	return false;
}


QByteArray CHierarchicalLayoutWidget::GetRootItemId()
{
	return m_rootId;
}


CHierarchicalLayoutWidget::IdsList CHierarchicalLayoutWidget::GetItemChildIdList(const QByteArray& id)
{
	IdsList retVal;
	InternalItemData* internalItemDataPtr = GetInternalItem(id);
	if (internalItemDataPtr != NULL){
		retVal.append(internalItemDataPtr->childItems);
	}

	return retVal;
}


CHierarchicalLayoutWidget::LayoutType CHierarchicalLayoutWidget::GetItemLayoutType(const QByteArray& id)
{
	CHierarchicalLayoutWidget::LayoutType retVal;
	InternalItemData* internalItemDataPtr = GetInternalItem(id);
	if (internalItemDataPtr != NULL){
		retVal = internalItemDataPtr->layoutType;
	}

	return retVal;
}


CHierarchicalLayoutWidget::SizeList CHierarchicalLayoutWidget::GetItemSizes(const QByteArray& id)
{
	CHierarchicalLayoutWidget::SizeList retVal;
	InternalItemData* internalItemDataPtr = GetInternalItem(id);
	if (internalItemDataPtr != NULL){
		retVal = internalItemDataPtr->sizeList;
	}

	return retVal;
}


void CHierarchicalLayoutWidget::ClearAll()
{
	CleanLayoutRecursive(layout());
	m_customWidgetMap.clear();
	m_internalItemList.clear();

	m_rootId = QUuid::createUuid().toByteArray();
	CCustomLayoutWidget* customLayoutWidgetPtr = new CCustomLayoutWidget(m_rootId, *this, NULL, this);
	if (customLayoutWidgetPtr != NULL){
		layout()->addWidget(customLayoutWidgetPtr);
		m_customWidgetMap.insert(m_rootId, customLayoutWidgetPtr);
		m_internalItemList.push_back(InternalItemData(m_rootId));
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
				SetWidgetToItem(id, NULL);
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
	InternalItemData* internalItemDataPtr = GetInternalItem(id);
	if (m_customWidgetMap.contains(id) && (internalItemDataPtr != NULL)){
		internalItemDataPtr->sizeList.clear();
		internalItemDataPtr->sizeList << sizeList;
	}
}


void CHierarchicalLayoutWidget::SetSplitterLayout(const QByteArray& id, Qt::Orientation orientation, int count, IdsList *idsListPtr)
{
	IdsList localIdsList;
	InternalItemData* internalItemDataPtr = GetInternalItem(id);
	QList<int> sizes;

	if (m_customWidgetMap.contains(id) && (internalItemDataPtr != NULL)){
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
		internalItemDataPtr->childItems << newId;
		m_internalItemList << InternalItemData(newId);
	}

}


void CHierarchicalLayoutWidget::MergeLayout(const QByteArray& id)
{
	InternalItemData* internalItemDataPtr = GetInternalItem(id);
	if ((internalItemDataPtr != NULL) && (internalItemDataPtr->layoutType != LT_OBJECT) && m_customWidgetMap.contains(id)){
		if (internalItemDataPtr->layoutType == LT_NONE){
			// transfer to parent
			QByteArray parentId = GetParentId(id);
			if (!parentId.isEmpty()){
				MergeLayout(parentId);
				InternalItemData* parentInternalItemDataPtr = GetInternalItem(parentId);
				if (parentInternalItemDataPtr != NULL){
					parentInternalItemDataPtr->layoutType = LT_NONE;
				}
				return;
			}
		}

		QLayout* layoutPtr = m_customWidgetMap[id]->layout();
		if (layoutPtr != NULL){
			IdsList childIdList;
			IdsListCollectChildIdsRecursive(id, childIdList);
			for (QByteArray& childId : childIdList){
				m_customWidgetMap[childId]->deleteLater();
				m_customWidgetMap.remove(childId);
				m_internalItemList.removeAll(InternalItemData(childId));
			}
			CleanLayoutRecursive(layoutPtr);
			m_customWidgetMap[id]->setAcceptDrops(true);
			delete layoutPtr;
		}
	}
}


void CHierarchicalLayoutWidget::RemoveLayout(const QByteArray& id)
{

	InternalItemData* internalItemDataPtr = GetInternalItem(id);
	if ((internalItemDataPtr != NULL) && m_customWidgetMap.contains(id)){
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
			m_internalItemList.removeAll(InternalItemData(id));
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
	InternalItemData* internalItemDataPtr = GetInternalItem(id);
	if (internalItemDataPtr != NULL){
		idList.append(internalItemDataPtr->childItems);
		for (QByteArray& childId : internalItemDataPtr->childItems){
			IdsListCollectChildIdsRecursive(childId, idList);
		}
	}
}


QByteArray CHierarchicalLayoutWidget::GetParentId(const QByteArray& id)
{
	for (int i = 0; i < m_internalItemList.count(); ++i){
		if (m_internalItemList[i].childItems.contains(id)){
			return m_internalItemList[i].id;
		}
	}

	return QByteArray();
}


CHierarchicalLayoutWidget::InternalItemData* CHierarchicalLayoutWidget::GetInternalItem(const QByteArray& id)
{
	for (int i = 0; i < m_internalItemList.count(); ++i){
		if (m_internalItemList[i].id == id){
			return &m_internalItemList[i];
		}
	}

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
	m_isHaveChilds(false)
{
	setAutoFillBackground(true);
	SetDefaultPalette();

	setAcceptDrops(true);
	setMinimumSize(m_titleSize * 2, m_titleSize * 2);
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

		if (layout() == NULL){
			setLayout(new QVBoxLayout(this));
		}

		if (layout()->count() == 0){
			QWidget *widget = new QWidget();
			widget->setFixedSize(m_titleSize, m_titleSize);
			layout()->addWidget(widget);
			layout()->addWidget(widgetPtr);
			m_externalWidgetPtr = widgetPtr;
			return true;
		}
	}

	if (m_externalWidgetPtr == NULL){
		return true;
	}

	// take care about external widget
	if ((layout() != NULL) && (layout()->count() == 1) && (layout()->itemAt(0)->widget() != NULL)){
		QWidget* widgetPtr = layout()->takeAt(0)->widget();
		widgetPtr->setParent(NULL);
	}

	delete layout();
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
	m_name = name;
}


QString CCustomLayoutWidget::GetName()
{
	return m_name;
}


// protected methods

// reimplemented (QWidget)

void CCustomLayoutWidget::paintEvent(QPaintEvent* eventPtr)
{
	if (m_isHaveChilds == true)
		return;
	QPainter painter(this);
	if ( m_hierarchicalLayoutWidget.m_viewMode == CHierarchicalLayoutWidget::VM_EDIT){
		painter.save();
		painter.setOpacity(0.7);
		painter.setPen(QPen(QColor("green")));
	//	painter.setBrush(palette().color(QPalette::Background));
		painter.drawRect(rect().adjusted(0, 0, -1, -1));
		QRect r;
		r.setWidth(rect().width());
		r.setHeight(m_titleSize);
		painter.drawRect(r);
		r.setLeft(rect().right() - m_titleSize);
		r.setTop(rect().top());
		r.setWidth(m_titleSize);
		r.setHeight(m_titleSize);
		painter.drawRect(r);
		painter.restore();
	}
	QFont font = painter.font();
	font.setPixelSize(m_titleSize * 0.8);
	painter.setFont(font);

	const QRect rectangle = QRect(0, 0, rect().width(), 50);
	QRect boundingRect;
	painter.drawText(rectangle, Qt::AlignHCenter, m_name, &boundingRect);
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


} // namespace imtgui
