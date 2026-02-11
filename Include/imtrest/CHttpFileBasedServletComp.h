// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtrest/IRequestServlet.h>


namespace imtrest
{


class CHttpFileBasedServletComp:
			public ilog::CLoggerComponentBase,
			virtual public IRequestServlet
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHttpFileBasedServletComp);
		I_REGISTER_INTERFACE(IRequestServlet);
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "List of command-ID used with corresponded handlers", true, "");
		I_ASSIGN(m_fileTemplatePathCompPtr, "FileTemplatePath", "Comment", true, "FileTemplatePath");
	I_END_COMPONENT

	// reimplemented (IRequestHandler)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual ConstResponsePtr ProcessRequest(const IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

private:
	I_ATTR(QByteArray, m_commandIdAttrPtr);
	I_REF(ifile::IFileNameParam, m_fileTemplatePathCompPtr);
};


} // namespace imtrest


