#ifndef IMAGENODEDOCX_H
#define IMAGENODEDOCX_H

#include "imagenode.h"


class ImageNodeDocx: public ImageNode{
public:
    ImageNodeDocx();
    QDomElement addImage(QString name, int x, int y, int id, int idJ);
};

#endif // IMAGENODEDOCX_H
