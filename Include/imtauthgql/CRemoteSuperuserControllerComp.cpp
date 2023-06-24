#include <imtauthgql/CRemoteSuperuserControllerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtauthgql
{


// public methods

// reimplemented (ISuperuserController)

bool CRemoteSuperuserControllerComp::SetSuperuserPassword(const QByteArray& password) const
{
	imtgql::CGqlRequest request(imtgql::CGqlRequest::RT_QUERY, "UserAdd");
	imtgql::CGqlObject inputObject("input");
	inputObject.InsertField("Id", "su");

	QByteArray json = QString("{\"Username\": \"%1\", \"UserId\": \"%2\", \"Password\": \"%3\", \"Name\": \"%4\"}")
			.arg(qPrintable("su"))
			.arg(qPrintable("su"))
			.arg(qPrintable(password))
			.arg(qPrintable("username")).toUtf8();

	inputObject.InsertField ("Item", QVariant(json));
	request.AddParam(inputObject);

	imtgql::CGqlObject fieldObject("addedNotification");
	fieldObject.InsertField("Id");
	request.AddField(fieldObject);

	imtbase::CTreeItemModel responseModel;
	bool retVal = SendModelRequest(request, responseModel);
	if (!retVal){
	}

	return retVal;
}


} // namespace imtauthgql


