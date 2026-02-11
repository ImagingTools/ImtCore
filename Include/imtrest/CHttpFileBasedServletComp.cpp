// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpFileBasedServletComp.h>


// Qt includes
#include <QtCore/QFile>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

// reimplemented (IRequestHandler)

bool CHttpFileBasedServletComp::IsCommandSupported(const QByteArray& commandId) const
{
	return (commandId  == *m_commandIdAttrPtr);
}


ConstResponsePtr CHttpFileBasedServletComp::ProcessRequest(const IRequest& request, const QByteArray& /*subCommandId*/) const
{
	const IProtocolEngine& engine = request.GetProtocolEngine();
	QByteArray errorBody = "<html><head><title>Error</title></head><body><p>File resource was not found</p></body></html>";
	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	ConstResponsePtr errorResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_RESOURCE_NOT_AVAILABLE, errorBody, reponseTypeId).PopInterfacePtr());

	if (!m_fileTemplatePathCompPtr.IsValid()){
		return errorResponsePtr;
	}

	QString templateFilePath = m_fileTemplatePathCompPtr->GetPath();
	if (templateFilePath.isEmpty()){
		return errorResponsePtr;
	}

	QFile templateFile(templateFilePath);
	if (!templateFile.open(QIODevice::ReadOnly)){
		return errorResponsePtr;
	}

	QByteArray body = templateFile.readAll();

	ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, body, reponseTypeId).PopInterfacePtr());

	return responsePtr;
}


} // namespace imtrest


