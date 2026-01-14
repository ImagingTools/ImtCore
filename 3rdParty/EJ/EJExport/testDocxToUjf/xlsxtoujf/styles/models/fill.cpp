#include "fill.h"

Fill::Fill()
{
    color = QColor("black");
}

Fill::Fill(QColor color)
{
    this->color = color;
}

QColor Fill::getColor()
{
    return color;
}


