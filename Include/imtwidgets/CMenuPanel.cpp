#include <imtwidgets/CMenuPanel.h>
#include <imtwidgets/CMenuPanelDelegate.h>

// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtCore/QStack>
#include <QtCore/QModelIndex>
#include <QtGui/QStandardItem>
#include <QtWidgets/QScrollBar>


namespace imtwidgets
{


// public methods

CMenuPanel::CMenuPanel(QWidget* parent)
	:QWidget(parent),
	m_maxWidth(200),
	m_indent(20),
	m_padding(0)
{
	setupUi(this);

	m_model.setColumnCount(1);
	m_model.setSortRole(Qt::UserRole + 100);

	setMouseTracking(true);

	PageTree->setModel(&m_model);
	PageTree->setSelectionMode(QAbstractItemView::SingleSelection);
	PageTree->selectionModel()->clearSelection();
	connect(PageTree->selectionModel(), &QItemSelectionModel::currentChanged, this, &CMenuPanel::OnPageIdChanged);

	PageTree->setItemDelegate(new CMenuPanelDelegate(this));
	PageTree->setProperty("indent", 0);
	PageTree->setProperty("indentMax", m_indent);
	PageTree->setProperty("padding", 0);

	PageTree->setHeaderHidden(true);
	PageTree->setIconSize(QSize(16, 16));
	PageTree->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	PageTree->setIndentation(0);

	PageTree->setStyleSheet("QTreeView::item::hover {background - color:rgb(0, 0, 0);}");
	PageTree->verticalScrollBar()->installEventFilter(this);

	PageTree->setContentsMargins(QMargins(0,0,0,0));
	PageTree->setMaximumWidth(PageTree->iconSize().width() + 4 + 2 * m_padding + 2 * m_padding);

	m_animationWidth.setTargetObject(PageTree);
	m_animationWidth.setPropertyName("maximumWidth");
	m_animationIndent.setTargetObject(PageTree);
	m_animationIndent.setPropertyName("indent");

	pushTop->setIcon(QIcon(":/Icons/Up"));
	pushTop->setAutoRepeat(true);
	pushBottom->setIcon(QIcon(":/Icons/Down"));
	pushBottom->setAutoRepeat(true);
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
	PageTree->setCurrentIndex(index);
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
		m_model.itemFromIndex(index)->setEnabled(isPageEnabled);
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

		m_maxWidth = CalculateMaxItemWith();
		PageTree->expandAll();

		return true;
	}

	QModelIndex parentModelIndex = GetModelIndex(parentPageId);
	if (parentModelIndex.isValid()){
		int row = m_model.rowCount(parentModelIndex);

		QStandardItem* itemPtr = new QStandardItem();
		m_model.itemFromIndex(parentModelIndex)->insertRow(row,itemPtr);

		QModelIndex modelIndex = m_model.index(row, 0, parentModelIndex);
		m_model.setData(modelIndex, pageId, DR_PAGE_ID);

		m_maxWidth = CalculateMaxItemWith();
		PageTree->expandAll();

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

		m_maxWidth = CalculateMaxItemWith();

		return true;
	}

	return false;
}


void CMenuPanel::SetItemPadding(int padding)
{
	if (padding > 0){
		m_padding = padding;
		PageTree->setProperty("padding", padding);
		QString style = QString("QTreeView::item{padding: %1px;} QTreeView::item::hover{background-color:rgb(0, 0, 0);}").arg(padding);		
		PageTree->setStyleSheet(style);
		PageTree->setMaximumWidth(PageTree->iconSize().width() + 4 + 2 * m_padding + 2 * m_padding);
	}
}


void CMenuPanel::SetIconSize(int size)
{
	if (size > 8){
		PageTree->setIconSize(QSize(size, size));
		PageTree->setMaximumWidth(PageTree->iconSize().width() + 4 + 2 * m_padding + 2 * m_padding);
	}
}


void CMenuPanel::SetItemSelectedColor(QColor color)
{
	PageTree->setProperty("ItemSelectedColor", color);
}


void CMenuPanel::SetItemMouserOverColor(QColor color)
{
	PageTree->setProperty("ItemMouserOverColor", color);
}


void CMenuPanel::SetItemMouserOverSelectedColor(QColor color)
{
	PageTree->setProperty("ItemMouserOverSelectedColor", color);
}


