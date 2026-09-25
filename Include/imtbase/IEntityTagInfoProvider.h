// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


class IOperationContext;


/**
	Tags of entities, for showing them next to the entities, e.g. in collection lists.
	\ingroup Collection
*/
class IEntityTagInfoProvider: virtual public istd::IPolymorphic
{
public:
	struct TagInfo
	{
		QByteArray id;
		QString name;

		/**
			Hex color without the leading '#'.
		*/
		QString color;
		bool isSystem = false;
	};

	typedef QList<TagInfo> TagInfoList;
	typedef QMap<QByteArray, TagInfoList> EntityTagInfos;

	/**
		Get the tags of entities visible in the tenant of the operation context.
		\param entityType	Type under which tags are assigned, usually the collection ID.
		\return Tags per entity ID; entities without tags are left out.
	*/
	virtual EntityTagInfos GetEntityTagInfos(
				const QByteArray& entityType,
				const QByteArrayList& entityIds,
				const IOperationContext* operationContextPtr) const = 0;
};


} // namespace imtbase


