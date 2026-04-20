// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlDocumentDataController.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtqml/CGqlLiteral.h>
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

	const QString query = gql::BuildRequest(operation, commandId, inputArgs, selection);
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
