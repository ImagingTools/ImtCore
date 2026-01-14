#ifndef TABLENODEDOCX_H
#define TABLENODEDOCX_H

#include "tablenode.h"

class TableNodeDocx: public TableNode{


public:
    TableNodeDocx();
    QDomElement addTable(int row, int column, QList<QDomElement> cellList, Empty emptyCell);
    QDomElement addTablePlus(int row, int column, QList<QDomElement> cellList, int  styles[]);

    QList<int> gridColList;
    QString width;
    QString tblInd;
    //int* vectorGridCol;
};

#endif // TABLENODEDOCX_H
