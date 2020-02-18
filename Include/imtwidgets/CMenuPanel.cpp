#include <imtwidgets/CMenuPanel.h>
#include <QDebug>


// Qt includes
#include <QtCore/QEvent>
#include <QtCore/QStack>
#include <QtCore/QModelIndex>
#include <QtGui/QStandardItem>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QScrollBar>

// ImtCore includes
#include <imtwidgets/CMenuPanelDelegate.h>


namespace imtwidgets
{


// public methods

CMenuPanel::CMenuPanel(QWidget* parent)
	:QWidget(parent),
	m_maxWidth(200),
	m_indent(20),
	m_padding(0),
	m_animationAction(AA_NONE),
	m_animationTimerIdentifier(0),
	m_animationDelay(100),
	m_animationDuration(100),
	m_mainWidget(nullptr),
	m_leftFrame(parent),
	m_parentWidget(nullptr),
	m_shadowPtr(nullptr)
{
	setupUi(this);

	m_model.setColumnCount(1);
	m_model.setSortRole(Qt::UserRole + 100);

	setMouseTracking(true);
	PageTree->installEventFilter(this);

	PageTree->setModel(&m_model);
	PageTree->setSelectionMode(QAbstractItemView::SingleSelection);
	PageTree->selectionModel()->clearSelection();
	connect(PageTree->selectionModel(), &QItemSelectionModel::currentChanged, this, &CMenuPanel::OnPageIdChanged);

	PageTree->setItemDelegate(new CMenuPanelDelegate(PageTree));
	PageTree->setProperty("indent", 0);
	PageTree->setProperty("indentMax", m_indent);
	PageTree->setProperty("padding", 0);

	PageTree->setProperty("ItemSelectedColor", QColor(0,0,0));
	PageTree->setProperty("ItemMouserOverColor", QColor(0, 0, 0));
	PageTree->setProperty("ItemMouserOverSelectedColor", QColor(0, 0, 0));
	PageTree->setProperty("ItemSelectedContourColor", QColor(0, 0, 0));
	PageTree->setProperty("ItemTextColor", QColor(0, 0, 0));

	PageTree->setHeaderHidden(true);
	PageTree->setIconSize(QSize(16, 16));
	PageTree->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	PageTree->setIndentation(0);

	PageTree->setStyleSheet("QTreeView::item::hover {background - color:rgb(0, 0, 0);}");
	PageTree->verticalScrollBar()->installEventFilter(this);

	PageTree->setContentsMargins(QMargins(0,0,0,0));
	PageTree->setMaximumWidth(m_minWidth);


	pushTop->setStyleSheet("QPushButton:hover{	background: white } "
		" QPushButton{ border: 1px solid #9d9d9d; background: #e0e0e0; }" );

	pushBottom->setStyleSheet(pushTop->styleSheet());

	m_animationWidthComp.setTargetObject(this);
	m_animationWidthComp.setPropertyName("geometry");
	m_animationWidth.setTargetObject(PageTree);
	m_animationWidth.setPropertyName("maximumWidth");
	m_animationIndent.setTargetObject(PageTree);
	m_animationIndent.setPropertyName("indent");
	connect(&m_animationWidth, &QPropertyAnimation::finished, this, &CMenuPanel::OnAnimationFinished);
	connect(&m_animationWidthComp, &QPropertyAnimation::finished, this, &CMenuPanel::OnAnimationFinished);

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

	QModelIndex prevIndex = PageTree->currentIndex();
	PageTree->selectionModel()->clear();
	PageTree->setCurrentIndex(index);

	if (pageId.isEmpty()){
		if (m_animationTimerIdentifier){
			killTimer(m_animationTimerIdentifier);
		}

		m_animationAction = AA_EXPAND;
		StartAnimation();
	}

	if (!prevIndex.isValid() && index.isValid()){
		if (m_animationTimerIdentifier){
			killTimer(m_animationTimerIdentifier);
		}

		m_animationAction = AA_COLLAPSE;
		StartAnimation();
	}
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
		m_minWidth = CalculateMinItemWith();
		SetMinimumPanelWidth(m_minWidth);
	}
}


