// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CLdapGroupChangeListener.h>


// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QMutexLocker>

#ifdef Q_OS_WIN
// STL includes
#include <string>

// Windows includes
#include <windows.h>
#include <winldap.h>
#pragma comment(lib, "wldap32.lib")
#endif


namespace imtauth
{


namespace
{


// A burst of directory changes is reported once the directory has been quiet for this time.
const int s_quietPeriod = 2000;
// A continuous stream of changes is still reported at least this often.
const int s_maximumDelay = 30000;
// Upper bound for waiting on the socket before the stop request is checked again.
const int s_resultWaitTime = 500;
const int s_minimumRetryDelay = 5000;
const int s_maximumRetryDelay = 300000;
// A registration that lasted this long resets the reconnect delay.
const int s_stableRegistrationTime = 60000;


#ifdef Q_OS_WIN
WCHAR s_notificationControlOid[] = L"1.2.840.113556.1.4.528";
WCHAR s_showDeletedControlOid[] = L"1.2.840.113556.1.4.417";
WCHAR s_anyObjectFilter[] = L"(objectClass=*)";
WCHAR s_objectClassAttribute[] = L"objectClass";
WCHAR s_namingContextAttribute[] = L"defaultNamingContext";
WCHAR s_supportedControlAttribute[] = L"supportedControl";
WCHAR s_rootBase[] = L"";


class ConnectionScope
{
public:
	explicit ConnectionScope(LDAP* connectionPtr)
		:m_connectionPtr(connectionPtr)
	{
	}

	~ConnectionScope()
	{
		if (m_connectionPtr != nullptr){
			ldap_unbind(m_connectionPtr);
		}
	}

private:
	LDAP* m_connectionPtr;
};


class MessageScope
{
public:
	MessageScope()
		:messagePtr(nullptr)
	{
	}

	~MessageScope()
	{
		if (messagePtr != nullptr){
			ldap_msgfree(messagePtr);
		}
	}

