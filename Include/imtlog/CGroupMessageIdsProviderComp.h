#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlog/IGroupMessageIdsProvider.h>


namespace imtlog
{


class CGroupMessageIdsProviderComp:
			public icomp::CComponentBase,
			virtual public IGroupMessageIdsProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupMessageIdsProviderComp)
		I_REGISTER_INTERFACE(IGroupMessageIdsProvider);
		I_TASSIGN_MULTI_0(m_idsAttrPtr, "MessageIdList", "Message id list", false);
		I_ASSIGN(m_groupIdAttrPtr, "GroupId", "Group id", false, "");
		I_ASSIGN(m_slaveAttrPtr, "SlaveProvider", "Slave group message ids provider", false, "");
	I_END_COMPONENT

	// reimplemented (imtlog::IGroupMessageIdsProvider)
	virtual bool GetGroupMessageIds(const QByteArray& groupId, QSet<int>& messageIds) const override;

private:
	I_MULTIATTR(int, m_idsAttrPtr);
	I_ATTR(QByteArray, m_groupIdAttrPtr);
	I_REF(IGroupMessageIdsProvider, m_slaveAttrPtr);
};


} // namespace imtlog


