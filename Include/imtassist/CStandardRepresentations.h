// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QJsonArray>

// ImtCore includes
#include <imtassist/CParameterSchema.h>


namespace imtassist
{


/**
	UI representation types provided by ImtCore itself.
	Each has a QML component in \b imtassistgui with the same type name and version.

	\ingroup imtassist
*/
class CStandardRepresentations
{
public:
	/**
		Type of the filterable list with selectable entries ("imtassist.selectableList", version 1).
		Data: \c title (optional), \c selectionMode ("single" or "multiple", optional, default "single"),
		\c selectionKind (optional kind of the context item the selection is handed over as, default "selection"),
		\c items: list of { \c id, \c label, optional \c description }.
	*/
	static const QByteArray s_selectableListType;

	enum
	{
		SELECTABLE_LIST_VERSION = 1,
		SELECTABLE_LIST_MAX_ITEMS = 500
	};

	/**
		Data schema of the selectable list.
	*/
	static CParameterSchema CreateSelectableListSchema();

	/**
		Item of the selectable list.
	*/
	static QJsonObject CreateSelectableListItem(const QByteArray& id, const QString& label, const QString& description = QString());

	/**
		Data of the selectable list.
	*/
	static QJsonObject CreateSelectableListData(const QJsonArray& items, const QString& title = QString(), bool isMultiSelection = false);
};


} // namespace imtassist


