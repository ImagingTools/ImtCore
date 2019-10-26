#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequestHandler.h>


namespace imtrest
{


class CHttpRootHandlerComp:
			public ilog::CLoggerComponentBase,
			virtual public IRequestHandler
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHttpRootHandlerComp);
		I_REGISTER_INTERFACE(IRequestHandler);
		I_ASSIGN_MULTI_0(m_commandIdsAttrPtr, "CommandIds", "List of command-IDs used with corresponded handlers", true);
		I_ASSIGN_MULTI_0(m_requestHandlersCompPtr, "RequestHandlers", "List of request handlers for corresponding command-IDs", true);
	I_END_COMPONENT

	// reimplemented (IRequestHandler)
	virtual bool ProcessRequest(const IRequest& request) const override;

protected:
	virtual IRequestHandler* FindRequestHandler(const QByteArray& commandId) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

private:
	I_MULTIATTR(QByteArray, m_commandIdsAttrPtr);
	I_MULTIREF(IRequestHandler, m_requestHandlersCompPtr);

	typedef QMap<QString, IRequestHandler*> HandlersMap;

	HandlersMap m_handlersMap;
	QMutex m_handlersMapMutex;
};


} // namespace imtrest


