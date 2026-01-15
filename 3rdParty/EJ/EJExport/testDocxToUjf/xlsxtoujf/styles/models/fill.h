#ifndef FILL_H
#define FILL_H
#include <QColor>

class Fill
{
    QColor color;
public:
    Fill();
    Fill(QColor color);
    QColor getColor();
};

#endif // FILL_H
