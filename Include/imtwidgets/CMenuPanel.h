#pragma once

// Qt includes
#include <QtCore/QPropertyAnimation>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QTreeView>

namespace imtwidgets
{

class CMenuPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CMenuPanel(QWidget *parent = nullptr);
    ~CMenuPanel();

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:
    int m_minWidth;
    int m_maxWidth;
    int m_indent;

    QTreeView *m_treeViewPtr;
    QStandardItemModel *m_modelPtr;
    QPropertyAnimation *m_animationWidthPtr;
    QPropertyAnimation *m_animationIndentPtr;
};

} //namespace imtwidgets
