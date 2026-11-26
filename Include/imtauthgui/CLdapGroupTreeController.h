// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QVariantList>

// ImtCore includes
#include <imtauth/CLdapUserGroupSynchronizerComp.h>


namespace imtauthgui
{


/**
	QML controller reading the LDAP groups below a base DN (the LDAP group filter) and providing them as group tree.
	The groups are read in a worker thread with the same logic as used by imtauth::CLdapUserGroupSynchronizerComp,
	so the tree shows exactly the groups that would be synchronized.
	The tree is provided as flat list in depth-first order, each item is a map with the keys
	\c name, \c description, \c sid and \c level (depth in the tree, 0 for root groups).
	A group with several parent groups is shown below each of its parents.
	While reading, the number of groups read so far is provided, a running reading can be cancelled.
*/
class CLdapGroupTreeController: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QVariantList groups READ GetGroups NOTIFY groupsChanged)
	Q_PROPERTY(int groupCount READ GetGroupCount NOTIFY groupsChanged)
	Q_PROPERTY(bool busy READ IsBusy NOTIFY busyChanged)
	Q_PROPERTY(QString errorText READ GetErrorText NOTIFY errorTextChanged)
	Q_PROPERTY(int readGroupCount READ GetReadGroupCount NOTIFY readGroupCountChanged)
	Q_PROPERTY(bool cancelled READ IsCancelled NOTIFY cancelledChanged)

public:
	explicit CLdapGroupTreeController(QObject* parentPtr = nullptr);
	~CLdapGroupTreeController() override;

	QVariantList GetGroups() const;
	int GetGroupCount() const;
	bool IsBusy() const;
	QString GetErrorText() const;
	/**
		Number of groups read so far by the running reading.
	*/
	int GetReadGroupCount() const;
	/**
		\c true if the last reading was cancelled.
	*/
	bool IsCancelled() const;

	/**
		Read the LDAP groups below the given base DN (empty: domain of the computer) matching the name filter.
		If a reading is running, the request is executed after it.
	*/
	Q_INVOKABLE void refresh(const QString& baseDn, const QString& nameFilter = QStringLiteral("*"));

	/**
		Cancel the running reading and a pending request, the groups shown before are removed.
		The controller is not busy anymore immediately, the reading thread finishes in the background.
	*/
	Q_INVOKABLE void cancel();

	/**
		Build the flat group tree (see imtauth::CLdapUserGroupSynchronizerComp::CreateGroupTree) from the read LDAP groups.
	*/
	static QVariantList CreateGroupTree(const imtauth::CLdapUserGroupSynchronizerComp::LdapGroups& groups);

Q_SIGNALS:
	void groupsChanged();
	void busyChanged();
	void errorTextChanged();
	void readGroupCountChanged();
	void cancelledChanged();

private:
	struct ReadingState;

	void StartReading(const QString& baseDn, const QString& nameFilter);
	void OnReadingFinished(bool isSuccessful, const imtauth::CLdapUserGroupSynchronizerComp::LdapGroups& groups);
	void UpdateReadGroupCount();
	void SetErrorText(const QString& errorText);
	void SetReadGroupCount(int readGroupCount);
	void SetCancelled(bool isCancelled);

private:
	QVariantList m_groups;
	int m_groupCount;
	QString m_errorText;
	int m_readGroupCount;
	bool m_isCancelled;
	std::shared_ptr<ReadingState> m_readingStatePtr;
	QTimer m_progressTimer;

	bool m_hasPendingRequest;
	QString m_pendingBaseDn;
	QString m_pendingNameFilter;
};


} // namespace imtauthgui


