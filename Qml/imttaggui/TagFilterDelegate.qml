// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtguigql 1.0

/*!
	\qmltype TagFilterDelegate
	\inqmlmodule imttaggui
	\brief Filter-panel chip that filters a taggable collection by tags.

	Opens a searchable multi-select of the tag catalog with a mode switch:
	any of the tags, all of them, none of them, or objects without tags.

	\code
	registerFieldFilterDelegate("Tags", tagFilterComp)

	Component {
		id: tagFilterComp
		TagFilterDelegate {}
	}
	\endcode
*/
FilterDelegateBase {
	id: tagFilterDelegateRoot

	property string context: ""

	//! Set by the filter panel.
	property CollectionFilter collectionFilter: null

	name: qsTr("Tags")
	isActive: tagFilter.isActive
	mainButtonText: tagFilter.isActive ? tagFilterDelegateRoot.summaryText() : name

	function summaryText(){
		if (tagFilter.mode === "none"){
			return qsTr("none")
		}

		if (tagFilter.mode === "all"){
			return qsTr("all of %1").arg(tagFilter.tagIds.length)
		}

		if (tagFilter.mode === "exclude"){
			return qsTr("not %1").arg(tagFilter.tagIds.length)
		}

		return qsTr("any of %1").arg(tagFilter.tagIds.length)
	}

	function setTags(tagIds, mode, beQuiet){
		tagFilter.setFilter(tagIds, mode, beQuiet)
	}

	onOpenFilter: {
		var point = tagFilterDelegateRoot.popupPoint()
		ModalDialogManager.openDialog(tagSelectComp, {"x": point.x, "y": point.y})
	}

	onClearFilter: {
		tagFilter.clear(beQuiet)
	}

	TagFilter {
		id: tagFilter
		collectionFilter: tagFilterDelegateRoot.collectionFilter
	}

	Component {
		id: modeSwitchComp

		Row {
			id: modeSwitchRow
			spacing: Style.spacingS

			readonly property int buttonWidth: (modeSwitchRow.width - 3 * modeSwitchRow.spacing) / 4

			Button {
				width: modeSwitchRow.buttonWidth
				text: qsTr("Any")
				variant: tagFilter.mode === "any" ? "primary" : "default"
				onClicked: {
					tagFilter.setFilter(tagFilter.tagIds, "any")
				}
			}

			Button {
				width: modeSwitchRow.buttonWidth
				text: qsTr("All")
				variant: tagFilter.mode === "all" ? "primary" : "default"
				onClicked: {
					tagFilter.setFilter(tagFilter.tagIds, "all")
				}
			}

			Button {
				width: modeSwitchRow.buttonWidth
				text: qsTr("Exclude")
				variant: tagFilter.mode === "exclude" ? "primary" : "default"
				onClicked: {
					tagFilter.setFilter(tagFilter.tagIds, "exclude")
				}
			}

			Button {
				width: modeSwitchRow.buttonWidth
				text: qsTr("No tags")
				variant: tagFilter.mode === "none" ? "primary" : "default"
				onClicked: {
					tagFilter.setFilter([], "none")
				}
			}
		}
	}

	Component {
		id: tagSelectComp

		TagSelectPopup {
			preselectedIds: tagFilter.tagIds
			headerComponent: modeSwitchComp

			dataProvider: TagSelectDataProvider {
				context: tagFilterDelegateRoot.context
			}

			onSelectionChanged: {
				tagFilter.setFilter(selectedIds, tagFilter.mode === "none" ? "any" : tagFilter.mode)
			}
		}
	}
}
