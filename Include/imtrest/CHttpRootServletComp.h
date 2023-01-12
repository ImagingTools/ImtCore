#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequestServlet.h>


namespace imtrest
{


class CHttpRootServletComp:
			public ilog::CLoggerComponentBase,
			virtual public IRequestServlet
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHttpRootServletComp);
		I_REGISTER_INTERFACE(IRequestServlet);
		I_ASSIGN_MULTI_0(m_commandIdsAttrPtr, "CommandIds", "List of command-IDs used with corresponded handlers", true);
		I_ASSIGN_MULTI_0(m_requestHandlersCompPtr, "RequestHandlers", "List of request handlers for corresponding command-IDs", false);
	I_END_COMPONENT

	// reimplemented (IRequestHandler)
	virtual ConstResponsePtr ProcessRequest(const IRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

protected:
	virtual IRequestServlet* FindRequestHandler(const QByteArray& commandId) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

private:
	I_MULTIATTR(QByteArray, m_commandIdsAttrPtr);
	I_MULTIREF(IRequestServlet, m_requestHandlersCompPtr);

	typedef QMap<QString, IRequestServlet*> HandlersMap;

	HandlersMap m_handlersMap;
	QMutex m_handlersMapMutex;
};


} // namespace imtrest


