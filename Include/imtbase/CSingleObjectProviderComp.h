#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <icomp/CComponentBase.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/IObjectProvider.h>


namespace imtbase
{


/**
	Implementation of the data object provider (single instance)
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

	// reimplemented (BaseFramework::IMultiObjectProvider)
	virtual int GetObjectCount() const;
	virtual const istd::IChangeable* GetObject(int objectIndex) const;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();
	virtual void OnComponentDestroyed();

protected:
	I_REF(istd::IChangeable, m_dataObjectCompPtr);
	I_REF(imod::IModel, m_dataObjectModelCompPtr);

private:
	imod::CModelUpdateBridge m_modelUpdateBridge;
};


} // namespace imtbase


