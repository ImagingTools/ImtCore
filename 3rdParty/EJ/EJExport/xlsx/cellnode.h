#ifndef CELLNODE_H
#define CELLNODE_H

#include "QDomDocument"

class CellNode{
public:
    virtual QDomElement addCellOne(QString node)=0;
};
#endif // CELLNODE_H
