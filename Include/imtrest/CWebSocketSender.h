#pragma once


// ImtCore includes
#include <imtrest/ISender.h>


namespace imtrest
{


class CWebSocketSender: public QObject, virtual public ISender
{
	Q_OBJECT
public:
	// reimplemented (ISender)
	virtual bool SendResponse(const IResponse& response) const override;
	virtual bool SendRequest(const IRequest& reguest) const override;
};


} // namespace imtrest


