#pragma once


// ImtCore includes
#include <imtbase/ICompositeObjectProvider.h>
#include <imtbase/IObjectContainer.h>
#include <imtbase/TDelegatedSupplierCompBase.h>


namespace imtbase
{


/**
	Image supplier delegating the calls to another one or accessing some point cloud object directly.
*/
class CDelegatedObjectContainerSupplierComp:
	public imtbase::TDelegatedSupplierCompBase<imtbase::ICompositeObjectProvider, imtbase::IObjectContainer>
{
public:
	typedef imtbase::TDelegatedSupplierCompBase<imtbase::ICompositeObjectProvider, imtbase::IObjectContainer> BaseClass;

	I_BEGIN_COMPONENT(CDelegatedObjectContainerSupplierComp);
	I_END_COMPONENT;

	// reimplemented (imtbase::ICompositeObjectProvider)
	virtual const imtbase::IObjectContainer* GetCompositeObject() const override;
};


} // namespace imtbase


