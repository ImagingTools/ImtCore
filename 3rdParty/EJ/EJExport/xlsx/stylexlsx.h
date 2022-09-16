#ifndef STYLEXLSX_H
#define STYLEXLSX_H

#include <QDomDocument>



class StyleXlsx
{
public:
    StyleXlsx();
    QDomElement addFonts();
    QDomElement addFills();
    QDomElement addBorders();
    QDomElement addCellXfs();

    QDomDocument glueStyle(QList<QDomElement> cellList);

};

#endif // STYLEXLSX_H
