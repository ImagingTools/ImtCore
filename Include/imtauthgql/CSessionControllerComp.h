#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>


namespace imtauthgql
{


class CSessionControllerComp: public imtservergql::CObjectCollectionControllerCompBase
{
public:
	typedef imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSessionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (CObjectCollectionControllerCompBase)
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtauthgql


