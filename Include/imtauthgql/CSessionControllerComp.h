#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>


namespace imtauthgql
{


class CSessionControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSessionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (CObjectCollectionControllerCompBase)
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtauthgql


