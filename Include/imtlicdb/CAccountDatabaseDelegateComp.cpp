#include <imtlicdb/CAccountDatabaseDelegateComp.h>


// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtauth/CContactInfo.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::IDatabaseObjectDelegate)

istd::IChangeable* CAccountDatabaseDelegateComp::CreateObjectFromRecord(
			const QByteArray& /*typeId*/,
			const QSqlRecord& record,
			QByteArray& objectId,
			QString& objectName,
			QString& objectDescription,
			QDateTime& /*lastModified*/,
			QDateTime& /*added*/) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_accountInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtauth::IAccountInfo> accountInfoPtr = m_accountInfoFactCompPtr.CreateInstance();
	if (!accountInfoPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		objectId = record.value("Id").toByteArray();
	}

	if (record.contains("Name")){
		objectName = record.value("Name").toString();

		accountInfoPtr->SetAccountName(objectName);
	}

	if (record.contains("Description")){
		objectDescription = record.value("Description").toString();

		accountInfoPtr->SetAccountDescription(objectDescription);
	}
	
	imtauth::CContactInfo contactInfo;

	if (record.contains("OwnerMail")){
		contactInfo.SetEmail(record.value("OwnerMail").toString());
	}

	if (record.contains("OwnerLastName")){
		contactInfo.SetNameField(imtauth::IContactInfo::NFT_LAST_NAME, record.value("OwnerLastName").toString());
	}

	if (record.contains("OwnerFirstName")){
		contactInfo.SetNameField(imtauth::IContactInfo::NFT_FIRST_NAME, record.value("OwnerFirstName").toString());
	}

	accountInfoPtr->SetAccountOwner(contactInfo);

	return accountInfoPtr.PopPtr();
}


QByteArray CAccountDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& objectName,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr) const
{
	const imtauth::IAccountInfo* accountInfoPtr = dynamic_cast<const imtauth::IAccountInfo*>(valuePtr);
	if (accountInfoPtr == nullptr){
		return QByteArray();
	}

	QString accountName = accountInfoPtr->GetAccountName();
	if (accountName.isEmpty()){
		accountName = objectName;
	}

	if (accountName.isEmpty()){
		return QByteArray();
	}

	QByteArray accountId = accountName.toUtf8();

	QByteArray accountTypeId;
	imtauth::IAccountInfo::AccountType accountType = accountInfoPtr->GetAccountType();
	if (accountType == imtauth::IAccountInfo::AT_COMPANY){
		accountTypeId = "company";
	}
	else if (accountType == imtauth::IAccountInfo::AT_PERSON){
		accountTypeId = "private";
	}

	QString accountDescription = accountInfoPtr->GetAccountDescription();

	QString mail;
	QString lastName;
	QString firstName;

	const imtauth::IContactInfo* ownerPtr = accountInfoPtr->GetAccountOwner();
	if (ownerPtr != nullptr){
		mail = ownerPtr->GetMail();
		lastName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_LAST_NAME);
		firstName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_FIRST_NAME);
	}

	QByteArray retVal = QString("INSERT INTO Accounts(Id, Name, Description, Type, OwnerMail, OwnerLastName, OwnerFirstName) VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7');")
				.arg(qPrintable(accountId))
				.arg(accountName)
				.arg(accountDescription)
				.arg(qPrintable(accountTypeId))
				.arg(mail)
				.arg(lastName)
				.arg(firstName)
		.toLocal8Bit();

	return retVal;
}


QByteArray CAccountDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId) const
{
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		const imtauth::IAccountInfo* accountInfoPtr = dynamic_cast<const imtauth::IAccountInfo*>(objectPtr.GetPtr());
		if (accountInfoPtr == nullptr){
			return QByteArray();
		}

		QByteArray retVal = QString("DELETE FROM Accounts WHERE Id = '%1';").arg(qPrintable(objectId)).toLocal8Bit();

		return retVal;
	}

	return QByteArray();
}


QByteArray CAccountDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object) const
{
	return QByteArray();
}


QByteArray CAccountDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& newObjectName) const
{
	const imtauth::IAccountInfo* accountInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		accountInfoPtr = dynamic_cast<const imtauth::IAccountInfo*>(objectPtr.GetPtr());
	}

	if (accountInfoPtr == nullptr){
		return QByteArray();
	}

	if (objectId.isEmpty()){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE Accounts SET Name = '%1' WHERE Id ='%2';").arg(newObjectName).arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


QByteArray CAccountDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& description) const
{
	const imtauth::IAccountInfo* accountInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		accountInfoPtr = dynamic_cast<const imtauth::IAccountInfo*>(objectPtr.GetPtr());
	}

	if (accountInfoPtr == nullptr){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE Accounts SET Description = '%1' WHERE Id ='%2';").arg(description).arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


} // namespace imtlicdb


