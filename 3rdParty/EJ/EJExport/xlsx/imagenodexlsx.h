#ifndef IMAGENODEDOCX_H
#define IMAGENODEDOCX_H

#include "imagenode.h"


class ImageNodeXlsx: public ImageNode{
public:
    ImageNodeXlsx();
    QDomElement addImage(QString name, int x, int y, int id, int idJ);
};

#endif // IMAGENODEXLSX_H
