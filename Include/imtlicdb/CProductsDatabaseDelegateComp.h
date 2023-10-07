#pragma once


// ImtCore includes
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>
#include <imtlic/IProductLicensingInfo.h>


namespace imtlicdb
{


class CProductsDatabaseDelegateComp: public imtdb::CSqlJsonDatabaseDelegateComp
{
public:
	typedef imtdb::CSqlJsonDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CProductsDatabaseDelegateComp)
	I_END_COMPONENT

	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
};


} // namespace imtlicdb


