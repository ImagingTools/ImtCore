#ifndef CALCULATORHELPER_H
#define CALCULATORHELPER_H
#include <QString>
#include <QStringList>

enum LinkType {
    LINK_CLEAN = -1,
    LINK_LABEL = 0,
    LINK_TABLE,
    LINK_FORMULA,
    LINK_LINK
};

struct LinkParams
{
    LinkParams() { numLink = -1; numType = -1; type = LINK_CLEAN; }
    int numLink;
    LinkType type;
    int numType;
    QString value;
};

class CalculatorHelper
{
   public:
    static void split(QString &txt,QStringList &lStr);
    static void getLinkParams(QString txt, LinkParams &params);
};

#endif // CALCULATORHELPER_H


