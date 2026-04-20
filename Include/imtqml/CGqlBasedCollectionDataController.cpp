// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlBasedCollectionDataController.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QtConcurrent/QtConcurrentRun>

// ImtCore includes
#include <imtqml/CGqlClientBridge.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


namespace imtqml
{


namespace imtcoll = sdl::imtbase::ImtCollection;


namespace
{


/**
	Schedule a callable on the GUI thread. The receiver is the global
	\c QCoreApplication instance (essentially app-lifetime), so the post
	itself never races with object destruction; the callable is expected
	to validate the \c QPointer it captured before touching the
	controller.
*/
template<class Fn>
void PostToMainThread(Fn&& fn)
{
	QCoreApplication* appPtr = QCoreApplication::instance();
	if (appPtr == nullptr){
		return;
	}
	QMetaObject::invokeMethod(appPtr, std::forward<Fn>(fn), Qt::QueuedConnection);
}


QByteArrayList VariantToByteArrayList(const QVariant& value)
{
	QByteArrayList result;
	if (value.canConvert<QVariantList>()){
		const QVariantList list = value.toList();
		result.reserve(list.size());
		for (const QVariant& item : list){
			result.append(item.toString().toUtf8());
		}
	}
	else if (value.canConvert<QStringList>()){
		const QStringList list = value.toStringList();
		result.reserve(list.size());
		for (const QString& item : list){
			result.append(item.toUtf8());
		}
	}
	else if (!value.toString().isEmpty()){
		result.append(value.toString().toUtf8());
	}
	return result;
}


QVariantList ByteArrayListToVariantList(const QByteArrayList& list)
{
	QVariantList result;
	result.reserve(list.size());
	for (const QByteArray& item : list){
		result.append(QString::fromUtf8(item));
	}
	return result;
}


} // namespace


// public methods

CGqlBasedCollectionDataController::CGqlBasedCollectionDataController(QObject* parent)
	:BaseClass(parent)
{
}


CGqlBasedCollectionDataController::~CGqlBasedCollectionDataController() = default;


const QString& CGqlBasedCollectionDataController::GetCollectionId() const
{
	return m_collectionId;
}


void CGqlBasedCollectionDataController::SetCollectionId(const QString& id)
{
	if (m_collectionId != id){
		m_collectionId = id;
		Q_EMIT collectionIdChanged(m_collectionId);
	}
}


// private methods

CGqlClientBridge* CGqlBasedCollectionDataController::ResolveBridge() const
{
	return CGqlClientBridge::Instance();
}


// public slots

void CGqlBasedCollectionDataController::getCollectionHeaders()
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT collectionHeadersReceiveFailed(QStringLiteral("GQL client bridge is not available"));
		return;
	}

	const QByteArray collectionId = m_collectionId.toUtf8();
	QPointer<CGqlBasedCollectionDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId]{
		imtcoll::GetCollectionHeadersRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;

		imtcoll::CGetCollectionHeadersPayload payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				imtcoll::GetCollectionHeadersRequestArguments,
				imtcoll::CGetCollectionHeadersPayload,
				imtcoll::CGetCollectionHeadersGqlRequest>(arguments, payload, errorMessage);

		if (!ok){
			const QString msg = errorMessage.isEmpty() ? QStringLiteral("Failed to fetch collection headers") : errorMessage;
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->collectionHeadersReceiveFailed(msg);
				}
			});
			return;
		}

		QVariantList headers;
		if (payload.Version_1_0->headers.HasValue()){
			for (const auto& header : payload.Version_1_0->headers->ToList()){
				QVariantMap entry;
				if (header.id){
					entry.insert(QStringLiteral("id"), QString::fromUtf8(*header.id));
				}
				if (header.name){
					entry.insert(QStringLiteral("name"), *header.name);
				}
				if (header.filterable){
					entry.insert(QStringLiteral("filterable"), *header.filterable);
				}
				if (header.sortable){
					entry.insert(QStringLiteral("sortable"), *header.sortable);
				}
				headers.append(entry);
			}
		}

		PostToMainThread([self, headers]{
			if (self){
				Q_EMIT self->collectionHeadersReceived(headers);
			}
		});
	});
}