void CMenuPanel::SetIconSize(int size)
{
	if (size > 8){
		PageTree->setIconSize(QSize(size, size));
		m_minWidth = CalculateMinItemWith();
		SetMinimumPanelWidth(m_minWidth);
	}
}


void CMenuPanel::SetAnimationDelay(int delay)
{
	m_animationDelay = delay;
}


void CMenuPanel::SetAnimationDuration(int duration)
{
	m_animationDuration = duration;
}


void CMenuPanel::SetItemIndent(int indent)
{
	m_indent = indent;
	PageTree->setProperty("indentMax", indent);
}


void CMenuPanel::SetItemTextColor(QColor color)
{
	PageTree->setProperty("ItemTextColor", color);
}


void CMenuPanel::SetItemSelectedColor(QColor color)
{
	PageTree->setProperty("ItemSelectedColor", color);
}


void CMenuPanel::SetItemSelectedContourColor(QColor color)
{
	PageTree->setProperty("ItemSelectedContourColor", color);
}


void CMenuPanel::SetItemMouserOverColor(QColor color)
{
	PageTree->setProperty("ItemMouserOverColor", color);
}


void CMenuPanel::SetItemMouserOverSelectedColor(QColor color)
{
	PageTree->setProperty("ItemMouserOverSelectedColor", color);
}

void CMenuPanel::SetMainWidget(QWidget* mainWidget)
{
	m_mainWidget = mainWidget;
	if (m_parentWidget == nullptr){
		m_parentWidget = new QWidget(mainWidget);
		this->setParent(m_parentWidget);
	}
	m_mainWidget->installEventFilter(this);

	if (m_shadowPtr == nullptr){
		m_shadowPtr = new QGraphicsDropShadowEffect(mainWidget);
		m_shadowPtr->setXOffset(0);
		m_shadowPtr->setYOffset(0);
		m_shadowPtr->setBlurRadius(12);
		m_shadowPtr->setEnabled(false);
		//shadowPtr->setColor(qRgba(74, 149, 217, 128));
		this->setGraphicsEffect(m_shadowPtr);
	}
	
	if (m_leftFrame){
		int minWidth = PageTree->iconSize().width() + 4 * m_padding;
		m_leftFrame->setMinimumWidth(minWidth);
		m_leftFrame->setMaximumWidth(minWidth);
	}
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

	PageIdChanged(selectedId, deselectedId);
}


void CMenuPanel::OnAnimationFinished()
{
	if (m_animationAction != AA_NONE && PageTree->currentIndex().isValid()){
		if (m_animationTimerIdentifier != 0){
			killTimer(m_animationTimerIdentifier);
		}
		m_animationTimerIdentifier = startTimer(m_animationDelay);
	}
}


void CMenuPanel::on_pushBottom_clicked()
{
	QModelIndex index = PageTree->indexAt(QPoint(0, PageTree->height()));

	PageTree->scrollTo(index);

	if (index == PageTree->indexAt(QPoint(0, PageTree->height()))){
		index = PageTree->indexBelow(index);

		PageTree->scrollTo(index);
	}

	CheckButtonsVisible();
}


void CMenuPanel::on_pushTop_clicked()
{
	QModelIndex index = PageTree->indexAt(QPoint(0, 0));

	PageTree->scrollTo(index);

	if (index == PageTree->indexAt(QPoint(0, 0))){
		index = PageTree->indexAbove(index);

		PageTree->scrollTo(index);
	}

	CheckButtonsVisible();
}


// protected methods

// reimplemented (QObject)

