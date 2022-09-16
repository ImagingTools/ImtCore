#ifndef SHAREDSTRINGS_H
#define SHAREDSTRINGS_H
#include <QList>
#include <QString>

class SharedStrings
{
    QList<QString> strings;
public:
    QString curText = "";
    SharedStrings();
    void addString(QString);
    QString getStringByIndex(int index);
};

#endif // SHAREDSTRINGS_H
