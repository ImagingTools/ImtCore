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

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual ConstResponsePtr OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;
	virtual ConstResponsePtr OnPost(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;
};


} // namespace imtservletdemo


