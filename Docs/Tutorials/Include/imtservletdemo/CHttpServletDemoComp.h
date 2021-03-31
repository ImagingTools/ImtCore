#pragma once


// ImtCore includes
#include <imtrest/CHttpServletCompBase.h>


namespace imtservletdemo
{


class CHttpServletDemoComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CHttpServletDemoComp);
	I_END_COMPONENT

	virtual const imtrest::IResponse* OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;
	virtual const imtrest::IResponse* OnPost(
				const QByteArray& commandId,
				const QByteArray body,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;
};


} // namespace imtservletdemo


