#ifndef TEXTNODEDOCX_H
#define TEXTNODEDOCX_H

#include "textnode.h"

class TextNodeDocx: public TextNode
{
public:
    TextNodeDocx();
    QDomElement addText(QString text);
    QDomElement addTextWithStyle(QString text, QMap<QString, int> tumbler);
    QString colorNear(int red, int green, int blue);


};

#endif // TEXTNODEDOCX_H
