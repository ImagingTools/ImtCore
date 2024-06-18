#pragma once


// Qt includes
#include <QtCore/QJsonObject.h>

// Acf includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


class IJsonObjectWrittable: virtual public istd::IPolymorphic
{
public:
	virtual bool WriteToJsonObject(QJsonObject& jsonObject) const = 0;
};


} // namespace imtbase


