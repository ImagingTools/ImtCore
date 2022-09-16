#include "sharedstrings.h"

SharedStrings::SharedStrings()
{

}

void SharedStrings::addString(QString str)
{
    strings.append(str);
}

QString SharedStrings::getStringByIndex(int index)
{
    return strings.at(index);
}
