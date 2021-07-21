#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iser/CJsonStringWriteArchive.h>

// ImtCore includes
#include <imtrest/IRequestServlet.h>
#include <imtrest/IProtocolEngine.h>

// ImtCore includes
#include <imtrest/IRepresentationDataProvider.h>


namespace imtrest
{


/**
 * \brief The CJsonModelBasedServletComp class generates JSON-model
 */
class CJsonModelBasedServletComp:
			public ilog::CLoggerComponentBase,
			virtual public IRequestServlet
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	CJsonModelBasedServletComp();

	I_BEGIN_COMPONENT(CJsonModelBasedServletComp);
		I_REGISTER_INTERFACE(IRequestServlet);
		I_ASSIGN(m_representationDataProvider, "RepresentationDataProvider", "Representation data provider", false, "RepresentationDataProvider");
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "List of command-ID used with corresponded handlers", true, "");
	I_END_COMPONENT

	// reimplemented (IRequestHandler)
	virtual ConstResponsePtr ProcessRequest(const IRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

private:
	I_REF(imtrest::IRepresentationDataProvider, m_representationDataProvider);
	I_ATTR(QByteArray, m_commandIdAttrPtr);
};


} // namespace imtrest


