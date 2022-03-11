#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequestServlet.h>
#include <imtrest/IRepresentationDataProvider.h>


namespace imtrest
{


class CHttpFileReceivedServletCompBase:
			public ilog::CLoggerComponentBase,
			virtual public IRequestServlet
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHttpFileReceivedServletCompBase)
		I_REGISTER_INTERFACE(IRequestServlet)
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "List of command-ID used with corresponded handlers", true, "");
		I_ASSIGN(m_redirectToCommandIdAttrPtr, "RedirectToCommandId", "Command-ID used with corresponded handlers", true, "");
	I_END_COMPONENT

	// reimplemented (imtrest::IRequestServlet)
	ConstResponsePtr ProcessRequest(const IRequest& request) const override;
	QByteArray GetSupportedCommandId() const override;

private:
	I_REF(imtrest::IRepresentationDataProvider, m_representationDataProvider);
	I_ATTR(QByteArray, m_commandIdAttrPtr);
	I_ATTR(QByteArray, m_redirectToCommandIdAttrPtr);
};


} // namespace imtrest


