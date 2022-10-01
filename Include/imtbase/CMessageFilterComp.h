#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <ilog/TMessageDelegatorComp.h>


namespace imtbase
{


class CMessageFilterComp: public ilog::TMessageDelegatorComp<icomp::CComponentBase>
{
public:
	typedef ilog::TMessageDelegatorComp<icomp::CComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CMessageFilterComp);
		I_ASSIGN_MULTI_0(m_acceptMessageIdsAttrPtr, "AcceptMessageIds", "Message IDs to accept", false);
		I_ASSIGN_MULTI_0(m_rejectMessageIdsAttrPtr, "RejectMessageIds", "Message IDs to reject", false);
	I_END_COMPONENT;

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = nullptr) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

private:
	bool IsMessageAccepted(int messageId);

private:
	I_MULTIATTR(int, m_acceptMessageIdsAttrPtr);
	I_MULTIATTR(int, m_rejectMessageIdsAttrPtr);
	I_REF(ilog::IMessageConsumer, m_filteredMessageConsumerCompPtr);
};


} // namespace imtbase


