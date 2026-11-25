// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QString>

// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtauth
{


/**
	Fields of the organization data in an archive: a leaf per value, a multiple tag per list.
*/
namespace ArchiveFields
{


template <typename Value>
inline bool Leaf(iser::IArchive& archive, const QByteArray& id, Value& value)
{
	const iser::CArchiveTag tag(id, id, iser::CArchiveTag::TT_LEAF);

	return archive.BeginTag(tag) && archive.Process(value) && archive.EndTag(tag);
}


inline bool Leaf(iser::IArchive& archive, const QByteArray& id, QDateTime& value)
{
	QString text = value.isValid() ? value.toUTC().toString(Qt::ISODateWithMs) : QString();
	const bool retVal = Leaf(archive, id, text);
	if (retVal && !archive.IsStoring()){
		value = QDateTime::fromString(text, Qt::ISODateWithMs);
		if (value.isValid()){
			value.setTimeSpec(Qt::UTC);
		}
	}

	return retVal;
}


inline bool Leaf(iser::IArchive& archive, const QByteArray& id, QByteArrayList& value)
{
	QByteArray text = value.join(';');
	const bool retVal = Leaf(archive, id, text);
	if (retVal && !archive.IsStoring()){
		value = text.split(';');
		value.removeAll(QByteArray());
	}

	return retVal;
}


template <typename Enum>
inline bool EnumLeaf(iser::IArchive& archive, const QByteArray& id, Enum& value)
{
	int number = int(value);
	const bool retVal = Leaf(archive, id, number);
	if (retVal && !archive.IsStoring()){
		value = Enum(number);
	}

	return retVal;
}


/**
	A list of items, each one a group serialized by \c serializeItem(archive, item).
*/
template <typename Item, typename SerializeItem>
inline bool List(iser::IArchive& archive, const QByteArray& listId, const QByteArray& itemId, QList<Item>& items, SerializeItem serializeItem)
{
	const iser::CArchiveTag listTag(listId, listId, iser::CArchiveTag::TT_MULTIPLE);
	const iser::CArchiveTag itemTag(itemId, itemId, iser::CArchiveTag::TT_GROUP, &listTag);

	int count = items.size();
	bool retVal = archive.BeginMultiTag(listTag, itemTag, count);
	if (retVal && !archive.IsStoring()){
		items.clear();
		items.resize(count);
	}

	for (int i = 0; retVal && i < count; ++i){
		retVal = archive.BeginTag(itemTag) && serializeItem(archive, items[i]) && archive.EndTag(itemTag);
	}

	return retVal && archive.EndTag(listTag);
}


} // namespace ArchiveFields


} // namespace imtauth


