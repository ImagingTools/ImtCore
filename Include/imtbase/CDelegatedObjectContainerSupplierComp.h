#pragma once


// ImtCore includes
#include <imtbase/ICompositeObjectProvider.h>
#include <imtbase/IObjectManager.h>
#include <imtbase/TDelegatedSupplierCompBase.h>


namespace imtbase
{


/**
	Composite object supplier delegating the calls to another one or accessing some underlaying object directly.
*/
class CDelegatedObjectContainerSupplierComp:
	public imtbase::TDelegatedSupplierCompBase<imtbase::ICompositeObjectProvider, imtbase::IObjectManager>
{
public:
	typedef imtbase::TDelegatedSupplierCompBase<imtbase::ICompositeObjectProvider, imtbase::IObjectManager> BaseClass;

	I_BEGIN_COMPONENT(CDelegatedObjectContainerSupplierComp);
	I_END_COMPONENT;

	// reimplemented (imtbase::ICompositeObjectProvider)
	virtual const imtbase::IObjectManager* GetCompositeObject() const override;
};


} // namespace imtbase


