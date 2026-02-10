// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/IBinaryDataController.h>
#include <imtrest/CHttpFileProviderBasedServletComp.h>


namespace imtrest
{


class CHttpFileControllerServletComp: public CHttpFileProviderBasedServletComp
{
public:
	typedef CHttpFileProviderBasedServletComp BaseClass;

	I_BEGIN_COMPONENT(CHttpFileControllerServletComp)
		I_REGISTER_INTERFACE(IRequestServlet)
		I_ASSIGN_MULTI_0(m_binaryDataControllersCompPtr, "BinaryDataControllers", "Binary data controllers", false);
	I_END_COMPONENT

protected:
	// reimplemented (CHttpServletCompBase)
	ConstResponsePtr OnPost(
				const QByteArray& commandId,
				const IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const CHttpRequest& request) const override;
	ConstResponsePtr OnDelete(
				const QByteArray& commandId,
				const IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const CHttpRequest& request) const override;

private:
	I_MULTIREF(imtrest::IBinaryDataController, m_binaryDataControllersCompPtr);
};


} // namespace imtrest


