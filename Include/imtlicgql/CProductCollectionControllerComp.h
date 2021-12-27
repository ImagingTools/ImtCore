#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>


namespace imtlicgql
{


class CProductCollectionControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductCollectionControllerComp);
	I_END_COMPONENT;

	CProductCollectionControllerComp();
protected:
	// reimplemented (imtgql::CObjectCollectionControllerCompBase)
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;


private:
//	I_REF(imtbase::IItemBasedRepresentationDataProvider, m_pagesDataProviderCompPtr);
};


} // namespace imtguigql


