// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QTimer>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IContractManager.h>


namespace imtauth
{


/**
	Reminds the parties of the dates of their contracts: the deadline of the signature, the end of the contract
	and the last day to refuse its renewal. A party is reminded once per date, the given number of days before it.
*/
class CContractReminderComp: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CContractReminderComp);
		I_ASSIGN(m_contractManagerCompPtr, "ContractManager", "Contracts whose parties are reminded", true, "ContractManager");
		I_ASSIGN(m_defaultReminderDaysAttrPtr, "DefaultReminderDays", "Days before a date a party is reminded, unless it chose its own", true, 7);
		I_ASSIGN(m_maxReminderDaysAttrPtr, "MaxReminderDays", "Most days before a date a party may be reminded", true, 90);
		I_ASSIGN(m_checkIntervalHoursAttrPtr, "CheckIntervalHours", "Hours between two checks of the dates", true, 1);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void RemindParties();
	void RemindParty(const IContractInfo& contract, const IContractInfo::Party& party, const QDateTime& now);

	/**
		Remind the party when the date is due in the reminder window; the key names the date, so it is reminded once.
	*/
	void RemindOfDate(const IContractInfo& contract, const QByteArray& tenantId, const QByteArray& kind, const QDateTime& date, int reminderDays, const QDateTime& now);

private:
	I_REF(IContractManager, m_contractManagerCompPtr);
	I_ATTR(int, m_defaultReminderDaysAttrPtr);
	I_ATTR(int, m_maxReminderDaysAttrPtr);
	I_ATTR(int, m_checkIntervalHoursAttrPtr);

	QTimer m_timer;
};


} // namespace imtauth


