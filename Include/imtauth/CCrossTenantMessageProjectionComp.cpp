// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CCrossTenantMessageProjectionComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSet>
#include <QtCore/QStringList>


namespace imtauth
{


QByteArray CCrossTenantMessageProjectionComp::ProjectPayload(
		CrossTenantMessageType /*messageType*/,
		const QString& /*customType*/,
		const QByteArray& sourcePayload,
		bool& isValid) const
{
	isValid = true;

	QStringList allowedFields;
	for (int i = 0; i < m_allowedFieldsAttrPtr.GetCount(); ++i){
		allowedFields << m_allowedFieldsAttrPtr[i];
	}

	QSet<QString> blockedFields;
	for (int i = 0; i < m_blockedFieldsAttrPtr.GetCount(); ++i){
		blockedFields.insert(m_blockedFieldsAttrPtr[i]);
	}

	// No projection configured: pass the payload through unchanged.
	if (allowedFields.isEmpty() && blockedFields.isEmpty()){
		return sourcePayload;
	}

	QJsonParseError parseError;
	QJsonDocument document = QJsonDocument::fromJson(sourcePayload, &parseError);
	if (parseError.error != QJsonParseError::NoError || !document.isObject()){
		// Non-object payloads cannot be field-projected; pass them through.
		return sourcePayload;
	}

	const QJsonObject sourceObject = document.object();
	QJsonObject projectedObject;

	if (!allowedFields.isEmpty()){
		for (const QString& field : allowedFields){
			if (blockedFields.contains(field)){
				continue;
			}
			if (sourceObject.contains(field)){
				projectedObject.insert(field, sourceObject.value(field));
			}
		}
	}
	else {
		for (auto it = sourceObject.constBegin(); it != sourceObject.constEnd(); ++it){
			if (blockedFields.contains(it.key())){
				continue;
			}
			projectedObject.insert(it.key(), it.value());
		}
	}

	return QJsonDocument(projectedObject).toJson(QJsonDocument::Compact);
}


} // namespace imtauth
