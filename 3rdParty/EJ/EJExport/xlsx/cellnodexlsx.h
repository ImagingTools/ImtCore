#ifndef CELLNODEXSLX_H
#define CELLNODEXSLX_H

#include "cellnode.h"
#include "common.h"

class CellNodeXlsx: public CellNode
{
public:
    CellNodeXlsx();
    QDomDocument glueCell(QList<QDomElement> nodesList);  //склейка
    QDomElement addCellOne(QString node); //добавили одну ячейку текста в массив текста
   // QDomElement addCellEmpty();
};

#endif // CELLNODEXLSX_H
