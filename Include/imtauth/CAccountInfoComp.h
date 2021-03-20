#pragma once


// ACF includes
#include <iauth/IRightsProvider.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IPersonInfo.h>
#include <imtauth/CAccountInfo.h>


namespace imtauth
{


/**
	Component implementation of IAccountInfo interface.
	\ingroup LicenseManagement
*/
class CAccountInfoComp:
			public icomp::CComponentBase,
			public CAccountInfo
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CAccountInfo BaseClass2;

	I_BEGIN_COMPONENT(CAccountInfoComp)
		I_REGISTER_INTERFACE(IAccountInfo);
		I_REGISTER_INTERFACE(iser::IObject);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_accountOwnerCompPtr, "AccountOwner", "Account owner", true, "AccountOwner");
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(IPersonInfo, m_accountOwnerCompPtr);
};


} // namespace imtauth


