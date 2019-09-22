#include <imtrest/CHttpProtocolEngineComp.h>


// ImtCore includes
#include <imtrest/http_parser.h>


namespace imtrest
{


static int onMessageBegin(http_parser *httpParser)
{
	return 0;
}


static int onUrl(http_parser *httpParser, const char *at, size_t length)
{
	return 0;
}


static int onStatus(http_parser *httpParser, const char *at, size_t length)
{
	return 0;
}


static int onHeaderField(http_parser *httpParser, const char *at, size_t length)
{
	return 0;
}


static int onHeaderValue(http_parser *httpParser, const char *at, size_t length)
{
	return 0;
}


static int onHeadersComplete(http_parser *httpParser)
{
	return 0;
}


static int onBody(http_parser *httpParser, const char *at, size_t length)
{
	return 0;
}


static int onMessageComplete(http_parser *httpParser)
{
	return 0;
}


static int onChunkHeader(http_parser *httpParser)
{
	return 0;
}


static int onChunkComplete(http_parser *httpParser)
{
	return 0;
}

http_parser s_httpParser;

http_parser_settings s_httpParserSettings
{
	&onMessageBegin,
	&onUrl,
	&onStatus,
	&onHeaderField,
	&onHeaderValue,
	&onHeadersComplete,
	&onBody,
	&onMessageComplete,
	&onChunkHeader,
	&onChunkComplete
};


// public methods

// reimplemented (IProtocolEngine)

QByteArray CHttpProtocolEngineComp::GetProtocolTypeId() const
{
	return "http";
}


const iser::IVersionInfo* CHttpProtocolEngineComp::GetProtocolVersion() const
{
	return nullptr;
}


bool CHttpProtocolEngineComp::GetRequestState(const QByteArray& data, RequestState& state) const
{
	http_parser_init(&s_httpParser, HTTP_REQUEST);

	const auto parsed = http_parser_execute(
				&s_httpParser,
				&s_httpParserSettings,
				data.constData(),
				size_t(data.size()));
	if (int(parsed) < data.size()) {
		qDebug("Parse error: %d", s_httpParser.http_errno);

		return false;
	}

	return true;
}


IRequest* CHttpProtocolEngineComp::CreateRequest(const QByteArray& data) const
{
	return nullptr;
}


IResponse* CHttpProtocolEngineComp::CreateResponse(const QByteArray& data, const IRequest& request, int statusCode) const
{
	return nullptr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CHttpProtocolEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


} // namespace imtrest