void CGqlBasedCollectionDataController::getObjectData(const QString& objectId)
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT objectReceiveFailed(QStringLiteral("GQL client bridge is not available"));
		return;
	}

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray objectIdUtf8 = objectId.toUtf8();
	QPointer<CGqlBasedCollectionDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, objectIdUtf8]{
		imtcoll::GetObjectDataRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->objectId = objectIdUtf8;

		imtcoll::CGetObjectDataPayload payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				imtcoll::GetObjectDataRequestArguments,
				imtcoll::CGetObjectDataPayload,
				imtcoll::CGetObjectDataGqlRequest>(arguments, payload, errorMessage);

		if (!ok || !payload.Version_1_0->objectData.HasValue()){
			const QString msg = errorMessage.isEmpty() ? QStringLiteral("Failed to fetch object data") : errorMessage;
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->objectReceiveFailed(msg);
				}
			});
			return;
		}

		const QString objectData = *payload.Version_1_0->objectData;
		PostToMainThread([self, objectData]{
			if (self){
				Q_EMIT self->objectReceived(objectData);
			}
		});
	});
}


void CGqlBasedCollectionDataController::insertNewObject(
			const QString& typeId,
			const QString& name,
			const QString& description,
			const QVariant& objectData,
			const QString& objectId)
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT objectAddFailed(QStringLiteral("GQL client bridge is not available"));
		return;
	}

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray typeIdUtf8 = typeId.toUtf8();
	const QString nameStr = name;
	const QString descriptionStr = description;
	const QString objectDataStr = objectData.isValid() ? objectData.toString() : QString();
	const bool hasObjectData = objectData.isValid();
	const QByteArray proposedObjectId = objectId.toUtf8();
	QPointer<CGqlBasedCollectionDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, typeIdUtf8, nameStr, descriptionStr, objectDataStr, hasObjectData, proposedObjectId]{
		imtcoll::InsertNewObjectRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->typeId = typeIdUtf8;
		arguments.input.Version_1_0->name = nameStr;
		arguments.input.Version_1_0->description = descriptionStr;
		if (hasObjectData){
			arguments.input.Version_1_0->objectData = objectDataStr;
		}
		if (!proposedObjectId.isEmpty()){
			arguments.input.Version_1_0->proposedObjectId = proposedObjectId;
		}

		imtcoll::CInsertNewObjectPayload payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				imtcoll::InsertNewObjectRequestArguments,
				imtcoll::CInsertNewObjectPayload,
				imtcoll::CInsertNewObjectGqlRequest>(arguments, payload, errorMessage);

		if (!ok || !payload.Version_1_0->objectId.HasValue()){
			const QString msg = errorMessage.isEmpty() ? QStringLiteral("Failed to insert new object") : errorMessage;
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->objectAddFailed(msg);
				}
			});
			return;
		}

		const QString newId = QString::fromUtf8(*payload.Version_1_0->objectId);
		PostToMainThread([self, newId]{
			if (self){
				Q_EMIT self->objectAdded(newId);
			}
		});
	});
}


