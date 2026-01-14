#ifndef EJCSVDOCUMENT_H
#define EJCSVDOCUMENT_H
#include <QDomDocument>
#include <QFile>
#include "imageblock.h"
#include "ejtableblocks.h"
#include "export_global.h"


class EjCSVDocument
{   
    QString text;
    int index_string = 0;
    int index_column = 1;
    int index_max_column = 1;
    QFile file;
    QTextStream out;
    bool active_table = false;
    int countStrFromCurrentTable = -1;
    int countColFromCurrentTable = -1;
public:
	 EjCSVDocument();
     void setFileName(QString fileName);
     bool newString();
     void addText(QString text, bool);
     void addTextFromTable(QString text);
     void addSemicolon();
     void addSpace();
	 bool findMaxIndexColumn(QList<EjBlock*> *blocks);
     bool getActiveTable() const;
     void setActiveTable(bool value);
     int getCountStrFromCurrentTable() const;
     void setCountStrFromCurrentTable(int value);
     void setCountColFromCurrentTable(int value);
     QString getText() const;
};

#endif // EJCSVDOCUMENT_H
