// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtcrypt/ISecretProtector.h>


namespace imtcrypt
{


/**
	Protection by the operating system. On Windows the data protection API binds a secret to the account running the process,
	or to the machine. Elsewhere there is no such service: the secret is kept as it is and the file holding it must be readable
	by the account of the process only.
*/
class CSystemSecretProtectorComp: public icomp::CComponentBase, virtual public ISecretProtector
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSystemSecretProtectorComp);
		I_REGISTER_INTERFACE(ISecretProtector);
		I_ASSIGN(m_isMachineScopeAttrPtr, "MachineScope", "Any account of the machine may read the secret, not only the one that protected it", true, false);
	I_END_COMPONENT;

	// reimplemented (ISecretProtector)
	virtual bool Protect(const QByteArray& secret, QByteArray& protectedSecret) const override;
	virtual bool Unprotect(const QByteArray& protectedSecret, QByteArray& secret) const override;

private:
	I_ATTR(bool, m_isMachineScopeAttrPtr);
};


} // namespace imtcrypt


