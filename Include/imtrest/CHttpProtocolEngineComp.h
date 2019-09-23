#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/http_parser.h>


namespace imtrest
{


class CHttpProtocolEngineComp: public ilog::CLoggerComponentBase, virtual public IProtocolEngine
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHttpProtocolEngineComp);
		I_REGISTER_INTERFACE(IProtocolEngine)
	I_END_COMPONENT

	// reimplemented (IProtocolEngine)
	virtual QByteArray GetProtocolTypeId() const override;
	virtual const iser::IVersionInfo* GetProtocolVersion() const override;
	virtual IRequest* CreateRequest(const QAbstractSocket* socketPtr, const IRequestHandler& requestHandler) const override;
	virtual IResponse* CreateResponse(const QIODevice* devicePtr, const QByteArray& data, const IRequest& request, int statusCode) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();
};


} // namespace imtrest


