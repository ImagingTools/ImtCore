#ifndef EJSTYLESDOCUMENT_H
#define EJSTYLESDOCUMENT_H
#include <QDomDocument>
#include <QList>
#include "ejtableblocks.h"

class EjStylesDocument
{
    QDomDocument document;
    QDomElement fonts;
    QDomElement fills;
    QDomElement borders;
    QDomElement cellXfs;
    QDomElement cellStyleXfs;
    QDomElement cellStyles;
    QDomElement dxfs;
    QDomElement tableStyles;
    QMap<int, QString> widthsBorders;
     QMap<int, QDomElement> aligns;
    int countBorders = 0;
    int countCellXfs = 0/*Здесь по стандарту одна запись*/;
    int countFills = 1;/*1 потому что там по стандарту есть уже 2 записи*/
    int align = EjParagraphStyle::AlignLeft;
    void aligns_init();
public:
    int addBorders(int left, int right, int top, int bottom);
    int addFill(QString);
    int addXfToCellXfs(int fillId, int borderId);

	EjStylesDocument();
    QDomDocument getDocument() const;
    void setAlign(int value);
};

#endif // EJSTYLESDOCUMENT_H
