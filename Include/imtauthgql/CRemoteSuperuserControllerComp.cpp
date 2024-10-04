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
	imtgql::CGqlObject inputObject;
	inputObject.InsertField("Id", "su");
	inputObject.InsertField("ProductId", qPrintable(*m_productIdAttrPtr));

	QByteArray json = QString("{\"Username\": \"%1\", \"UserId\": \"%2\", \"Password\": \"%3\", \"Name\": \"%4\"}")
			.arg(qPrintable("su"))
			.arg(qPrintable("su"))
			.arg(qPrintable(password))
			.arg(qPrintable("username")).toUtf8();

	inputObject.InsertField ("Item", QVariant(json));
	request.AddParam("input", inputObject);

	imtgql::CGqlObject fieldObject;
	fieldObject.InsertField("Id");
	request.AddField("addedNotification", fieldObject);

	imtbase::CTreeItemModel responseModel;

	bool retVal = SendModelRequest(request, responseModel);

	return retVal;
}


} // namespace imtauthgql


