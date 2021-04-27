#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequestHandler.h>


namespace imtrest
{


class CHttpRedirectComp:
		public ilog::CLoggerComponentBase,
		virtual public IRequestHandler
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHttpRedirectComp)
		I_REGISTER_INTERFACE(IRequestHandler)
	I_ASSIGN(m_commandIdAttrPtr, "CommandId", "List of command-ID used with corresponded handlers", true, "");
	I_ASSIGN(m_redirectToCommandID, "RedirectToCommandId", "Command-ID used with corresponded handlers", true, "");

	I_END_COMPONENT


	// reimplemented (icomp::IRequestHandler)
	ConstResponsePtr ProcessRequest(const IRequest& request) const override;
	QByteArray GetSupportedCommandId() const override;


protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QByteArray, m_commandIdAttrPtr);
	I_ATTR(QByteArray, m_redirectToCommandID);
};


} // namespace imtrest