void CGqlBasedCollectionDataController::setObjectData(const QString& objectId, const QVariant& newObjectData)
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT objectUpdateFailed(QStringLiteral("GQL client bridge is not available"));
		return;
	}

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray objectIdUtf8 = objectId.toUtf8();
	const QString newObjectDataStr = newObjectData.toString();
	const QString objectIdStr = objectId;
	QPointer<CGqlBasedCollectionDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, objectIdUtf8, newObjectDataStr, objectIdStr]{
		imtcoll::SetObjectDataRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->objectId = objectIdUtf8;
		arguments.input.Version_1_0->objectData = newObjectDataStr;

		imtcoll::CSetObjectDataPayload payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				imtcoll::SetObjectDataRequestArguments,
				imtcoll::CSetObjectDataPayload,
				imtcoll::CSetObjectDataGqlRequest>(arguments, payload, errorMessage);

		const bool success = ok
				&& payload.Version_1_0->success.HasValue()
				&& *payload.Version_1_0->success;
		if (!success){
			const QString msg = errorMessage.isEmpty() ? QStringLiteral("Failed to set object data") : errorMessage;
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->objectUpdateFailed(msg);
				}
			});
			return;
		}

		PostToMainThread([self, objectIdStr]{
			if (self){
				Q_EMIT self->objectUpdated(objectIdStr);
			}
		});
	});
}


void CGqlBasedCollectionDataController::removeObjects(const QVariant& objectIds)
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT removeObjectsFailed(QStringLiteral("GQL client bridge is not available"));
		return;
	}

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArrayList ids = VariantToByteArrayList(objectIds);
	const QVariant idsVariant = ByteArrayListToVariantList(ids);
	QPointer<CGqlBasedCollectionDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, ids, idsVariant]{
		imtcoll::RemoveElementsRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->elementIds.Emplace();
		arguments.input.Version_1_0->elementIds->FromList(ids);

		imtcoll::CRemoveElementsPayload payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				imtcoll::RemoveElementsRequestArguments,
				imtcoll::CRemoveElementsPayload,
				imtcoll::CRemoveElementsGqlRequest>(arguments, payload, errorMessage);

		const bool success = ok
				&& payload.Version_1_0->success.HasValue()
				&& *payload.Version_1_0->success;
		if (!success){
			const QString msg = errorMessage.isEmpty() ? QStringLiteral("Failed to remove elements") : errorMessage;
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->removeObjectsFailed(msg);
				}
			});
			return;
		}

		PostToMainThread([self, idsVariant]{
			if (self){
				Q_EMIT self->objectsRemoved(idsVariant);
			}
		});
	});
}


void CGqlBasedCollectionDataController::removeObjectSet(const QVariant& /*selectionParams*/)
{
	qWarning() << "CGqlBasedCollectionDataController::removeObjectSet:"
				<< "ParamsSet-based selection is not supported by the SDL bridge";
	Q_EMIT removeObjectsFailed(QStringLiteral("ParamsSet-based removal is not supported"));
}


void CGqlBasedCollectionDataController::restoreObjects(const QVariant& objectIds)
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT restoreObjectsFailed(QStringLiteral("GQL client bridge is not available"));
		return;
	}

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArrayList ids = VariantToByteArrayList(objectIds);
	const QVariant idsVariant = ByteArrayListToVariantList(ids);
	QPointer<CGqlBasedCollectionDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, ids, idsVariant]{
		imtcoll::RestoreObjectsRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->objectIds.Emplace();
		arguments.input.Version_1_0->objectIds->FromList(ids);

		imtcoll::CRestoreObjectsPayload payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				imtcoll::RestoreObjectsRequestArguments,
				imtcoll::CRestoreObjectsPayload,
				imtcoll::CRestoreObjectsGqlRequest>(arguments, payload, errorMessage);

		const bool success = ok
				&& payload.Version_1_0->success.HasValue()
				&& *payload.Version_1_0->success;
		if (!success){
			const QString msg = errorMessage.isEmpty() ? QStringLiteral("Failed to restore objects") : errorMessage;
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->restoreObjectsFailed(msg);
				}
			});
			return;
		}

		PostToMainThread([self, idsVariant]{
			if (self){
				Q_EMIT self->objectsRestored(idsVariant);
			}
		});
	});
}


void CGqlBasedCollectionDataController::restoreObjectSet(const QVariant& /*selectionParams*/)
{
	qWarning() << "CGqlBasedCollectionDataController::restoreObjectSet:"
				<< "ParamsSet-based selection is not supported by the SDL bridge";
	Q_EMIT restoreObjectsFailed(QStringLiteral("ParamsSet-based restore is not supported"));
}