bool CMenuPanel::eventFilter(QObject* watched, QEvent* event)
{
	int eventType = event->type();

	if (m_mainWidget != nullptr && watched == m_mainWidget){
		if (eventType == QEvent::Resize){
			QRect rect = this->geometry();
			rect.setHeight(m_mainWidget->height());
			this->setGeometry(rect);
		}
		return QObject::eventFilter(watched, event);
	}

	if (watched == PageTree) {
		if (eventType == QEvent::HoverMove) {
			QHoverEvent* hoverEvent = dynamic_cast<QHoverEvent*>(event);
			if (hoverEvent != nullptr) {
				HoverMoveEvent(hoverEvent);
			}
		}
		return QObject::eventFilter(watched, event);
	}

	CheckButtonsVisible();

	return QObject::eventFilter(watched, event);
}


void CMenuPanel::timerEvent(QTimerEvent* /*event*/)
{
	killTimer(m_animationTimerIdentifier);
	m_animationTimerIdentifier = 0;

	StartAnimation();
}


// reimplemented (QWidget)

void CMenuPanel::enterEvent(QEvent* /*event*/)
{
	if (!PageTree->currentIndex().isValid()){
		m_animationAction = AA_EXPAND; 
		return;
	}

	if (m_mainWidget != nullptr){
		if (m_animationWidthComp.state() == QPropertyAnimation::Stopped){
			if (m_animationTimerIdentifier != 0){
				killTimer(m_animationTimerIdentifier);
			}

			m_animationTimerIdentifier = startTimer(m_animationDelay);
		}
	}
	else{
		if (m_animationWidth.state() == QPropertyAnimation::Stopped){
			if (m_animationTimerIdentifier != 0){
				killTimer(m_animationTimerIdentifier);
			}

			m_animationTimerIdentifier = startTimer(m_animationDelay);
		}
	}

	m_animationAction = AA_EXPAND;
}


void CMenuPanel::leaveEvent(QEvent* /*event*/)
{
	if (!PageTree->currentIndex().isValid()){
		m_animationAction = AA_COLLAPSE;
		return;
	}

	if (m_mainWidget != nullptr){
		if (m_animationWidthComp.state() == QPropertyAnimation::Stopped){
			if (m_animationTimerIdentifier != 0){
				killTimer(m_animationTimerIdentifier);
			}

			m_animationTimerIdentifier = startTimer(m_animationDelay);
		}
	}
	else{
		if (m_animationWidth.state() == QPropertyAnimation::Stopped){
			if (m_animationTimerIdentifier != 0){
				killTimer(m_animationTimerIdentifier);
			}

			m_animationTimerIdentifier = startTimer(m_animationDelay);
		}
	}

	m_animationAction = AA_COLLAPSE;
}


void CMenuPanel::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event);

	if (m_mainWidget != nullptr){
		PageTree->setMaximumWidth(this->width());
		
		if (m_parentWidget){
			QRect rect = this->geometry();
			rect.setWidth(this->width() + 5);
			m_parentWidget->setGeometry(rect);
		}
	}

	CheckButtonsVisible();
}

// private methods

void CMenuPanel::HoverMoveEvent(QHoverEvent *event)
{
	if (!PageTree->currentIndex().isValid()) {
		return;
	}

	int dx = event->oldPos().x() - event->pos().x();
	int dy = event->oldPos().y() - event->pos().y();

	if (dx > 2 || dx < -2 || dy > 2 || dy < -2) {
		if (m_mainWidget != nullptr) {
			if (m_animationWidthComp.state() == QPropertyAnimation::Stopped) {
				if (m_animationTimerIdentifier != 0) {
					killTimer(m_animationTimerIdentifier);
				}

				m_animationTimerIdentifier = startTimer(m_animationDelay);
			}
		}
		else {
			if (m_animationWidth.state() == QPropertyAnimation::Stopped) {
				if (m_animationTimerIdentifier != 0) {
					killTimer(m_animationTimerIdentifier);
				}
				m_animationTimerIdentifier = startTimer(m_animationDelay);
			}
		}
	}

}


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

