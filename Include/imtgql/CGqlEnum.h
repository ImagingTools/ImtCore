#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QVariant>



namespace imtgql
{


class CGqlEnum
{
public:
	CGqlEnum(QByteArray value = "");
	QByteArray GetValue();
	operator QVariant() const;

protected:
	QByteArray m_value;
};


} // namespace imtgql


Q_DECLARE_METATYPE(imtgql::CGqlEnum)

