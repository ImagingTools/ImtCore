#ifndef EJBASEUJFCONVERTOR_H
#define EJBASEUJFCONVERTOR_H
#include "ejcommon.h"
#include "zipper.h"

class EjBaseUjfConvertor
{
public:
	virtual ~EjBaseUjfConvertor();
    virtual bool readDoc(EjDocument *doc, QString fileName) = 0;
};

#endif // EJBASEUJFCONVERTOR_H
