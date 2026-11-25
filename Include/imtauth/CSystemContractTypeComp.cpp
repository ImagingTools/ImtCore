// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CSystemContractTypeComp.h>


namespace imtauth
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CSystemContractTypeComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SetTypeId(*m_typeIdAttrPtr);
	// The catalog copies the type into an identifiable one
	SetObjectUuid(*m_typeIdAttrPtr);
	// The source texts: the server translates them into the language of the client
	SetName(m_nameAttrPtr.GetOriginalValue());
	SetDescription(m_descriptionAttrPtr.IsValid() ? m_descriptionAttrPtr.GetOriginalValue() : QString());
	SetNumberPrefix(*m_numberPrefixAttrPtr);

	IContractInfo::RoleSlots roleSlots;
	for (int i = 0; i < m_roleSlotsAttrPtr.GetCount(); ++i){
		const QByteArrayList parts = m_roleSlotsAttrPtr[i].split(':');
		if (parts.isEmpty() || parts[0].isEmpty()){
			continue;
		}

		IContractInfo::RoleSlot slot;
		slot.roleId = parts[0];
		slot.minCount = (parts.size() > 1) ? parts[1].toInt() : 0;
		slot.maxCount = (parts.size() > 2) ? parts[2].toInt() : 0;
		roleSlots.append(slot);
	}

	SetRoleSlots(roleSlots);
	SetLinkScheme(LinkScheme(qBound(int(LS_MANUAL), *m_linkSchemeAttrPtr, int(LS_STAR))));
	SetStarRoleId(m_starRoleIdAttrPtr.IsValid() ? *m_starRoleIdAttrPtr : QByteArray());
	SetTermDays(*m_termDaysAttrPtr);
	SetAutoRenew(*m_autoRenewAttrPtr);
	SetRenewalPeriodDays(*m_renewalPeriodDaysAttrPtr);
	SetRenewalNoticeDays(*m_renewalNoticeDaysAttrPtr);
	SetTerminationNoticeDays(*m_terminationNoticeDaysAttrPtr);
	SetSignOrder(*m_isSequentialAttrPtr ? IContractInfo::SO_SEQUENTIAL : IContractInfo::SO_PARALLEL);
	SetRequiredSignatures(qMax(1, *m_requiredSignaturesAttrPtr));

	QList<int> requiredKinds;
	for (int i = 0; i < m_requiredAttachmentKindsAttrPtr.GetCount(); ++i){
		requiredKinds.append(m_requiredAttachmentKindsAttrPtr[i]);
	}

	SetRequiredAttachmentKinds(requiredKinds);
}


} // namespace imtauth


