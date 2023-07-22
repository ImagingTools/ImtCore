#include <imtrest/CHttpFileProviderBasedServletComp.h>


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtrest/IRequestServlet.h>


namespace imtrest
{

QByteArray CHttpFileProviderBasedServletComp::GetMimeType(QByteArray fileSuffix) const
{
	QByteArray retval = "application/octet-stream";
	if (fileSuffix.startsWith('.')){
		fileSuffix = fileSuffix.remove(0,1);
	}
	auto setMimeTypeByExtention = [&retval, &fileSuffix](const QByteArray& mimeType, const std::initializer_list<QByteArray>& fileExtentions){
		bool mimeTypeSetByExtentionResult = false;
		for (const QString& ext: fileExtentions)
		{
			if (fileSuffix == ext)
			{
				retval = mimeType;
				mimeTypeSetByExtentionResult = true;
				break;
			}
		}
		return mimeTypeSetByExtentionResult;
	};

	if (setMimeTypeByExtention("text/html",						{"html", "htm"}));
	else if (setMimeTypeByExtention("application/json",			{"json"}));
	else if (setMimeTypeByExtention("application/pdf",			{"pdf", "lpdf", "pdc", "pdp", "spdf"}));
	else if (setMimeTypeByExtention("application/xhtml+xml",	{"xht", "xhtm", "xhtm"}));
	else if (setMimeTypeByExtention("application/xml-dtd",		{"dtd"}));
	else if (setMimeTypeByExtention("audio/mp4",				{"f4v", "mp4"}));
	else if (setMimeTypeByExtention("audio/aac",				{"aac"}));
	else if (setMimeTypeByExtention("audio/mpeg",				{"m4a", "m4b", "mp2", "mp4", "mpa", "mpga"}));
	else if (setMimeTypeByExtention("audio/vorbis",				{"oga", "ogg"}));
	else if (setMimeTypeByExtention("audio/ogg",				{"oga", "ogg", "oggstr", "flac"}));
	else if (setMimeTypeByExtention("audio/x-ms-wma",			{"wma"}));
	else if (setMimeTypeByExtention("audio/vnd.wave",			{"bfwav", "bwf", "bwg", "wav", "wave"}));
	else if (setMimeTypeByExtention("image/gif",				{"agif", "gfb", "gfw", "gif", "gifv"}));
	else if (setMimeTypeByExtention("image/jpeg",				{"j", "j2c", "j2k", "jfif", "jif", "jiff", "jng", "jp2", "jpc", "jpe", "jpeg", "jpf", "jpg", "jpg-large", "jpg2", "jps", "jpx", "jtf", "jxr", "ljp"}));
	else if (setMimeTypeByExtention("image/png",				{"apng", "png", "pns"}));
	else if (setMimeTypeByExtention("image/svg+xml",			{"svg", "svgz"}));
	else if (setMimeTypeByExtention("image/tiff",				{"kdk", "tif", "tiff", "tsr"}));
	else if (setMimeTypeByExtention("image/vnd.microsoft.icon",	{"ico", "icon"}));
	else if (setMimeTypeByExtention("image/vnd.wap.wbmp",		{"wbmp"}));
	else if (setMimeTypeByExtention("image/webp",				{"webp"}));
	else if (setMimeTypeByExtention("text/css",					{"css"}));
	else if (setMimeTypeByExtention("text/csv",					{"csv"}));
	else if (setMimeTypeByExtention("text/html",				{"appcache", "chm", "shtml", "dhtml", "edge", "hhc", "hhk", "hta", "htc", "htm", "html", "htx", "html", "mht", "mhtml", "oth", "rhtml", "shtm", "vbhtml"}));
	else if (setMimeTypeByExtention("text/javascript",			{"jgz", "js", "jsf", "jss", "rjs"}));
	else if (setMimeTypeByExtention("text/plain",				{"txt", "utxt"}));
	else if (setMimeTypeByExtention("text/php",					{"php"}));
	else if (setMimeTypeByExtention("text/xml",					{"xml"}));
	else if (setMimeTypeByExtention("text/markdown",			{"markdown", "md"}));
	else if (setMimeTypeByExtention("video/mpeg",				{"ffm", "m21", "m2v", "mp21", "mpeg", "mpg", "mpv"}));
	else if (setMimeTypeByExtention("video/mp4",				{"mp4"}));
	else if (setMimeTypeByExtention("video/ogg",				{"ogm", "ogm", "ogx"}));
	else if (setMimeTypeByExtention("video/quicktime",			{"hdmov", "mov", "qt", "qtm", "rts"}));
	else if (setMimeTypeByExtention("video/webm",				{"webm"}));
	else if (setMimeTypeByExtention("video/x-ms-wmv",			{"wmv"}));
	else if (setMimeTypeByExtention("video/x-flv",				{"f4f", "flv"}));
	else if (setMimeTypeByExtention("video/x-msvideo",			{"avi"}));
	else if (setMimeTypeByExtention("video/3gpp",				{"3gp", "3gp2", "3p2", "k3g"}));
	else if (setMimeTypeByExtention("video/3gpp2",				{"3g2", "3gpp2"}));

	retval.append("; charset=UTF-8");

	return retval;
}


// reimplemented (IRequestHandler)

IRequestServlet::ConstResponsePtr CHttpFileProviderBasedServletComp:: OnGet(
		const QByteArray& commandId,
		const imtrest::IRequest::CommandParams& commandParams,
		const HeadersMap& headers,
		const imtrest::CHttpRequest& request) const
{
	const IProtocolEngine& engine = request.GetProtocolEngine();
	QByteArray errorBody = "<html><head><title>Error</title></head><body><p>File resource was not found</p></body></html>";
	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	ConstResponsePtr errorResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_RESOURCE_NOT_AVAILABLE, errorBody, reponseTypeId));

	auto generateErrorResponsePtr = [&request, &engine, reponseTypeId](QByteArray errorBody, int _errorCode = IProtocolEngine::SC_INTERNAL_ERROR){
		qCritical() << __FILE__ << __LINE__ << "Error occurred" << errorBody;
		QByteArray generatedErrorBody = "<html><head><title>Error</title></head><body><p>";
		generatedErrorBody.append(errorBody);
		generatedErrorBody.append("</p></body></html>");
		return ConstResponsePtr(engine.CreateResponse(request, _errorCode, generatedErrorBody, reponseTypeId));
	};

	QByteArray body;

	QByteArray commandIdFileName;
	if (commandParams.contains(m_fileIdCommandParamAttrPtr->GetValue().toUtf8())){
		commandIdFileName = commandParams[m_fileIdCommandParamAttrPtr->GetValue().toUtf8()];
	}
	else{
		QByteArray commandId = request.GetCommandId();
		QByteArray commandIdBase = *m_commandIdAttrPtr;

		if (commandIdBase.endsWith('*')){
			commandIdBase.chop(1);
		}

		commandIdFileName = commandId;
		commandIdFileName = commandIdFileName.replace(commandIdBase, "");

		while (commandIdFileName.startsWith('/')){
			commandIdFileName.remove(0,1);
		}

		while (commandIdFileName.endsWith('/')){
			commandIdFileName.chop(1);
		}

	}

	bool loadRes = false;

	for (int i = 0; i < m_binaryDataProvidersCompPtr.GetCount(); ++i){

		if ((loadRes = (m_binaryDataProvidersCompPtr[i]->GetData(body, commandIdFileName)))){
			QByteArray fileSuffix;
			int index = commandIdFileName.lastIndexOf('.');
			if (index > 0){
				index = commandIdFileName.count() - index;
				fileSuffix = commandIdFileName.right(index);
			}
			reponseTypeId = this->GetMimeType(fileSuffix);
			break;
		}

	}
	ConstResponsePtr responsePtr;

	if (loadRes){
		responsePtr = ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, body, reponseTypeId));
	}

	else{
		responsePtr = generateErrorResponsePtr(QByteArray("Unable to open file ") + commandIdFileName, IProtocolEngine::SC_NOT_FOUND);
	}

	return responsePtr;
}


QByteArray CHttpFileProviderBasedServletComp::GetSupportedCommandId() const
{
	return *m_commandIdAttrPtr;
}


} // namespace imtrest


