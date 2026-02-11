// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/ICredentialController.h>
#include <imtcrypt/IHashGenerator.h>


namespace imtauth
{


class CCredentialControllerComp:
			public icomp::CComponentBase,
			public imtauth::ICredentialController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCredentialControllerComp);
		I_REGISTER_INTERFACE(imtauth::ICredentialController);
		I_ASSIGN(m_hashCalculatorCompPtr, "HashCalculator", "Hash calculator", true, "HashCalculator");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", true, "UserCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (iser::ICredentialController)
	virtual bool CheckCredential(const QByteArray& login, const QByteArray& password) const override;

private:
	I_REF(imtcrypt::IHashGenerator, m_hashCalculatorCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtauth
