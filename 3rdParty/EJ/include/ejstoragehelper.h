#ifndef STORAGEHELPER_H
#define STORAGEHELPER_H

#include "ejdocument.h"

class COMMONSHARED_EXPORT EjStorageHelper
{
public:
	EjStorageHelper();
	static EjStorageHelper *getInstance();
    static void loadSmallImage(QImage *image, QByteArray &name, bool isSmall = true);
	static void addImage(QString path, QByteArray &name);
	static void addImage(QImage &image, QByteArray &name);
    static QString pathPic();

	static void loadData(EjDocument *doc, QList<QByteArray *> &lData, bool isSaveHistory = false);
	static void updateDoc(EjDocument *doc, quint16 patchVer, quint32 patchTime, bool isSetKey = true);
};
#endif // STORAGEHELPER_H
