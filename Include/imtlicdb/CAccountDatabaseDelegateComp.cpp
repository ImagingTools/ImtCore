#include <imtlicdb/CAccountDatabaseDelegateComp.h>


// ImtCore includes
#include <imtauth/CContactInfo.h>
#include <imtauth/CAccountInfoMetaInfo.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CAccountDatabaseDelegateComp::CreateObjectFromRecord(const QByteArray& /*typeId*/, const QSqlRecord& record) const
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

	QByteArray accountId;
	if (record.contains("Id")){
		accountId = record.value("Id").toByteArray();
	}

	if (record.contains("Name")){
		QString accountName = record.value("Name").toString();

		accountInfoPtr->SetAccountName(accountName);
	}

	if (record.contains("Description")){
		QString accountDescription = record.value("Description").toString();

		accountInfoPtr->SetAccountDescription(accountDescription);
	}

	imtauth::CContactInfo contactInfo;
	if (record.contains("Type")){
		QString type = record.value("Type").toString();
	}

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


imtdb::IDatabaseObjectDelegate::NewObjectQuery CAccountDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& objectName,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr) const
{
	const imtauth::IAccountInfo* accountInfoPtr = dynamic_cast<const imtauth::IAccountInfo*>(valuePtr);
	if (accountInfoPtr == nullptr){
		return NewObjectQuery();
	}

	QString accountName = accountInfoPtr->GetAccountName();
	if (accountName.isEmpty()){
		accountName = objectName;
	}

	if (accountName.isEmpty()){
		return NewObjectQuery();
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
	QByteArray gender;
	QDate birthDay;

	const imtauth::IContactInfo* ownerPtr = accountInfoPtr->GetAccountOwner();
	if (ownerPtr != nullptr){
		mail = ownerPtr->GetMail();
		lastName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_LAST_NAME);
		firstName = ownerPtr->GetNameField(imtauth::IContactInfo::NFT_FIRST_NAME);

		imtauth::IContactInfo::GenderType genderType = ownerPtr->GetGenderType();
		if (genderType == imtauth::IContactInfo::GenderType::GT_MALE){
			gender = "male";
		}
		else if (genderType == imtauth::IContactInfo::GenderType::GT_FEMALE){
			gender = "female";
		}
		else if (genderType == imtauth::IContactInfo::GenderType::GT_DIVERSE){
			gender = "diverse";
		}

		birthDay = ownerPtr->GetBirthday();
	}

	NewObjectQuery retVal;

	retVal.query = QString("INSERT INTO \"Accounts\" (Id, Name, Description, Type, OwnerMail, OwnerLastName, OwnerFirstName) VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7');")
							.arg(qPrintable(accountId))
							.arg(accountName)
							.arg(accountDescription)
							.arg(qPrintable(accountTypeId))
							.arg(mail)
							.arg(lastName)
							.arg(firstName)
							.toLocal8Bit();

	retVal.objectName = accountName;

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

		QByteArray retVal = QString("DELETE FROM \"Accounts\" WHERE Id = '%1';").arg(qPrintable(objectId)).toLocal8Bit();

		return retVal;
	}

	return QByteArray();
}


QByteArray CAccountDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object) const
{
	const imtauth::IAccountInfo* accountInfoPtr = dynamic_cast<const imtauth::IAccountInfo*>(&object);
	if (accountInfoPtr == nullptr){
		return QByteArray();
	}

	QString accountName = accountInfoPtr->GetAccountName();

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

	QByteArray retVal = QString("UPDATE \"Accounts\" SET Id ='%1', Name = '%2', Description = '%3', Type = '%4', OwnerMail = '%5', OwnerLastName = '%6', OwnerFirstName = '%7' WHERE Id ='%8';")
							.arg(qPrintable(accountId))
							.arg(accountName)
							.arg(accountDescription)
							.arg(qPrintable(accountTypeId))
							.arg(mail)
							.arg(lastName)
							.arg(firstName)
							.arg(qPrintable(objectId))
							.toLocal8Bit();

	return retVal;
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

	QByteArray newId = newObjectName.toUtf8();
	QByteArray retVal = QString("UPDATE \"Accounts\" SET Id = '%1', Name = '%2' WHERE Id = '%3';")
			.arg(qPrintable(newId))
			.arg(newObjectName)
			.arg(qPrintable(objectId)).toLocal8Bit();

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

	QByteArray retVal = QString("UPDATE \"Accounts\" SET Description = '%1' WHERE Id ='%2';").arg(description).arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


// protected methods

// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::MetaInfoPtr CAccountDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr(new imod::TModelWrap<imtauth::CAccountInfoMetaInfo>);
}


bool CAccountDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("Name")){
		QString accountName = record.value("Name").toString();

		metaInfo.SetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_NAME, accountName);
	}

	if (record.contains("Description")){
		QString accountDescription = record.value("Description").toString();

		metaInfo.SetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_DESCRIPTION, accountDescription);
	}

	if (record.contains("OwnerMail")){
		QString mail = record.value("OwnerMail").toString();

		metaInfo.SetMetaInfo(imtauth::IAccountInfo::MIT_CONTACT_EMAIL, mail);
	}

	if (record.contains("OwnerLastName")){
		QString lastName = record.value("OwnerLastName").toString();

		metaInfo.SetMetaInfo(imtauth::IAccountInfo::MIT_CONTACT_LAST_NAME, lastName);
	}

	if (record.contains("OwnerFirstName")){
		QString firstName = record.value("OwnerFirstName").toString();

		metaInfo.SetMetaInfo(imtauth::IAccountInfo::MIT_CONTACT_FIRST_NAME, firstName);
	}

	if (record.contains("Type")){
		QByteArray accountTypeId = record.value("Type").toByteArray();
		QString typeName;
		if (accountTypeId == "company"){
			typeName = QObject::tr("Company");
		}
		else if (accountTypeId == "private"){
			typeName = QObject::tr("Private");
		}

		metaInfo.SetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_TYPE, typeName);
	}

	return true;
}


} // namespace imtlicdb


