// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtservice/CUrlConnectionParam.h>


namespace imtservice
{


class CUrlConnectionParamComp:
			public icomp::CComponentBase,
			public CUrlConnectionParam
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUrlConnectionParamComp);
		I_REGISTER_INTERFACE(imtcom::IServerConnectionInterface);
		I_REGISTER_INTERFACE(imtservice::IServiceConnectionParam);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;
};


} // namespace imtservice



