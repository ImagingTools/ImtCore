// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CFileSystemPublisherComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// std includes
#include <utility>

// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FileSystem.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CFileSystemPublisherComp::RegisterSubscription(
			const QByteArray& subscriptionId,
			const imtgql::CGqlRequest& gqlRequest,
			const imtrest::IRequest& networkRequest,
			QString& errorMessage)
{
	if (!m_changeNotifierCompPtr.IsValid()){
		errorMessage = QStringLiteral("Changes of the file system are not published");
		SendErrorMessage(0, QStringLiteral("Attribute 'ChangeNotifier' was not set"), "CFileSystemPublisherComp");

		return false;
	}

	const sdl::V1_0::imtbase::COnFileSystemChangedGqlRequest subscriptionRequest(gqlRequest, false);
	if (!subscriptionRequest.IsValid()){
		errorMessage = QStringLiteral("The subscription request is invalid");

		return false;
	}

	QString path;
	const sdl::V1_0::imtbase::OnFileSystemChangedRequestArguments& arguments = subscriptionRequest.GetRequestedArguments();
	if (arguments.input.has_value() && arguments.input->path.has_value()){
		path = *arguments.input->path;
	}

	// The checks of the subscriber go first: a folder is watched for an accepted subscription only
	if (!BaseClass::RegisterSubscription(subscriptionId, gqlRequest, networkRequest, errorMessage)){
		return false;
	}

	// The notifier resolves the path and checks the access in the context of this subscription request
	const QByteArray connectionId = GetConnectionId(networkRequest);
	QByteArray watchId;
	if (!m_changeNotifierCompPtr->AddWatch(path, *this, watchId, errorMessage)){
		BaseClass::UnregisterSubscription(connectionId, subscriptionId);

		return false;
	}

	QMutexLocker watchLocker(&m_watchMutex);
	const SubscriptionKey subscriptionKey(connectionId, subscriptionId);
	m_subscriptionsOfWatches.insert(watchId, subscriptionKey);
	m_watchesOfSubscriptions.insert(subscriptionKey, watchId);

	return true;
}


bool CFileSystemPublisherComp::UnregisterSubscription(const QByteArray& connectionId, const QByteArray& subscriptionId)
{
	QByteArray watchId;
	{
		QMutexLocker watchLocker(&m_watchMutex);
		watchId = m_watchesOfSubscriptions.take(SubscriptionKey(connectionId, subscriptionId));
		m_subscriptionsOfWatches.remove(watchId);
	}

	// Outside of the locks: the notifier may be calling the listener, which takes them
	if (!watchId.isEmpty() && m_changeNotifierCompPtr.IsValid()){
		m_changeNotifierCompPtr->RemoveWatch(watchId);
	}

	return BaseClass::UnregisterSubscription(connectionId, subscriptionId);
}


// reimplemented (imtfile::IFileSystemChangeListener)

void CFileSystemPublisherComp::OnFileSystemChanged(const QByteArray& watchId, const QString& path)
{
	SubscriptionKey subscriptionKey;
	{
		QMutexLocker watchLocker(&m_watchMutex);
		const auto found = m_subscriptionsOfWatches.constFind(watchId);
		if (found == m_subscriptionsOfWatches.constEnd()){
			// The subscription has ended meanwhile
			return;
		}

		subscriptionKey = found.value();
	}

	sdl::V1_0::imtbase::CFileSystemChangedEvent event;
	event.path = path;
	QJsonObject eventObject;
	if (!event.WriteToJsonObject(eventObject)){
		SendErrorMessage(0, QStringLiteral("Unable to write the change of the folder '%1'").arg(path), "CFileSystemPublisherComp");

		return;
	}

	const QByteArray data = QJsonDocument(eventObject).toJson(QJsonDocument::Compact);
	const QByteArray commandId = sdl::V1_0::imtbase::COnFileSystemChangedGqlRequest::GetCommandId();

	// The requests of the subscribers stay alive while m_mutex is held (see CGqlPublisherCompBase::PublishDataFiltered)
	QMutexLocker locker(&m_mutex);
	for (const RequestNetworks& entry : std::as_const(m_registeredSubscribers)){
		const auto found = entry.networkRequests.constFind(subscriptionKey.second);
		if (found == entry.networkRequests.constEnd()){
			continue;
		}

		for (const imtrest::IRequest* requestPtr : found.value()){
			if (requestPtr != nullptr && GetConnectionId(*requestPtr) == subscriptionKey.first){
				PushDataToSubscriber(subscriptionKey.second, commandId, data, *requestPtr);

				return;
			}
		}
	}
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CFileSystemPublisherComp::OnComponentDestroyed()
{
	QByteArrayList watchIds;
	{
		QMutexLocker watchLocker(&m_watchMutex);
		watchIds = m_subscriptionsOfWatches.keys();
		m_subscriptionsOfWatches.clear();
		m_watchesOfSubscriptions.clear();
	}

	if (m_changeNotifierCompPtr.IsValid()){
		for (const QByteArray& watchId : std::as_const(watchIds)){
			m_changeNotifierCompPtr->RemoveWatch(watchId);
		}
	}

	BaseClass::OnComponentDestroyed();
}


} // namespace imtservergql
