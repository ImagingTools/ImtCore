#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>


namespace imtlicgql
{


class CInstallationCollectionControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CInstallationCollectionControllerComp);
	I_END_COMPONENT;

	CInstallationCollectionControllerComp();
protected:
	// reimplemented (imtgql::CObjectCollectionControllerCompBase)
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;


private:
//	I_REF(imtbase::IItemBasedRepresentationDataProvider, m_pagesDataProviderCompPtr);
};


} // namespace imtguigql


