// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckValueConverter.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>


namespace imtduckdb
{


duckdb::Value CDuckValueConverter::ToDuckDbValue(const QVariant& value)
{
	if (value.isNull()){
		return duckdb::Value();
	}

	switch (value.typeId()){
	case QMetaType::Bool:
		return duckdb::Value::BOOLEAN(value.toBool());

	case QMetaType::Int:
		return duckdb::Value::INTEGER(value.toInt());

	case QMetaType::UInt:
		return duckdb::Value::UINTEGER(value.toUInt());

	case QMetaType::LongLong:
		return duckdb::Value::BIGINT(value.toLongLong());

	case QMetaType::ULongLong:
		return duckdb::Value::UBIGINT(value.toULongLong());

	case QMetaType::Double:
		return duckdb::Value::DOUBLE(value.toDouble());

	case QMetaType::QUuid:
		return duckdb::Value::UUID(value.toUuid().toString(QUuid::WithoutBraces).toStdString());

	case QMetaType::QByteArray: {
		const QByteArray bytes = value.toByteArray();
		return duckdb::Value::BLOB(reinterpret_cast<duckdb::const_data_ptr_t>(bytes.constData()), static_cast<duckdb::idx_t>(bytes.size()));
	}

	// DuckDB implicitly casts VARCHAR values to the expected target type, so ISO-formatted text
	// is used here instead of hand-rolling duckdb's internal epoch-based date/time types.
	case QMetaType::QDateTime:
		return duckdb::Value(value.toDateTime().toString(Qt::ISODateWithMs).toStdString());

	case QMetaType::QDate:
		return duckdb::Value(value.toDate().toString(Qt::ISODate).toStdString());

	default:
		return duckdb::Value(value.toString().toStdString());
	}
}


} // namespace imtduckdb
