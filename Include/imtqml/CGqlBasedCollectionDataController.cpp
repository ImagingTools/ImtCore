// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlBasedCollectionDataController.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QStringList>

// ImtCore includes
#include <imtqml/CGqlRequest.h>


namespace imtqml
{


namespace
{


bool ExtractError(const QJsonDocument& doc, QString& message, QString& type)
{
	if (!doc.isObject()) {
		return false;
	}

	const QJsonValue errorsValue = doc.object().value(QStringLiteral("errors"));
	if (!errorsValue.isArray()) {
		return false;
	}

	const QJsonArray errors = errorsValue.toArray();
	if (errors.isEmpty()) {
		message = QStringLiteral("Unknown error");
		type = QStringLiteral("Error");
		return true;
	}

	const QJsonObject first = errors.first().toObject();
	message = first.value(QStringLiteral("message")).toString();
	const QJsonObject extensions = first.value(QStringLiteral("extensions")).toObject();
	type = extensions.value(QStringLiteral("type")).toString();
	return true;
}


QVariant ExtractData(const QJsonDocument& doc, const QString& commandId)
{
	if (!doc.isObject()) {
		return QVariant();
	}

	const QJsonValue dataValue = doc.object().value(QStringLiteral("data"));
	if (!dataValue.isObject()) {
		return QVariant();
	}

	return dataValue.toObject().value(commandId).toVariant();
}


QString EscapeGqlString(const QString& s)
{
	QString out;
	out.reserve(s.size() + 2);
	out.append(QLatin1Char('"'));
	for (QChar c : s) {
		ushort u = c.unicode();
		switch (u) {
			case '"':  out.append(QLatin1String("\\\"")); break;
			case '\\': out.append(QLatin1String("\\\\")); break;
			case '\b': out.append(QLatin1String("\\b"));  break;
			case '\f': out.append(QLatin1String("\\f"));  break;
			case '\n': out.append(QLatin1String("\\n"));  break;
			case '\r': out.append(QLatin1String("\\r"));  break;
			case '\t': out.append(QLatin1String("\\t"));  break;
			default:
				if (u < 0x20) {
					out.append(QStringLiteral("\\u%1").arg(u, 4, 16, QLatin1Char('0')));
				} else {
					out.append(c);
				}
				break;
		}
	}
	out.append(QLatin1Char('"'));
	return out;
}


QString ToGqlLiteral(const QVariant& value)
{
	if (!value.isValid() || value.isNull()) {
		return QStringLiteral("null");
	}

	switch (static_cast<QMetaType::Type>(value.userType())) {
		case QMetaType::Bool:
			return value.toBool() ? QStringLiteral("true") : QStringLiteral("false");

		case QMetaType::Int:
		case QMetaType::UInt:
		case QMetaType::LongLong:
		case QMetaType::ULongLong:
		case QMetaType::Short:
		case QMetaType::UShort:
		case QMetaType::Long:
		case QMetaType::ULong:
			return value.toString();

		case QMetaType::Float:
		case QMetaType::Double:
			return QString::number(value.toDouble(), 'g', 17);

		case QMetaType::QString:
		case QMetaType::QByteArray:
		case QMetaType::QChar:
			return EscapeGqlString(value.toString());

		case QMetaType::QVariantList:
		case QMetaType::QStringList: {
			const QVariantList list = value.toList();
			QStringList parts;
			parts.reserve(list.size());
			for (const QVariant& item : list) {
				parts.append(ToGqlLiteral(item));
			}
			return QLatin1Char('[') + parts.join(QLatin1Char(',')) + QLatin1Char(']');
		}

		case QMetaType::QVariantMap: {
			const QVariantMap map = value.toMap();
			QStringList parts;
			parts.reserve(map.size());
			for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
				parts.append(it.key() + QLatin1Char(':') + ToGqlLiteral(it.value()));
			}
			return QLatin1Char('{') + parts.join(QLatin1Char(',')) + QLatin1Char('}');
		}

		default:
			break;
	}

