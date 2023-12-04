#include <imtdb/CRemoteDatabaseServerConnectionComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtdb
{


// reimplemented (IDatabaseServerConnectionChecker)

bool CRemoteDatabaseServerConnectionComp::CheckDatabaseConnection(QString& errorMessage) const
{
	QByteArray commandID = m_commandIdAttrPtr.IsValid() ? *m_commandIdAttrPtr : QByteArrayLiteral("GetDatabaseStatus");

	imtgql::CGqlRequest request(imtgql::CGqlRequest::RT_QUERY, commandID);

	imtbase::CTreeItemModel responseModel;
	bool retVal = SendModelRequest(request, responseModel);
	if (retVal){
		QString json = responseModel.toJSON();
		if (responseModel.ContainsKey("errors")){
			imtbase::CTreeItemModel* errorsModelPtr = responseModel.GetTreeItemModel("errors");
			if (errorsModelPtr != nullptr){
				if (errorsModelPtr->ContainsKey("message")){
					QString message = errorsModelPtr->GetData("message").toString();

					errorMessage = message;
				}
			}

			return false;
		}

		return true;
	}

	return false;
}


} // namespace imtdb


