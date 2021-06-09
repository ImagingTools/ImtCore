#pragma once

#include <QtCore/QtDebug>
#include <imtdb/IDataBaseChangeable.h>

namespace imtdb
{

class CDataBaseAddress: public IDataBaseChangeable
{

public:
	CDataBaseAddress();
	virtual ~CDataBaseAddress();

// IDataBaseChangeable interface
public:
	QByteArrayList GetProperties() const override;
	QVariant GetProperty(const QByteArray& propName) const override;
	bool SetProperty(const QVariant& value, const QByteArray& propName) override;
	QByteArray GetSelectSqlQuery() const override;
	QByteArray GetUpdateSqlQuery() const override;
	QByteArray GetInsertSqlQuery() const override;
	QByteArray GetDeleteSqlQuery() const override;
	virtual QByteArray GetTypeId() const override;
	IDataBaseChangeable* Join(const IDataBaseChangeable* const baseObject, const IDataBaseChangeable* const objectWithNewProps) const override;


private:
	typedef QPair<QByteArray, QVariant> AddressProperty;
	QMap<QByteArray, QVariant> m_properties;

	QString m_Country;
	QString m_City;
	int m_Corpus;
	QString m_Street;
	QString m_Letter;
	int m_HouseNum;
	QString m_Region;
	QString m_JsonData;
	int m_TimeShift;
	QString m_Note;
	long m_CoordLong;
	long m_CoordLat;
	QString m_CompanyId;
	QString m_PostIndex;
	QString m_Name;

};
}
