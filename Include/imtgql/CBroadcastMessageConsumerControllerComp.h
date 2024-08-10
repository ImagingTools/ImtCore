#pragma once


// ACF includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtgql/CGqlRequestHandlerCompBase.h>


namespace imtgql
{


class CBroadcastMessageConsumerControllerComp:
			public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CBroadcastMessageConsumerControllerComp);
		I_ASSIGN(m_messageConsumerCompPtr, "MessageConsumer", "MessageConsumer sender", false, "MessageConsumer")
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(ilog::IMessageConsumer, m_messageConsumerCompPtr);
};


} // namespace imtguigql


