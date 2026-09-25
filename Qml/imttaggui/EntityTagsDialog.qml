// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0

/*!
	\qmltype EntityTagsDialog
	\inqmlmodule imttaggui
	\brief Picks the tags of one or several entities.

	Tags every entity already has are ticked. On Apply only the difference is sent:
	ticked tags are added to all entities, unticked ones removed from all of them;
	tags only some of the entities have stay as they are unless touched.
*/
Dialog {
	id: entityTagsDialog

	property string context: ""
	property string entityType: ""
	property var entityIds: []

	property var initialTagIds: []
	property bool tagsLoaded: false
	property int pendingRequestCount: 0

	signal tagsChanged()

	title: entityTagsDialog.entityIds.length > 1 ? qsTr("Tags of %1 objects").arg(entityTagsDialog.entityIds.length) : qsTr("Tags")
	width: 380
	notClosingButtons: Enums.apply

	Component.onCompleted: {
		addButton(Enums.apply, qsTr("Apply"), false)
		addButton(Enums.cancel, qsTr("Cancel"), true)

		entityTagsProvider.load(entityTagsDialog.entityIds)
	}

	onFinished: {
		if (buttonId == Enums.apply){
			entityTagsDialog.apply()
		}
	}

	function commonTagIds(){
		var common = entityTagsProvider.getTagIds(entityTagsDialog.entityIds[0])
		for (var i = 1; i < entityTagsDialog.entityIds.length; i++){
			var entityTagIds = entityTagsProvider.getTagIds(entityTagsDialog.entityIds[i])
			var remaining = []
			for (var j = 0; j < common.length; j++){
				if (entityTagIds.indexOf(common[j]) >= 0){
					remaining.push(common[j])
				}
			}

			common = remaining
		}

		return common
	}

	function apply(){
		var selectedIds = entityTagsDialog.contentItem ? entityTagsDialog.contentItem.getSelectedIds() : entityTagsDialog.initialTagIds

		var addedIds = []
		for (var i = 0; i < selectedIds.length; i++){
			if (entityTagsDialog.initialTagIds.indexOf(selectedIds[i]) < 0){
				addedIds.push(selectedIds[i])
			}
		}

		var removedIds = []
		for (var j = 0; j < entityTagsDialog.initialTagIds.length; j++){
			if (selectedIds.indexOf(entityTagsDialog.initialTagIds[j]) < 0){
				removedIds.push(entityTagsDialog.initialTagIds[j])
			}
		}

		entityTagsDialog.pendingRequestCount = (addedIds.length > 0 ? 1 : 0) + (removedIds.length > 0 ? 1 : 0)
		if (entityTagsDialog.pendingRequestCount === 0){
			entityTagsDialog.root.closeDialog()

			return
		}

		entityTagsDialog.setButtonEnabled(Enums.apply, false)

		if (addedIds.length > 0){
			entityTagsEditor.addTags(entityTagsDialog.entityType, entityTagsDialog.entityIds, addedIds)
		}

		if (removedIds.length > 0){
			entityTagsEditor.removeTags(entityTagsDialog.entityType, entityTagsDialog.entityIds, removedIds)
		}
	}

	function handleRequestDone(){
		entityTagsDialog.pendingRequestCount--
		if (entityTagsDialog.pendingRequestCount <= 0){
			entityTagsDialog.tagsChanged()
			entityTagsDialog.root.closeDialog()
		}
	}

	EntityTagsProvider {
		id: entityTagsProvider
		context: entityTagsDialog.context
		entityType: entityTagsDialog.entityType

		onLoaded: {
			entityTagsDialog.initialTagIds = entityTagsDialog.commonTagIds()
			entityTagsDialog.tagsLoaded = true
			entityTagsDialog.setButtonEnabled(Enums.apply, true)
		}

		onFailed: {
			PopupManager.addErrorMessage(message)
		}
	}

	EntityTagsEditor {
		id: entityTagsEditor
		context: entityTagsDialog.context

		onChanged: {
			entityTagsDialog.handleRequestDone()
		}

		onFailed: {
			PopupManager.addErrorMessage(message)
			entityTagsDialog.handleRequestDone()
		}
	}

	contentComp: Component {
		Item {
			id: tagsContent
			width: entityTagsDialog.width
			height: tagSelectLoader.height + 2 * Style.marginL

			function getSelectedIds(){
				return tagSelectLoader.item ? tagSelectLoader.item.dataProvider.getSelectedIds() : entityTagsDialog.initialTagIds
			}

			Loader {
				id: tagSelectLoader
				anchors.top: parent.top
				anchors.topMargin: Style.marginL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginL
				height: item ? item.height : Style.controlHeightL
				active: entityTagsDialog.tagsLoaded

				sourceComponent: Component {
					TagSelectPopup {
						embedded: true
						itemWidth: tagSelectLoader.width
						preselectedIds: entityTagsDialog.initialTagIds

						dataProvider: TagSelectDataProvider {
							context: entityTagsDialog.context
						}
					}
				}

				onLoaded: {
					tagSelectLoader.item.started()
				}
			}
		}
	}
}