int CMenuPanel::CalculateMinItemWith()
{
	return PageTree->iconSize().width() + 4 * m_padding;
}


void CMenuPanel::SetMinimumPanelWidth(int width)
{
	if (m_mainWidget != nullptr){
		if (m_leftFrame){
			m_leftFrame->setMinimumWidth(width);
			m_leftFrame->setMaximumWidth(width);
		}

		m_animationWidthComp.stop();
		m_animationWidth.stop();

		QRect rect = this->geometry();
		rect.setWidth(width);
		this->setGeometry(rect);
	}
	else{
		PageTree->setMaximumWidth(width);
		PageTree->setMinimumWidth(width);
	}
}


int CMenuPanel::CalculateMaxItemWith()
{
	QStack<QModelIndex> stack;
	QModelIndex index = m_model.index(0, 0);
	int maxWidth = 0;

	QStyleOptionViewItem opt;
	opt.initFrom(this);

	while (index.isValid()){
		QSize itemSize = PageTree->itemDelegate()->sizeHint(opt, index);
		if (maxWidth < itemSize.width()){
			maxWidth = itemSize.width();
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


void CMenuPanel::CheckButtonsVisible()
{
	int position = PageTree->verticalScrollBar()->value();
	if (position == 0){
		pushTop->setVisible(false);
	}
	else{
		pushTop->setVisible(true);
	}

	if (position == PageTree->verticalScrollBar()->maximum()){
		pushBottom->setVisible(false);
	}
	else{
		pushBottom->setVisible(true);
	}
}


void CMenuPanel::StartAnimation()
{
	if (m_animationAction == AA_EXPAND && this->width() != m_maxWidth){
		if (m_mainWidget != nullptr){
			m_animationWidthComp.stop();
			m_animationWidthComp.setStartValue(QRect(0, 0, this->width(), height()));
			m_animationWidthComp.setEndValue(QRect(0, 0, m_maxWidth, height()));
			m_animationWidthComp.setDuration(m_animationDuration);
			m_animationWidthComp.start();
			if (m_shadowPtr){
				m_shadowPtr->setEnabled(true);
			}
		}
		else{
			m_animationWidth.stop();
			m_animationWidth.setStartValue(PageTree->maximumWidth());
			m_animationWidth.setEndValue(m_maxWidth);
			m_animationWidth.setDuration(m_animationDuration);
			m_animationWidth.start();
		}

		m_animationIndent.stop();
		m_animationIndent.setStartValue(PageTree->maximumWidth() * m_indent / m_maxWidth);
		m_animationIndent.setEndValue(m_indent);
		m_animationIndent.setDuration(m_animationDuration - 10);
		m_animationIndent.start();
	}

	int minWidth = PageTree->iconSize().width() + 4 * m_padding;

	if (m_animationAction == AA_COLLAPSE && this->width() != minWidth){
		if (m_mainWidget != nullptr){
			m_animationWidthComp.stop();
			m_animationWidthComp.setStartValue(QRect(0, 0, width(), height()));
			m_animationWidthComp.setEndValue(QRect(0, 0, minWidth, height()));
			m_animationWidthComp.setDuration(m_animationDuration);
			m_animationWidthComp.start();
			if (m_shadowPtr){
				m_shadowPtr->setEnabled(false);
			}
		}
		else{
			m_animationWidth.stop();
			m_animationWidth.setStartValue(PageTree->maximumWidth());
			m_animationWidth.setEndValue(minWidth);
			m_animationWidth.setDuration(m_animationDuration);
			m_animationWidth.start();
		}
		m_animationIndent.stop();
		m_animationIndent.setStartValue(PageTree->maximumWidth() * m_indent / m_maxWidth);
		m_animationIndent.setEndValue(0);
		m_animationIndent.setDuration(m_animationDuration - 10);
		m_animationIndent.start();
	}

	m_animationAction = AA_NONE;
}


} // namespace imtwidgets


