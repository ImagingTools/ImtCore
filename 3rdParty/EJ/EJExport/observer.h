#ifndef OBSERVER_H
#define OBSERVER_H

#include "common.h"
#include "QDomDocument"
class Observer
{
public:
    virtual QList<QDomElement> observe(Document *doc)=0;
    virtual QDomDocument glue(QList<QDomElement> list)=0;
    virtual void write(QDomDocument doc)=0;
};

#endif // OBSERVER_H
