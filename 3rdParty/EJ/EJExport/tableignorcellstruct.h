#ifndef TABLEIGNORCELLSTRUCT_H
#define TABLEIGNORCELLSTRUCT_H

#include <QList>
#include <QMap>

struct tripleCell{
    int column;
    int row;
    int merge;
};

struct Empty{
    QList<int> column;
    QList<int> row;
};

struct Buff{
    QList<int> column;
    QList<int> row;
    QMap<QString, QMap<QString, int> > styles;
};

#endif // TABLEIGNORCELLSTRUCT_H
