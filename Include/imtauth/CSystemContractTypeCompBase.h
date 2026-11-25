// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>


namespace imtauth
{


/**
	The terms of a system type of contracts; split off for the limit of static objects of MSVC.
*/
class CSystemContractTypeCompBase: public icomp::CComponentBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CSystemContractTypeCompBase);
		I_ASSIGN(m_termDaysAttrPtr, "TermDays", "Length of the first period in days, 0 without an end", true, 0);
		I_ASSIGN(m_autoRenewAttrPtr, "AutoRenew", "The contract renews at the end of a period", true, false);
		I_ASSIGN(m_renewalPeriodDaysAttrPtr, "RenewalPeriodDays", "Length of a renewal in days", true, 0);
		I_ASSIGN(m_renewalNoticeDaysAttrPtr, "RenewalNoticeDays", "Days of notice before the end of a period not to renew", true, 0);
		I_ASSIGN(m_terminationNoticeDaysAttrPtr, "TerminationNoticeDays", "Days between the notice of a party and its leaving", true, 0);
		I_ASSIGN(m_isSequentialAttrPtr, "IsSequential", "The parties sign one after another", true, false);
		I_ASSIGN(m_requiredSignaturesAttrPtr, "RequiredSignatures", "Signatures each party gives", true, 1);
		I_ASSIGN_MULTI_0(m_requiredAttachmentKindsAttrPtr, "RequiredAttachmentKinds", "Kinds of attachments a contract needs to be sent", false);
	I_END_COMPONENT;

protected:
	I_ATTR(int, m_termDaysAttrPtr);
	I_ATTR(bool, m_autoRenewAttrPtr);
	I_ATTR(int, m_renewalPeriodDaysAttrPtr);
	I_ATTR(int, m_renewalNoticeDaysAttrPtr);
	I_ATTR(int, m_terminationNoticeDaysAttrPtr);
	I_ATTR(bool, m_isSequentialAttrPtr);
	I_ATTR(int, m_requiredSignaturesAttrPtr);
	I_MULTIATTR(int, m_requiredAttachmentKindsAttrPtr);
};


} // namespace imtauth


