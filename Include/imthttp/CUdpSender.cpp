#include <imthttp/CUdpSender.h>


// Qt includes
#include <QtCore/QDataStream>

// ImtCore includes
#include <imthttp/IResponse.h>
#include <imthttp/IProtocolEngine.h>


namespace imthttp
{


// public methods

CUdpSender::CUdpSender(CUdpRequest* request)
{
	m_socket = request->m_socket;
	m_address = request->m_address;
	m_port = request->m_port;
    m_requestId = request->GetRequestId();
}


// reimplemented (IRequest)

bool CUdpSender::SendResponse(ConstResponsePtr& response) const
{
	if (!response.IsValid()){
		return false;
	}

	const QByteArray& contentData = response->GetData();
    bool result =m_socket->writeDatagram(contentData,m_address,m_port);
    Q_EMIT sended(m_requestId);

    return result;
}


bool imthttp::CUdpSender::SendRequest(ConstRequestPtr& /*reguest*/) const
{
	return false;
}


} // namespace imthttp


