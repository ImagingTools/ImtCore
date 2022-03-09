#pragma once


// ImtCore includes
#include <imtbase/IBinaryDataController.h>
#include <imtrest/CHttpFileProviderBasedServletComp.h>


namespace imtrest
{


class CHttpFileControllerServletComp: public CHttpFileProviderBasedServletComp
{
public:
	typedef CHttpFileProviderBasedServletComp BaseClass;

	I_BEGIN_COMPONENT(CHttpFileControllerServletComp)
		I_REGISTER_INTERFACE(IRequestServlet)
		I_ASSIGN_MULTI_0(m_binaryDataControllersCompPtr, "BinaryDataProviders", "Binary data providers", true);

	I_END_COMPONENT

protected:
	// reimplemented (CHttpServletCompBase)
	ConstResponsePtr OnPost(
				const QByteArray& commandId,
				const IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const CHttpRequest& request
				) const override;
	ConstResponsePtr OnDelete(
				const QByteArray& commandId,
				const IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const CHttpRequest& request
				) const override;

private:
	I_MULTIREF(imtbase::IBinaryDataController, m_binaryDataControllersCompPtr);
};


} // namespace imtrest


