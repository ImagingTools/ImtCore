#ifndef TABLENODE_H
#define TABLENODE_H

#include "QDomDocument"
#include "common.h"
#include "tableignorcellstruct.h"

class TableNode{
public:
    virtual QDomElement addTable(int row, int column, QList<QDomElement> cellList, Empty emptyCell)=0;
};
#endif // TABLENODE_H
