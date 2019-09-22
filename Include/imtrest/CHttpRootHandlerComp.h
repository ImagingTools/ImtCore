#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequestHandler.h>


namespace imtrest
{


class CHttpRootHandlerComp:
			public ilog::CLoggerComponentBase,
			virtual public IRequestHandler
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHttpRootHandlerComp);
		I_REGISTER_INTERFACE(IRequestHandler)
	I_END_COMPONENT

	// reimplemented (IRequestHandler)
	virtual bool ProcessRequest(
				const IRequest& request,
				const IProtocolEngine& protocolEngine) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();
};


} // namespace imtrest


