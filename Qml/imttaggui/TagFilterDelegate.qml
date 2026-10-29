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

	Opens a searchable multi-select of the tag catalog. A segmented switch picks how the
	ticked tags combine (any, all, exclude); "No tags" finds untagged objects. The chip
	shows the names of the picked tags, the popup keeps them in its "Selected" group.
	Keyboard: Alt+1 any, Alt+2 all, Alt+3 exclude, Alt+0 no tags.

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

	property Item openedPopup: null

	name: qsTr("Tags")
	isActive: tagFilter.isActive
	mainButtonText: tagFilter.isActive ? tagFilterDelegateRoot.summaryText() : name

	function summaryText(){
		if (tagFilter.mode === "none"){
			return qsTr("none")
		}

		var names = tagFilter.getTagNames()
		var namesText = names.length > 0 ? names.join(tagFilter.mode === "all" ? " + " : ", ") : qsTr("%1 tags").arg(tagFilter.tagIds.length)

		return tagFilter.mode === "exclude" ? qsTr("not %1").arg(namesText) : namesText
	}

	function setTags(tagIds, mode, beQuiet){
		tagFilter.setFilter(tagIds, mode, beQuiet)
	}

	function setWithoutTags(withoutTags){
		if (withoutTags && tagFilterDelegateRoot.openedPopup){
			tagFilterDelegateRoot.openedPopup.dataProvider.clearSelection()
		}

		tagFilter.setMode(withoutTags ? "none" : "any")
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
			id: modeRow
			spacing: Style.marginL

			Shortcut {
				sequence: "Alt+1"
				onActivated: {
					tagFilter.setMode("any")
				}
			}

			Shortcut {
				sequence: "Alt+2"
				onActivated: {
					tagFilter.setMode("all")
				}
			}

			Shortcut {
				sequence: "Alt+3"
				onActivated: {
					tagFilter.setMode("exclude")
				}
			}

			Shortcut {
				sequence: "Alt+0"
				onActivated: {
					tagFilterDelegateRoot.setWithoutTags(tagFilter.mode !== "none")
				}
			}

			SegmentedButton {
				id: modeSegmented
				anchors.verticalCenter: parent.verticalCenter
				objectName: "TagFilterModeSegmented"
				height: Style.controlHeightM
				checkable: false

				Button {
					objectName: "TagFilterModeAny"
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Any")
					tooltipText: qsTr("Objects with any of the ticked tags (Alt+1)")
					checked: tagFilter.mode === "any"
					onClicked: {
						tagFilter.setMode("any")
					}
				}

				Button {
					objectName: "TagFilterModeAll"
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("All")
					tooltipText: qsTr("Objects with all of the ticked tags (Alt+2)")
					checked: tagFilter.mode === "all"
					onClicked: {
						tagFilter.setMode("all")
					}
				}

				Button {
					objectName: "TagFilterModeExclude"
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Exclude")
					tooltipText: qsTr("Objects without any of the ticked tags (Alt+3)")
					checked: tagFilter.mode === "exclude"
					onClicked: {
						tagFilter.setMode("exclude")
					}
				}
			}

			Row {
				id: withoutTagsRow
				anchors.verticalCenter: parent.verticalCenter
				objectName: "TagFilterWithoutTags"
				spacing: Style.marginS

				CheckBox {
					anchors.verticalCenter: parent.verticalCenter
					width: Style.itemSizeS
					height: Style.itemSizeS
					mainMargin: Style.marginM
					borderColor: Style.grayColor
					checkState: tagFilter.mode === "none" ? Qt.Checked : Qt.Unchecked

					function nextCheckState(){
						tagFilterDelegateRoot.setWithoutTags(tagFilter.mode !== "none")
					}
				}

				BaseText {
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("No tags")

					MouseArea {
						anchors.fill: parent
						cursorShape: Qt.PointingHandCursor
						onClicked: {
							tagFilterDelegateRoot.setWithoutTags(tagFilter.mode !== "none")
						}
					}
				}
			}
		}
	}

	Component {
		id: tagSelectComp

		TagSelectPopup {
			id: tagSelectPopupItem
			preselectedIds: tagFilter.tagIds
			knownItems: tagFilter.tags
			showSelectedGroup: true
			headerComponent: modeSwitchComp
			itemWidth: 360

			dataProvider: TagSelectDataProvider {
				context: tagFilterDelegateRoot.context
			}

			Component.onCompleted: {
				tagFilterDelegateRoot.openedPopup = tagSelectPopupItem
			}

			Component.onDestruction: {
				tagFilterDelegateRoot.openedPopup = null
			}

			onSelectionChanged: {
				tagFilter.tags = tagSelectPopupItem.dataProvider.getSelectedItems()
				tagFilter.setFilter(selectedIds, tagFilter.mode === "none" ? "any" : tagFilter.mode)
			}
		}
	}
}
