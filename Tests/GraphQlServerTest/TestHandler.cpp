#include "TestHandler.h"

// Qt includes
#include <QtCore/QJsonObject>


QJsonObject TestHandler::CreateResponse(
	const QByteArray& commandId,
	const graphqlserver::RequestType requestType,
	const QJsonObject& inputParams,
	const graphqlserver::ResultKeys& resultKeys,
	QString& errorMessage) const
{
	QJsonObject retVal;
	QJsonObject data;
	data.insert("message", "hellow world!");
	retVal.insert("data", data);

	return retVal;
}



