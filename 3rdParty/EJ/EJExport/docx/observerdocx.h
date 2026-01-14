#ifndef OBSERVERDOCX_H
#define OBSERVERDOCX_H

#include "observer.h"
#include "docx/cellnodedocx.h"
#include "tableblocks.h"
#include "export_global.h"

class ObserverDocx: public Observer
{
public:
    ObserverDocx();
    QList<QDomElement> observe(Document *doc);
    QDomDocument glue(QList<QDomElement> nodesList);
    QMap<QString, int> changeStyle(QMap<QString, int> styles, NumStyleBlock *nsb); //non decl
    QMap<QString, int> border(QMap<QString, int> styles, CellStyle *cellStyle);
    bool unzipDocument(QString format);
    bool archved(QString pathFile, QString zipPath);
    void write(QDomDocument doc);
    void addImageRelationship(QString path,int id);
    QDomElement createSectPr(QString width, QString height, QString left, QString right, QString top,
                             QString bottom, QString header, QString footer, QString gutter);

    QList<int> startColumnMerge;
    QList<int> countRowsMerge;
    double cmToTwip;

    int sectW;
    int sectH;
    int sectPgMarLeft;
    int sectPgMarRight;
    int sectPgMarBottom;
    int sectPgMarTop;
    int sectPgMarGutter;
    int sectPgMarHeader;
    int sectPgMarFooter;

};

#endif // OBSERVERDOCX_H
