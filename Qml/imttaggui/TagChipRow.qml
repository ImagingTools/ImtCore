// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

/*!
	\qmltype TagChipRow
	\inqmlmodule imttaggui
	\brief Wrapping row of tag chips, e.g. for a collection cell.

	\c tags is an array of objects with \c id, \c name, \c color and \c isSystem,
	as produced by EntityTagsProvider.getTags().
*/
Flow {
	id: tagChipRowRoot
	objectName: "TagChipRow"

	property var tags: []

	spacing: Style.spacingS

	Repeater {
		model: tagChipRowRoot.tags

		delegate: TagChip {
			tagName: String(modelData.name)
			tagColor: String(modelData.color)
			isSystem: modelData.isSystem === true
		}
	}
}
