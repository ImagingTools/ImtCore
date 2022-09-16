#ifndef TABLENODEDOCX_H
#define TABLENODEDOCX_H

#include "tablenode.h"

class TableNodeXlsx: public TableNode{


public:
    TableNodeXlsx();
    QDomDocument glueTable(int row, int column, QList<QString> cellList);
  //  QDomElement addTablePlus(int row, int column, QList<QDomElement> cellList, int  styles[]);
    QString getRefValue(int tPosition, int column, int row, bool type);

    QDomElement addTable(int row, int column, QList<QString> cellList);
};


#endif // TABLENODEXLSX_H
