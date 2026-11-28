// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtcrypt/ISecretProtector.h>
#include <imtcrypt/ISecretTextParam.h>


namespace imtcrypt
{


/**
	Secret text parameter that is stored protected by an ISecretProtector, e.g. inside the
	server settings file. Without a working protector nothing is stored.
*/
class CProtectedTextParamComp:
			public ilog::CLoggerComponentBase,
			public iprm::CTextParam,
			virtual public ISecretTextParam
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef iprm::CTextParam BaseClass2;

	I_BEGIN_COMPONENT(CProtectedTextParamComp);
		I_REGISTER_INTERFACE(iprm::ITextParam);
		I_REGISTER_INTERFACE(ISecretTextParam);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_secretProtectorCompPtr, "SecretProtector", "Protection of the secret at rest", true, "SecretProtector");
	I_END_COMPONENT;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	I_REF(ISecretProtector, m_secretProtectorCompPtr);
};


} // namespace imtcrypt


