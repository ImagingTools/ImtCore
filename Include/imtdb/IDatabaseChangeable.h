#pragma once

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QVariant>

// ACF includes
#include <istd/IChangeable.h>

namespace imtdb
{

/**
	Common interface for database based data model objects, which can be changed.
	Data model changes are executed in two steps: begin and end.
	The caller can also put some flags and additional data to the update functions, to control the update logic.
*/
class IDatabaseChangeable: virtual public istd::IChangeable
{
public:

	virtual QByteArray GetID() const {return m_id;}
	virtual void SetId(const QByteArray& newId) {m_id = newId;}

protected:
	QByteArray m_id;
};
}
