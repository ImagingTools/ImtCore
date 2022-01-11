#include <imtwidgets/CMenuPanel.h>
#include <math.h>
#include <QDebug>


// Qt includes
#include <QtCore/QEvent>
#include <QtCore/QStack>
#include <QtCore/QModelIndex>
#include <QtGui/QStandardItem>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QScrollBar>


namespace imtwidgets
{


// public methods

CMenuPanel::CMenuPanel(QWidget* parent)
	:QWidget(parent),
	m_maxWidth(0),
	m_minWidth(0),
	m_indent(0),
	m_verticalPadding(0),
	m_animationAction(AA_NONE),
	m_animationTimerIdentifier(0),
	m_animationDelay(0),
	m_animationDuration(0),
	m_animationEnabled(true),
	m_mainWidgetPtr(nullptr),
	m_leftFramePtr(parent),
	m_parentWidgetPtr(nullptr),
	m_delegatePtr(nullptr),
	m_shadowPtr(nullptr)
{
	setupUi(this);

	//===========================================Top setup

	m_model.setColumnCount(1);
	m_model.setSortRole(Qt::UserRole + 100);

	setMouseTracking(true);
	PageTree->installEventFilter(this);

	PageTree->setModel(&m_model);
	PageTree->setSelectionMode(QAbstractItemView::SingleSelection);
	PageTree->selectionModel()->clearSelection();
	connect(PageTree->selectionModel(), &QItemSelectionModel::currentChanged, this, &CMenuPanel::OnPageIdChanged);

	PageTree->setHeaderHidden(true);
	PageTree->setIconSize(QSize(16, 16));
	SetItemHeight(16);
	PageTree->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	PageTree->setIndentation(0);

	PageTree->verticalScrollBar()->installEventFilter(this);

	PageTree->setContentsMargins(QMargins(0,0,0,0));
	PageTree->setMaximumWidth(m_minWidth);
	PageTree->setItemsExpandable(false);


	//===========================================Bottom setup


	m_bottomModel.setColumnCount(1);
	m_bottomModel.setSortRole(Qt::UserRole + 100);

	setMouseTracking(true);
	BottomPageTree->installEventFilter(this);

	BottomPageTree->setModel(&m_bottomModel);
	BottomPageTree->setSelectionMode(QAbstractItemView::SingleSelection);
	BottomPageTree->selectionModel()->clearSelection();
	connect(BottomPageTree->selectionModel(), &QItemSelectionModel::currentChanged, this, &CMenuPanel::OnPageIdChanged);

	BottomPageTree->setHeaderHidden(true);
	BottomPageTree->setIconSize(QSize(16, 16));

	BottomPageTree->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	BottomPageTree->setIndentation(0);

	BottomPageTree->verticalScrollBar()->installEventFilter(this);

	BottomPageTree->setContentsMargins(QMargins(0,0,0,0));
	BottomPageTree->setMaximumWidth(m_minWidth);
	BottomPageTree->setItemsExpandable(false);
	BottomPageTree->setVisible(false);


	//===========================================

	m_animationWidth.setTargetObject(PageTree);
	m_animationWidth.setPropertyName("maximumWidth");
	m_animationIndent.setPropertyName("indent");
	connect(&m_animationWidth, &QPropertyAnimation::finished, this, &CMenuPanel::OnAnimationFinished);

	pushTop->setIcon(QIcon(":/Icons/Up"));
	pushTop->setAutoRepeat(true);
	pushBottom->setIcon(QIcon(":/Icons/Down"));
	pushBottom->setAutoRepeat(true);

	SetMinimumPanelWidth(0);
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
	const QStandardItemModel* activeModelPtr = &m_model;

	QModelIndex index;
	if (!pageId.isEmpty()){
		index = GetModelIndex(pageId, &activeModelPtr);
	}

	QTreeView* activePageTree = activeModelPtr == &m_model ? PageTree : BottomPageTree;

	QModelIndex prevIndex = PageTree->currentIndex().isValid() ? PageTree->currentIndex() : BottomPageTree->currentIndex();

	PageTree->selectionModel()->clear();
	BottomPageTree->selectionModel()->clear();
	activePageTree->setCurrentIndex(index);

	if (!prevIndex.isValid() && index.isValid()){
		StopTimer();
		m_animationAction = AA_COLLAPSE;
		StartAnimation();
	}

	PageTree->viewport()->update();
	BottomPageTree->viewport()->update();
}


bool CMenuPanel::IsPageEnabled(const QByteArray& pageId) const
{
	const QStandardItemModel* activeModelPtr = &m_model;
	QModelIndex index = GetModelIndex(pageId, &activeModelPtr);
	if (index.isValid()){
		return activeModelPtr->itemFromIndex(index)->data(DR_PAGE_ENABLED).toBool();
	}

	return false;
}


bool CMenuPanel::SetPageEnabled(const QByteArray& pageId, bool isPageEnabled)
{
	const QStandardItemModel* activeModelPtr = &m_model;
	QModelIndex index = GetModelIndex(pageId, &activeModelPtr);
	if (index.isValid()){
		activeModelPtr->itemFromIndex(index)->setData(isPageEnabled, DR_PAGE_ENABLED);
		activeModelPtr->itemFromIndex(index)->setEnabled(isPageEnabled);
		return true;
	}

	return false;
}


bool CMenuPanel::IsPageVisible(const QByteArray& pageId) const
{
	const QStandardItemModel* activeModelPtr = &m_model;
	QModelIndex index = GetModelIndex(pageId, &activeModelPtr);
	if (index.isValid()){
		return activeModelPtr->itemFromIndex(index)->data(DR_PAGE_VISIBLE).toBool();
	}

	return false;
}


bool CMenuPanel::SetPageVisible(const QByteArray& pageId, bool isPageVisible)
{
	const QStandardItemModel* activeModelPtr = &m_model;
	QModelIndex index = GetModelIndex(pageId, &activeModelPtr);
	if (index.isValid()){
		activeModelPtr->itemFromIndex(index)->setData(isPageVisible, DR_PAGE_VISIBLE);

		return true;
	}

	return false;
}


void CMenuPanel::ResetPages()
{
	m_model.clear();
	m_model.setColumnCount(1);

	m_bottomModel.clear();
	m_bottomModel.setColumnCount(1);
}


bool CMenuPanel::IsPageIdExist(const QByteArray& pageId) const
{
	const QStandardItemModel* activeModelPtr = &m_model;
	QModelIndex index = GetModelIndex(pageId, &activeModelPtr);
	return index.isValid();
}


bool CMenuPanel::RemovePage(const QByteArray& pageId)
{
	const QStandardItemModel* activeModelPtr = &m_model;
	QModelIndex index = GetModelIndex(pageId, &activeModelPtr);
	if (index.isValid()){
		return const_cast<QStandardItemModel*>(activeModelPtr)->removeRow(index.row(), index.parent());
	}

	return false;
}


bool CMenuPanel::InsertPage(const QByteArray& pageId, const QByteArray& parentPageId, bool insertToBottom)
{
	QTreeView* activeTreePtr = insertToBottom ? BottomPageTree : PageTree;
	QStandardItemModel& activeModelPtr = insertToBottom ? m_bottomModel : m_model;


	if (activeTreePtr == BottomPageTree){
		activeTreePtr->setVisible(true);
	}

	if (pageId.isEmpty()){
		return false;
	}

	if (IsPageIdExist(pageId)){
		return false;
	}

	if (parentPageId.isEmpty()){
		int row = activeModelPtr.rowCount(QModelIndex());

		QStandardItem* itemPtr = new QStandardItem();
		activeModelPtr.insertRow(row, itemPtr);

		QModelIndex modelIndex = activeModelPtr.index(row, 0, QModelIndex());
		activeModelPtr.setData(modelIndex, pageId, DR_PAGE_ID);

		activeTreePtr->expandAll();

		m_maxWidth = CalculateMaxItemWith();

		return true;
	}

	QModelIndex parentModelIndex = GetModelIndexFromModel(parentPageId, activeModelPtr);
	if (parentModelIndex.isValid()){
		int row = activeModelPtr.rowCount(parentModelIndex);

		QStandardItem* itemPtr = new QStandardItem();
		activeModelPtr.itemFromIndex(parentModelIndex)->setData(this->width() == m_minWidth, DR_PAGE_HIDDEN_WHILE_COLLAPSED);
		activeModelPtr.itemFromIndex(parentModelIndex)->insertRow(row,itemPtr);

		QModelIndex modelIndex = activeModelPtr.index(row, 0, parentModelIndex);
		activeModelPtr.setData(modelIndex, pageId, DR_PAGE_ID);

		activeTreePtr->expandAll();

		m_maxWidth = CalculateMaxItemWith();

		return true;
	}

	return false;
}


int CMenuPanel::GetPageOrder(const QByteArray& pageId) const
{
	const QStandardItemModel* activeModelPtr = &m_model;
	QModelIndex index = GetModelIndex(pageId, &activeModelPtr);
	if (index.isValid()){
		return index.row();
	}

	return -1;
}


bool CMenuPanel::SetPageOrder(const QByteArray& pageId, int position)
{
	const QStandardItemModel* activeModelPtr = &m_model;
	QModelIndex index = GetModelIndex(pageId, &activeModelPtr);
	if (index.isValid()){
		QStandardItem* item = activeModelPtr->itemFromIndex(index.parent());
		int row;
		if (item == nullptr){
			row = activeModelPtr->rowCount()-1;
		}
		else{
			row = item->rowCount()-1;
		}

		if (position < row){
			QModelIndex parent = index.parent();
			QStandardItem* oldItem = activeModelPtr->itemFromIndex(index);
			QStandardItem* newItem = new QStandardItem();
			newItem->setText(oldItem->text());
			newItem->setIcon(oldItem->icon());
			newItem->setData(oldItem->data((DR_PAGE_ID)));

			const_cast<QStandardItemModel*>(activeModelPtr)->removeRow(index.row(), parent);

			if (parent.isValid()){
				const_cast<QStandardItemModel*>(activeModelPtr)->insertRow(position, newItem);
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

	const QStandardItemModel* activeModelPtr = &m_model;
	QModelIndex index = GetModelIndex(pageId, &activeModelPtr);
	if (index.isValid()){
		QStandardItem* itemPtr = activeModelPtr->itemFromIndex(index);
		for (int i = 0; i < itemPtr->rowCount(); i++){
			childs.append(itemPtr->child(i)->data(DR_PAGE_ID).toByteArray());
		}
	}

	return childs;
}


QIcon CMenuPanel::GetPageIcon(const QByteArray& pageId) const
{
	const QStandardItemModel* activeModelPtr = &m_model;
	QModelIndex index = GetModelIndex(pageId, &activeModelPtr);
	if (index.isValid()){
		return activeModelPtr->itemFromIndex(index)->icon();
	}

	return QIcon();
}


bool CMenuPanel::SetPageIcon(const QByteArray& pageId, const QIcon& pageIcon)
{
	const QStandardItemModel* activeModelPtr = &m_model;
	QModelIndex index = GetModelIndex(pageId, &activeModelPtr);
	if (index.isValid()){
		activeModelPtr->itemFromIndex(index)->setIcon(pageIcon);
		return true;
	}

	return false;
}


QString CMenuPanel::GetPageName(const QByteArray& pageId) const
{
	const QStandardItemModel* activeModelPtr = &m_model;
	QModelIndex index = GetModelIndex(pageId, &activeModelPtr);
	if (index.isValid()){
		return activeModelPtr->itemFromIndex(index)->text();
	}

	return QString();
}


bool CMenuPanel::SetPageName(const QByteArray& pageId, const QString& pageName)
{
	const QStandardItemModel* activeModelPtr = &m_model;
	QModelIndex index = GetModelIndex(pageId, &activeModelPtr);
	if (index.isValid()){
		activeModelPtr->itemFromIndex(index)->setText(pageName);
		m_maxWidth = CalculateMaxItemWith();
		return true;
	}

	return false;
}


void CMenuPanel::SetItemIndent(int indent)
{
	m_indent = indent;
	if (m_delegatePtr != nullptr){
		m_delegatePtr->SetIndent(indent);
	}
	AfterSizesChanged();
}


void CMenuPanel::SetItemHeight(int height)
{
	m_cachedItemHeight = height;
	if (m_delegatePtr != nullptr){
		m_delegatePtr->SetItemHeight(height);
	}
	UpdateFontSize();
	AfterSizesChanged();
}


void CMenuPanel::SetIconSizeRatio(double ratio)
{
	m_cachedIconSizeRatio = ratio;
	if (m_delegatePtr != nullptr){
		m_delegatePtr->SetIconSizeRatio(ratio);
	}
	UpdateFontSize();
	AfterSizesChanged();
}


void CMenuPanel::SetIconSizeHoverRatio(double ratio)
{
	if (m_delegatePtr != nullptr){
		m_delegatePtr->SetIconSizeHoverRatio(ratio);
	}
}


void CMenuPanel::SetFontSizeRatio(double ratio)
{
	m_cachedFontSizeRatio = ratio;	
	UpdateFontSize();
	AfterSizesChanged();
}


void CMenuPanel::SetItemVerticalPadding(int padding)
{
	m_verticalPadding = padding;
	if (m_delegatePtr != nullptr){
		m_delegatePtr->SetTopPadding(padding);
	}
}


void CMenuPanel::SetItemLeftPadding(int padding)
{
	if (m_delegatePtr != nullptr){
		m_delegatePtr->SetLeftPadding(padding);
	}
	AfterSizesChanged(); 
}


void CMenuPanel::SetItemRightPadding(int padding)
{
	if (m_delegatePtr != nullptr){
		m_delegatePtr->SetRightPadding(padding);
	}
	AfterSizesChanged(); 
}


void CMenuPanel::SetItemIconToTextPadding(int padding)
{
	if (m_delegatePtr != nullptr){
		m_delegatePtr->SetIconToTextPadding(padding);
	}
	AfterSizesChanged();
}


void CMenuPanel::SetAnimationDelay(int delay)
{
	m_animationDelay = delay;
}


void CMenuPanel::SetAnimationDuration(int duration)
{
	m_animationDuration = duration;
}


void CMenuPanel::SetAnimationEnabled(int animationEnabled)
{
	m_animationEnabled = animationEnabled;
}


void CMenuPanel::SetMainWidget(QWidget* mainWidget)
{
	m_mainWidgetPtr = mainWidget;
	if (m_parentWidgetPtr == nullptr){
		m_parentWidgetPtr = new QWidget(mainWidget);
		this->setParent(m_parentWidgetPtr);
	}
	m_mainWidgetPtr->installEventFilter(this);

	m_animationWidth.setTargetObject(this);
	m_animationWidth.setPropertyName("geometry");

	if (m_shadowPtr == nullptr){
		m_shadowPtr = new QGraphicsDropShadowEffect(mainWidget);
		m_shadowPtr->setXOffset(0);
		m_shadowPtr->setYOffset(0);
		m_shadowPtr->setBlurRadius(12);
		m_shadowPtr->setEnabled(false);
		//shadowPtr->setColor(qRgba(74, 149, 217, 128));
		this->setGraphicsEffect(m_shadowPtr);
	}
	
	SetMinimumPanelWidth(0);
}


void CMenuPanel::CollapsePanelImmideatly()
{
	StopTimer();
	StopAnimation();

	if (m_mainWidgetPtr != nullptr){
		setGeometry(QRect(0, 0, m_minWidth, height()));

		if (m_shadowPtr){
			m_shadowPtr->setEnabled(false);
		}
	}
	else{
		PageTree->setMaximumWidth(m_minWidth);
		BottomPageTree->setMaximumWidth(m_minWidth);
	}

	for (int i = 0; i < m_model.rowCount(); i++){
		if (m_model.item(i)->hasChildren()){
			m_model.item(i)->setData(true, DR_PAGE_HIDDEN_WHILE_COLLAPSED);
		}
	}

	for (int i = 0; i < m_bottomModel.rowCount(); i++){
		if (m_bottomModel.item(i)->hasChildren()){
			m_bottomModel.item(i)->setData(true, DR_PAGE_HIDDEN_WHILE_COLLAPSED);
		}
	}

	if (m_delegatePtr != nullptr){
		m_delegatePtr->setProperty("indent", 0);
	}
}


void CMenuPanel::UpdateFontSize()
{
	QFont font = PageTree->font();
	font.setPixelSize(m_cachedItemHeight * m_cachedIconSizeRatio * m_cachedFontSizeRatio);
	PageTree->setFont(font);
	BottomPageTree->setFont(font);
	if (m_delegatePtr != nullptr){
		m_delegatePtr->SetFontMetrics(PageTree->fontMetrics());
	}
}


void CMenuPanel::SetDelegate(IMenuPanelDelegate* menuPanelDelegate)
{
	m_delegatePtr = menuPanelDelegate;
	m_delegatePtr->SetFontMetrics(PageTree->fontMetrics());
	PageTree->setItemDelegate(m_delegatePtr);
	BottomPageTree->setItemDelegate(m_delegatePtr);
	m_animationIndent.setTargetObject(m_delegatePtr);
}


void CMenuPanel::OnPageIdChanged(const QModelIndex& selected, const QModelIndex& deselected)
{
	QTreeView* activeTreeView = selected.model() != &m_model ? PageTree : BottomPageTree;
	activeTreeView->selectionModel()->clearSelection();
	activeTreeView->selectionModel()->clearCurrentIndex();


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
	if (this->width() == m_minWidth){
		for (int i = 0; i < m_model.rowCount(); i++){
			if (m_model.item(i)->hasChildren()){
				m_model.item(i)->setData(true, DR_PAGE_HIDDEN_WHILE_COLLAPSED);
			}
		}

		for (int i = 0; i < m_bottomModel.rowCount(); i++){
			if (m_bottomModel.item(i)->hasChildren()){
				m_bottomModel.item(i)->setData(true, DR_PAGE_HIDDEN_WHILE_COLLAPSED);
			}
		}
	}

	if (m_animationAction != AA_NONE && PageTree->currentIndex().isValid()){
		StartTimer();
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

	if (m_mainWidgetPtr != nullptr && watched == m_mainWidgetPtr){
		if (eventType == QEvent::Resize){
			QRect rect = this->geometry();
			rect.setHeight(m_mainWidgetPtr->height());
			this->setGeometry(rect);
			CheckButtonsVisible();
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

		switch (eventType){
		case QEvent::Show:
		case QEvent::Hide:
		case QEvent:: ChildAdded:
		case QEvent:: ChildPolished:
		case QEvent:: ChildRemoved:
			CheckButtonsVisible();
		break;

		default:
		break;
		}

		return QObject::eventFilter(watched, event);
	}

	if (watched == BottomPageTree) {
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
	StopTimer();

	if (m_animationAction != AA_NONE){
		StartAnimation();
	}
}


// reimplemented (QWidget)

void CMenuPanel::enterEvent(QEvent* /*event*/)
{
	if (m_animationEnabled == false)
		return;
	if (!PageTree->currentIndex().isValid()){
		m_animationAction = AA_EXPAND; 
		return;
	}

	if (m_animationWidth.state() == QPropertyAnimation::Stopped){
		StartTimer();
	}

	m_animationAction = AA_EXPAND;
}


void CMenuPanel::leaveEvent(QEvent* /*event*/)
{
	if (m_animationEnabled == false)
		return;
	if (!PageTree->currentIndex().isValid()){
		m_animationAction = AA_COLLAPSE;
		return;
	}

	if (m_animationWidth.state() == QPropertyAnimation::Stopped){
		StartTimer(100);
	}

	m_animationAction = AA_COLLAPSE;
}


void CMenuPanel::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event);

	if (m_mainWidgetPtr != nullptr){
		PageTree->setMaximumWidth(this->width());
		PageTree->setMinimumWidth(this->width());

		BottomPageTree->setMaximumWidth(this->width());
		BottomPageTree->setMinimumWidth(this->width());

		if (m_parentWidgetPtr){
			QRect rect = this->geometry();
			rect.setWidth(this->width() + 5);
			m_parentWidgetPtr->setGeometry(rect);
		}
	}

	CheckButtonsVisible();
}

// private methods

void CMenuPanel::HoverMoveEvent(QHoverEvent* event)
{
	if (!PageTree->currentIndex().isValid()) {
		return;
	}

	int dx = event->oldPos().x() - event->pos().x();
	int dy = event->oldPos().y() - event->pos().y();

	if (dx > 2 || dx < -2 || dy > 2 || dy < -2) {
		if (m_animationWidth.state() == QPropertyAnimation::Stopped) {
			StartTimer();
		}
	}
}


QModelIndex CMenuPanel::GetModelIndex(const QByteArray& pageId, const QStandardItemModel** modelPtr) const
{
	QModelIndex retval = QModelIndex();

	*modelPtr = &m_model;
	retval = GetModelIndexFromModel(pageId, **modelPtr);

	if (!retval.isValid()){

		*modelPtr = &m_bottomModel;
		retval = GetModelIndexFromModel(pageId, **modelPtr);

	}

	return retval;
}

QModelIndex CMenuPanel::GetModelIndexFromModel(const QByteArray& pageId, const QStandardItemModel& model) const
{
	QStack<QModelIndex> stack;
	QModelIndex index = model.index(0, 0);
	while (index.isValid()){
		QVariant itemData = index.data(DR_PAGE_ID);
		if (itemData.isValid()){
			if (itemData.toByteArray() == pageId){
				return index;
			}
		}

		QModelIndex childIndex = model.index(0, 0, index);
		if (childIndex.isValid()){
			QModelIndex sibling = index.siblingAtRow(index.row() + 1);
			if (sibling.isValid()){
				stack.push(sibling);
			}

			index = model.index(0, 0, index);

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


void CMenuPanel::SetMinimumPanelWidth(int width)
{
	if (m_mainWidgetPtr != nullptr){
		if (m_leftFramePtr){
			m_leftFramePtr->setMinimumWidth(width);
			m_leftFramePtr->setMaximumWidth(width);
		}

		QRect rect = this->geometry();
		rect.setWidth(width);
		this->setGeometry(rect);
	}
	else{
		PageTree->setMaximumWidth(width);
		PageTree->setMinimumWidth(width);
		BottomPageTree->setMaximumWidth(width);
		BottomPageTree->setMinimumWidth(width);
	}

	StartAnimation();
}


int CMenuPanel::CalculateMaxItemWith()
{
	int maxWidth = std::max(1,
				std::max(this->CalculateMaxItemWithByModel(m_model),
				this->CalculateMaxItemWithByModel(m_bottomModel)
				)
				);

	return maxWidth;
}


int CMenuPanel::CalculateMaxItemWithByModel(const QStandardItemModel& model)
{
	QStack<QModelIndex> stack;
	QModelIndex index = model.index(0, 0);
	int maxWidth = 1;

	QStyleOptionViewItem opt;
	opt.initFrom(this);

	while (index.isValid()){
		QSize itemSize = PageTree->itemDelegate()->sizeHint(opt, index);
		if (maxWidth < itemSize.width()){
			maxWidth = itemSize.width();
		}

		QModelIndex childIndex = model.index(0, 0, index);
		if (childIndex.isValid()){
			QModelIndex sibling = index.siblingAtRow(index.row() + 1);
			if (sibling.isValid()){
				stack.push(sibling);
			}

			index = model.index(0, 0, index);

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


void CMenuPanel::StartTimer(int time)
{
	StopTimer();

	m_animationTimerIdentifier = startTimer(time < 0 ? m_animationDelay : time);
}


void CMenuPanel::StopTimer()
{
	if (m_animationTimerIdentifier != 0){
		killTimer(m_animationTimerIdentifier);
		m_animationTimerIdentifier = 0;
	}
}


void CMenuPanel::StartAnimation()
{
	StopTimer();
	StopAnimation();

	if (m_animationAction == AA_EXPAND && this->width() != m_maxWidth){
		if (m_mainWidgetPtr != nullptr){
			m_animationWidth.setStartValue(QRect(0, 0, this->width(), height()));
			m_animationWidth.setEndValue(QRect(0, 0, m_maxWidth, height()));
			if (m_shadowPtr){
				m_shadowPtr->setEnabled(true);
			}
		}
		else{
			m_animationWidth.setStartValue(PageTree->maximumWidth());
			m_animationWidth.setEndValue(m_maxWidth);
		}
		m_animationWidth.setDuration(m_animationDuration);
		m_animationWidth.start();

		m_animationIndent.setStartValue((PageTree->maximumWidth() - m_minWidth) * m_indent / m_maxWidth);
		m_animationIndent.setEndValue(m_indent);
		m_animationIndent.setDuration(m_animationDuration - 10);
		m_animationIndent.start();

		for (int i = 0; i < m_model.rowCount(); i++){
			if (m_model.item(i)->hasChildren()){
				m_model.item(i)->setData(false, DR_PAGE_HIDDEN_WHILE_COLLAPSED);
			}
		}
	}
	
	if (m_animationAction == AA_COLLAPSE && this->width() != m_minWidth){
		if (m_mainWidgetPtr != nullptr){
			m_animationWidth.setStartValue(QRect(0, 0, width(), height()));
			m_animationWidth.setEndValue(QRect(0, 0, m_minWidth, height()));
			if (m_shadowPtr){
				m_shadowPtr->setEnabled(false);
			}
		}
		else{
			m_animationWidth.setStartValue(PageTree->maximumWidth());
			m_animationWidth.setEndValue(m_minWidth);
		}
		m_animationWidth.setDuration(m_animationDuration);
		m_animationWidth.start();

		m_animationIndent.setStartValue(PageTree->maximumWidth() * m_indent / m_maxWidth);
		m_animationIndent.setEndValue(0);
		m_animationIndent.setDuration(m_animationDuration - 10);
		m_animationIndent.start();
	}

	m_animationAction = AA_NONE;
}


void CMenuPanel::StopAnimation()
{
	m_animationWidth.stop();
	m_animationIndent.stop();
}


void CMenuPanel::ReconnectModel()
{
	disconnect(PageTree->selectionModel(), &QItemSelectionModel::currentChanged, this, &CMenuPanel::OnPageIdChanged);
	PageTree->setModel(nullptr);
	PageTree->setModel(&m_model);
	connect(PageTree->selectionModel(), &QItemSelectionModel::currentChanged, this, &CMenuPanel::OnPageIdChanged);

	disconnect(BottomPageTree->selectionModel(), &QItemSelectionModel::currentChanged, this, &CMenuPanel::OnPageIdChanged);
	BottomPageTree->setModel(nullptr);
	BottomPageTree->setModel(&m_bottomModel);
	connect(BottomPageTree->selectionModel(), &QItemSelectionModel::currentChanged, this, &CMenuPanel::OnPageIdChanged);
}


void CMenuPanel::AfterSizesChanged()
{
	if (m_delegatePtr != nullptr){
		m_minWidth = m_delegatePtr->GetMinimumWidth();
	}
	SetMinimumPanelWidth(m_minWidth);
	m_maxWidth = CalculateMaxItemWith();

	ReconnectModel();
	PageTree->expandAll();

	m_animationAction = AA_COLLAPSE;
	if (PageTree->currentIndex().isValid()){
		m_animationAction = AA_EXPAND;
	}

	if (m_delegatePtr != nullptr && BottomPageTree->model()->rowCount() > 0){
		QStyleOptionViewItem options;
		QSize size = m_delegatePtr->sizeHint(options, QModelIndex());
		if (size.height() > 0){
			int bottomPageHeight = size.height() * BottomPageTree->model()->rowCount() + m_verticalPadding + 10;
			BottomPageTree->setMaximumHeight(bottomPageHeight);
		}
	}

	StartAnimation();
}


} // namespace imtwidgets


