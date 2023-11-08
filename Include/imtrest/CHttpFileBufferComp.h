#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionComp.h>
#include<imtrest/CHttpServletCompBase.h>


namespace imtrest
{


/**
	Temporary storage for uploaded files to the server of files prepared to download to client
*/
class CHttpFileBufferComp:
			public CHttpServletCompBase
{

public:
	typedef CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CHttpFileBufferComp);
	I_END_COMPONENT;


protected:
	// reimplemented (CHttpServletCompBase)
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

private:
	I_REF(imtbase::IObjectCollection, m_tempFileCollectionCompPtr);
};


} // namespace imtrest


