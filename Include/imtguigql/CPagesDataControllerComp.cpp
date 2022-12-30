#include <imtguigql/CPagesDataControllerComp.h>


namespace imtguigql
{


// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CPagesDataControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_pagesDataProviderCompPtr.IsValid()){
		return nullptr;
	}

	QByteArray commandId = gqlRequest.GetCommandId();
	imtbase::CTreeItemModel* retVal = m_pagesDataProviderCompPtr->CreateResponse(gqlRequest, errorMessage);
	if (retVal == nullptr){
		QString decoratedErrorMessage = QString("Cannot create response for the command with ID: %1. The reason was: %2").arg(qPrintable(commandId)).arg(errorMessage);

		errorMessage = decoratedErrorMessage;
	}

	return retVal;
}


} // namespace imtgql


