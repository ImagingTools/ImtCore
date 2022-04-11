#include <imtrest/CHttpHtmlFolderBasedServletComp.h>


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


CHttpHtmlFolderBasedServletComp::CHttpHtmlFolderBasedServletComp()
{
	QFile messagePartFile;

	messagePartFile.setFileName(":/Html/index_files_begin.dom");
	messagePartFile.open(QFile::ReadOnly);
	m_beginOfMessage = messagePartFile.readAll();
	messagePartFile.close();

	messagePartFile.setFileName(":/Html/index_files_end.dom");
	messagePartFile.open(QFile::ReadOnly);
	m_endOfMessage = messagePartFile.readAll();
	messagePartFile.close();
}


QByteArray CHttpHtmlFolderBasedServletComp::Generate(const QString& directoryPath, const QString& commandId) const
{
	QString retval;
	QString homeDirPath = *m_homeDirPathAttrPtr;
	if ( m_fileTemplatePathCompPtr.IsValid() && m_fileTemplatePathCompPtr.GetPtr()->GetPath().length())
	{
		homeDirPath = m_fileTemplatePathCompPtr.GetPtr()->GetPath();
	}

	QFileInfo fileInfo(directoryPath);
	if (!fileInfo.exists())
	{
		qCritical() << __FILE__ << __LINE__ << "Trying to get info of nonexistent value!" << fileInfo.absoluteFilePath();
	}
	else if (!fileInfo.isReadable())
	{
		qCritical() << __FILE__ << __LINE__ << "Trying to get info of nonreadable value!" << fileInfo.absoluteFilePath();
	}
	else if (fileInfo.isFile())
	{
		qWarning() << __FILE__ << __LINE__ << "Trying to get info of file" << fileInfo.absoluteFilePath();
	}
	else if (!fileInfo.isDir())
	{
		qCritical() << __FILE__ << __LINE__ << "Trying to get info of unexpected value!" << fileInfo.absoluteFilePath();
	}
	else
	{
		retval = this->m_beginOfMessage;
		QDir dir(directoryPath);
		using _dfs = QDir::Filter;
		QDir::Filters dirFilters = _dfs::NoDot | _dfs::Files | _dfs::Dirs;
		if (QDir(homeDirPath) == QDir(directoryPath))
		{
			dirFilters |= _dfs::NoDotDot;
		}
		QFileInfoList diryEnties = dir.entryInfoList(dirFilters, QDir::SortFlag::Type | QDir::SortFlag::Name);
		for (const auto& entry: ::qAsConst(diryEnties)){
			retval.append(this->GenerateSingleEntry(entry, commandId)).append('\n');
		}
		retval.append(this->m_endOfMessage);
	}
	return retval.toUtf8();
}


QByteArray CHttpHtmlFolderBasedServletComp::GenerateSingleEntry(const QFileInfo& fileInfo, const QString& commandId) const
{
	QString retval;
	retval.append(fileInfo.birthTime().toString(Qt::ISODate));
	retval.append('\t');

	if (fileInfo.isDir()){
		retval.append(QObject::tr("Directory"));
	}
	else{
		QByteArray sizeValue;
		qint64 fileSize = fileInfo.size();
		if (fileSize >1073741824){
			sizeValue = QByteArray::number(fileSize / 1073741824.00, 'f', 2);
			sizeValue.append(' ');
			sizeValue.append(QObject::tr("GB").toUtf8());
		}
		else if (fileSize > 1048576){
			sizeValue = QByteArray::number(fileSize / 1048576.00, 'f', 2);
			sizeValue.append(' ');
			sizeValue.append(QObject::tr("MB").toUtf8());
		}
		else if (fileSize > 1024){
			sizeValue = QByteArray::number(fileSize / 1024.00, 'f', 2);
			sizeValue.append(' ');
			sizeValue.append(QObject::tr("KB").toUtf8());
		}
		else{
			sizeValue = QByteArray::number(fileSize);
			sizeValue.append(' ');
			sizeValue.append(QObject::tr("B").toUtf8());
		}
		retval.append(QObject::tr("File"));
		retval.append('[');
		retval.append(sizeValue);
		retval.append(']');
	}
	QString rootLinkPath = *this->m_commandIdAttrPtr;
	if (rootLinkPath.endsWith('*')){
		rootLinkPath.chop(1);
	}

	QString fileName = fileInfo.fileName();
	if (fileName == ".."){
		fileName = '[';
		fileName.append(QObject::tr("Parent direcotry"));
		fileName.append(']');
	}
	QString fileLink = rootLinkPath + commandId + fileInfo.fileName();


	retval.append("\t\t");
	retval.append(R"(<A HREF="/)");
	retval.append(fileLink);
	retval.append(R"(/"><B>)");
	retval.append(fileName);
	retval.append(R"(</B></A>)");

	return retval.toUtf8();
}


