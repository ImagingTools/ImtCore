#ifndef TEXTNODE_H
#define TEXTNODE_H

#include "QDomDocument"
#include "common.h"

class TextNode{
public:
    virtual QDomElement addText(QString text)=0;
};
#endif // TEXTNODE_H
