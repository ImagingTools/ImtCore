#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <istd/CCrcCalculator.h>
#include <istd/TOptDelPtr.h>

// ImtCore includes
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>
#include <imtauth/CRole.h>


namespace imtauthdb
{


template <class BaseDelegate>
class TRoleDatabaseDelegateComp: public BaseDelegate
{
public:
	typedef imtdb::CSqlJsonDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(TRoleDatabaseDelegateComp)
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual istd::IChangeable* CreateObjectFromRecord(const QSqlRecord& record) const override;
	virtual imtdb::IDatabaseObjectDelegate::NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr,
				bool useExternDelegate = true) const override;
};


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

template <class BaseDelegate>
istd::IChangeable* TRoleDatabaseDelegateComp<BaseDelegate>::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!this->m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!this->m_documentFactoriesCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<istd::IChangeable> documentPtr;
	documentPtr.SetPtr(new imtauth::CIdentifiableRoleInfo());
	if (!documentPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Document")){
		QByteArray documentContent = record.value(qPrintable("Document")).toByteArray();

		if (BaseDelegate::ReadDataFromMemory("RoleInfo", documentContent, *documentPtr)){
			return documentPtr.PopPtr();
		}
	}

	return nullptr;
}


template <class BaseDelegate>
imtdb::IDatabaseObjectDelegate::NewObjectQuery TRoleDatabaseDelegateComp<BaseDelegate>::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& proposedObjectId,
			const QString& objectName,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* operationContextPtr) const
{
	imtdb::IDatabaseObjectDelegate::NewObjectQuery retVal;

	istd::TOptDelPtr<const istd::IChangeable> workingDocumentPtr;
	if (valuePtr != nullptr){
		workingDocumentPtr.SetPtr(valuePtr, false);
	}

	if (workingDocumentPtr.IsValid()){
		QByteArray documentContent;
		if (BaseDelegate::WriteDataToMemory("RoleInfo", *workingDocumentPtr, documentContent)){
			QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8() : proposedObjectId;
			quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

			int revisionVersion = 1;
			retVal.query = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\"(\"DocumentId\", \"Document\", \"RevisionNumber\", \"LastModified\", \"Checksum\", \"IsActive\") VALUES('%2', '%3', '%4', '%5', '%6', true);")
						.arg(qPrintable("Roles"))
						.arg(qPrintable(objectId))
						.arg(BaseDelegate::SqlEncode(documentContent))
						.arg(revisionVersion)
						.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate))
						.arg(checksum).toUtf8();

			retVal.query += BaseDelegate::CreateOperationDescriptionQuery(objectId, operationContextPtr);

			retVal.objectName = objectName;
		}
	}

	return retVal;
}


template <class BaseDelegate>
QByteArray TRoleDatabaseDelegateComp<BaseDelegate>::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			bool /*useExternDelegate*/) const
{
	QByteArray retVal;

	QByteArray documentContent;
	if (BaseDelegate::WriteDataToMemory("RoleInfo", object, documentContent)){
		quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());
		retVal = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\" (\"DocumentId\", \"Document\", \"LastModified\", \"Checksum\", \"IsActive\", \"RevisionNumber\") VALUES('%2', '%3', '%4', '%5', true, (SELECT COUNT(\"Id\") FROM \"%1\" WHERE \"DocumentId\" = '%2') + 1 );")
					.arg(qPrintable("Roles"))
					.arg(qPrintable(objectId))
					.arg(BaseDelegate::SqlEncode(documentContent))
					.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate))
					.arg(checksum).toUtf8();

		retVal += BaseDelegate::CreateOperationDescriptionQuery(objectId, operationContextPtr);
	}

	return retVal;
}


} // namespace imtauthdb


