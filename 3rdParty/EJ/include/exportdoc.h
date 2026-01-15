#ifndef EXPORTDOC_H
#define EXPORTDOC_H
#include <QString>
#include <QPrinter>
#include "ejcommon.h"

#if defined(EXPORT_LIBRARY)
#  define EXPORTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define EXPORTSHARED_EXPORT Q_DECL_IMPORT
#endif

class EXPORTSHARED_EXPORT ExportDoc
{
public:
    ExportDoc();
    bool exportDocument(EjDocument *doc, QString fileName, QString ext);
    bool importToDocument(EjDocument *doc, QString fileName, QString ext);
    bool printDocument(EjDocument *doc, QPrinter *printer);
    Q_INVOKABLE bool openFile(QString fName);
};

#endif // EXPORTDOC_H
