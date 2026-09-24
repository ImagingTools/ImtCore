// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0

/*!
	\qmltype TagFilterButton
	\inqmlmodule imttaggui
	\brief Filter-panel button that filters a taggable collection by tags.

	Opens a searchable multi-select of the tag catalog with a mode switch:
	any of the tags, all of them, none of them, or objects without tags.

	\code
	TagFilterButton {
		collectionFilter: deviceCollectionView.collectionFilter
	}
	\endcode
*/
Button {
	id: tagFilterButtonRoot
	objectName: "TagFilterButton"

	property string context: ""
	property CollectionFilter collectionFilter: null

	text: tagFilter.isActive ? qsTr("Tags (%1)").arg(tagFilterButtonRoot.modeText()) : qsTr("Tags")
	variant: tagFilter.isActive ? "primary" : "default"

	function modeText(){
		if (tagFilter.mode === "none"){
			return qsTr("none")
		}

		return String(tagFilter.tagIds.length)
	}

	onClicked: {
		ModalDialogManager.openDialog(tagSelectComp, {})
	}

	TagFilter {
		id: tagFilter
		collectionFilter: tagFilterButtonRoot.collectionFilter
	}

	Component {
		id: modeSwitchComp

		Row {
			spacing: Style.spacingS

			Button {
				text: qsTr("Any")
				variant: tagFilter.mode === "any" ? "primary" : "default"
				onClicked: {
					tagFilter.setFilter(tagFilter.tagIds, "any")
				}
			}

			Button {
				text: qsTr("All")
				variant: tagFilter.mode === "all" ? "primary" : "default"
				onClicked: {
					tagFilter.setFilter(tagFilter.tagIds, "all")
				}
			}

			Button {
				text: qsTr("Exclude")
				variant: tagFilter.mode === "exclude" ? "primary" : "default"
				onClicked: {
					tagFilter.setFilter(tagFilter.tagIds, "exclude")
				}
			}

			Button {
				text: qsTr("No tags")
				variant: tagFilter.mode === "none" ? "primary" : "default"
				onClicked: {
					tagFilter.setFilter([], "none")
				}
			}

			Button {
				text: qsTr("Clear")
				enabled: tagFilter.isActive
				onClicked: {
					tagFilter.clear()
				}
			}
		}
	}

	Component {
		id: tagSelectComp

		FilterableSelectPopup {
			showCheckBox: true
			filterPlaceholder: qsTr("Filter tags...")
			preselectedIds: tagFilter.tagIds
			headerComponent: modeSwitchComp

			dataProvider: FilterableSelectGqlDataProvider {
				context: tagFilterButtonRoot.context
				collectionId: "Tags"
				multiSelect: true
			}

			onSelectionChanged: {
				tagFilter.setFilter(selectedIds, tagFilter.mode === "none" ? "any" : tagFilter.mode)
			}
		}
	}
}
