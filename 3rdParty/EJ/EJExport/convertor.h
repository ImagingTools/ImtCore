#ifndef CONVERTOR_H
#define CONVERTOR_H
#include <QString>
#include "ejcommon.h"


class Convertor {
    //virtual bool write(Document *doc) = 0;
public:
    virtual bool convert(EjDocument *doc, QString const& fileName) = 0;
    virtual ~Convertor() {}
};

#endif // CONVERTOR_H
