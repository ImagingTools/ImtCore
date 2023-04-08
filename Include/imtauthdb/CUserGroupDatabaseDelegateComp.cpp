#include <imtauthdb/CUserGroupDatabaseDelegateComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <istd/CCrcCalculator.h>

// ImtCore includes
#include <imtauth/CUserGroupInfo.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CUserGroupDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_documentFactoriesCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<istd::IChangeable> documentPtr;
	documentPtr.SetPtr(new imtauth::CIdentifiableUserGroupInfo());
	if (!documentPtr.IsValid()){
		return nullptr;
	}

	if (record.contains(*m_documentContentColumnIdAttrPtr)){
		QByteArray documentContent = record.value(qPrintable(*m_documentContentColumnIdAttrPtr)).toByteArray();

		if (ReadDataFromMemory("UserGroupInfo", documentContent, *documentPtr)){
			return documentPtr.PopPtr();
		}
	}

	return nullptr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CUserGroupDatabaseDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& objectName,
		const QString& objectDescription,
		const istd::IChangeable* valuePtr) const
{
	NewObjectQuery retVal;

	istd::TOptDelPtr<const istd::IChangeable> workingDocumentPtr;
	if (valuePtr != nullptr){
		workingDocumentPtr.SetPtr(valuePtr, false);
	}

	if (workingDocumentPtr.IsValid()){
		QByteArray documentContent;
		if (WriteDataToMemory("UserGroupInfo", *workingDocumentPtr, documentContent)){
			const imtauth::IUserGroupInfo* groupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(workingDocumentPtr.GetPtr());
			Q_ASSERT(groupInfoPtr != nullptr);
			if (groupInfoPtr == nullptr){
				return NewObjectQuery();
			}

			QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8() : proposedObjectId;
			quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

			QByteArray accountId = "";
			QByteArray groupId = groupInfoPtr->GetId();

			int revisionVersion = 1;
			retVal.query = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\"(\"DocumentId\", \"Document\", \"RevisionNumber\", \"LastModified\", \"Checksum\", \"IsActive\") VALUES('%2', '%3', '%4', '%5', '%6', true);")
						.arg(qPrintable(*m_tableNameAttrPtr))
						.arg(qPrintable(objectId))
						.arg(SqlEncode(documentContent))
						.arg(revisionVersion)
						.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
						.arg(checksum).toLocal8Bit();

			retVal.objectName = objectName;
		}
	}

	return retVal;
}


QByteArray CUserGroupDatabaseDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const istd::IChangeable& object) const
{
	QByteArray retVal;

	QByteArray documentContent;
	if (WriteDataToMemory("UserGroupInfo", object, documentContent)){
		quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());
		const imtauth::CIdentifiableUserGroupInfo* groupInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserGroupInfo*>(&object);
		Q_ASSERT(groupInfoPtr != nullptr);
		if (groupInfoPtr == nullptr){
			return QByteArray();
		}
//		QByteArray objectUuid = groupInfoPtr->GetObjectUuid();

		retVal = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\" (\"DocumentId\", \"Document\", \"LastModified\", \"Checksum\", \"IsActive\", \"RevisionNumber\") VALUES('%2', '%3', '%4', '%5', true, (SELECT COUNT(\"Id\") FROM \"%1\" WHERE \"DocumentId\" = '%2') + 1 );")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(objectId))
					.arg(SqlEncode(documentContent))
					.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
					.arg(checksum).toLocal8Bit();
	}

	return retVal;
}


// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

QString CUserGroupDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	return QString("SELECT \"Id\", \"%1\", \"Document\", \"RevisionNumber\", \"LastModified\","
					"(SELECT \"LastModified\" FROM \"%2\" as t1 WHERE \"RevisionNumber\" = 1 AND t2.\"%1\" = t1.\"%1\" LIMIT 1) as \"Added\" FROM \"%2\""
					" as t2 WHERE \"IsActive\" = true")
			.arg(qPrintable(*m_objectIdColumnAttrPtr))
			.arg(qPrintable(*m_tableNameAttrPtr));
}



} // namespace imtauthdb


