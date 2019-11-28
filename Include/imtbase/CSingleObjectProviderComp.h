#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/IObjectProvider.h>


namespace imtbase
{


/**
	Component implementation of the single data-object provider.
	The implementation uses the underlaying component reference to the object for providing the instance.
	The data model updates of the underlaying object will be delegated to the observers of this object.
*/
class CSingleObjectProviderComp:
			public icomp::CComponentBase,
			virtual public IObjectProvider
{
public:
	typedef icomp::CComponentBase BaseClass;
		
	I_BEGIN_COMPONENT(CSingleObjectProviderComp);
		I_REGISTER_INTERFACE(IObjectProvider);
		I_ASSIGN(m_dataObjectCompPtr, "Object", "Object", false, "Object");
		I_ASSIGN_TO(m_dataObjectModelCompPtr, m_dataObjectCompPtr, false);
	I_END_COMPONENT;

	CSingleObjectProviderComp();

	// reimplemented (IObjectProvider)
	virtual const istd::IChangeable* GetDataObject() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_REF(istd::IChangeable, m_dataObjectCompPtr);
	I_REF(imod::IModel, m_dataObjectModelCompPtr);

private:
	/**
		Model update bridge used for delegation of the changes from the underlaying object to the observers of this component.
	*/
	imod::CModelUpdateBridge m_modelUpdateBridge;
};


} // namespace imtbase


