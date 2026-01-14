#ifndef PARAGRAPHNODE_H
#define PARAGRAPHNODE_H

#include "QDomDocument"

class ParagraphNode{
public:
    virtual QDomElement addParagraph(QList<QDomElement> nodesList)=0;
};
#endif // PARAGRAPHNODE_H
