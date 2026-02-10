// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CDelegatedServletComp.h>


// Qt includes
#include <QtCore/QRegularExpression>

// ACF includes
#include <istd/CIdManipBase.h>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

// reimplemented (IRequestHandler)

bool CDelegatedServletComp::IsCommandSupported(const QByteArray& commandId) const
{
	QByteArray cleanedCommandId = commandId;
	if (cleanedCommandId.startsWith('/')){
		cleanedCommandId = cleanedCommandId.remove(0, 1);
	}

	if (cleanedCommandId.endsWith('/')){
		cleanedCommandId = cleanedCommandId.remove(cleanedCommandId.length() - 1, 1);
	}

	QByteArray slaveCommandId = commandId;

	QByteArray myCommandId = *m_commandIdAttrPtr;
	if (!myCommandId.isEmpty()){
		QByteArray command;
		istd::CIdManipBase::SplitId(cleanedCommandId, command, slaveCommandId);
		if (command != myCommandId){
			return false;
		}

		if (slaveCommandId.isEmpty()){
			return true;
		}
	}

	for (int i = 0; i < m_slaveRequestHandlersCompPtr.GetCount(); ++i){
		IRequestServlet* handlerPtr = m_slaveRequestHandlersCompPtr[i];
		if (handlerPtr != nullptr){
			bool isSupported = handlerPtr->IsCommandSupported(slaveCommandId);
			if (isSupported){
				return true;
			}
		}
	}

	return false;
}


ConstResponsePtr CDelegatedServletComp::ProcessRequest(const IRequest& request, const QByteArray& subCommandId) const
{
	QByteArray commandId = subCommandId.isEmpty() ? request.GetCommandId() : subCommandId;

	Q_ASSERT(!commandId.isEmpty());
	Q_ASSERT(IsCommandSupported(commandId));

	if (commandId.startsWith('/')){
		commandId = commandId.remove(0, 1);
	}

	if (commandId.endsWith('/')){
		commandId = commandId.remove(commandId.length() - 1, 1);
	}

	QByteArray command;
	QByteArray slaveCommandId = commandId;

	const IProtocolEngine& engine = request.GetProtocolEngine();

	QByteArray myCommandId = *m_commandIdAttrPtr;

	if (!myCommandId.isEmpty()){
		istd::CIdManipBase::SplitId(commandId, command, slaveCommandId);
		if (command != myCommandId){
			QString commandIdSafe = commandId;
			commandIdSafe = commandIdSafe.replace(QRegularExpression("[<>\":;()= .]"), "_");
			QByteArray body = QString("<html><head><title>Error</title></head><body><p>The requested command could not be executed. No servlet was found for the given command: '%1'</p></body></html>").arg(qPrintable(commandIdSafe)).toUtf8();
			QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

			ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId).PopInterfacePtr());

			SendErrorMessage(0, QString("No request handler found for: '%1'").arg(qPrintable(commandId)));

			return responsePtr;
		}
	}

	// Print all supported slave handlers of the API to the user:
	if (slaveCommandId.isEmpty()){
		QStringList slavehandlers;

		QByteArray body = QString("<html><head><title>API-Info</title></head><body><p>Supported paths are: %1</p></body></html>").arg(slavehandlers.join("\n")).toUtf8();
		QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

		ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId).PopInterfacePtr());

		return responsePtr;
	}

	for (int i = 0; i < m_slaveRequestHandlersCompPtr.GetCount(); ++i){
		IRequestServlet* handlerPtr = m_slaveRequestHandlersCompPtr[i];
		Q_ASSERT(handlerPtr != nullptr);

		if (handlerPtr->IsCommandSupported(slaveCommandId)){
			return handlerPtr->ProcessRequest(request, slaveCommandId);
		}
	}

	Q_ASSERT(false);

	return ConstResponsePtr();
}


} // namespace imtrest


