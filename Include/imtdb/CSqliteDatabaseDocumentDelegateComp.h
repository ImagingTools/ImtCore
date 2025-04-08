#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>


namespace imtdb
{


class CSqliteDatabaseDocumentDelegateComp: public imtdb::CSqlDatabaseDocumentDelegateComp
{
public:
	typedef imtdb::CSqlDatabaseDocumentDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CSqliteDatabaseDocumentDelegateComp)
	I_END_COMPONENT

	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual QString GetBaseSelectionQuery() const override;
	virtual QByteArray CreateJsonBuildObjectQuery(const QVariantMap& paramMap) const override;
	virtual QString CreateJsonExtractSql(
				const QString& jsonName,
				const QString& key,
				QMetaType::Type metaType = QMetaType::QString,
				const QString& tableAlias = QString("root")) const override;
	virtual bool CreatePaginationQuery(int offset, int count, QByteArray& paginationQuery) const override;
	virtual bool CreateTimeFilterQuery(const imtbase::ITimeFilterParam& timeFilter, QString& timeFilterQuery) const override;
};


} // namespace imtdb


