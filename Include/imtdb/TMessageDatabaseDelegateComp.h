#pragma once


// ACF includes
#include <istd/CCrcCalculator.h>
#include <istd/TOptDelPtr.h>
#include <ilog/CMessage.h>

// ImtCore includes
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>


namespace imtdb
{


template <class BaseDelegate>
class TMessageDatabaseDelegateComp: public BaseDelegate
{
public:
	typedef imtdb::CSqlJsonDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(TMessageDatabaseDelegateComp)
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
istd::IChangeable* TMessageDatabaseDelegateComp<BaseDelegate>::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!this->m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!this->m_documentFactoriesCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<istd::IChangeable> documentPtr;
	documentPtr.SetPtr(new ilog::CMessage());
	if (!documentPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Document")){
		QByteArray documentContent = record.value(qPrintable("Document")).toByteArray();

		if (BaseDelegate::ReadDataFromMemory("MessageInfo", documentContent, *documentPtr)){
			return documentPtr.PopPtr();
		}
	}

	return nullptr;
}


template <class BaseDelegate>
imtdb::IDatabaseObjectDelegate::NewObjectQuery TMessageDatabaseDelegateComp<BaseDelegate>::CreateNewObjectQuery(
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
		if (BaseDelegate::WriteDataToMemory("MessageInfo", *workingDocumentPtr, documentContent)){
			QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8() : proposedObjectId;
			quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

			int revisionVersion = 1;
			retVal.query = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\"(\"DocumentId\", \"Document\", \"RevisionNumber\", \"LastModified\", \"Checksum\", \"IsActive\") VALUES('%2', '%3', '%4', '%5', '%6', true);")
						.arg(qPrintable("Messages"))
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
QByteArray TMessageDatabaseDelegateComp<BaseDelegate>::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			bool /*useExternDelegate*/) const
{
	return QByteArray();
}


} // namespace imtdb


