// ImtCore includes
#include <imtwidgets/CMenuPanel.h>
#include <GeneratedFiles/imtwidgets/ui_CMenuPanel.h>

// Qt includes
#include <QtCore/QEvent>
#include <QtCore/QByteArray>
#include <QtCore/QStack>
#include <QtWidgets/QHBoxLayout>

namespace imtwidgets
{

CMenuPanel::CMenuPanel(QWidget *parent) :
    QWidget(parent)
    , ui(new Ui::CMenuPanel)
    , m_activePage(QByteArray())
{
    ui->setupUi(this);

    m_maxWidth = 200;
    m_indent = 30;

    setMouseTracking(true);

    m_modelPtr = new QStandardItemModel(this);
    m_modelPtr->setColumnCount(1);
    m_modelPtr->setSortRole(Qt::UserRole + 100);

    InsertPage("page1", "");
    InsertPage("page2", "");
    InsertPage("page3", "");
    SetPageName("page1", "page 1");
    SetPageName("page2", "page 2");
    SetPageName("page3", "page 3");

    InsertPage("page21", "page2");
    SetPageName("page21", "page 2.1");
    InsertPage("page22", "page2");
    SetPageName("page22", "page 2.2");

    InsertPage("page221", "page22");
    SetPageName("page221", "page 2.2.1");

    SetPageIcon("page1", QIcon(":/Error"));
    SetPageIcon("page2", QIcon(":/Error"));
    SetPageIcon("page3", QIcon(":/Error"));
    SetPageIcon("page21", QIcon(":/Error"));
    SetPageIcon("page22", QIcon(":/Error"));
    SetPageIcon("page221", QIcon(":/Error"));

    ui->treeView->setModel(m_modelPtr);
    ui->treeView->setMinimumWidth(200);
    ui->treeView->setHeaderHidden(true);
    ui->treeView->setIconSize(QSize(24, 24));
    ui->treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    ui->treeView->setIndentation(0);
    ui->treeView->setMinimumWidth(ui->treeView->iconSize().width() + 10);

    m_animationWidthPtr = new QPropertyAnimation(ui->treeView, "minimumWidth", this);
    m_animationIndentPtr = new QPropertyAnimation(ui->treeView, "indentation", this);
}

CMenuPanel::~CMenuPanel()
{

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
    return m_activePage;
}

void CMenuPanel::SetActivePage(const QByteArray &pageId)
{
    m_activePage = pageId;
}

bool CMenuPanel::IsPageEnabled(const QByteArray &pageId) const
{
    QModelIndex index = GetModelIndex(pageId);
    if (index.isValid() == true){
        return m_modelPtr->itemFromIndex(index)->data(IDT_PAGE_ENABLED).toBool();
    }

    return false;
}

bool CMenuPanel::SetPageEnabled(const QByteArray &pageId, bool isPageEnabled)
{
    QModelIndex index = GetModelIndex(pageId);
    if (index.isValid() == true){
        m_modelPtr->itemFromIndex(index)->setData(isPageEnabled, IDT_PAGE_ENABLED);
        return true;
    }

    return false;
}

bool CMenuPanel::IsPageVisible(const QByteArray &pageId) const
{
    QModelIndex index = GetModelIndex(pageId);
    if (index.isValid() == true){
        return m_modelPtr->itemFromIndex(index)->data(IDT_PAGE_VISIBLE).toBool();
    }

    return false;
}

bool CMenuPanel::SetPageVisible(const QByteArray &pageId, bool isPageVisible)
{
    QModelIndex index = GetModelIndex(pageId);
    if (index.isValid() == true){
        m_modelPtr->itemFromIndex(index)->setData(isPageVisible, IDT_PAGE_VISIBLE);
        return true;
    }

    return false;
}

void CMenuPanel::ResetPages()
{
    m_modelPtr->clear();
    m_modelPtr->setColumnCount(1);
}

bool CMenuPanel::IsPageIdExist(const QByteArray &pageId) const
{
    QModelIndex index = GetModelIndex(pageId);
    return index.isValid();
}

bool CMenuPanel::RemovePage(const QByteArray &pageId)
{
    QModelIndex index = GetModelIndex(pageId);
    if (index.isValid() == true){
        return m_modelPtr->removeRow(index.row(), index.parent());
    }

    return false;
}

bool CMenuPanel::InsertPage(const QByteArray &pageId, const QByteArray &parentPageId)
{
    if (pageId.isEmpty() == true){
        return false;
    }

    if (IsPageIdExist(pageId) == true){
        return false;
    }

    if (parentPageId.isEmpty()){
        int row = m_modelPtr->rowCount(QModelIndex());

        QStandardItem *itemPtr = new QStandardItem();
        m_modelPtr->insertRow(row, itemPtr);

        QModelIndex modelIndex = m_modelPtr->index(row, 0, QModelIndex());
        m_modelPtr->setData(modelIndex, pageId, IDT_PAGE_ID);

        return true;
    }

    QModelIndex parentModelIndex = GetModelIndex(parentPageId);
    if (parentModelIndex.isValid()) {
        int row = m_modelPtr->rowCount(parentModelIndex);

        QStandardItem *itemPtr = new QStandardItem();
        m_modelPtr->itemFromIndex(parentModelIndex)->insertRow(row,itemPtr);

        QModelIndex modelIndex = m_modelPtr->index(row, 0, parentModelIndex);
        m_modelPtr->setData(modelIndex, pageId, Qt::UserRole);

        return true;
    }

    return false;
}

int CMenuPanel::GetPageOrder(const QByteArray &pageId) const
{
    QModelIndex index = GetModelIndex(pageId);
    if (index.isValid() == true){
        return index.row();
    }

    return -1;
}

bool CMenuPanel::SetPageOrder(const QByteArray &pageId, int position)
{
    QModelIndex index = GetModelIndex(pageId);
    if (index.isValid() == true){
        QStandardItem *item = m_modelPtr->itemFromIndex(index.parent());
        int row;
        if (item == nullptr){
            row = m_modelPtr->rowCount()-1;
        } else
        {
            row = item->rowCount()-1;
        }

        if (position < row){
            QModelIndex parent = index.parent();
            QStandardItem *oldItem = m_modelPtr->itemFromIndex(index);
            QStandardItem *newItem = new QStandardItem();
            newItem->setText(oldItem->text());
            newItem->setIcon(oldItem->icon());
            newItem->setData(oldItem->data((IDT_PAGE_ID)));

            m_modelPtr->removeRow(index.row(), parent);

            if (parent.isValid()){
                m_modelPtr->insertRow(position, newItem);
            } else{

            }
        }
    }

    return false;
}

QList<QByteArray> CMenuPanel::GetChilds(const QByteArray &pageId)
{
	QList<QByteArray> childs;

	if (pageId.isEmpty() == true){
		for (int i = 0; i < m_modelPtr->rowCount(); i++){
			childs.append(m_modelPtr->item(i, 0)->data(IDT_PAGE_ID).toByteArray());
		}
	}

	QModelIndex index = GetModelIndex(pageId);
	if (index.isValid() == true){
		QStandardItem *itemPtr = m_modelPtr->itemFromIndex(index);

		for (int i = 0; i < itemPtr->rowCount(); i++){
			childs.append(itemPtr->child(i)->data(IDT_PAGE_ID).toByteArray());
		}
	}

	return childs;
}

QIcon CMenuPanel::GetPageIcon(const QByteArray &pageId) const
{
    QModelIndex index = GetModelIndex(pageId);
    if (index.isValid() == true){
        return m_modelPtr->itemFromIndex(index)->icon();
    }

    return QIcon();
}

bool CMenuPanel::SetPageIcon(const QByteArray &pageId, const QIcon &pageIcon)
{
    QModelIndex index = GetModelIndex(pageId);
    if (index.isValid() == true){
        m_modelPtr->itemFromIndex(index)->setIcon(pageIcon);
        return true;
    }

    return false;
}

QString CMenuPanel::GetPageName(const QByteArray &pageId) const
{
    QModelIndex index = GetModelIndex(pageId);
    if (index.isValid() == true){
        return m_modelPtr->itemFromIndex(index)->text();
    }

    return QString();
}

bool CMenuPanel::SetPageName(const QByteArray &pageId, const QString &pageName)
{
    QModelIndex index = GetModelIndex(pageId);
    if (index.isValid() == true){
        m_modelPtr->itemFromIndex(index)->setText(pageName);
        return true;
    }

    return false;
}

void CMenuPanel::OnPageIdChanged(const QByteArray &pageId)
{
	Q_UNUSED(pageId)
}

void CMenuPanel::enterEvent(QEvent *event)
{
    Q_UNUSED(event)

    m_animationWidthPtr->setStartValue(ui->treeView->minimumWidth());
    m_animationWidthPtr->setEndValue(m_maxWidth);
    m_animationWidthPtr->setDuration(150);
    m_animationWidthPtr->start();

    m_animationIndentPtr->setStartValue(ui->treeView->minimumWidth() * m_indent/ m_maxWidth);
    m_animationIndentPtr->setEndValue(m_indent);
    m_animationIndentPtr->setDuration(150);
    m_animationIndentPtr->start();
}

void CMenuPanel::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)

    m_animationWidthPtr->setStartValue(ui->treeView->minimumWidth());
    m_animationWidthPtr->setEndValue(ui->treeView->iconSize().width() + 10);
    m_animationWidthPtr->setDuration(150);
    m_animationWidthPtr->start();

    m_animationIndentPtr->setStartValue(ui->treeView->minimumWidth() * m_indent / m_maxWidth);
    m_animationIndentPtr->setEndValue(0);
    m_animationIndentPtr->setDuration(150);
    m_animationIndentPtr->start();
}

QModelIndex CMenuPanel::GetModelIndex(const QByteArray &pageId) const
{
    QStack<QModelIndex> stack;

    QModelIndex index = m_modelPtr->index(0, 0);

    while (index.isValid() == true){
        QVariant itemData = index.data(IDT_PAGE_ID);
        if (itemData.isValid()){
            if (itemData.toByteArray() == pageId){
                return index;
            }
        }

        QModelIndex childIndex = m_modelPtr->index(0, 0, index);
        if (childIndex.isValid() == true){
            QModelIndex sibling = index.siblingAtRow(index.row() + 1);
            if (sibling.isValid() == true){
                stack.push(sibling);
            }

            index = m_modelPtr->index(0, 0, index);
            continue;
        }

        QModelIndex siblingIndex = index.siblingAtRow(index.row() + 1);
        if (siblingIndex.isValid()){
            index = siblingIndex;
            continue;
        }

        if (stack.isEmpty() == true){
            break;
        }

        index = stack.pop();
    }

    return QModelIndex();
}

}
