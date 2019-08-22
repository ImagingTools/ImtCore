#pragma once


// ImtCore includes
#include <imtbase/IObjectCollectionProvider.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/TDelegatedSupplierCompBase.h>


namespace imtbase
{


/**
	Composite object supplier delegating the calls to another one or accessing some underlaying object directly.
*/
class CDelegatedObjectContainerSupplierComp:
	public imtbase::TDelegatedSupplierCompBase<imtbase::IObjectCollectionProvider, imtbase::IObjectCollection>
{
public:
	typedef imtbase::TDelegatedSupplierCompBase<imtbase::IObjectCollectionProvider, imtbase::IObjectCollection> BaseClass;

	I_BEGIN_COMPONENT(CDelegatedObjectContainerSupplierComp);
	I_END_COMPONENT;

	// reimplemented (imtbase::IObjectCollectionProvider)
	virtual const imtbase::IObjectCollection* GetObjectCollection() const override;
};


} // namespace imtbase


