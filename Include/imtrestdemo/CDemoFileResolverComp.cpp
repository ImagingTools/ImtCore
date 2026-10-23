// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoFileResolverComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

// ImtCore includes
#include <imtrest/CHttpRequest.h>


namespace imtrestdemo
{


namespace
{


const QByteArray s_filesPrefix("/files/");


QByteArray GetDataTypeId(const QString& filePath)
{
	const QString suffix = QFileInfo(filePath).suffix().toLower();

	if (suffix == "txt" || suffix == "log"){
		return "text/plain; charset=utf-8";
	}

	if (suffix == "json"){
		return "application/json";
	}

	if (suffix == "html" || suffix == "htm"){
		return "text/html; charset=utf-8";
	}

	if (suffix == "png"){
		return "image/png";
	}

	return "application/octet-stream";
}


bool HasBearerToken(const imtrest::CHttpRequest& request, const QByteArray& token)
{
	return request.GetHeaderValue("authorization") == "Bearer " + token;
}


} // namespace


// reimplemented (imtrest::IHttpFileResolver)

imtrest::IProtocolEngine::StatusCode CDemoFileResolverComp::ResolveFile(const imtrest::CHttpRequest& request, FileDescription& file) const
{
	typedef imtrest::IProtocolEngine Engine;

	const QByteArray commandId = request.GetCommandId();
	if (!commandId.startsWith(s_filesPrefix) || commandId.size() == s_filesPrefix.size()){
		return Engine::SC_NOT_FOUND;
	}

	const QString relativePath = QString::fromUtf8(commandId.mid(s_filesPrefix.size()));
	const QStringList segments = relativePath.split('/', Qt::KeepEmptyParts);

	for (const QString& segment : segments){
		// Parent directories, empty and hidden names, the separators of Windows and the drive letters are never allowed
		if (segment.isEmpty() || segment.startsWith('.') || segment.contains('\\') || segment.contains(':') || segment.contains(QChar(0))){
			return Engine::SC_FORBIDDEN;
		}
	}

	if (segments.first() == QLatin1String("private") && !HasBearerToken(request, *m_privateTokenAttrPtr)){
		file.headers.insert("WWW-Authenticate", "Bearer realm=\"imtrestdemo\"");

		return Engine::SC_UNAUTHORIZED;
	}

	const QString filePath = QDir(m_canonicalRoot).absoluteFilePath(relativePath);

	// A link inside of the root can lead out of it
	const QFileInfo fileInfo(filePath);
	if (fileInfo.exists()){
		const QString canonicalPath = fileInfo.canonicalFilePath();
		if (!canonicalPath.startsWith(m_canonicalRoot + '/')){
			return Engine::SC_FORBIDDEN;
		}
	}

	file.filePath = filePath;
	file.dataTypeId = GetDataTypeId(filePath);
	file.headers.insert("Cache-Control", "no-cache");

	if (request.GetCommandParams().value("download") == "1"){
		file.headers.insert("Content-Disposition", "attachment; filename=\"" + fileInfo.fileName().toUtf8() + "\"");
	}

	return Engine::SC_OK;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CDemoFileResolverComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_canonicalRoot = QFileInfo(*m_rootDirectoryAttrPtr).canonicalFilePath();
	if (m_canonicalRoot.isEmpty()){
		m_canonicalRoot = QDir(*m_rootDirectoryAttrPtr).absolutePath();
	}
}


} // namespace imtrestdemo