void CGqlBasedCollectionDataController::setObjectName(const QString& objectId, const QString& name)
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT objectRenameFailed(QStringLiteral("GQL client bridge is not available"));
		return;
	}

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray objectIdUtf8 = objectId.toUtf8();
	const QString objectIdStr = objectId;
	const QString nameStr = name;
	QPointer<CGqlBasedCollectionDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, objectIdUtf8, objectIdStr, nameStr]{
		imtcoll::SetObjectNameRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->objectId = objectIdUtf8;
		arguments.input.Version_1_0->name = nameStr;

		imtcoll::CSetObjectNamePayload payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				imtcoll::SetObjectNameRequestArguments,
				imtcoll::CSetObjectNamePayload,
				imtcoll::CSetObjectNameGqlRequest>(arguments, payload, errorMessage);

		const bool success = ok
				&& payload.Version_1_0->success.HasValue()
				&& *payload.Version_1_0->success;
		if (!success){
			const QString msg = errorMessage.isEmpty() ? QStringLiteral("Failed to set object name") : errorMessage;
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->objectRenameFailed(msg);
				}
			});
			return;
		}

		QString resolvedId = objectIdStr;
		QString resolvedName = nameStr;
		if (payload.Version_1_0->objectId.HasValue()){
			resolvedId = QString::fromUtf8(*payload.Version_1_0->objectId);
		}
		if (payload.Version_1_0->name.HasValue()){
			resolvedName = *payload.Version_1_0->name;
		}

		PostToMainThread([self, resolvedId, resolvedName]{
			if (self){
				Q_EMIT self->objectRenamed(resolvedId, resolvedName);
			}
		});
	});
}


void CGqlBasedCollectionDataController::setObjectDescription(const QString& objectId, const QString& description)
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT objectSetDescriptionFailed(QStringLiteral("GQL client bridge is not available"));
		return;
	}

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray objectIdUtf8 = objectId.toUtf8();
	const QString objectIdStr = objectId;
	const QString descriptionStr = description;
	QPointer<CGqlBasedCollectionDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, objectIdUtf8, objectIdStr, descriptionStr]{
		imtcoll::SetObjectDescriptionRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->objectId = objectIdUtf8;
		arguments.input.Version_1_0->description = descriptionStr;

		imtcoll::CSetObjectDescriptionPayload payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				imtcoll::SetObjectDescriptionRequestArguments,
				imtcoll::CSetObjectDescriptionPayload,
				imtcoll::CSetObjectDescriptionGqlRequest>(arguments, payload, errorMessage);

		const bool success = ok
				&& payload.Version_1_0->success.HasValue()
				&& *payload.Version_1_0->success;
		if (!success){
			const QString msg = errorMessage.isEmpty() ? QStringLiteral("Failed to set object description") : errorMessage;
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->objectSetDescriptionFailed(msg);
				}
			});
			return;
		}

		QString resolvedId = objectIdStr;
		QString resolvedDescription = descriptionStr;
		if (payload.Version_1_0->objectId.HasValue()){
			resolvedId = QString::fromUtf8(*payload.Version_1_0->objectId);
		}
		if (payload.Version_1_0->description.HasValue()){
			resolvedDescription = *payload.Version_1_0->description;
		}

		PostToMainThread([self, resolvedId, resolvedDescription]{
			if (self){
				Q_EMIT self->objectDescriptionSetted(resolvedId, resolvedDescription);
			}
		});
	});
}


void CGqlBasedCollectionDataController::createSubCollection(int /*offset*/, int /*count*/, const QVariant& /*selectionParams*/)
{
	qWarning() << "CGqlBasedCollectionDataController::createSubCollection:"
				<< "ParamsSet-based sub-collections are not supported by the SDL bridge";
	Q_EMIT subCollectionCreateFailed(QStringLiteral("ParamsSet-based sub-collections are not supported"));
}


} // namespace imtqml
