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
		I_ASSIGN(m_autoCompressionAttrPtr, "AutoCompression", "Automatic data compression", true, false);
		I_ASSIGN(m_compressionLevelAttrPtr, "CompressionLevel", "Compression level for automatic compression", true, 8);
	I_END_COMPONENT

	// reimplemented (IRequestHandler)
	virtual ConstResponsePtr ProcessRequest(const IRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

protected:
	enum EncodingType {
		ET_NONE,
		ET_GZIP,
		ET_DEFLATE
	};

	virtual IRequestServlet* FindRequestHandler(const QByteArray& commandId) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIATTR(QByteArray, m_commandIdsAttrPtr);
	I_MULTIREF(IRequestServlet, m_requestHandlersCompPtr);
	I_ATTR(bool, m_autoCompressionAttrPtr);
	I_ATTR(int, m_compressionLevelAttrPtr);

	typedef QMap<QByteArray, IRequestServlet*> HandlersMap;

	HandlersMap m_handlersMap;
	mutable QMutex m_handlersMapMutex;
};


} // namespace imtrest