void CMenuPanel::OnPageIdChanged(const QModelIndex& selected, const QModelIndex& deselected)
{
	QByteArray selectedId;
	QByteArray deselectedId;

	if (selected.isValid()){
		selectedId = selected.data(DR_PAGE_ID).toByteArray();
	}

	if (deselected.isValid()){
		deselectedId = deselected.data(DR_PAGE_ID).toByteArray();
	}

	//QVariant pageEnabled = selected.data(DR_PAGE_ENABLED);
	//if (/*pageEnabled.isValid() && */pageEnabled.type() == QVariant::Bool){
	//	if (pageEnabled.toBool()){
			PageIdChanged(selectedId, deselectedId);
	//	}
	//	else {
	//		PageTree->selectionModel()->clear();
	//		PageTree->setCurrentIndex(deselected);
	//	}
	//}
}


void imtwidgets::CMenuPanel::on_pushBottom_clicked()
{
	QModelIndex index = PageTree->indexAt(QPoint(0, PageTree->height()));
	PageTree->scrollTo(index);
	if (index == PageTree->indexAt(QPoint(0, PageTree->height())))
	{
		index = PageTree->indexBelow(index);
		PageTree->scrollTo(index);
	}
	checkButtonsVisible();
}


void imtwidgets::CMenuPanel::on_pushTop_clicked()
{
	QModelIndex index = PageTree->indexAt(QPoint(0, 0));
	PageTree->scrollTo(index);

	if (index == PageTree->indexAt(QPoint(0, 0)))
	{
		index = PageTree->indexAbove(index);
		PageTree->scrollTo(index);
	}
	checkButtonsVisible();
}


// protected methods


// reimplemented (QObject)

bool CMenuPanel::eventFilter(QObject *obj, QEvent *event)
{
	checkButtonsVisible();
	return QObject::eventFilter(obj, event);
}


// reimplemented (QWidget)

void CMenuPanel::enterEvent(QEvent* event)
{
	Q_UNUSED(event)

	m_animationWidth.stop();
	m_animationWidth.setStartValue(PageTree->maximumWidth());
	m_animationWidth.setEndValue(m_maxWidth);
	m_animationWidth.setDuration(150);
	m_animationWidth.start();

	m_animationIndent.stop();
	m_animationIndent.setStartValue(PageTree->maximumWidth() * m_indent/ m_maxWidth);
	m_animationIndent.setEndValue(m_indent);
	m_animationIndent.setDuration(150);
	m_animationIndent.start();
}


void CMenuPanel::leaveEvent(QEvent* event)
{
	Q_UNUSED(event)

	m_animationWidth.stop();
	m_animationWidth.setStartValue(PageTree->maximumWidth());
	m_animationWidth.setEndValue(PageTree->iconSize().width() + 4 + 2 * m_padding + 2 * m_padding);
	m_animationWidth.setDuration(150);
	m_animationWidth.start();

	m_animationIndent.stop();
	m_animationIndent.setStartValue(PageTree->maximumWidth() * m_indent / m_maxWidth);
	m_animationIndent.setEndValue(0);
	m_animationIndent.setDuration(150);
	m_animationIndent.start();
}


void CMenuPanel::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event)
	checkButtonsVisible();
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


int CMenuPanel::CalculateMaxItemWith()
{
	QStack<QModelIndex> stack;
	QModelIndex index = m_model.index(0, 0);
	int maxWidth = 0;

	while (index.isValid()){
		QString text = index.data(Qt::DisplayRole).toString();
		

		int offset = -m_indent;
		QModelIndex check = index;
		while (check.isValid()){
			offset += m_indent;
			check = check.parent();
		}

		int current = offset + m_padding + 2 + m_padding + PageTree->iconSize().width() + 2 * m_padding;
		
		QFontMetrics fm = PageTree->fontMetrics();
		current += fm.boundingRect(text).width() + 2 * m_padding;
		if (current > maxWidth){
			maxWidth = current;
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

	return maxWidth;
}


void CMenuPanel::checkButtonsVisible()
{
	int position = PageTree->verticalScrollBar()->value();
	if (position == 0)
	{
		pushTop->setVisible(false);
	}
	else
	{
		pushTop->setVisible(true);
	}

	if (position == PageTree->verticalScrollBar()->maximum())
	{
		pushBottom->setVisible(false);
	}
	else
	{
		pushBottom->setVisible(true);
	}
}


} // namespace imtwidgets


