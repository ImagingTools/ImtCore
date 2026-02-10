// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtcom/IConnectionStatusProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtcom
{


class CSimpleConnectionCheckerComp:
			public imtclientgql::CClientRequestManagerCompBase,
			virtual public IConnectionStatusProvider
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSimpleConnectionCheckerComp);
		I_REGISTER_INTERFACE(IConnectionStatusProvider);
		I_ASSIGN(m_commandIdAttrPtr, "Command-ID", "Request command-ID", true, "");
	I_END_COMPONENT;

	CSimpleConnectionCheckerComp();
	// reimplemented (IConnectionStatusProvider)
	virtual ConnectionStatus GetConnectionStatus() const override;

private:
	I_ATTR(QByteArray, m_commandIdAttrPtr);

	ConnectionStatus m_status;
};


} // namespace imtcom


