#include <imtguigql/CCommandsDataControllerComp.h>


namespace imtguigql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CCommandsDataControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray commandId;

	const QList<imtgql::CGqlObject>* paramsPtr = gqlRequest.GetParams();
	if (!paramsPtr->isEmpty()){
		commandId = paramsPtr->at(0).GetFieldArgumentValue("PageId").toByteArray();
	}

	if (commandId.isEmpty()){
		errorMessage = QString("A command with an empty ID was requested");

		return nullptr;
	}

	if(m_commandsDataProviderCompPtr.IsValid()){
		for (int index = 0; index < m_commandsDataProviderCompPtr.GetCount(); index++){
			const imtgql::IGqlRequestHandler* gqlRequestHandlerPtr = m_commandsDataProviderCompPtr[index];
			if (gqlRequestHandlerPtr != nullptr){
				if (gqlRequestHandlerPtr->IsRequestSupported(gqlRequest)){
					imtbase::CTreeItemModel* retVal = gqlRequestHandlerPtr->CreateResponse(gqlRequest, errorMessage);
					if (retVal == nullptr){
						QString decoratedErrorMessage = QString("Cannot create response for the command with ID: %1. The reason was: %2").arg(qPrintable(commandId)).arg(errorMessage);

						errorMessage = decoratedErrorMessage;
					}

					return retVal;
				}
			}
		}
	}

	return nullptr;
}


} // namespace imtgql


