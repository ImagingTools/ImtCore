#include <imtcache/CViewBuilderComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtSql/QSqlError>


namespace imtcache
{


// reimplemented (imtcache::ICacheTableBuilder)

QString CViewBuilderComp::GetCacheTableName() const
{
	return QString::fromUtf8(*m_viewNameAttrPtr);
}


QStringList CViewBuilderComp::GetRequiredCacheTables() const
{
	QStringList retVal;
	for (int i = 0; i < m_requiredTablesAttrPtr.GetCount(); ++ i){
		retVal << QString::fromUtf8(m_requiredTablesAttrPtr[i]);
	}

	return retVal;
}


CViewBuilderComp::BuildResult CViewBuilderComp::Rebuild(imtduckdb::IDuckConnection& connection) const
{
	BuildResult retVal;
	retVal.wasFullRebuild = true;

	QFile scriptFile(QString::fromUtf8(*m_createViewScriptPathAttrPtr));
	if (!scriptFile.open(QFile::ReadOnly)){
		retVal.errorMessage = QStringLiteral("Unable to read the creation script for %1").arg(GetCacheTableName());

		return retVal;
	}

	QByteArray createViewQuery = scriptFile.readAll();
	scriptFile.close();
	createViewQuery.replace(QByteArrayLiteral("${ViewName}"), GetCacheTableName().toUtf8());

	QSqlError sqlError;
	connection.ExecSqlQuery(createViewQuery, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		// The old definition stays in place, so a failed update leaves the last working view rather than none.
		retVal.errorMessage = QStringLiteral("Unable to create view %1. Error: %2").arg(GetCacheTableName(), sqlError.text());

		return retVal;
	}

	retVal.isOk = true;

	return retVal;
}


CViewBuilderComp::BuildResult CViewBuilderComp::ApplyChanges(
			imtduckdb::IDuckConnection& connection,
			const QDateTime& /*lastSourceUpdateTime*/) const
{
	// A view has no rows of its own to patch; recreating it is the whole update and costs nothing.
	return Rebuild(connection);
}


} // namespace imtcache
