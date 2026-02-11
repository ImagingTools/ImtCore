// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>


namespace imtdb
{


/**
	Common interface for a SQL-table delegate.
	The delegate is responsible for description of a table in the SQL database.
*/
class IMetaInfoTableDelegate: virtual public istd::IPolymorphic
{
public:
	virtual QByteArrayList GetColumnIds() const = 0;
	virtual int GetMetaInfoType(const QByteArray& columnId) const = 0;
	virtual QVariant ToTableRepresentation(const QVariant& data, const QByteArray& columnId) const = 0;
	virtual QVariant FromTableRepresentation(const QVariant& data, const QByteArray& columnId) const = 0;
	virtual idoc::MetaInfoPtr CreateMetaInfo(const istd::IChangeable* dataPtr, const QByteArray& typeId) const = 0;
};


} // namespace imtdb


