#ifndef TEXTNODEDOCX_H
#define TEXTNODEDOCX_H

#include "textnode.h"

class TextNodeXslx: public TextNode
{
public:
    TextNodeXslx();
    QDomElement addText(QString text);
    QDomElement addTextWithStyle(QString text, int tumbler[]);
};

#endif // TEXTNODEXSLX_H
