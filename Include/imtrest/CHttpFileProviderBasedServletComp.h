#pragma once

// ACF includes
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequestServlet.h>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtbase/IBinaryDataProvider.h>
#include <imtrest/IRequest.h>
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
		I_REGISTER_INTERFACE(IRequestServlet);
		I_ASSIGN(m_fileIdCommandParamAttrPtr, "FileIdCommandParam", "The command param for data providers", true, "FileId");

		I_ASSIGN_MULTI_0(m_binaryDataProvidersCompPtr, "BinaryDataProviders", "Binary data providers", true);
	I_END_COMPONENT

	QByteArray GetMimeType(QByteArray fileSuffix) const;

	// reimplemented (IRequestHandler)
	virtual ConstResponsePtr OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

protected:
	I_ATTR(QString, m_fileIdCommandParamAttrPtr);
	I_MULTIREF(imtbase::IBinaryDataProvider, m_binaryDataProvidersCompPtr);

private:
	QByteArray m_beginOfMessage;
	QByteArray m_endOfMessage;
};


} // namespace imtrest