	const QJsonDocument fallback = QJsonDocument::fromVariant(value);
	if (!fallback.isNull()) {
		return QString::fromUtf8(fallback.toJson(QJsonDocument::Compact));
	}
	return EscapeGqlString(value.toString());
}


QString BuildGqlRequest(
			const QString& operation,
			const QString& commandId,
			const QVariantMap& inputArgs,
			const QString& selection)
{
	QString args;
	if (!inputArgs.isEmpty()) {
		args = QStringLiteral("(input:") + ToGqlLiteral(QVariant(inputArgs)) + QLatin1Char(')');
	}

	const QString selectionPart = selection.isEmpty()
			? QString()
			: (QLatin1Char(' ') + selection);

	return operation + QLatin1Char(' ') + commandId
			+ QStringLiteral(" { ") + commandId + args + selectionPart + QStringLiteral(" }");
}


} // namespace


CGqlBasedCollectionDataController::CGqlBasedCollectionDataController(QObject* parent)
	: BaseClass(parent)
{
}


CGqlBasedCollectionDataController::~CGqlBasedCollectionDataController() = default;


const QVariantMap& CGqlBasedCollectionDataController::GetHeaders() const
{
	return m_headers;
}


void CGqlBasedCollectionDataController::SetHeaders(const QVariantMap& headers)
{
	if (m_headers != headers) {
		m_headers = headers;
		Q_EMIT headersChanged(m_headers);
	}
}


void CGqlBasedCollectionDataController::EmitFailure(OperationKind kind, const QString& message)
{
	switch (kind) {
		case OperationKind::GetCollectionHeaders:
			Q_EMIT collectionHeadersReceiveFailed(QVariant());
			break;
		case OperationKind::GetObjectData:
			Q_EMIT objectReceiveFailed(message);
			break;
		case OperationKind::InsertNewObject:
			Q_EMIT objectAddFailed(message);
			break;
		case OperationKind::SetObjectData:
			Q_EMIT objectUpdateFailed(message);
			break;
		case OperationKind::RemoveElements:
		case OperationKind::RemoveElementSet:
			Q_EMIT removeObjectsFailed(message);
			break;
		case OperationKind::RestoreObjects:
		case OperationKind::RestoreObjectSet:
			Q_EMIT restoreObjectsFailed(message);
			break;
		case OperationKind::SetObjectName:
			Q_EMIT objectRenameFailed(message);
			break;
		case OperationKind::SetObjectDescription:
			Q_EMIT objectSetDescriptionFailed(message);
			break;
		case OperationKind::CreateSubCollection:
			Q_EMIT subCollectionCreateFailed(message);
			break;
	}
}


void CGqlBasedCollectionDataController::HandleReplyState(const QString& state, PendingOperation context)
{
	if (state != QLatin1String("Ready") && state != QLatin1String("Error")) {
		return;
	}

	const QString json = context.request->property("json").toString();

	// Schedule destruction of the request irrespective of outcome.
	context.request->deleteLater();

	if (state == QLatin1String("Error")) {
		EmitFailure(context.kind, QStringLiteral("Network error"));
		return;
	}

	QJsonParseError parseError{};
	const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &parseError);
	if (parseError.error != QJsonParseError::NoError) {
		qWarning() << "CGqlBasedCollectionDataController: failed to parse response JSON:" << parseError.errorString();
		EmitFailure(context.kind, parseError.errorString());
		return;
	}

	QString message;
	QString type;
	if (ExtractError(doc, message, type)) {
		EmitFailure(context.kind, message);
		return;
	}

	const QVariant payload = ExtractData(doc, context.commandId);
	const QVariantMap payloadMap = payload.toMap();

	switch (context.kind) {
		case OperationKind::GetCollectionHeaders:
			Q_EMIT collectionHeadersReceived(payloadMap.value(QStringLiteral("headers")));
			break;
		case OperationKind::GetObjectData:
			Q_EMIT objectReceived(payloadMap.value(QStringLiteral("objectData")));
			break;
		case OperationKind::InsertNewObject:
			Q_EMIT objectAdded(payloadMap.value(QStringLiteral("objectId")).toString());
			break;
		case OperationKind::SetObjectData:
			Q_EMIT objectUpdated(context.objectId);
			break;
		case OperationKind::RemoveElements:
		case OperationKind::RemoveElementSet:
			Q_EMIT objectsRemoved(context.objectIds);
			break;
		case OperationKind::RestoreObjects:
		case OperationKind::RestoreObjectSet:
			Q_EMIT objectsRestored(context.objectIds);
			break;
		case OperationKind::SetObjectName: {
			const bool ok = payloadMap.value(QStringLiteral("success"), true).toBool();
			if (ok) {
				Q_EMIT objectRenamed(
							payloadMap.value(QStringLiteral("objectId"), context.objectId).toString(),
							payloadMap.value(QStringLiteral("name"), context.name).toString());
			} else {
				Q_EMIT objectRenameFailed(QString());
			}
			break;
		}
		case OperationKind::SetObjectDescription: {
			const bool ok = payloadMap.value(QStringLiteral("success"), true).toBool();
			if (ok) {
				Q_EMIT objectDescriptionSetted(
							payloadMap.value(QStringLiteral("objectId"), context.objectId).toString(),
							payloadMap.value(QStringLiteral("description"), context.description).toString());
			} else {
				Q_EMIT objectSetDescriptionFailed(QString());
			}
			break;
		}
		case OperationKind::CreateSubCollection:
			Q_EMIT subCollectionCreated(payloadMap.value(QStringLiteral("items")));
			break;
	}
}


