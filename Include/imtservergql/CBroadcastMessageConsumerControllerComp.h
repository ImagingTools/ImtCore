// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


