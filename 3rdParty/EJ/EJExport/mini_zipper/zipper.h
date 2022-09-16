#ifndef ZIPPER_H
#define ZIPPER_H

#include <QDataStream>
#include <QBuffer>
#include <QDirIterator>
#include "qzipwriter_p.h"
#include "qtiocompressor.h"

class Zipper {
public:
    bool toZip(QString path_files, QString const& zip_path) const;
    bool unzip(QString path_files, QString const& zip_path) const;
};

#endif // ZIPPER_H
