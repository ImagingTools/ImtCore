#pragma once


// ImtCore includes
#include <imtbase/ICompositeObjectProvider.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/TDelegatedSupplierCompBase.h>


namespace imtbase
{


/**
	Composite object supplier delegating the calls to another one or accessing some underlaying object directly.
*/
class CDelegatedObjectContainerSupplierComp:
	public imtbase::TDelegatedSupplierCompBase<imtbase::ICompositeObjectProvider, imtbase::IObjectCollection>
{
public:
	typedef imtbase::TDelegatedSupplierCompBase<imtbase::ICompositeObjectProvider, imtbase::IObjectCollection> BaseClass;

	I_BEGIN_COMPONENT(CDelegatedObjectContainerSupplierComp);
	I_END_COMPONENT;

	// reimplemented (imtbase::ICompositeObjectProvider)
	virtual const imtbase::IObjectCollection* GetCompositeObject() const override;
};


} // namespace imtbase


