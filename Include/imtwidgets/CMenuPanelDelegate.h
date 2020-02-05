#pragma once

#include <QStyledItemDelegate>

class CMenuPanelDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit CMenuPanelDelegate(QObject *parent = nullptr){}

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
