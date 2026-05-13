// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CModelSubscriberComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtclientgql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriptionClient)

void CModelSubscriberComp::OnResponseReceived(const QByteArray& subscriptionId, const QByteArray& subscriptionData)
{
	istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
	imtbase::ICollectionInfo::NotifierInfo notifierInfo;

	QJsonDocument document = QJsonDocument::fromJson(subscriptionData);
	QJsonObject subscriptionObject = document.object();

	if (subscriptionObject.contains("data")){
		subscriptionObject = subscriptionObject.value("data").toObject();
	}

	if (m_subscriptionIds.contains(subscriptionId)){
		if (subscriptionObject.contains("id")){
			notifierInfo.elementId = subscriptionObject.value("id").toString().toUtf8();
			changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ALL_CHANGED, QVariant::fromValue(notifierInfo));
		}

		if (subscriptionObject.contains("token")){
			QByteArray token = subscriptionObject.value("token").toString().toUtf8();
			changeSet.SetChangeInfo("token", token);
		}

		if (subscriptionObject.contains("changeSet")){
			QJsonObject changeSetObject = subscriptionObject.value("changeSet").toObject();

			if (changeSetObject.contains("ids")){
				QSet<int> ids;
				QJsonArray idsArray = changeSetObject.value("ids").toArray();
				for (const auto& value : idsArray){
					if (value.isObject()){
						QJsonObject obj = value.toObject();
						ids << obj.value("id").toInt();
					}
				}

				changeSet += ids;
			}

			if (changeSetObject.contains("infoMap")){
				QJsonObject infoMapObject = changeSetObject.value("infoMap").toObject();
				for (const QString& key : infoMapObject.keys()){
					changeSet.SetChangeInfo(key.toUtf8(), infoMapObject[key].toVariant());
				}
			}
		}
	}

	if (m_changeableModelCompPtr.IsValid()){
		istd::CChangeNotifier notifier(m_changeableModelCompPtr.GetPtr(), &changeSet);
	}
}


} // namespace imtclientgql


