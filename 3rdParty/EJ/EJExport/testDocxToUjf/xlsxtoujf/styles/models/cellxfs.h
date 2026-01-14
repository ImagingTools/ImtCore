#ifndef CELLXFS_H
#define CELLXFS_H
#include <QFont>
#include "fill.h"
#include "borders.h"
#include "font.h"

class CellXfs
{
    Font font;
    Fill fill;
    Borders border;
    QString horizontal;
    QString vertical;
public:
    CellXfs();
    CellXfs(Font, Fill, Borders);
    void setFont(Font);
    void setFill(Fill);
    void setBorder(Borders);
    QString getHAlign() const;
    QString getVAlign() const;
    void setHAlign(QString align);
    void setVAlign(QString align);
    Font getFont();
    Fill getFill();
    Borders getBorders();
};

#endif // CELLXFS_H
