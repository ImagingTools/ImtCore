#include <imtdb/CRemoteDatabaseServerConnectionComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtdb
{


// reimplemented (IDatabaseServerConnectionChecker)

bool CRemoteDatabaseServerConnectionComp::IsDatabaseServerConnected(QString& errorMessage) const
{
	imtgql::CGqlRequest request(imtgql::CGqlRequest::RT_QUERY, "GetDatabaseStatus");

	imtbase::CTreeItemModel responseModel;
	bool retVal = SendModelRequest(request, responseModel);
	if (retVal){
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


