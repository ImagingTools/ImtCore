// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

/**
 * Filter chip choosing which documents of a collection with document sharing are listed:
 * the own ones, the ones shared with the organization or the own ones shared with others.
 * Without a selection all documents are listed. The scope is sent as a request header, not as a field filter.
 */
OptionsFilterDelegate {
	id: filterDelegate

	name: qsTr("Sharing")

	property string sharingScope: "All"

	Component.onCompleted: {
		createAndAddOption("Own", qsTr("Our documents"), "", true)
		createAndAddOption("Incoming", qsTr("Shared with us"), "", true)
		createAndAddOption("Outgoing", qsTr("Shared by us"), "", true)
	}

	onOptionSelectionChanged: {
		filterDelegate.sharingScope = optionIds.length > 0 ? String(optionIds[0]) : "All"

		if (!beQuiet && filterDelegate.collectionFilter){
			filterDelegate.collectionFilter.filterChanged()
		}
	}
}
