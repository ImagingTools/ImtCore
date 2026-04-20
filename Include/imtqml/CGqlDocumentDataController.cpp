// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlDocumentDataController.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
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


QVariantMap ExtractDataMap(const QJsonDocument& doc, const QString& commandId)
{
	if (!doc.isObject()) {
		return {};
	}
	const QJsonValue dataValue = doc.object().value(QStringLiteral("data"));
	if (!dataValue.isObject()) {
		return {};
	}
	return dataValue.toObject().value(commandId).toVariant().toMap();
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


CGqlDocumentDataController::CGqlDocumentDataController(QObject* parent)
	: BaseClass(parent)
{
}


CGqlDocumentDataController::~CGqlDocumentDataController() = default;


const QVariantMap& CGqlDocumentDataController::GetHeaders() const
{
	return m_headers;
}


void CGqlDocumentDataController::SetHeaders(const QVariantMap& headers)
{
	if (m_headers != headers) {
		m_headers = headers;
		Q_EMIT headersChanged(m_headers);
	}
}


void CGqlDocumentDataController::HandleReplyState(OperationKind kind, const QString& commandId, CGqlRequest* request, const QString& state)
{
	if (state != QLatin1String("Ready") && state != QLatin1String("Error")) {
		return;
	}

	const QString json = request->property("json").toString();
	request->deleteLater();

	if (state == QLatin1String("Error")) {
		Q_EMIT error(QStringLiteral("Network error"), QStringLiteral("Critical"));
		return;
	}

	QJsonParseError parseError{};
	const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &parseError);
	if (parseError.error != QJsonParseError::NoError) {
		Q_EMIT error(parseError.errorString(), QStringLiteral("Warning"));
		return;
	}

	QString message;
	QString type;
	if (ExtractError(doc, message, type)) {
		Q_EMIT error(message, type);
		return;
	}

	const QVariantMap payload = ExtractDataMap(doc, commandId);

	switch (kind) {
		case OperationKind::Get:
			SetDocumentModel(payload);
			break;
		case OperationKind::Add:
		case OperationKind::Update: {
			const QString notificationKey = (kind == OperationKind::Add)
					? QStringLiteral("addedNotification")
					: QStringLiteral("updatedNotification");
			const QVariantMap notification = payload.value(notificationKey).toMap();

			const QString resultId = notification.contains(QStringLiteral("id"))
					? notification.value(QStringLiteral("id")).toString()
					: GetDocumentId();
			const QString resultName = notification.value(QStringLiteral("name")).toString();

			Q_EMIT saved(resultId, resultName);
			break;
		}
	}
}


void CGqlDocumentDataController::Dispatch(
			OperationKind kind,
			const QString& operation,
			const QString& commandId,
			const QVariantMap& inputArgs,
			const QString& selection)
{
	if (commandId.isEmpty()) {
		const QString opName = (kind == OperationKind::Get)
				? QStringLiteral("get")
				: ((kind == OperationKind::Add)
						? QStringLiteral("add")
						: QStringLiteral("update"));
		qWarning() << "CGqlDocumentDataController: missing GraphQL command id for" << opName;
		Q_EMIT error(QStringLiteral("Missing GraphQL command id"), QStringLiteral("Critical"));
		return;
	}

	auto* request = new CGqlRequest(this);
	connect(request, &CGqlRequest::stateChanged,
			this, [this, kind, commandId, request](const QString& state) {
				HandleReplyState(kind, commandId, request, state);
			});

	const QString query = BuildGqlRequest(operation, commandId, inputArgs, selection);
	if (!request->SetGqlQuery(query, m_headers)) {
		qWarning() << "CGqlDocumentDataController: failed to send GraphQL request" << commandId;
		Q_EMIT error(QStringLiteral("Failed to send request"), QStringLiteral("Critical"));
		request->deleteLater();
	}
}


void CGqlDocumentDataController::updateDocumentModel()
{
	QVariantMap input;
	input.insert(QStringLiteral("id"), GetDocumentId());
	input.insert(QStringLiteral("typeId"), GetTypeId());

	Dispatch(OperationKind::Get, QStringLiteral("query"),
			m_gqlGetCommandId, input, QStringLiteral("{ item }"));
}


void CGqlDocumentDataController::insertDocument()
{
	QVariantMap input;
	input.insert(QStringLiteral("id"), GetDocumentId());
	input.insert(QStringLiteral("item"), GetDocumentModel());
	input.insert(QStringLiteral("typeId"), GetTypeId());
	input.insert(QStringLiteral("name"), GetDocumentName());
	input.insert(QStringLiteral("description"), GetDocumentDescription());

	Dispatch(OperationKind::Add, QStringLiteral("mutation"),
			m_gqlAddCommandId, input, QStringLiteral("{ addedNotification { id name } }"));
}


void CGqlDocumentDataController::saveDocument()
{
	QVariantMap input;
	input.insert(QStringLiteral("id"), GetDocumentId());
	input.insert(QStringLiteral("item"), GetDocumentModel());
	input.insert(QStringLiteral("typeId"), GetTypeId());
	input.insert(QStringLiteral("name"), GetDocumentName());
	input.insert(QStringLiteral("description"), GetDocumentDescription());

	Dispatch(OperationKind::Update, QStringLiteral("mutation"),
			m_gqlUpdateCommandId, input, QStringLiteral("{ updatedNotification { id name } }"));
}


} // namespace imtqml
