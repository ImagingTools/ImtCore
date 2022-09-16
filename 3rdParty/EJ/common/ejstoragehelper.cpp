#include "ejstoragehelper.h"
#include "ejdocument.h"


EjStorageHelper::EjStorageHelper()
{

}

EjStorageHelper *EjStorageHelper::getInstance()
{
    static EjStorageHelper instance;
    return &instance;
}

void EjStorageHelper::loadSmallImage(QImage *image, QByteArray &name, bool isSmall)
{

}

void EjStorageHelper::addImage(QString path, QByteArray &name)
{

}

void EjStorageHelper::addImage(QImage &image, QByteArray &name)
{

}

QString EjStorageHelper::pathPic()
{
	return QString();
}

