#include "CHttpHtmlFolderBasedHandlerComp.h"

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtrest/IRequestHandler.h>


namespace imtrest
{


CHttpHtmlFolderBasedHandlerComp::CHttpHtmlFolderBasedHandlerComp()
{
	QFile messagePartFile;

	messagePartFile.setFileName("/Volumes/ALL_DATA/ZProjects/Acf/SibNavAcf/index_files_begin.html");
	messagePartFile.open(QFile::ReadOnly);
	m_beginOfMessage = messagePartFile.readAll();
	messagePartFile.close();

	messagePartFile.setFileName("/Volumes/ALL_DATA/ZProjects/Acf/SibNavAcf/index_files_end.html");
	messagePartFile.open(QFile::ReadOnly);
	m_endOfMessage = messagePartFile.readAll();
	messagePartFile.close();

}

QByteArray CHttpHtmlFolderBasedHandlerComp::Generate(const QString& directoryPath) const
{
	QString retval;
	QString homeDirPath = *m_homeDirPath;
	if( m_fileTemplatePathCompPtr.IsValid() && m_fileTemplatePathCompPtr.GetPtr()->GetPath().length())
	{
		homeDirPath = m_fileTemplatePathCompPtr.GetPtr()->GetPath();
	}

	QFileInfo fileInfo(directoryPath);
	if(!fileInfo.exists())
	{
		qCritical() << __FILE__ << __LINE__ << "Trying to get info of nonexistent value!" << fileInfo.absoluteFilePath();
	}
	else if(!fileInfo.isReadable())
	{
		qCritical() << __FILE__ << __LINE__ << "Trying to get info of nonreadable value!" << fileInfo.absoluteFilePath();
	}
	else if(fileInfo.isFile())
	{
		qWarning() << __FILE__ << __LINE__ << "Trying to get info of file" << fileInfo.absoluteFilePath();
	}
	else if(!fileInfo.isDir())
	{
		qCritical() << __FILE__ << __LINE__ << "Trying to get info of unexpected value!" << fileInfo.absoluteFilePath();
	}
	else
	{
		retval = this->m_beginOfMessage;
		QDir dir(directoryPath);
		using _dfs = QDir::Filter;
		QDir::Filters dirFilters = _dfs::NoDot | _dfs::Files | _dfs::Dirs;
		if(homeDirPath != directoryPath)
		{
			dirFilters |= _dfs::NoDotDot;
		}
		QFileInfoList diryEnties = dir.entryInfoList(dirFilters, QDir::SortFlag::Name);
		for (const auto& entry: ::qAsConst(diryEnties))
		{
			retval.append(this->GenerateSingleEntry(entry)).append('\n');
		}
		retval.append(this->m_endOfMessage);
	}
	return retval.toUtf8();
}


QByteArray CHttpHtmlFolderBasedHandlerComp::GenerateSingleEntry(const QFileInfo& fileInfo) const
{
	QString retval;
	retval.append(fileInfo.birthTime().toString(Qt::ISODate));
	retval.append('\t');

	if(fileInfo.isDir())
	{
		retval.append(QObject::tr("Directory"));
	}
	else
	{
		QByteArray sizeValue;
		qint64 fileSize = fileInfo.size();
		if(fileSize > 1024 * 1024 * 1024)
		{
			sizeValue = QByteArray::number(fileSize / (1024*1024*1024), 'f', 2);
			sizeValue.append(' ');
			sizeValue.append(QObject::tr("GB"));
		}
		else if(fileSize > 1024 * 1024)
		{
			sizeValue = QByteArray::number(fileSize / (1024*1024), 'f', 2);
			sizeValue.append(' ');
			sizeValue.append(QObject::tr("MB"));
		}
		else if(fileSize > 1024)
		{
			sizeValue = QByteArray::number(fileSize / (1024), 'f', 2);
			sizeValue.append(' ');
			sizeValue.append(QObject::tr("KB"));
		}
		else
		{
			sizeValue = QByteArray::number(fileSize);
			sizeValue.append(' ');
			sizeValue.append(QObject::tr("B"));
		}
		retval.append(QObject::tr("File"));
		retval.append('[');
		retval.append(sizeValue);
		retval.append(']');
	}
	QString fileName = fileInfo.baseName() + fileInfo.suffix();
	QString fileLink = *this->m_homeDirPath + fileInfo.baseName() + fileInfo.suffix();


	retval.append("\t\t");
	retval.append(R"(<A HREF="/)");
	retval.append(fileName);
	retval.append(R"(/"><B>)");
	retval.append(fileInfo.baseName() + fileInfo.suffix());
	retval.append(R"(</B></A>)");

	return retval.toUtf8();
}

// reimplemented (IRequestHandler)

IRequestHandler::ConstResponsePtr CHttpHtmlFolderBasedHandlerComp::ProcessRequest(const IRequest& request) const
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
	if(commandIdBase.endsWith('*'))
	{
		commandIdBase.chop(1);
	}
	QString homeDirPath = *this->m_homeDirPath;
	if( m_fileTemplatePathCompPtr.IsValid() && m_fileTemplatePathCompPtr.GetPtr()->GetPath().length())
	{
		homeDirPath = m_fileTemplatePathCompPtr.GetPtr()->GetPath();
	}
	if(!homeDirPath.endsWith('/'))
	{
		homeDirPath.append('/');
	}
	commandId.replace(commandIdBase,"");
	//	files*
	//	files/dir
	QString destinationEntryPath = homeDirPath + commandId;
	QFileInfo destinationEntry(destinationEntryPath);
	if(destinationEntry.isDir())
	{
		body = this->Generate(destinationEntry.absoluteFilePath());
	}
	else
	{
		QFile destinationFile(destinationEntry.absoluteFilePath());
		if(!destinationFile.open(QFile::ReadOnly))
		{
			generateErrorResponsePtr(QByteArray("Cannot open file for read ").append(destinationFile.fileName()));
		}
		reponseTypeId = QByteArray("application/");
		QByteArray mimeType = destinationEntry.suffix().toUtf8();
		if(mimeType.startsWith('.'))
		{
			mimeType = mimeType.remove(0,1);
		}
		reponseTypeId.append(mimeType);
		reponseTypeId.append(QByteArray("; charset=utf-8"));
		body = destinationFile.readAll();
		destinationFile.close();
	}

	ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, body, reponseTypeId));

	return responsePtr;
}

QByteArray CHttpHtmlFolderBasedHandlerComp::GetSupportedCommandId() const
{
	return *m_commandIdAttrPtr;
}

} // namespace imtrest
