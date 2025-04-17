#pragma once


// Qt includes
#include <QtCore/QMetaType>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdb
{


class ISqlJsonXPathExtractor: virtual public istd::IPolymorphic
{
public:
	virtual QString ExtractXPath(const QString& jsonKey, const QString& fieldId, QMetaType::Type metaType = QMetaType::QString, const QString& tableAlias = QString()) const = 0;
};


} // namespace imtdb


