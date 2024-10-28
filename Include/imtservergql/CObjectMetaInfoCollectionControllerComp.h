#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>


namespace imtservergql
{


class CObjectMetaInfoCollectionControllerComp:
			public CObjectCollectionControllerCompBase
{
public:
	typedef CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CObjectMetaInfoCollectionControllerComp);
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtservergql


