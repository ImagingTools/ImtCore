#pragma once


// ImtCore includes
#include <imtcom/IConnectionStatusProvider.h>
#include <imtgql/TClientRequestManagerCompWrap.h>


namespace imtcom
{


class CSimpleConnectionCheckerComp:
			public imtgql::CClientRequestManagerCompBase,
			virtual public IConnectionStatusProvider
{
public:
	typedef imtgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSimpleConnectionCheckerComp);
		I_REGISTER_INTERFACE(IConnectionStatusProvider);
	I_END_COMPONENT;

	// reimplemented (IConnectionStatusProvider)
	virtual ConnectionStatus GetConnectionStatus() const override;
};


} // namespace imtcom


