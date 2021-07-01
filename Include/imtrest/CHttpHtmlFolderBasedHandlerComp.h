#pragma once

// ACF includes
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequestHandler.h>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponder.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>

namespace imtrest
{

/**
 * \brief The CHttpHtmlFolderBasedHandlerComp class generates HTML-document with list of objects for all the files and directories in the directory
 */
class CHttpHtmlFolderBasedHandlerComp:
			public ilog::CLoggerComponentBase,
			virtual public IRequestHandler
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	CHttpHtmlFolderBasedHandlerComp();

	I_BEGIN_COMPONENT(CHttpHtmlFolderBasedHandlerComp);
		I_REGISTER_INTERFACE(IRequestHandler);
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "List of command-ID used with corresponded handlers", true, "");
		I_ASSIGN(m_homeDirPath, "HomePath", "Home path", true, QDir::currentPath());
		I_ASSIGN(m_pathsProblemsAutoSolve, "AutoSolvePathsProblems", "If 'true' servelt'll automatically solve paths problems", true, true);
		I_ASSIGN(m_fileTemplatePathCompPtr, "FileTemplateHomePath", "FileTemplate based Home path\n Note: type is ignored only value matters \n Warning: overwrites HomePath!", true, "FileTemplatePath");
	I_END_COMPONENT

	QByteArray Generate(const QString& directoryPath, const QString& commandId = QString()) const;
	QByteArray GenerateSingleEntry(const QFileInfo& fileInfo, const QString& commandId = QString()) const;
	QByteArray GetMimeType(const QFileInfo& fileInfo) const;

	// reimplemented (IRequestHandler)
	virtual ConstResponsePtr ProcessRequest(const IRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

private:
	I_ATTR (QString, m_homeDirPath);
	I_ATTR (bool, m_pathsProblemsAutoSolve);
	I_REF(ifile::IFileNameParam, m_fileTemplatePathCompPtr);

	I_ATTR(QByteArray, m_commandIdAttrPtr);

	QByteArray m_beginOfMessage;
	QByteArray m_endOfMessage;

};


} // namespace imtrest