void CGqlBasedCollectionDataController::Dispatch(
			OperationKind kind,
			const QString& operation,
			const QString& commandId,
			const QVariantMap& inputArgs,
			const QString& selection,
			PendingOperation context)
{
	if (commandId.isEmpty()) {
		qWarning() << "CGqlBasedCollectionDataController: missing GraphQL command id for operation; aborting";
		EmitFailure(kind, QStringLiteral("Missing GraphQL command id"));
		return;
	}

	auto* request = new CGqlRequest(this);
	context.kind = kind;
	context.commandId = commandId;
	context.request = request;

	connect(request, &CGqlRequest::stateChanged,
			this, [this, context](const QString& state) mutable {
				HandleReplyState(state, context);
			});

	const QString query = BuildGqlRequest(operation, commandId, inputArgs, selection);
	if (!request->SetGqlQuery(query, m_headers)) {
		qWarning() << "CGqlBasedCollectionDataController: failed to send GraphQL request" << commandId;
		EmitFailure(kind, QStringLiteral("Failed to send request"));
		request->deleteLater();
	}
}


void CGqlBasedCollectionDataController::getCollectionHeaders()
{
	QVariantMap input;
	input.insert(QStringLiteral("collectionId"), GetCollectionId());

	Dispatch(OperationKind::GetCollectionHeaders, QStringLiteral("query"),
			m_getCollectionHeadersCommandId, input,
			QStringLiteral("{ headers }"), {});
}


void CGqlBasedCollectionDataController::getObjectData(const QString& objectId)
{
	QVariantMap input;
	input.insert(QStringLiteral("collectionId"), GetCollectionId());
	input.insert(QStringLiteral("objectId"), objectId);

	PendingOperation ctx;
	ctx.objectId = objectId;
	Dispatch(OperationKind::GetObjectData, QStringLiteral("query"),
			m_getObjectDataCommandId, input,
			QStringLiteral("{ objectData }"), ctx);
}


void CGqlBasedCollectionDataController::insertNewObject(
			const QString& typeId,
			const QString& name,
			const QString& description,
			const QVariant& objectData,
			const QString& objectId)
{
	QVariantMap input;
	input.insert(QStringLiteral("collectionId"), GetCollectionId());
	input.insert(QStringLiteral("typeId"), typeId);
	input.insert(QStringLiteral("name"), name);
	input.insert(QStringLiteral("description"), description);
	if (objectData.isValid()) {
		input.insert(QStringLiteral("objectData"), objectData);
	}
	if (!objectId.isEmpty()) {
		input.insert(QStringLiteral("proposedObjectId"), objectId);
	}

	PendingOperation ctx;
	ctx.objectId = objectId;
	Dispatch(OperationKind::InsertNewObject, QStringLiteral("mutation"),
			m_insertNewObjectCommandId, input,
			QStringLiteral("{ objectId }"), ctx);
}


