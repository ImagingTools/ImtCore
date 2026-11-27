// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckValueConverter.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QUuid>
#include <QtCore/QtEndian>


namespace imtduckdb
{


duckdb_value CDuckValueConverter::ToDuckDbValue(const QVariant& value)
{
	if (value.isNull()){
		return duckdb_create_null_value();
	}

	QByteArray text;

	switch (value.typeId()){
	case QMetaType::Bool:
		return duckdb_create_bool(value.toBool());

	case QMetaType::Int:
		return duckdb_create_int32(value.toInt());

	case QMetaType::UInt:
		return duckdb_create_uint32(value.toUInt());

	case QMetaType::LongLong:
		return duckdb_create_int64(value.toLongLong());

	case QMetaType::ULongLong:
		return duckdb_create_uint64(value.toULongLong());

	case QMetaType::Double:
		return duckdb_create_double(value.toDouble());

	/**
		DuckDB takes the 128 bits of the UUID in big-endian order, upper half first.
	*/
	case QMetaType::QUuid: {
		const QByteArray bytes = value.toUuid().toRfc4122();

		duckdb_uhugeint uuid;
		uuid.upper = qFromBigEndian<quint64>(bytes.constData());
		uuid.lower = qFromBigEndian<quint64>(bytes.constData() + 8);

		return duckdb_create_uuid(uuid);
	}

	case QMetaType::QByteArray: {
		const QByteArray bytes = value.toByteArray();

		return duckdb_create_blob(reinterpret_cast<const uint8_t*>(bytes.constData()), static_cast<idx_t>(bytes.size()));
	}

	/**
		The element type follows the values, so a list of QUuid is a UUID[]. The appender and the parameter binding
		cast it to the type of the column.
	*/
	case QMetaType::QVariantList:
	case QMetaType::QStringList: {
		const QVariantList items = value.toList();

		QList<duckdb_value> children;
		children.reserve(items.size());
		for (const QVariant& item : items){
			children.append(ToDuckDbValue(item));
		}

		/**
			An empty list has no element to take the type from, and nothing to cast either. DuckDB rejects a null
			array of values even for an empty list, hence the placeholder.
		*/
		duckdb_logical_type emptyListType = children.isEmpty() ? duckdb_create_logical_type(DUCKDB_TYPE_VARCHAR) : nullptr;
		duckdb_value noValue = nullptr;

		duckdb_value retVal = nullptr;
		if (!children.contains(nullptr)){
			retVal = duckdb_create_list_value(
						children.isEmpty() ? emptyListType : duckdb_get_value_type(children.first()),
						children.isEmpty() ? &noValue : children.data(),
						static_cast<idx_t>(children.size()));
		}

		for (duckdb_value child : std::as_const(children)){
			duckdb_destroy_value(&child);
		}

		duckdb_destroy_logical_type(&emptyListType);

		return retVal;
	}

	/**
		DuckDB implicitly casts VARCHAR values to the expected target type, so ISO-formatted text
		is used here instead of hand-rolling DuckDB's internal epoch-based date/time types.
	*/
	case QMetaType::QDateTime:
		text = value.toDateTime().toString(Qt::ISODateWithMs).toUtf8();
		break;

	case QMetaType::QDate:
		text = value.toDate().toString(Qt::ISODate).toUtf8();
		break;

	default:
		text = value.toString().toUtf8();
		break;
	}

	return duckdb_create_varchar_length(text.constData(), static_cast<idx_t>(text.size()));
}


} // namespace imtduckdb

