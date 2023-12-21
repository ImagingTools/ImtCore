#include <imtgql/CGqlRequestHandlerCompBase.h>


namespace imtgql
{


// public methods

// reimplemented (imtgql::IGqlRequestHandler)

bool CGqlRequestHandlerCompBase::IsRequestSupported(const CGqlRequest& gqlRequest) const
{
	QByteArray commandId = gqlRequest.GetCommandId();

	return m_commandIdsAttrPtr.FindValue(commandId) != -1;
}


imtbase::CTreeItemModel* CGqlRequestHandlerCompBase::CreateResponse(const CGqlRequest& gqlRequest, QString& errorMessage) const
{
	Q_ASSERT(IsRequestSupported(gqlRequest));

	if (!IsRequestSupported(gqlRequest)){
		SendErrorMessage(0, QString("GQL handler is not supported GQL Request with command %1").arg(qPrintable(gqlRequest.GetCommandId())));

		return nullptr;
	}

	return CreateInternalResponse(gqlRequest, errorMessage);
}


// protected methods

iprm::IParamsSet* CGqlRequestHandlerCompBase::CreateContextParams(const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	return nullptr;
}


} // namespace imtgql


