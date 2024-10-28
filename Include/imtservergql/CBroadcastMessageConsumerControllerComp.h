#pragma once


// ACF includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtservergql
{


class CBroadcastMessageConsumerControllerComp:
			public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CBroadcastMessageConsumerControllerComp);
		I_ASSIGN(m_messageConsumerCompPtr, "MessageConsumer", "MessageConsumer sender", false, "MessageConsumer")
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(ilog::IMessageConsumer, m_messageConsumerCompPtr);
};


} // namespace imtservergql


