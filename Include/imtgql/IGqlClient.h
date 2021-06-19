#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>


namespace imtgql
{


class IGqlClient: virtual public istd::IPolymorphic
{
public:
	class ResponseHandler: virtual public istd::IPolymorphic
	{
	public:
		virtual void OnReply(const QByteArray& replyData) = 0;
	};

	virtual bool SendRequest(const IGqlRequest& request, ResponseHandler& responseHandler) const = 0;
};


} // namespace imtgql


