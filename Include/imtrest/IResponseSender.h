#pragma once


// ACF includes
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtrest/IResponse.h>


namespace imtrest
{


class IRequest;
class IProtocolEngine;
class IResponse;

/*!
  \brief The IRequestSender class provides ability to direct send requests for Network Frameworks ex.: HTTPFramweork, MQTTFramework etc
 */
class IResponseSender: virtual public istd::IPolymorphic
{
public:

	/*!
		\brief SendRsponse sends resopnse to socket
		\param socketDescriptor - descriptor of socket
			\value = 0 - send to all
		\return result of sending request
			\retval true - success
			\retval false - fail
	*/
	virtual bool SendRsponse(const IResponse& resopnse, const long long& socketDescriptor = 0) const = 0;
};


} // namespace imtrest


