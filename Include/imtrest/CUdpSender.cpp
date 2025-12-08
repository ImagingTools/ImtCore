#include <imtrest/CUdpSender.h>


// Qt includes
#include <QtCore/QDataStream>

// ImtCore includes
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

CUdpSender::CUdpSender(CUdpRequest* request)
{
	m_socket = request->m_socket;
	m_address = request->m_address;
	m_port = request->m_port;
}


// reimplemented (IRequest)

bool CUdpSender::SendResponse(ConstResponsePtr& response) const
{
	if (!response.IsValid()){
		return false;
	}

	int protocolStatusCode = -1;
	QByteArray statusLiteral;

	bool retVal = response->GetProtocolEngine().GetProtocolStatusCode(response->GetStatusCode(), protocolStatusCode, statusLiteral);
	if (!retVal){
		return false;
	}

	const QByteArray& contentData = response->GetData();
	retVal = m_socket->writeDatagram(contentData,m_address,m_port);

	return false;
}


bool imtrest::CUdpSender::SendRequest(ConstRequestPtr& /*reguest*/) const
{
	return false;
}


} // namespace imtrest


