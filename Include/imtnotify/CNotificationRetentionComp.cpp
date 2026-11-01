// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotify/CNotificationRetentionComp.h>


// Qt includes
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtnotify
{


namespace
{


// The first removal soon after the start, not during it
const int s_firstCheckDelay = 60 * 1000;

// A large removal is made in portions with pauses, so the thread keeps serving
const int s_maxRemovedCount = 500;
const int s_portionDelay = 200;


} // namespace


// protected methods

// reimplemented (icomp::CComponentBase)

void CNotificationRetentionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_userCollectionModelCompPtr.IsValid()){
		RegisterModel(m_userCollectionModelCompPtr.GetPtr());
	}

	QObject::connect(&m_timer, &QTimer::timeout, [this](){
		RemoveExpired();
	});

	m_timer.start(s_firstCheckDelay);
}


void CNotificationRetentionComp::OnComponentDestroyed()
{
	m_timer.stop();

	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CNotificationRetentionComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& changeSet)
{
	if (!changeSet.Contains(imtbase::ICollectionInfo::CF_REMOVED)){
		return;
	}

	const imtbase::ICollectionInfo::ElementsRemoveInfo removeInfo =
				changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENTS_REMOVED).value<imtbase::ICollectionInfo::ElementsRemoveInfo>();
	for (const QByteArray& userId : removeInfo.elementIds){
		if (!m_inboxControllerCompPtr->RemoveUserNotifications(QByteArray(), userId)){
			SendErrorMessage(0, QStringLiteral("The notifications of the removed user '%1' could not be removed").arg(QString::fromUtf8(userId)), "CNotificationRetentionComp");
		}
	}
}


// private methods

void CNotificationRetentionComp::RemoveExpired()
{
	const int checkInterval = qMax(1, *m_checkIntervalHoursAttrPtr) * 3600 * 1000;

	const QDateTime createdBefore = QDateTime::currentDateTimeUtc().addDays(-qMax(1, *m_retentionDaysAttrPtr));
	bool isComplete = true;
	if (!m_inboxControllerCompPtr->RemoveExpired(createdBefore, s_maxRemovedCount, isComplete)){
		SendWarningMessage(0, QStringLiteral("The expired notifications could not be removed"), "CNotificationRetentionComp");
	}

	m_timer.start(isComplete ? checkInterval : s_portionDelay);
}


} // namespace imtnotify


