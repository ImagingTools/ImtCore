// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlLiteral.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QStringList>


namespace imtqml
{
namespace gql
{


namespace
{


QString EscapeString(const QString& s)
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


} // namespace


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
			return EscapeString(value.toString());

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

	// Fallback: encode as JSON string literal.
	const QJsonDocument doc = QJsonDocument::fromVariant(value);
	if (!doc.isNull()) {
		return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
	}
	return EscapeString(value.toString());
}


QString BuildRequest(
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


} // namespace gql
} // namespace imtqml
