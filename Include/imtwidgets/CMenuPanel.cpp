#include "CMenuPanel.h"

// Qt includes
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QEvent>

namespace imtwidgets
{

CMenuPanel::CMenuPanel(QWidget *parent) :
    QWidget(parent)
{
    m_minWidth = 32;
    m_maxWidth = 200;
    m_indent = 30;

    setMouseTracking(true);

    QHBoxLayout *layoutPtr = new QHBoxLayout(this);
    setLayout(layoutPtr);
    layoutPtr->setMargin(0);

    m_modelPtr = new QStandardItemModel(this);
    m_modelPtr->setColumnCount(1);

    QStandardItem *itemWithChild = nullptr;
    for(int i = 0; i < 5; i++){
        QStandardItem *pItem = new QStandardItem();
        pItem->setText(QString("parent %1").arg(i));
        pItem->setIcon(QIcon(":/Icons/Error"));
        m_modelPtr->insertRow(m_modelPtr->rowCount(), pItem);
        if (i == 2){
            itemWithChild = pItem;
        }
    }

    for(int i = 0; i < 5; i++){
        QStandardItem *pItem = new QStandardItem();
        pItem->setText(QString("child %1").arg(i));
        pItem->setIcon(QIcon(":/Error"));
        itemWithChild->insertRow(itemWithChild->rowCount(), pItem);
    }

    m_treeViewPtr = new QTreeView(this);
    m_treeViewPtr->setModel(m_modelPtr);
    m_treeViewPtr->setMinimumWidth(200);
    m_treeViewPtr->setHeaderHidden(true);
    m_treeViewPtr->setIconSize(QSize(24, 24));
    m_treeViewPtr->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    m_treeViewPtr->setIndentation(0);
    m_treeViewPtr->setMinimumWidth(m_minWidth);

    layoutPtr->insertWidget(0, m_treeViewPtr, 1);

    m_animationWidthPtr = new QPropertyAnimation(m_treeViewPtr, "minimumWidth", this);
    m_animationIndentPtr = new QPropertyAnimation(m_treeViewPtr, "indentation", this);
}

CMenuPanel::~CMenuPanel()
{
}

void CMenuPanel::enterEvent(QEvent *event)
{
    Q_UNUSED(event)

    m_animationWidthPtr->setStartValue(m_treeViewPtr->minimumWidth());
    m_animationWidthPtr->setEndValue(m_maxWidth);
    m_animationWidthPtr->setDuration(150);
    m_animationWidthPtr->start();

    m_animationIndentPtr->setStartValue(m_treeViewPtr->minimumWidth() * m_indent/ m_maxWidth);
    m_animationIndentPtr->setEndValue(m_indent);
    m_animationIndentPtr->setDuration(150);
    m_animationIndentPtr->start();
}

void CMenuPanel::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)

    m_animationWidthPtr->setStartValue(m_treeViewPtr->minimumWidth());
    m_animationWidthPtr->setEndValue(24 + 8);
    m_animationWidthPtr->setDuration(150);
    m_animationWidthPtr->start();

    m_animationIndentPtr->setStartValue(m_treeViewPtr->minimumWidth() * m_indent / m_maxWidth);
    m_animationIndentPtr->setEndValue(0);
    m_animationIndentPtr->setDuration(150);
    m_animationIndentPtr->start();
}

} //namespace imtwidgets
