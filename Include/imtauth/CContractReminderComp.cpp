// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContractReminderComp.h>


// ImtCore includes
#include <imtauth/ContractState.h>


namespace imtauth
{


namespace
{


// The first check soon after the start, not during it
const int s_firstCheckDelay = 60 * 1000;


} // anonymous namespace


// protected methods

// reimplemented (icomp::CComponentBase)

void CContractReminderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QObject::connect(&m_timer, &QTimer::timeout, [this](){
		m_timer.setInterval(qMax(1, *m_checkIntervalHoursAttrPtr) * 60 * 60 * 1000);

		RemindParties();
	});

	m_timer.start(s_firstCheckDelay);
}


void CContractReminderComp::OnComponentDestroyed()
{
	m_timer.stop();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CContractReminderComp::RemindParties()
{
	bool isRead = false;
	const ContractInfoList contracts = m_contractManagerCompPtr->GetOpenContracts(&isRead);
	if (!isRead){
		SendWarningMessage(0, QStringLiteral("The contracts could not be read completely: some parties are reminded with the next check"), "CContractReminderComp");
	}

	const QDateTime now = QDateTime::currentDateTimeUtc();
	for (const IContractInfoSharedPtr& contractPtr : contracts){
		for (const IContractInfo::Party& party : contractPtr->GetParties()){
			RemindParty(*contractPtr, party, now);
		}
	}
}


void CContractReminderComp::RemindParty(const IContractInfo& contract, const IContractInfo::Party& party, const QDateTime& now)
{
	const ContractStateInfo stateInfo = ComputeContractState(contract, now);

	QList<QPair<QByteArray, QDateTime>> dates;
	if (stateInfo.state == CS_ACTIVE && IsPartyBoundAt(contract, party, now)){
		// A refusal of any party ends a renewing contract with the period it allows
		bool isRenewalRefused = false;
		for (const IContractInfo::Party& otherParty : contract.GetParties()){
			isRenewalRefused = isRenewalRefused || otherParty.nonRenewalNoticeAt.isValid();
		}

		const bool isRenewing = contract.IsAutoRenew() && !isRenewalRefused;

		// The end that is known: a notice, a term that is not renewed or a refused renewal
		dates.append({QByteArrayLiteral("end"), stateInfo.endsAt.isValid() ? stateInfo.endsAt : (isRenewing ? QDateTime() : stateInfo.periodEnd)});

		// The last day to refuse a renewal that still comes
		if (isRenewing && !stateInfo.endsAt.isValid() && stateInfo.periodEnd.isValid() && contract.GetRenewalNoticeDays() > 0){
			dates.append({QByteArrayLiteral("renewal"), stateInfo.periodEnd.addDays(-contract.GetRenewalNoticeDays())});
		}
	}

	bool hasComingDate = false;
	for (const QPair<QByteArray, QDateTime>& date : std::as_const(dates)){
		hasComingDate = hasComingDate || (date.second.isValid() && date.second > now && date.second <= now.addDays(*m_maxReminderDaysAttrPtr));
	}

	if (!hasComingDate){
		return;
	}

	IContractPartyPrivateInfoUniquePtr privateInfoPtr = m_contractManagerCompPtr->GetPrivateInfo(contract.GetContractId(), party.tenantId);
	if (!privateInfoPtr.IsValid()){
		return;
	}

	const int chosenDays = privateInfoPtr->GetReminderDays();
	const int reminderDays = qMin((chosenDays > 0) ? chosenDays : *m_defaultReminderDaysAttrPtr, *m_maxReminderDaysAttrPtr);
	for (const QPair<QByteArray, QDateTime>& date : std::as_const(dates)){
		RemindOfDate(contract, party.tenantId, date.first, date.second, reminderDays, now);
	}
}


void CContractReminderComp::RemindOfDate(
			const IContractInfo& contract,
			const QByteArray& tenantId,
			const QByteArray& kind,
			const QDateTime& date,
			int reminderDays,
			const QDateTime& now)
{
	if (!date.isValid() || now >= date || now < date.addDays(-reminderDays)){
		return;
	}

	const QByteArray reminderKey = kind + QByteArrayLiteral(":") + date.toUTC().toString(Qt::ISODate).toUtf8();
	m_contractManagerCompPtr->RecordReminder(contract.GetContractId(), tenantId, reminderKey);
}


} // namespace imtauth


