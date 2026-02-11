// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/CLogCompBase.h>
#include <ilog/TLoggerCompWrap.h>
#include <ibase/TRuntimeStatusHanderCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtlog
{


class CCollectionBasedMessageConsumerComp:
			public ibase::TRuntimeStatusHanderCompWrap<ilog::CLogCompBase>
{
public:
	typedef ibase::TRuntimeStatusHanderCompWrap<ilog::CLogCompBase> BaseClass;

	I_BEGIN_COMPONENT(CCollectionBasedMessageConsumerComp);
		I_ASSIGN(m_messageCollectionCompPtr, "MessageCollection", "Message collection", true, "MessageCollection");
	I_END_COMPONENT;

	CCollectionBasedMessageConsumerComp();

protected:
	// reimplemented (ibase::TRuntimeStatusHanderCompWrap)
	virtual void OnSystemStarted() override;

	// reimplemented (ilog::CLogCompBase)
	virtual void WriteMessageToLog(const MessagePtr& messagePtr) override;

private:
	I_REF(imtbase::IObjectCollection, m_messageCollectionCompPtr);

	bool m_isSystemStarted;
	QList<MessagePtr> m_startMessages;
};


} // namespace imtlog


