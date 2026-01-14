#include <QCoreApplication>
#include <QDebug>

#include "zipper.h"
#include "qzipwriter_p.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << Zipper().unzip("D:/project/job/test/", "D:/project/job/Test3.docx");
    qDebug() << Zipper().toZip("D:/project/job/test/", "D:/project/job/Test4.docx");
    return a.exec();
}
