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
class IDataBaseChangeable: virtual public istd::IChangeable
{
public:
	/*!
	 * \brief GetProperties extracts all
	 * \return a list containing all the keys associated with properties
	 */
	virtual QByteArrayList GetProperties() const = 0;

	/*!
	 * \brief GetProperty
	 * \retval QVariant::Null - means that object has no property with 'propName'
	 * \return the stored value
	 */
	virtual QVariant GetProperty(const QByteArray& propName) const = 0;

	/*!
	 * \brief SetProperty
	 * \return result of setting the property
	 * \retval false - if the object has no supports that property or invalid value in value
	 */
	virtual bool SetProperty(const QVariant& value, const QByteArray& propName) = 0;
	virtual QByteArray GetSelectSqlQuery() const = 0;
	virtual QByteArray GetUpdateSqlQuery() const = 0;
	virtual QByteArray GetInsertSqlQuery() const = 0;
	virtual QByteArray GetDeleteSqlQuery() const = 0;
	virtual QByteArray GetTypeId() const = 0;

	virtual IDataBaseChangeable* Join(const IDataBaseChangeable* const baseObject, const IDataBaseChangeable* const objectWithNewProps) const = 0;


	// comfort methods
	virtual QVariant GetProperty(const QString& propName) const {return this->GetProperty(propName.toUtf8());}
	virtual QVariant GetProperty(const char* propName) const {return this->GetProperty(QByteArray(propName));}
	virtual QVariant SetProperty(const QVariant& value, const QString& propName){return this->SetProperty(value, propName.toUtf8());}
	virtual QVariant SetProperty(const QVariant& value, const char* propName){return this->SetProperty(value, QByteArray(propName));}
};

}
