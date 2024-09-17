#pragma once


// ImtCore includes
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>
#include <imtauth/IRole.h>


namespace imtauthdb
{


class CRoleDatabaseDelegateComp: public imtdb::CSqlJsonDatabaseDelegateComp
{
public:
	typedef imtdb::CSqlJsonDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CRoleDatabaseDelegateComp)
	I_END_COMPONENT

	// reimplemented (imtdb::CSqlJsonDatabaseDelegateComp)
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual QByteArray CreateDeleteObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr,
				bool useExternDelegate = true) const override;
};


} // namespace imtauthdb


