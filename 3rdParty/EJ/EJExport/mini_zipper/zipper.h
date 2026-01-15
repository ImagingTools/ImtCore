#ifndef ZIPPER_H
#define ZIPPER_H

#include <QtCore/QString>

class Zipper {
public:
    bool toZip(QString path_files, QString const& zip_path) const;
    bool unzip(QString path_files, QString const& zip_path) const;
};

#endif // ZIPPER_H
