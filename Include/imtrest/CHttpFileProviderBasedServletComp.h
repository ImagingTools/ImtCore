// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IBinaryDataProvider.h>
#include <imtrest/CHttpServletCompBase.h>


namespace imtrest
{


/**
	\brief The CHttpFileProviderBasedServletComp class generates HTML-document with list of objects for all the files and directories in the directory
*/
class CHttpFileProviderBasedServletComp: public CHttpServletCompBase
{
public:
	typedef CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CHttpFileProviderBasedServletComp);
		I_ASSIGN(m_fileIdCommandParamAttrPtr, "FileIdCommandParam", "The command param for data providers", true, "FileId");
		I_ASSIGN(m_limitIdCommandParamAttrPtr, "LimitIdCommandParam", "Defines maximum size of data to be read", true, "Limit");
		I_ASSIGN(m_offsetIdCommandParamAttrPtr, "OffsetIdCommandParam", "Defines the position to start reading data", true, "Offset");
		I_ASSIGN_MULTI_0(m_binaryDataProvidersCompPtr, "BinaryDataProviders", "Binary data providers", true);
	I_END_COMPONENT

	QByteArray GetMimeType(QByteArray fileSuffix) const;

	// reimplemented (IRequestHandler)
	virtual ConstResponsePtr OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;

protected:
	I_ATTR(QString, m_fileIdCommandParamAttrPtr);
	I_ATTR(QString, m_limitIdCommandParamAttrPtr);
	I_ATTR(QString, m_offsetIdCommandParamAttrPtr);
	I_MULTIREF(imtrest::IBinaryDataProvider, m_binaryDataProvidersCompPtr);

private:
	QByteArray m_beginOfMessage;
	QByteArray m_endOfMessage;
};


} // namespace imtrest


