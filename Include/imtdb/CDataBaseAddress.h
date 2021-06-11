#pragma once

#include <QtCore/QtDebug>
#include <imtdb/IDataBaseChangeable.h>

namespace imtdb
{

class CDataBaseAddress: public istd::IChangeable
{

public:
	CDataBaseAddress();
	virtual ~CDataBaseAddress();

public:

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
