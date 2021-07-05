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
#include <imtfile/IFileProvider.h>
#include <imtrest/IRequest.h>
#include <imtrest/IResponder.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CHttpServletCompBase.h>

namespace imtrest
{

/**
 * \brief The CHttpFileProviderBasedServletComp class generates HTML-document with list of objects for all the files and directories in the directory
 */
class CHttpFileProviderBasedServletComp: public CHttpServletCompBase
{
public:
	typedef CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CHttpFileProviderBasedServletComp);
		I_REGISTER_INTERFACE(IRequestHandler);
		I_ASSIGN_MULTI_0(m_fileProvidersPtrs, "FileTemplateHomePath", "FileTemplate based Home path\n Note: type is ignored only value matters \n Warning: overwrites HomePath!", true);
	I_END_COMPONENT

	QByteArray GetMimeType(const QFileInfo& fileInfo) const;

// reimplemented (IRequestHandler)
	virtual ConstResponsePtr ProcessRequest(const IRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

private:
	I_MULTIREF(imtfile::IFileProvider, m_fileProvidersPtrs);

	QByteArray m_beginOfMessage;
	QByteArray m_endOfMessage;

};


} // namespace imtrest


