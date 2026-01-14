#ifndef CELLNODEDOCX_H
#define CELLNODEDOCX_H

#include <QDomDocument>
#include <QList>
#include <QMap>

class CellNodeDocx
{
public:
    CellNodeDocx();
    QDomElement addCell(QList<QDomElement> listText);
    QDomElement addCellWithStyle(QDomElement listText, QMap<QString, int> style_tumbler);
    QDomElement addCellWithStyleAndMerge(QDomElement listText, QMap<QString, int> style_tumbler, QString merge, QString restart);

    QList<QString> tcwList;
    bool isMerge;
    bool restart;
};

#endif // CELLNODEDOCX_H
