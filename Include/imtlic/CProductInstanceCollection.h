#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtlic/IProductInstanceInfoProvider.h>
#include <imtlic/CProductInstanceInfo.h>


namespace imtlic
{


/**
	\ingroup LicenseManagement
*/
class CProductInstanceCollection: public imtbase::TAggergatedObjectCollectionWrap<imtlic::IProductInstanceInfoProvider, CProductInstanceInfo>
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<imtlic::IProductInstanceInfoProvider, CProductInstanceInfo> BaseClass;
	
	CProductInstanceCollection();

	// reimplemented (imtbase::IProductLicensingInfoProvider)
	virtual const imtbase::ICollectionInfo& GetProductInstanceList() const override;
	virtual const imtlic::IProductInstanceInfo* GetProductInstance(const QByteArray& instanceId) const override;
};


} // namespace imtlic


