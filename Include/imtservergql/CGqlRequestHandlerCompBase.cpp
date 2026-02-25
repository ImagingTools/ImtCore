// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlRequestHandler)

bool CGqlRequestHandlerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	QByteArray commandId = gqlRequest.GetCommandId();

	return m_commandIdsAttrPtr.FindValue(commandId) != -1;
}


imtbase::CTreeItemModel* CGqlRequestHandlerCompBase::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	Q_ASSERT(IsRequestSupported(gqlRequest));

	if (!IsRequestSupported(gqlRequest)){
		SendErrorMessage(0, QString("GQL handler is not supported GQL request with command-ID:'%1'").arg(qPrintable(gqlRequest.GetCommandId())));

		return nullptr;
	}

	return CreateInternalResponse(gqlRequest, errorMessage);
}


// protected methods

iprm::IParamsSetUniquePtr CGqlRequestHandlerCompBase::CreateContextParams(const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	return nullptr;
}


} // namespace imtservergql


