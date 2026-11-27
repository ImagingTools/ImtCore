// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/CIdParam.h>


namespace imttest
{


/**
	An id taken from an environment variable of the test run, such as a token that a setup step of the run created.
	It is not registered as serializable, so the value is never persisted.
*/
class CEnvironmentIdParamComp:
			public icomp::CComponentBase,
			public iprm::CIdParam
{
public:
	using BaseClass		= icomp::CComponentBase;
	using BaseClass2	= iprm::CIdParam;

	I_BEGIN_COMPONENT(CEnvironmentIdParamComp);
		I_REGISTER_INTERFACE(iprm::IIdParam);
		I_ASSIGN(m_environmentVariableAttrPtr, "EnvironmentVariable", "Name of the environment variable that holds the id", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QByteArray, m_environmentVariableAttrPtr);
};


} // namespace imttest


