#include "imtdb/CDataBaseAddress.h"

namespace imtdb
{

CDataBaseAddress::CDataBaseAddress()
{
	m_properties.insert("Country"  , QVariant());
	m_properties.insert("City"	   , QVariant());
	m_properties.insert("Corpus"   , QVariant());
	m_properties.insert("Street"   , QVariant());
	m_properties.insert("Letter"   , QVariant());
	m_properties.insert("HouseNum" , QVariant());
	m_properties.insert("Region"   , QVariant());
	m_properties.insert("JsonData" , QVariant());
	m_properties.insert("TimeShift", QVariant());
	m_properties.insert("Note"	   , QVariant());
	m_properties.insert("CoordLong", QVariant());
	m_properties.insert("CoordLat" , QVariant());
	m_properties.insert("CompanyId", QVariant());
	m_properties.insert("PostIndex", QVariant());
	m_properties.insert("Name"	   , QVariant());
}

CDataBaseAddress::~CDataBaseAddress()
{

}

QByteArrayList CDataBaseAddress::GetProperties() const
{
	return m_properties.keys();
}

QVariant CDataBaseAddress::GetProperty(const QByteArray& propName) const
{
	if(!m_properties.contains(propName)) return QVariant();
	return m_properties[propName];
}

bool CDataBaseAddress::SetProperty(const QVariant& value, const QByteArray& propName)
{
	m_properties[propName] = value;
	if(!m_properties.contains(propName))
	{
		qCritical() << "setting nondefault property!";
	}
	return true;
}

QByteArray CDataBaseAddress::GetSelectSqlQuery() const
{
	return "SELECT "
				" Country"
				" , City"
				" , Corpus"
				" , Street"
				" , Letter"
				" , HouseNum"
				" , Region"
				" , JsonData"
				" , TimeShift"
				" , Note"
				" , CoordLong"
				" , CoordLat"
				" , CompanyId"
				" , PostIndex"
				" , Name "
			" FROM Addresses; ";
}

QByteArray CDataBaseAddress::GetUpdateSqlQuery() const
{
	return "UPDATE Addresses SET "
			"  Country=			:Country"
			", City=			:City"
			", Corpus=			:Corpus"
			", Street=			:Street"
			", Letter=			:Letter"
			", HouseNum=		:HouseNum"
			", Region=			:Region"
			", JsonData=		:JsonData="
			", TimeShift=		:TimeShift"
			", Note=			:Note="
			", CoordLong=		:CoordLong"
			", CoordLat=		:CoordLat"
			", CompanyId=		:CompanyId"
			", PostIndex=		:PostIndex"
			", Name=			:Name;";

}

QByteArray CDataBaseAddress::GetInsertSqlQuery() const
{
	return R"(
				INSERT INTO Addresses (
										  Country
										, City
										, Corpus
										, Street
										, Letter
										, HouseNum
										, Region
										, JsonData
										, TimeShift
										, Note
										, CoordLong
										, CoordLat
										, CompanyId
										, PostIndex
										, Name
				)
				VALUES(
						  :Country
						, :City
						, :Corpus
						, :Street
						, :Letter
						, :HouseNum
						, :Region
						, :JsonData
						, :TimeShift
						, :Note
						, :CoordLong
						, :CoordLat
						, :CompanyId
						, :PostIndex
						, :Name
				);
			)";
}

QByteArray CDataBaseAddress::GetDeleteSqlQuery() const
{
	return R"(
				DELETE FROM Addresses
				WHERE CompanyId= :CompanyId;
			)";
}

QByteArray CDataBaseAddress::GetTypeId() const
{
	return "DB_Address";
}

IDataBaseChangeable* CDataBaseAddress::Join(const IDataBaseChangeable* const baseObject, const IDataBaseChangeable* const objectWithNewProps) const
{
	CDataBaseAddress* retval = new CDataBaseAddress;
	for(auto p:baseObject->GetProperties())
	{
		retval->SetProperty(baseObject->GetProperty(p), p);
	}
	for(auto p:objectWithNewProps->GetProperties())
	{
		QVariant prop = objectWithNewProps->GetProperty(p);
		if(prop.isValid() && !prop.isNull())
		{
			retval->SetProperty(prop, p);
		}
	}
	return retval;

}






}
