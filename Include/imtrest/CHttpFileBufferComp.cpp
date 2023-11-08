#include<imtrest/CHttpFileBufferComp.h>


// ImtCore includes
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


ConstResponsePtr CHttpFileBufferComp::OnGet(
			const QByteArray& commandId,
			const IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const CHttpRequest& request) const
{

	int statusCode = 200;
	QByteArray payload;

	return ConstResponsePtr(request.GetProtocolEngine().CreateResponse(request, statusCode, payload, QByteArrayLiteral("application/json; charset=utf-8")));
}

ConstResponsePtr CHttpFileBufferComp::OnPost(
			const QByteArray& commandId,
			const IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const CHttpRequest& request) const
{
	int statusCode = 200;
	QByteArray payload;
	/// \todo add creation from factory and FactComp
	imtbase::CObjectCollectionBase::DataPtr valuePtr;

	const QByteArray createdFileId = m_tempFileCollectionCompPtr->InsertNewObject(
		QByteArray(),
		QString(),
		QString(),
		valuePtr
		);

	return ConstResponsePtr(request.GetProtocolEngine().CreateResponse(request, statusCode, payload, QByteArrayLiteral("application/json; charset=utf-8")));
}



} // namespace imtrest


