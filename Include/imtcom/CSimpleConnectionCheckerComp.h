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

	// reimplemented (IConnectionStatusProvider)
	virtual ConnectionStatus GetConnectionStatus() const override;

private:
	I_ATTR(QByteArray, m_commandIdAttrPtr);

	ConnectionStatus m_status;
};


} // namespace imtcom


