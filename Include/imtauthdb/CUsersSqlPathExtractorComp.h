#pragma once


// ImtCore includes
#include <imtdb/CPostgresXPathExtractorComp.h>


namespace imtauthdb
{


class CUsersSqlPathExtractorComp: public imtdb::CPostgresXPathExtractorComp
{
public:
	typedef imtdb::CPostgresXPathExtractorComp BaseClass;
	
	I_BEGIN_COMPONENT(CUsersSqlPathExtractorComp)
	I_END_COMPONENT

	// reimplemented (ISqlJsonXPathExtractor)
	virtual QString ExtractXPath(
				const QString& jsonKey,
				const QString& fieldId,
				QMetaType::Type metaType = QMetaType::QString,
				const QString& tableAlias = QString()) const override;
};


} // namespace imtauthdb


