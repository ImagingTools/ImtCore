#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>


namespace imtgql
{


class CCollectionObjectProviderComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionObjectProviderComp);
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtgql


