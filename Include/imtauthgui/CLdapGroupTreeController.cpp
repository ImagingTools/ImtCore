// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgui/CLdapGroupTreeController.h>


// STL includes
#include <atomic>

// Qt includes
#include <QtCore/QSet>
#include <QtCore/QVariantMap>


namespace imtauthgui
{


// State of a reading shared between the controller and the reading thread.
struct CLdapGroupTreeController::ReadingState
{
	std::atomic<bool> isCancelled{false};
	std::atomic<int> readGroupCount{0};

	// Written by the reading thread, read after it is finished.
	bool isSuccessful = false;
	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups groups;
};


// public methods

CLdapGroupTreeController::CLdapGroupTreeController(QObject* parentPtr)
	:QObject(parentPtr),
	m_groupCount(0),
	m_readGroupCount(0),
	m_isCancelled(false),
	m_hasPendingRequest(false)
{
	m_progressTimer.setInterval(200);

	connect(&m_progressTimer, &QTimer::timeout, this, &CLdapGroupTreeController::UpdateReadGroupCount);
}


CLdapGroupTreeController::~CLdapGroupTreeController()
{
	// The reading thread only uses the shared state and deletes itself when finished,
	// a reading blocked by the directory server does not block the closing of the application.
	if (m_readingStatePtr){
		m_readingStatePtr->isCancelled = true;
	}
}


QVariantList CLdapGroupTreeController::GetGroups() const
{
	return m_groups;
}


int CLdapGroupTreeController::GetGroupCount() const
{
	return m_groupCount;
}


bool CLdapGroupTreeController::IsBusy() const
{
	return m_readingStatePtr != nullptr;
}


QString CLdapGroupTreeController::GetErrorText() const
{
	return m_errorText;
}


int CLdapGroupTreeController::GetReadGroupCount() const
{
	return m_readGroupCount;
}


bool CLdapGroupTreeController::IsCancelled() const
{
	return m_isCancelled;
}


void CLdapGroupTreeController::refresh(const QString& baseDn, const QString& nameFilter)
{
	if (IsBusy()){
		m_hasPendingRequest = true;
		m_pendingBaseDn = baseDn;
		m_pendingNameFilter = nameFilter;

		return;
	}

	StartReading(baseDn, nameFilter);
}


void CLdapGroupTreeController::cancel()
{
	m_hasPendingRequest = false;

	if (!IsBusy()){
		return;
	}

	m_progressTimer.stop();

	// The thread cannot be stopped while waiting for the directory server, it is left to finish and its result is ignored.
	if (m_readingStatePtr){
		m_readingStatePtr->isCancelled = true;
		m_readingStatePtr.reset();
	}

	// The groups shown before could belong to another filter.
	m_groups.clear();
	m_groupCount = 0;

	SetErrorText(QString());
	SetCancelled(true);

	Q_EMIT groupsChanged();
	Q_EMIT busyChanged();
}


QVariantList CLdapGroupTreeController::CreateGroupTree(const imtauth::CLdapUserGroupSynchronizerComp::LdapGroups& groups)
{
	QVariantList result;

	const imtauth::CLdapUserGroupSynchronizerComp::LdapGroupTree tree = imtauth::CLdapUserGroupSynchronizerComp::CreateGroupTree(groups);
	for (const imtauth::CLdapUserGroupSynchronizerComp::LdapGroupTreeItem& treeItem : tree){
		const imtauth::CLdapUserGroupSynchronizerComp::LdapGroup& group = groups[treeItem.groupIndex];

		QVariantMap item;
		item[QStringLiteral("name")] = group.name;
		item[QStringLiteral("description")] = group.description;
		item[QStringLiteral("sid")] = QString::fromUtf8(group.sid);
		item[QStringLiteral("level")] = treeItem.level;
		result.append(item);
	}

	return result;
}


// private methods

void CLdapGroupTreeController::StartReading(const QString& baseDn, const QString& nameFilter)
{
	std::shared_ptr<ReadingState> statePtr = std::make_shared<ReadingState>();

	QThread* threadPtr = QThread::create([statePtr, baseDn, nameFilter](){
		statePtr->isSuccessful = imtauth::CLdapUserGroupSynchronizerComp::ReadLdapGroups(
					nameFilter.isEmpty() ? QStringLiteral("*") : nameFilter,
					baseDn,
					statePtr->groups,
					[statePtr](int readGroupCount){
						statePtr->readGroupCount = readGroupCount;

						return !statePtr->isCancelled;
					});
	});

	m_readingStatePtr = statePtr;

	// The thread deletes itself, also if the reading was cancelled or the controller is already destroyed.
	connect(threadPtr, &QThread::finished, threadPtr, &QObject::deleteLater);

	connect(threadPtr, &QThread::finished, this, [this, statePtr](){
		if (m_readingStatePtr != statePtr){
			// The reading was cancelled.
			return;
		}

		m_readingStatePtr.reset();
		m_progressTimer.stop();

		SetReadGroupCount(statePtr->readGroupCount);

		OnReadingFinished(statePtr->isSuccessful, statePtr->groups);
	});

	SetReadGroupCount(0);
	SetCancelled(false);

	threadPtr->start();

	m_progressTimer.start();

	Q_EMIT busyChanged();
}


void CLdapGroupTreeController::OnReadingFinished(bool isSuccessful, const imtauth::CLdapUserGroupSynchronizerComp::LdapGroups& groups)
{
	if (m_hasPendingRequest){
		// The settings were changed during the reading, the result is outdated.
		m_hasPendingRequest = false;

		StartReading(m_pendingBaseDn, m_pendingNameFilter);

		return;
	}

	if (isSuccessful){
		m_groups = CreateGroupTree(groups);

		QSet<QByteArray> groupSids;
		for (const imtauth::CLdapUserGroupSynchronizerComp::LdapGroup& group : groups){
			if (group.isMatching && !group.sid.isEmpty()){
				groupSids.insert(group.sid);
			}
		}

		m_groupCount = groupSids.count();

		SetErrorText(QString());
	}
	else{
		m_groups.clear();
		m_groupCount = 0;

#ifdef Q_OS_WIN
		SetErrorText(tr("The LDAP groups could not be read. Please check the LDAP group filter (base DN) and the connection to the domain."));
#else
		SetErrorText(tr("Reading of LDAP groups is supported on Windows only."));
#endif
	}

	Q_EMIT groupsChanged();
	Q_EMIT busyChanged();
}


void CLdapGroupTreeController::UpdateReadGroupCount()
{
	if (m_readingStatePtr){
		SetReadGroupCount(m_readingStatePtr->readGroupCount);
	}
}


void CLdapGroupTreeController::SetErrorText(const QString& errorText)
{
	if (m_errorText != errorText){
		m_errorText = errorText;

		Q_EMIT errorTextChanged();
	}
}


void CLdapGroupTreeController::SetReadGroupCount(int readGroupCount)
{
	if (m_readGroupCount != readGroupCount){
		m_readGroupCount = readGroupCount;

		Q_EMIT readGroupCountChanged();
	}
}


void CLdapGroupTreeController::SetCancelled(bool isCancelled)
{
	if (m_isCancelled != isCancelled){
		m_isCancelled = isCancelled;

		Q_EMIT cancelledChanged();
	}
}


} // namespace imtauthgui