void CGqlBasedCollectionDataController::setObjectData(const QString& objectId, const QVariant& newObjectData)
{
	QVariantMap input;
	input.insert(QStringLiteral("collectionId"), GetCollectionId());
	input.insert(QStringLiteral("objectId"), objectId);
	input.insert(QStringLiteral("objectData"), newObjectData);

	PendingOperation ctx;
	ctx.objectId = objectId;
	Dispatch(OperationKind::SetObjectData, QStringLiteral("mutation"),
			m_setObjectDataCommandId, input,
			QStringLiteral("{ success }"), ctx);
}


void CGqlBasedCollectionDataController::removeObjects(const QVariant& objectIds)
{
	QVariantMap input;
	input.insert(QStringLiteral("collectionId"), GetCollectionId());
	input.insert(QStringLiteral("elementIds"), objectIds);

	PendingOperation ctx;
	ctx.objectIds = objectIds;
	Dispatch(OperationKind::RemoveElements, QStringLiteral("mutation"),
			m_removeElementsCommandId, input,
			QStringLiteral("{ success }"), ctx);
}


void CGqlBasedCollectionDataController::removeObjectSet(const QVariant& selectionParams)
{
	QVariantMap input;
	input.insert(QStringLiteral("collectionId"), GetCollectionId());
	input.insert(QStringLiteral("selectionParams"), selectionParams);

	PendingOperation ctx;
	ctx.objectIds = QVariantList();
	Dispatch(OperationKind::RemoveElementSet, QStringLiteral("mutation"),
			m_removeElementSetCommandId, input,
			QStringLiteral("{ success }"), ctx);
}


void CGqlBasedCollectionDataController::restoreObjects(const QVariant& objectIds)
{
	QVariantMap input;
	input.insert(QStringLiteral("collectionId"), GetCollectionId());
	input.insert(QStringLiteral("objectIds"), objectIds);

	PendingOperation ctx;
	ctx.objectIds = objectIds;
	Dispatch(OperationKind::RestoreObjects, QStringLiteral("mutation"),
			m_restoreObjectsCommandId, input,
			QStringLiteral("{ success }"), ctx);
}


void CGqlBasedCollectionDataController::restoreObjectSet(const QVariant& selectionParams)
{
	QVariantMap input;
	input.insert(QStringLiteral("collectionId"), GetCollectionId());
	input.insert(QStringLiteral("selectionParams"), selectionParams);

	PendingOperation ctx;
	ctx.objectIds = QVariantList();
	Dispatch(OperationKind::RestoreObjectSet, QStringLiteral("mutation"),
			m_restoreObjectSetCommandId, input,
			QStringLiteral("{ success }"), ctx);
}


void CGqlBasedCollectionDataController::setObjectName(const QString& objectId, const QString& name)
{
	QVariantMap input;
	input.insert(QStringLiteral("collectionId"), GetCollectionId());
	input.insert(QStringLiteral("objectId"), objectId);
	input.insert(QStringLiteral("name"), name);

	PendingOperation ctx;
	ctx.objectId = objectId;
	ctx.name = name;
	Dispatch(OperationKind::SetObjectName, QStringLiteral("mutation"),
			m_setObjectNameCommandId, input,
			QStringLiteral("{ success objectId name }"), ctx);
}


void CGqlBasedCollectionDataController::setObjectDescription(const QString& objectId, const QString& description)
{
	QVariantMap input;
	input.insert(QStringLiteral("collectionId"), GetCollectionId());
	input.insert(QStringLiteral("objectId"), objectId);
	input.insert(QStringLiteral("description"), description);

	PendingOperation ctx;
	ctx.objectId = objectId;
	ctx.description = description;
	Dispatch(OperationKind::SetObjectDescription, QStringLiteral("mutation"),
			m_setObjectDescriptionCommandId, input,
			QStringLiteral("{ success objectId description }"), ctx);
}


void CGqlBasedCollectionDataController::createSubCollection(int offset, int count, const QVariant& selectionParams)
{
	QVariantMap input;
	input.insert(QStringLiteral("collectionId"), GetCollectionId());
	input.insert(QStringLiteral("offset"), offset);
	input.insert(QStringLiteral("count"), count);
	input.insert(QStringLiteral("selectionParams"), selectionParams);

	Dispatch(OperationKind::CreateSubCollection, QStringLiteral("query"),
			m_createSubCollectionCommandId, input,
			QStringLiteral("{ items }"), {});
}


} // namespace imtqml
