#include <imtlicdb/CCompanyDatabaseDelegateComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtauth/CCompanyInfo.h>
#include <imtauth/CAccountInfoMetaInfo.h>



namespace imtlicdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CCompanyDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_companyInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtauth::ICompanyInfo> companyInfoPtr = m_companyInfoFactCompPtr.CreateInstance();
	if (!companyInfoPtr.IsValid()){
		return nullptr;
	}

	QByteArray accountId;
	if (record.contains("Id")){
		accountId = record.value("Id").toByteArray();
	}

	if (record.contains("Name")){
		QString accountName = record.value("Name").toString();

		companyInfoPtr->SetAccountName(accountName);
	}

	if (record.contains("Description")){
		QString accountDescription = record.value("Description").toString();

		companyInfoPtr->SetAccountDescription(accountDescription);
	}

	if (record.contains("Mail")){
		QString mail = record.value("Mail").toString();

		companyInfoPtr->SetMail(mail);
	}

	if (record.contains("CompanyName")){
		QString companyName = record.value("CompanyName").toString();

		companyInfoPtr->SetCompanyName(companyName);
	}

	// SetAddress
	imtauth::CAddress address;
	if (record.contains("Country")){
		address.SetCountry(record.value("Country").toString());
	}

	if (record.contains("City")){
		address.SetCity(record.value("City").toString());
	}

	if (record.contains("PostalCode")){
		address.SetPostalCode(record.value("PostalCode").toInt());
	}

	if (record.contains("Street")){
		address.SetStreet(record.value("Street").toString());
	}

	companyInfoPtr->SetAddress(address);

	return companyInfoPtr.PopPtr();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CCompanyDatabaseDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& /*proposedObjectId*/,
		const QString& objectName,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr) const
{
	const imtauth::ICompanyInfo* companyInfoPtr = dynamic_cast<const imtauth::ICompanyInfo*>(valuePtr);
	if (companyInfoPtr == nullptr){
		return NewObjectQuery();
	}

	QString accountName = companyInfoPtr->GetAccountName();
	if (accountName.isEmpty()){
		accountName = objectName;
	}

	if (accountName.isEmpty()){
		return NewObjectQuery();
	}

	QByteArray accountId = accountName.toUtf8();

	QString companyDescription = companyInfoPtr->GetAccountDescription();

	QString mail = companyInfoPtr->GetMail();

	QString companyName = companyInfoPtr->GetCompanyName();

	imtauth::CAddress address = companyInfoPtr->GetAddress();

	QString country = address.GetCountry();
	QString city = address.GetCity();
	int postalCode = address.GetPostalCode();
	QString street = address.GetStreet();

	NewObjectQuery retVal;

	retVal.query = QString("INSERT INTO \"Accounts\" (\"Id\", \"Name\", \"Description\", \"Mail\", \"CompanyName\", \"Country\", \"City\", \"PostalCode\", \"Street\") VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9');")
			.arg(qPrintable(accountId))
			.arg(accountName)
			.arg(companyDescription)
			.arg(mail)
			.arg(companyName)
			.arg(country)
			.arg(city)
			.arg(postalCode)
			.arg(street)
			.toLocal8Bit();

	retVal.objectName = companyName;

	return retVal;
}


QByteArray CCompanyDatabaseDelegateComp::CreateDeleteObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId) const
{
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		const imtauth::ICompanyInfo* companyInfoPtr = dynamic_cast<const imtauth::ICompanyInfo*>(objectPtr.GetPtr());
		if (companyInfoPtr == nullptr){
			return QByteArray();
		}

		QByteArray retVal = QString("DELETE FROM \"Accounts\" WHERE \"Id\" = '%1';").arg(qPrintable(objectId)).toLocal8Bit();

		return retVal;
	}

	return QByteArray();
}


QByteArray CCompanyDatabaseDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		bool /*useExternDelegate*/) const
{
	const imtauth::ICompanyInfo* companyInfoPtr = dynamic_cast<const imtauth::ICompanyInfo*>(&object);
	if (companyInfoPtr == nullptr){
		return QByteArray();
	}

	QString accountName = companyInfoPtr->GetAccountName();

	if (accountName.isEmpty()){
		return QByteArray();
	}

	QByteArray accountId = accountName.toUtf8();

	QString accountDescription = companyInfoPtr->GetAccountDescription();

	QString mail = companyInfoPtr->GetMail();

	QString companyName = companyInfoPtr->GetCompanyName();

	imtauth::CAddress address = companyInfoPtr->GetAddress();

	QString country = address.GetCountry();
	QString city = address.GetCity();
	int postalCode = address.GetPostalCode();
	QString street = address.GetStreet();

	QByteArray retVal = QString("UPDATE \"Accounts\" SET \"Id\" ='%1',\"Name\" = '%2', \"Description\" = '%3', \"Mail\" = '%4', \"CompanyName\" = '%5', \"Country\" = '%6', \"City\" = '%7', \"PostalCode\" = '%8', \"Street\" = '%9' WHERE \"Id\" ='%10';")
			.arg(qPrintable(accountId))
			.arg(accountName)
			.arg(accountDescription)
			.arg(mail)
			.arg(companyName)
			.arg(country)
			.arg(city)
			.arg(postalCode)
			.arg(street)
			.arg(qPrintable(objectId))
			.toLocal8Bit();

	return retVal;
}


QByteArray CCompanyDatabaseDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const QString& newObjectName) const
{
	const imtauth::ICompanyInfo* companyInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		companyInfoPtr = dynamic_cast<const imtauth::ICompanyInfo*>(objectPtr.GetPtr());
	}

	if (companyInfoPtr == nullptr){
		return QByteArray();
	}

	if (objectId.isEmpty()){
		return QByteArray();
	}

	QByteArray newId = newObjectName.toUtf8();
	QByteArray retVal = QString("UPDATE \"Accounts\" SET \"Id\" = '%1', \"Name\" = '%2' WHERE \"Id\" = '%3';")
			.arg(qPrintable(newId))
			.arg(newObjectName)
			.arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


QByteArray CCompanyDatabaseDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const QString& description) const
{
	const imtauth::ICompanyInfo* companyInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		companyInfoPtr = dynamic_cast<const imtauth::ICompanyInfo*>(objectPtr.GetPtr());
	}

	if (companyInfoPtr == nullptr){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE \"Accounts\" SET \"Description\" = '%1' WHERE \"Id\" ='%2';").arg(description).arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


// protected methods

// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::MetaInfoPtr CCompanyDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr(new imod::TModelWrap<imtauth::CAccountInfoMetaInfo>);
}


bool CCompanyDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	const istd::IChangeable* instancePtr = CreateObjectFromRecord(record);
	if ((instancePtr != nullptr) && m_metaInfoCreatorCompPtr.IsValid()){
		idoc::MetaInfoPtr retVal;
		if (m_metaInfoCreatorCompPtr->CreateMetaInfo(instancePtr, "CompanyInfo", retVal)){
			Q_ASSERT(retVal.IsValid());

			return metaInfo.CopyFrom(*retVal);
		}
	}

	return false;
}


} // namespace imtlicdb


