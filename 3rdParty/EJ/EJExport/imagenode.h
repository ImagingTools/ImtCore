#ifndef IMAGENODE_H
#define IMAGENODE_H

#include "QDomDocument"

class ImageNode{

public:
    virtual QDomElement addImage(QString name, int x, int y, int id,  int idJ)=0;
};

#endif // IMAGENODE_H
