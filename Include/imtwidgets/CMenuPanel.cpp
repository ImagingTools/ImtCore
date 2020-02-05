#include <imtwidgets/CMenuPanel.h>
#include <imtwidgets/CMenuPanelDelegate.h>

// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtCore/QStack>
#include <QtCore/QModelIndex>
#include <QtGui/QStandardItem>


namespace imtwidgets
{


// public methods

CMenuPanel::CMenuPanel(QWidget* parent)
	:QWidget(parent),
	m_minWidth(38),
	m_maxWidth(200),
	m_indent(20)
{
	setupUi(this);

	m_model.setColumnCount(1);
	m_model.setSortRole(Qt::UserRole + 100);

	setMouseTracking(true);

	PageTree->setModel(&m_model);
	PageTree->setSelectionMode(QAbstractItemView::SingleSelection);
	PageTree->selectionModel()->clearSelection();
	connect(PageTree->selectionModel(), &QItemSelectionModel::currentChanged, this, &CMenuPanel::OnPageIdChanged);

	PageTree->setItemDelegate(new CMenuPanelDelegate(PageTree));
	PageTree->setProperty("indent", 0);

	PageTree->setHeaderHidden(true);
	PageTree->setIconSize(QSize(24, 24));
	PageTree->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	PageTree->setIndentation(0);
	PageTree->setMaximumWidth(m_minWidth);
	PageTree->setStyleSheet("QTreeView::item::hover{background-color:rgb(160,160,160);}" "QTreeView::item{padding: 6px;}");

	m_animationWidth.setTargetObject(PageTree);
	m_animationWidth.setPropertyName("maximumWidth");
	m_animationIndent.setTargetObject(PageTree);
	m_animationIndent.setPropertyName("indent");

	//m_focusDecoratorPtr = new iwidgets::CFocusDecorator(this);
	//m_focusDecoratorPtr->RegisterWidget(PageTree, &m_graphicsEffectFactory);
}


int CMenuPanel::GetMinWidth() const
{
	return m_minWidth;
}


void CMenuPanel::SetMinWidth(int maxWidth)
{
	m_minWidth = maxWidth;
}


int CMenuPanel::GetMaxWidth() const
{
	return m_maxWidth;
}


void CMenuPanel::SetMaxWidth(int maxWidth)
{
	m_maxWidth = maxWidth;
}


QByteArray CMenuPanel::GetActivePage() const
{
	return QByteArray();
}


void CMenuPanel::SetActivePage(const QByteArray& pageId)
{
	QModelIndex index;
	if (!pageId.isEmpty()){
		index = GetModelIndex(pageId);
	}

	PageTree->selectionModel()->clear();
	PageTree->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
}


bool CMenuPanel::IsPageEnabled(const QByteArray& pageId) const
{
	QModelIndex index = GetModelIndex(pageId);
	if (index.isValid()){
		return m_model.itemFromIndex(index)->data(DR_PAGE_ENABLED).toBool();
	}

	return false;
}


bool CMenuPanel::SetPageEnabled(const QByteArray& pageId, bool isPageEnabled)
{
	QModelIndex index = GetModelIndex(pageId);
	if (index.isValid()){
		m_model.itemFromIndex(index)->setData(isPageEnabled, DR_PAGE_ENABLED);

		return true;
	}

	return false;
}


bool CMenuPanel::IsPageVisible(const QByteArray& pageId) const
{
	QModelIndex index = GetModelIndex(pageId);
	if (index.isValid()){
		return m_model.itemFromIndex(index)->data(DR_PAGE_VISIBLE).toBool();
	}

	return false;
}


bool CMenuPanel::SetPageVisible(const QByteArray& pageId, bool isPageVisible)
{
	QModelIndex index = GetModelIndex(pageId);
	if (index.isValid()){
		m_model.itemFromIndex(index)->setData(isPageVisible, DR_PAGE_VISIBLE);

		return true;
	}

	return false;
}


void CMenuPanel::ResetPages()
{
	m_model.clear();
	m_model.setColumnCount(1);
}


bool CMenuPanel::IsPageIdExist(const QByteArray& pageId) const
{
	QModelIndex index = GetModelIndex(pageId);
	return index.isValid();
}


bool CMenuPanel::RemovePage(const QByteArray& pageId)
{
	QModelIndex index = GetModelIndex(pageId);
	if (index.isValid()){
		return m_model.removeRow(index.row(), index.parent());
	}

	return false;
}


bool CMenuPanel::InsertPage(const QByteArray& pageId, const QByteArray& parentPageId)
{
	if (pageId.isEmpty()){
		return false;
	}

	if (IsPageIdExist(pageId)){
		return false;
	}

	if (parentPageId.isEmpty()){
		int row = m_model.rowCount(QModelIndex());

		QStandardItem* itemPtr = new QStandardItem();
		m_model.insertRow(row, itemPtr);

		QModelIndex modelIndex = m_model.index(row, 0, QModelIndex());
		m_model.setData(modelIndex, pageId, DR_PAGE_ID);

		return true;
	}

	QModelIndex parentModelIndex = GetModelIndex(parentPageId);
	if (parentModelIndex.isValid()) {
		int row = m_model.rowCount(parentModelIndex);

		QStandardItem* itemPtr = new QStandardItem();
		m_model.itemFromIndex(parentModelIndex)->insertRow(row,itemPtr);

		QModelIndex modelIndex = m_model.index(row, 0, parentModelIndex);
		m_model.setData(modelIndex, pageId, Qt::UserRole);

		return true;
	}

	return false;
}


int CMenuPanel::GetPageOrder(const QByteArray& pageId) const
{
	QModelIndex index = GetModelIndex(pageId);
	if (index.isValid()){
		return index.row();
	}

	return -1;
}


bool CMenuPanel::SetPageOrder(const QByteArray& pageId, int position)
{
	QModelIndex index = GetModelIndex(pageId);
	if (index.isValid()){
		QStandardItem* item = m_model.itemFromIndex(index.parent());
		int row;
		if (item == nullptr){
			row = m_model.rowCount()-1;
		}
		else{
			row = item->rowCount()-1;
		}

		if (position < row){
			QModelIndex parent = index.parent();
			QStandardItem* oldItem = m_model.itemFromIndex(index);
			QStandardItem* newItem = new QStandardItem();
			newItem->setText(oldItem->text());
			newItem->setIcon(oldItem->icon());
			newItem->setData(oldItem->data((DR_PAGE_ID)));

			m_model.removeRow(index.row(), parent);

			if (parent.isValid()){
				m_model.insertRow(position, newItem);
			} else{

			}
		}
	}

	return false;
}


QList<QByteArray> CMenuPanel::GetChilds(const QByteArray& pageId)
{
	QList<QByteArray> childs;

	if (pageId.isEmpty()){
		for (int i = 0; i < m_model.rowCount(); i++){
			childs.append(m_model.item(i, 0)->data(DR_PAGE_ID).toByteArray());
		}
	}

	QModelIndex index = GetModelIndex(pageId);
	if (index.isValid()){
		QStandardItem* itemPtr = m_model.itemFromIndex(index);

		for (int i = 0; i < itemPtr->rowCount(); i++){
			childs.append(itemPtr->child(i)->data(DR_PAGE_ID).toByteArray());
		}
	}

	return childs;
}


QIcon CMenuPanel::GetPageIcon(const QByteArray& pageId) const
{
	QModelIndex index = GetModelIndex(pageId);
	if (index.isValid()){
		return m_model.itemFromIndex(index)->icon();
	}

	return QIcon();
}


bool CMenuPanel::SetPageIcon(const QByteArray& pageId, const QIcon& pageIcon)
{
	QModelIndex index = GetModelIndex(pageId);
	if (index.isValid()){
		m_model.itemFromIndex(index)->setIcon(pageIcon);
		return true;
	}

	return false;
}


QString CMenuPanel::GetPageName(const QByteArray& pageId) const
{
	QModelIndex index = GetModelIndex(pageId);
	if (index.isValid()){
		return m_model.itemFromIndex(index)->text();
	}

	return QString();
}


bool CMenuPanel::SetPageName(const QByteArray& pageId, const QString& pageName)
{
	QModelIndex index = GetModelIndex(pageId);
	if (index.isValid()){
		m_model.itemFromIndex(index)->setText(pageName);
		return true;
	}

	return false;
}


void CMenuPanel::OnPageIdChanged(const QModelIndex& selected, const QModelIndex& deselected)
{
	//QModelIndexList selectedList = selected.indexes();
	//QModelIndexList deselectedList = deselected.indexes();

	QByteArray selectedId;
	QByteArray deselectedId;

	if (selected.isValid()){
		selectedId = selected.data(DR_PAGE_ID).toByteArray();
	}

	if (deselected.isValid()){
		deselectedId = deselected.data(DR_PAGE_ID).toByteArray();
	}

	qDebug() << 111;
	PageIdChanged(selectedId, deselectedId);
}


// protected methods

// reimplemented (QWidget)

void CMenuPanel::enterEvent(QEvent* event)
{
	Q_UNUSED(event)

	m_animationWidth.setStartValue(PageTree->maximumWidth());
	m_animationWidth.setEndValue(m_maxWidth);
	m_animationWidth.setDuration(150);
	m_animationWidth.start();

	m_animationIndent.setStartValue(PageTree->maximumWidth() * m_indent/ m_maxWidth);
	m_animationIndent.setEndValue(m_indent);
	m_animationIndent.setDuration(150);
	m_animationIndent.start();
}


void CMenuPanel::leaveEvent(QEvent* event)
{
	Q_UNUSED(event)

	m_animationWidth.setStartValue(PageTree->maximumWidth());
	m_animationWidth.setEndValue(m_minWidth);
	m_animationWidth.setDuration(150);
	m_animationWidth.start();

	m_animationIndent.setStartValue(PageTree->maximumWidth() * m_indent / m_maxWidth);
	m_animationIndent.setEndValue(0);
	m_animationIndent.setDuration(150);
	m_animationIndent.start();
}


// private methods

QModelIndex CMenuPanel::GetModelIndex(const QByteArray& pageId) const
{
	QStack<QModelIndex> stack;

	QModelIndex index = m_model.index(0, 0);
	while (index.isValid()){
		QVariant itemData = index.data(DR_PAGE_ID);
		if (itemData.isValid()){
			if (itemData.toByteArray() == pageId){
				return index;
			}
		}

		QModelIndex childIndex = m_model.index(0, 0, index);
		if (childIndex.isValid()){
			QModelIndex sibling = index.siblingAtRow(index.row() + 1);
			if (sibling.isValid()){
				stack.push(sibling);
			}

			index = m_model.index(0, 0, index);
			continue;
		}

		QModelIndex siblingIndex = index.siblingAtRow(index.row() + 1);
		if (siblingIndex.isValid()){
			index = siblingIndex;
			continue;
		}

		if (stack.isEmpty()){
			break;
		}

		index = stack.pop();
	}

	return QModelIndex();
}


// protected methods of the embedded class FocusDecorationFactory

// reimplemented (iGraphicsEffectFactory)

//QGraphicsEffect* CMenuPanel::FocusDecorationFactory::CreateInstance(const QByteArray& /*keyId*/) const
//{
//	QGraphicsDropShadowEffect* shadowPtr = new QGraphicsDropShadowEffect;
//	shadowPtr->setXOffset(0);
//	shadowPtr->setYOffset(0);
//	shadowPtr->setBlurRadius(12);
//	//shadowPtr->setColor(qRgba(74, 149, 217, 128));
//
//	return shadowPtr;
//}
//
//
//istd::IFactoryInfo::KeyList CMenuPanel::FocusDecorationFactory::GetFactoryKeys() const
//{
//	istd::IFactoryInfo::KeyList retVal;
//
//	return retVal;
//}


} // namespace imtwidgets


