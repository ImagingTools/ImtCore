#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imttest/CTestInfo.h>


namespace imttest
{


/**
	Component implementation of ITestInfo interface with reference to the list of all defined tests and customers.
	\ingroup LicenseManagement
*/
class CTestInfoComp:
			public icomp::CComponentBase,
			public CTestInfo
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CTestInfo BaseClass2;

	I_BEGIN_COMPONENT(CTestInfoComp)
		I_REGISTER_INTERFACE(imttest::ITestInfo);
		I_REGISTER_INTERFACE(iser::IObject);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_testCollectionCompPtr, "TestCollection", "Test database", false, "TestCollection");
	I_END_COMPONENT

protected:
	I_REF(imtbase::IObjectCollection, m_testCollectionCompPtr);
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

};


} // namespace imtlic


