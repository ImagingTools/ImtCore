// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QSslConfiguration>

// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/CEnableableParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtcom/ISslConfigurationManager.h>


/**
	The SSL settings of a server in the tests: only whether the encryption is on.
*/
class CTestSslParamsComp:
			public icomp::CComponentBase,
			public iprm::CParamsSet
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestSslParamsComp);
		I_REGISTER_INTERFACE(iprm::IParamsSet);
	I_END_COMPONENT;

	CTestSslParamsComp()
	{
		SetEditableParameter(imtcom::ISslConfigurationManager::ParamKeys::s_enableSslModeParamKey, &enableParam);
	}

	iprm::CEnableableParam enableParam{false};
};


/**
	The manager of the SSL configuration in the tests: it cannot make the configuration (a certificate is missing or broken), or it makes
	an empty one.
*/
class CTestSslManagerComp:
			public icomp::CComponentBase,
			virtual public imtcom::ISslConfigurationManager
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestSslManagerComp);
		I_REGISTER_INTERFACE(imtcom::ISslConfigurationManager);
	I_END_COMPONENT;

	bool isFailing = true;

	// reimplemented (imtcom::ISslConfigurationManager)
	bool CreateSslConfiguration(const iprm::IParamsSet& /*params*/, QSslConfiguration& output) const override
	{
		if (isFailing){
			return false;
		}

		output = QSslConfiguration();

		return true;
	}
};


