// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>
#include <memory>

// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>


namespace imtauth
{


/**
	Receives Active Directory change notifications for group objects.

	The listener registers one persistent asynchronous search with the Active Directory
	change-notification control (LDAP_SERVER_NOTIFICATION_OID, 1.2.840.113556.1.4.528)
	on the default naming context of the service identity's domain. The domain controller
	pushes every change; the listener does not poll the directory. The connection uses
	integrated (Negotiate) authentication with signing and sealing.

	Events are delivered on the listener thread. Group changes are coalesced: an event is
	sent after the directory has been quiet for a short time. After every registration
	(startup and reconnect) E_CONNECTED is sent, because changes made while the listener
	was not registered are not delivered by the server.

	A directory server that does not announce the notification control and every platform
	other than Windows are reported as unsupported; there is no polling fallback.

	Start() and Stop() must be called from the same owner thread. The callback must not
	call Start() or Stop(). Stop() returns after the listener thread has finished, so the
	callback is never called after Stop() has returned.
*/
class CLdapGroupChangeListener
{
public:
	enum Event
	{
		/**
			The notification search is registered (startup or reconnect).
			Changes made before may have been missed; the receiver should resynchronize.
		*/
		E_CONNECTED,
		/**
			At least one group object was created, changed, renamed, moved or deleted.
		*/
		E_GROUPS_CHANGED,
		/**
			The notification search failed or was ended by the server; the listener reconnects.
		*/
		E_DISCONNECTED,
		/**
			The platform or the directory server does not support change notifications.
			The listener thread ends.
		*/
		E_UNSUPPORTED
	};

	typedef std::function<void (Event event, const QString& message)> Callback;

	CLdapGroupChangeListener();
	~CLdapGroupChangeListener();

	static bool IsSupported();

	/**
		Starts the listener thread.
		\return false with an error message if notifications are not supported on this platform
		or the listener is already running.
	*/
	bool Start(const Callback& callback, QString& errorMessage);
	void Stop();
	bool IsRunning() const;

private:
	void Run();
	bool Listen(QString& message, bool& isUnsupported);
	bool IsStopRequested();
	bool WaitForStop(int milliseconds);

private:
	Q_DISABLE_COPY(CLdapGroupChangeListener)

	Callback m_callback;
	std::unique_ptr<QThread> m_threadPtr;
	QMutex m_stopMutex;
	QWaitCondition m_stopCondition;
	bool m_isStopRequested;
};


} // namespace imtauth