QByteArray CHttpHtmlFolderBasedServletComp::GetMimeType(const QFileInfo& fileInfo) const
{
	QByteArray retval = "application/octet-stream";
	QByteArray fileSuffix = fileInfo.suffix().toUtf8();	
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
	else if (setMimeTypeByExtention("application/xhtml+xml",		{"xht", "xhtm", "xhtm"}));
	else if (setMimeTypeByExtention("application/xml-dtd",		{"dtd"}));
	else if (setMimeTypeByExtention("audio/mp4",					{"f4v", "mp4"}));
	else if (setMimeTypeByExtention("audio/aac",					{"aac"}));
	else if (setMimeTypeByExtention("audio/mpeg",				{"m4a", "m4b", "mp2", "mp4", "mpa", "mpga"}));
	else if (setMimeTypeByExtention("audio/vorbis",				{"oga", "ogg"}));
	else if (setMimeTypeByExtention("audio/ogg",					{"oga", "ogg", "oggstr", "flac"}));
	else if (setMimeTypeByExtention("audio/x-ms-wma",			{"wma"}));
	else if (setMimeTypeByExtention("audio/vnd.wave",			{"bfwav", "bwf", "bwg", "wav", "wave"}));
	else if (setMimeTypeByExtention("image/gif",					{"agif", "gfb", "gfw", "gif", "gifv"}));
	else if (setMimeTypeByExtention("image/jpeg",				{"j", "j2c", "j2k", "jfif", "jif", "jiff", "jng", "jp2", "jpc", "jpe", "jpeg", "jpf", "jpg", "jpg-large", "jpg2", "jps", "jpx", "jtf", "jxr", "ljp"}));
	else if (setMimeTypeByExtention("image/png",					{"apng", "png", "pns"}));
	else if (setMimeTypeByExtention("image/svg+xml",				{"svg", "svgz"  }));
	else if (setMimeTypeByExtention("image/tiff",				{"kdk", "tif", "tiff", "tsr"}));
	else if (setMimeTypeByExtention("image/vnd.microsoft.icon",	{"ico", "icon"}));
	else if (setMimeTypeByExtention("image/vnd.wap.wbmp",		{"wbmp"}));
	else if (setMimeTypeByExtention("image/webp",				{"webp"}));
	else if (setMimeTypeByExtention("text/css",					{"css"}));
	else if (setMimeTypeByExtention("text/csv",					{"csv"}));
	else if (setMimeTypeByExtention("text/html",					{"appcache", "chm", "shtml", "dhtml", "edge", "hhc", "hhk", "hta", "htc", "htm", "html", "htx", "html", "mht", "mhtml", "oth", "rhtml", "shtm", "vbhtml"}));
	else if (setMimeTypeByExtention("text/javascript",			{"jgz", "js", "jsf", "jss", "rjs"}));
	else if (setMimeTypeByExtention("text/plain",				{"txt", "utxt"}));
	else if (setMimeTypeByExtention("text/php",					{"php"}));
	else if (setMimeTypeByExtention("text/xml",					{"xml"}));
	else if (setMimeTypeByExtention("text/markdown",				{"markdown", "md"}));
	else if (setMimeTypeByExtention("video/mpeg",				{"ffm", "m21", "m2v", "mp21", "mpeg", "mpg", "mpv"}));
	else if (setMimeTypeByExtention("video/mp4",					{"mp4"}));
	else if (setMimeTypeByExtention("video/ogg",					{"ogm", "ogm", "ogx"}));
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

IRequestServlet::ConstResponsePtr CHttpHtmlFolderBasedServletComp::ProcessRequest(const IRequest& request) const
{
	const IProtocolEngine& engine = request.GetProtocolEngine();
	QByteArray errorBody = "<html><head><title>Error</title></head><body><p>File resource was not found</p></body></html>";
	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	ConstResponsePtr errorResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_RESOURCE_NOT_AVAILABLE, errorBody, reponseTypeId));

	auto generateErrorResponsePtr = [&request, &engine, reponseTypeId](QByteArray errorBody){
		qCritical() << __FILE__ << __LINE__ << "Error occurred" << errorBody;
		QByteArray generatedErrorBody = "<html><head><title>Error</title></head><body><p>";
		generatedErrorBody.append(errorBody);
		generatedErrorBody.append("</p></body></html>");
		return ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_INTERNAL_ERROR, generatedErrorBody, reponseTypeId));
	};

	QByteArray body;
	QByteArray commandId = request.GetCommandId();
	QByteArray commandIdBase = *this->m_commandIdAttrPtr;

	if (commandIdBase.endsWith('*')){
		commandIdBase.chop(1);
	}

	if (!commandIdBase.startsWith('/')){
		commandIdBase.prepend('/');
	}

	QString homeDirPath = *this->m_homeDirPathAttrPtr;

	if ( m_fileTemplatePathCompPtr.IsValid() && m_fileTemplatePathCompPtr.GetPtr()->GetPath().length()){
		homeDirPath = m_fileTemplatePathCompPtr.GetPtr()->GetPath();
	}

	commandId.replace(commandIdBase,"");

	if (*m_pathsProblemsAutoSolveAttrPtr){
		int indexOfPathSeparator = -1;
		QRegularExpression regexp("(.\\/.)");

		QRegularExpressionMatch indexOfPathSeparatorMatch = regexp.match(commandId);
		if (indexOfPathSeparatorMatch.hasMatch() && (indexOfPathSeparatorMatch.capturedStart() + 1) != commandId.length()){
			indexOfPathSeparator = indexOfPathSeparatorMatch.capturedStart() + 1;
		}

		if (indexOfPathSeparator > 0){
			if (QFileInfo(homeDirPath + commandId.mid(0,indexOfPathSeparator)).isFile()){
				commandId = commandId.remove(0, indexOfPathSeparator);
			}
		}
	}

	QString destinationEntryPath = homeDirPath + commandId;
	QFileInfo destinationEntry(destinationEntryPath);

	if (destinationEntry.isDir()){
		body = this->Generate(destinationEntry.absoluteFilePath(), commandId);
	}
	else{
		QString destinationFileAbsoluteFilePath = destinationEntry.absoluteFilePath();
		if (homeDirPath == ":"){
			destinationFileAbsoluteFilePath = destinationEntryPath;
		}

		if (destinationFileAbsoluteFilePath.endsWith('/')){
			destinationFileAbsoluteFilePath.chop(1);
		}

		QFile destinationFile(destinationFileAbsoluteFilePath);

		if (!destinationFile.open(QFile::ReadOnly)){
			generateErrorResponsePtr(QByteArray("Cannot open file for read ").append(destinationFile.fileName().toUtf8()));
		}

		reponseTypeId = this->GetMimeType(QFileInfo(destinationFileAbsoluteFilePath));
		reponseTypeId.append(QByteArray("; charset=utf-8"));

		body = destinationFile.readAll();

		destinationFile.close();
	}

	ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, body, reponseTypeId));

	return responsePtr;
}


QByteArray CHttpHtmlFolderBasedServletComp::GetSupportedCommandId() const
{
	return *m_commandIdAttrPtr;
}


} // namespace imtrest