	LDAPMessage* messagePtr;
};


QString LdapErrorMessage(const QString& action, ULONG status)
{
	return QStringLiteral("%1 (LDAP error %2)").arg(action).arg(status);
}


bool ValuesContain(PWCHAR* values, const wchar_t* expectedValue)
{
	bool found = false;
	for (ULONG index = 0; values[index] != nullptr; ++index){
		if (_wcsicmp(values[index], expectedValue) == 0){
			found = true;
		}
	}

	return found;
}


bool ContainsValue(LDAP* connectionPtr, LDAPMessage* entryPtr, PWCHAR attribute, const wchar_t* expectedValue)
{
	PWCHAR* values = ldap_get_valuesW(connectionPtr, entryPtr, attribute);
	if (values == nullptr){
		return false;
	}

	bool found = ValuesContain(values, expectedValue);

	ldap_value_freeW(values);

	return found;
}


bool ContainsGroupObject(LDAP* connectionPtr, LDAPMessage* resultPtr)
{
	for (LDAPMessage* entryPtr = ldap_first_entry(connectionPtr, resultPtr); entryPtr != nullptr; entryPtr = ldap_next_entry(connectionPtr, entryPtr)){
		PWCHAR* classes = ldap_get_valuesW(connectionPtr, entryPtr, s_objectClassAttribute);
		if (classes == nullptr){
			// An entry without readable classes may hide a group change; resynchronize rather than miss it.
			return true;
		}

		bool isGroup = ValuesContain(classes, L"group");

		ldap_value_freeW(classes);

		if (isGroup){
			return true;
		}
	}

	return false;
}
#endif


} // namespace


CLdapGroupChangeListener::CLdapGroupChangeListener()
	:m_isStopRequested(false)
{
}


CLdapGroupChangeListener::~CLdapGroupChangeListener()
{
	Stop();
}


bool CLdapGroupChangeListener::IsSupported()
{
#ifdef Q_OS_WIN
	return true;
#else
	return false;
#endif
}


bool CLdapGroupChangeListener::Start(const Callback& callback, QString& errorMessage)
{
	if (!IsSupported()){
		errorMessage = QStringLiteral("Directory change notifications require Active Directory on Windows (LDAP_SERVER_NOTIFICATION_OID); they are not supported on this platform and are not replaced by polling");

		return false;
	}

	if (m_threadPtr != nullptr || !callback){
		errorMessage = QStringLiteral("Directory change listener is already running or has no receiver");

		return false;
	}

	{
		QMutexLocker locker(&m_stopMutex);

		m_isStopRequested = false;
	}

	m_callback = callback;
	m_threadPtr.reset(QThread::create([this](){
		Run();
	}));
	m_threadPtr->start();

	return true;
}


void CLdapGroupChangeListener::Stop()
{
	if (m_threadPtr == nullptr){
		return;
	}

	{
		QMutexLocker locker(&m_stopMutex);

		m_isStopRequested = true;
		m_stopCondition.wakeAll();
	}

	m_threadPtr->wait();
	m_threadPtr.reset();
	m_callback = Callback();
}


bool CLdapGroupChangeListener::IsRunning() const
{
	return m_threadPtr != nullptr && m_threadPtr->isRunning();
}


// private methods

void CLdapGroupChangeListener::Run()
{
	int retryDelay = s_minimumRetryDelay;
	while (!IsStopRequested()){
		QString message;
		bool isUnsupported = false;
		QElapsedTimer registrationTimer;
		registrationTimer.start();
		bool wasRegistered = Listen(message, isUnsupported);
		if (IsStopRequested()){
			break;
		}

		if (isUnsupported){
			m_callback(E_UNSUPPORTED, message);

			break;
		}

		m_callback(E_DISCONNECTED, message);

		// A server that ends the registration immediately is not retried faster and faster.
		if (wasRegistered && registrationTimer.elapsed() >= s_stableRegistrationTime){
			retryDelay = s_minimumRetryDelay;
		}

		if (WaitForStop(retryDelay)){
			break;
		}

		retryDelay = qMin(retryDelay * 2, s_maximumRetryDelay);
	}
}


bool CLdapGroupChangeListener::Listen(QString& message, bool& isUnsupported)
{
	isUnsupported = false;

#ifdef Q_OS_WIN
	// A null host name selects a domain controller of the service identity's domain.
	LDAP* connectionPtr = ldap_initW(nullptr, LDAP_PORT);
	if (connectionPtr == nullptr){
		message = QStringLiteral("Unable to locate an Active Directory domain controller");

		return false;
	}

	ConnectionScope connection(connectionPtr);

	ULONG version = LDAP_VERSION3;
	ULONG status = ldap_set_optionW(connectionPtr, LDAP_OPT_PROTOCOL_VERSION, &version);
	if (status == LDAP_SUCCESS){
		status = ldap_set_optionW(connectionPtr, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);
	}

	if (status == LDAP_SUCCESS){
		status = ldap_set_optionW(connectionPtr, LDAP_OPT_SIGN, LDAP_OPT_ON);
	}

	if (status == LDAP_SUCCESS){
		status = ldap_set_optionW(connectionPtr, LDAP_OPT_ENCRYPT, LDAP_OPT_ON);
	}

	if (status != LDAP_SUCCESS){
		message = LdapErrorMessage(QStringLiteral("Unable to configure a signed and sealed directory connection"), status);

		return false;
	}

	l_timeval connectTimeout;
	connectTimeout.tv_sec = 30;
	connectTimeout.tv_usec = 0;
	status = ldap_connect(connectionPtr, &connectTimeout);
	if (status == LDAP_SUCCESS){
		status = ldap_bind_sW(connectionPtr, nullptr, nullptr, LDAP_AUTH_NEGOTIATE);
	}

	if (status != LDAP_SUCCESS){
		message = LdapErrorMessage(QStringLiteral("Unable to connect to Active Directory with the service identity"), status);

		return false;
	}

	std::wstring namingContext;
	bool hasNotificationControl = false;
	{
		PWCHAR rootAttributes[] = {s_namingContextAttribute, s_supportedControlAttribute, nullptr};
		MessageScope rootResult;
		status = ldap_search_sW(connectionPtr, s_rootBase, LDAP_SCOPE_BASE, s_anyObjectFilter, rootAttributes, 0, &rootResult.messagePtr);
		if (status != LDAP_SUCCESS){
			message = LdapErrorMessage(QStringLiteral("Unable to read the directory root entry"), status);

			return false;
		}

		LDAPMessage* entryPtr = ldap_first_entry(connectionPtr, rootResult.messagePtr);
		if (entryPtr != nullptr){
			PWCHAR* values = ldap_get_valuesW(connectionPtr, entryPtr, s_namingContextAttribute);
			if (values != nullptr){
				if (values[0] != nullptr){
					namingContext = values[0];
				}

				ldap_value_freeW(values);
			}

			hasNotificationControl = ContainsValue(connectionPtr, entryPtr, s_supportedControlAttribute, s_notificationControlOid);
		}
	}

	if (!hasNotificationControl){
		isUnsupported = true;
		message = QStringLiteral("The directory server does not support Active Directory change notifications (1.2.840.113556.1.4.528); group changes are not received and are not polled");

		return false;
	}

	if (namingContext.empty()){
		message = QStringLiteral("Unable to read the default naming context of the domain");

		return false;
	}

	// Active Directory accepts subtree notifications only on the head of a naming context and only with this filter.
	LDAPControlW notificationControl = {};
	notificationControl.ldctl_oid = s_notificationControlOid;
	notificationControl.ldctl_iscritical = TRUE;
	// Deleted groups are reported as tombstones only with the show-deleted control.
	LDAPControlW showDeletedControl = {};
	showDeletedControl.ldctl_oid = s_showDeletedControlOid;
	showDeletedControl.ldctl_iscritical = TRUE;
	PLDAPControlW controls[] = {&notificationControl, &showDeletedControl, nullptr};
	PWCHAR attributes[] = {s_objectClassAttribute, nullptr};
	ULONG messageId = 0;
	status = ldap_search_extW(
				connectionPtr,
				&namingContext[0],
				LDAP_SCOPE_SUBTREE,
				s_anyObjectFilter,
				attributes,
				0,
				controls,
				nullptr,
				0,
				0,
				&messageId);
	if (status == LDAP_UNAVAILABLE_CRIT_EXTENSION){
		isUnsupported = true;
		message = QStringLiteral("The directory server rejected the Active Directory change-notification control; group changes are not received and are not polled");

		return false;
	}

	if (status != LDAP_SUCCESS){
		message = LdapErrorMessage(QStringLiteral("Unable to register for Active Directory change notifications"), status);

		return false;
	}

	m_callback(E_CONNECTED, QString());

	bool isPending = false;
	QElapsedTimer pendingTimer;
	QElapsedTimer quietTimer;
	while (!IsStopRequested()){
		l_timeval resultTimeout;
		resultTimeout.tv_sec = 0;
		resultTimeout.tv_usec = s_resultWaitTime * 1000;
		MessageScope result;
		ULONG resultType = ldap_result(connectionPtr, messageId, LDAP_MSG_ONE, &resultTimeout, &result.messagePtr);
		if (resultType == LDAP_RES_SEARCH_ENTRY){
			if (ContainsGroupObject(connectionPtr, result.messagePtr)){
				if (!isPending){
					isPending = true;
					pendingTimer.start();
				}

				quietTimer.start();
			}
		}
		else if (resultType == LDAP_RES_SEARCH_RESULT){
			ULONG resultStatus = ldap_result2error(connectionPtr, result.messagePtr, FALSE);
			message = LdapErrorMessage(QStringLiteral("Active Directory ended the change-notification search"), resultStatus);

			return true;
		}
		else if (resultType == ULONG(-1)){
			message = LdapErrorMessage(QStringLiteral("The Active Directory change-notification connection failed"), LdapGetLastError());

			return true;
		}

		if (isPending && (quietTimer.elapsed() >= s_quietPeriod || pendingTimer.elapsed() >= s_maximumDelay)){
			isPending = false;
			m_callback(E_GROUPS_CHANGED, QString());
		}
	}

	ldap_abandon(connectionPtr, messageId);

	return true;
#else
	isUnsupported = true;
	message = QStringLiteral("Directory change notifications are not supported on this platform");

	return false;
#endif
}


bool CLdapGroupChangeListener::IsStopRequested()
{
	QMutexLocker locker(&m_stopMutex);

	return m_isStopRequested;
}


bool CLdapGroupChangeListener::WaitForStop(int milliseconds)
{
	QMutexLocker locker(&m_stopMutex);

	if (!m_isStopRequested){
		m_stopCondition.wait(&m_stopMutex, milliseconds);
	}

	return m_isStopRequested;
}


} // namespace imtauth


