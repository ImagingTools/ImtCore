// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CExternalIdentityDatabaseDelegateComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtdb/imtdb.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeableUniquePtr CExternalIdentityDatabaseDelegateComp::CreateObjectFromRecord(
			const QSqlRecord& record,
			const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_identityFactCompPtr.IsValid()){
		return nullptr;
	}

	imtauth::IExternalIdentityUniquePtr identityPtr = m_identityFactCompPtr.CreateInstance();
	if (!identityPtr.IsValid()){
		return nullptr;
	}

	identityPtr->SetId(imtdb::VariantToByteArray(record.value("Id")));
	identityPtr->SetUserId(imtdb::VariantToByteArray(record.value("UserId")));
	identityPtr->SetProviderId(imtdb::VariantToByteArray(record.value("ProviderId")));
	identityPtr->SetSubject(record.value("Subject").toString());
	identityPtr->SetEmail(record.value("Email").toString());
	identityPtr->SetLinkedAt(FromSqlDateTime(record.value("LinkedAt")));
	identityPtr->SetLastSignInAt(FromSqlDateTime(record.value("LastSignInAt")));

	return identityPtr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CExternalIdentityDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& proposedObjectId,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const imtauth::IExternalIdentity* identityPtr = dynamic_cast<const imtauth::IExternalIdentity*>(valuePtr);
	if (identityPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray identityId = proposedObjectId;
	if (identityId.isEmpty()){
		identityId = identityPtr->GetId();
	}
	if (identityId.isEmpty()){
		identityId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	NewObjectQuery retVal;
	retVal.query = QStringLiteral(
				R"(INSERT INTO "ExternalIdentities" ("Id", "UserId", "ProviderId", "Subject", "Email", "LinkedAt", "LastSignInAt") )"
				R"(VALUES('%1', '%2', '%3', '%4', '%5', %6, %7);)")
				.arg(
					SqlEncode(QString::fromUtf8(identityId)),
					SqlEncode(QString::fromUtf8(identityPtr->GetUserId())),
					SqlEncode(QString::fromUtf8(identityPtr->GetProviderId())),
					SqlEncode(identityPtr->GetSubject()),
					SqlEncode(identityPtr->GetEmail()),
					ToSqlDateTime(identityPtr->GetLinkedAt()),
					ToSqlDateTime(identityPtr->GetLastSignInAt()))
				.toUtf8();

	return retVal;
}


QByteArray CExternalIdentityDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	const imtauth::IExternalIdentity* identityPtr = dynamic_cast<const imtauth::IExternalIdentity*>(&object);
	if (identityPtr == nullptr || objectId.isEmpty()){
		return QByteArray();
	}

	// The linked account (user, provider, subject) never changes, only its sign-in data.
	return QStringLiteral(R"(UPDATE "ExternalIdentities" SET "Email" = '%1', "LastSignInAt" = %2 WHERE "Id" = '%3';)")
				.arg(
					SqlEncode(identityPtr->GetEmail()),
					ToSqlDateTime(identityPtr->GetLastSignInAt()),
					SqlEncode(QString::fromUtf8(objectId)))
				.toUtf8();
}


QByteArray CExternalIdentityDatabaseDelegateComp::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArrayList& objectIds,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}

	QStringList encodedIds;
	for (const QByteArray& objectId : objectIds){
		encodedIds.append(QStringLiteral("'%1'").arg(SqlEncode(QString::fromUtf8(objectId))));
	}

	return QStringLiteral(R"(DELETE FROM "ExternalIdentities" WHERE "Id" IN (%1);)").arg(encodedIds.join(QStringLiteral(", "))).toUtf8();
}


QByteArray CExternalIdentityDatabaseDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CExternalIdentityDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CExternalIdentityDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


// private static methods

QString CExternalIdentityDatabaseDelegateComp::ToSqlDateTime(const QDateTime& dateTime)
{
	if (!dateTime.isValid()){
		return QStringLiteral("NULL");
	}

	return QStringLiteral("'%1'").arg(dateTime.toUTC().toString(Qt::ISODate));
}


QDateTime CExternalIdentityDatabaseDelegateComp::FromSqlDateTime(const QVariant& value)
{
	if (value.isNull()){
		return QDateTime();
	}

	QDateTime dateTime = value.toDateTime();
	if (!dateTime.isValid()){
		dateTime = QDateTime::fromString(value.toString(), Qt::ISODate);
	}

	if (!dateTime.isValid()){
		return QDateTime();
	}

	// Stored without time zone, always as UTC.
	return QDateTime(dateTime.date(), dateTime.time(), Qt::UTC);
}


} // namespace imtauthdb


