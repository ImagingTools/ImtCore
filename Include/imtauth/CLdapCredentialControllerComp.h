// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/ICredentialController.h>


namespace imtauth
{


class CLdapCredentialControllerComp:
			public icomp::CComponentBase,
			public imtauth::ICredentialController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLdapCredentialControllerComp);
		I_REGISTER_INTERFACE(imtauth::ICredentialController);
	I_END_COMPONENT;

protected:
	// reimplemented (iser::ICredentialController)
	virtual bool CheckCredential(const QByteArray& login, const QByteArray& password) const override;
};


} // namespace imtauth


