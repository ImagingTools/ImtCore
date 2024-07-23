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
	if (cleanedCommandId.startsWith('/')) {
		cleanedCommandId = cleanedCommandId.remove(0, 1);
	}

	if (cleanedCommandId.endsWith('/')) {
		cleanedCommandId = cleanedCommandId.remove(cleanedCommandId.length() - 1, 1);
	}

	QByteArray myCommand;
	QByteArray slaveCommandId;

	istd::CIdManipBase::SplitId(cleanedCommandId, myCommand, slaveCommandId);

	if (myCommand != *m_commandIdAttrPtr) {
		return false;
	}

	if (slaveCommandId.isEmpty()) {
		return true;
	}

	for (int i = 0; i < m_slaveRequestHandlersCompPtr.GetCount(); ++i) {
		IRequestServlet* handlerPtr = m_slaveRequestHandlersCompPtr[i];
		if (handlerPtr != nullptr) {
			bool isSupported = handlerPtr->IsCommandSupported(slaveCommandId);
			if (isSupported) {
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

	if (commandId.startsWith('/')) {
		commandId = commandId.remove(0, 1);
	}

	if (commandId.endsWith('/')) {
		commandId = commandId.remove(commandId.length() - 1, 1);
	}

	QByteArray myCommand;
	QByteArray slaveCommandId;

	istd::CIdManipBase::SplitId(commandId, myCommand, slaveCommandId);

	const IProtocolEngine& engine = request.GetProtocolEngine();

	if (myCommand != *m_commandIdAttrPtr) {
		QString commandIdSafe = commandId;
		commandIdSafe = commandIdSafe.replace(QRegularExpression("[<>\":;()= .]"), "_");
		QByteArray body = QString("<html><head><title>Error</title></head><body><p>The requested command could not be executed. No servlet was found for the given command: '%1'</p></body></html>").arg(qPrintable(commandIdSafe)).toUtf8();
		QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

		ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));

		SendErrorMessage(0, QString("No request handler found for: '%1'").arg(qPrintable(commandId)));

		return responsePtr;
	}

	// Print all supported slave handlers of the API to the user:
	if (slaveCommandId.isEmpty()) {
		QByteArray body = QByteArray("<html><head><title>API-Info</title></head><body><p>Supported paths are: </p></body></html>");
		QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

		ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, body, reponseTypeId));

		return responsePtr;
	}

	for (int i = 0; i < m_slaveRequestHandlersCompPtr.GetCount(); ++i) {
		IRequestServlet* handlerPtr = m_slaveRequestHandlersCompPtr[i];
		Q_ASSERT(handlerPtr != nullptr);

		if (handlerPtr->IsCommandSupported(slaveCommandId)) {
			return handlerPtr->ProcessRequest(request, slaveCommandId);
		}
	}

	Q_ASSERT(false);

	return ConstResponsePtr();
}


} // namespace imtrest


