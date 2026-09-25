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
	}

	// The dialog manager assigns entityIds after creation, just before started().
	onStarted: {
		entityTagsProvider.load(entityTagsDialog.entityIds)
	}

	onFinished: {
		if (buttonId == Enums.apply){
			entityTagsDialog.apply()
		}
	}

	//! Ticked tags as select items, so the "Selected" group shows their names.
	function initialTags(){
		var tags = entityTagsProvider.getTags(entityTagsDialog.entityIds[0])
		var items = []
		for (var i = 0; i < tags.length; i++){
			if (entityTagsDialog.initialTagIds.indexOf(tags[i].id) >= 0){
				items.push({id: tags[i].id, title: tags[i].name, description: tags[i].description, color: tags[i].color, parameters: []})
			}
		}

		return items
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

			if (entityTagsDialog.contentItem){
				entityTagsDialog.contentItem.setSelectedIds(entityTagsDialog.initialTagIds)
			}
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

	Shortcut {
		sequence: "Ctrl+Return"
		onActivated: {
			if (entityTagsDialog.tagsLoaded && entityTagsDialog.pendingRequestCount === 0){
				entityTagsDialog.apply()
			}
		}
	}

	Shortcut {
		sequence: "Ctrl+Enter"
		onActivated: {
			if (entityTagsDialog.tagsLoaded && entityTagsDialog.pendingRequestCount === 0){
				entityTagsDialog.apply()
			}
		}
	}

	contentComp: Component {
		Item {
			id: tagsContent
			width: entityTagsDialog.width
			height: tagSelectPopup.height + hintText.height + 2 * Style.marginL

			function getSelectedIds(){
				return tagSelectPopup.dataProvider.getSelectedIds()
			}

			function setSelectedIds(tagIds){
				var knownTags = entityTagsDialog.initialTags()
				for (var i = 0; i < knownTags.length; i++){
					tagSelectPopup.dataProvider.addKnownItem(knownTags[i].id, knownTags[i])
				}

				tagSelectPopup.dataProvider.setPreselectedIds(tagIds)
			}

			Component.onCompleted: {
				tagSelectPopup.started()

				if (entityTagsDialog.tagsLoaded){
					tagsContent.setSelectedIds(entityTagsDialog.initialTagIds)
				}
			}

			TagSelectPopup {
				id: tagSelectPopup
				anchors.top: parent.top
				anchors.topMargin: Style.marginL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginL
				embedded: true
				showSelectedGroup: true
				itemWidth: tagsContent.width - 2 * Style.marginL
				enabled: entityTagsDialog.tagsLoaded

				dataProvider: TagSelectDataProvider {
					context: entityTagsDialog.context
				}
			}

			BaseText {
				id: hintText
				anchors.top: tagSelectPopup.bottom
				anchors.topMargin: Style.marginS
				anchors.left: tagSelectPopup.left
				anchors.right: tagSelectPopup.right
				text: qsTr("Space toggles a tag, Ctrl+Enter applies, Esc cancels")
				font.pixelSize: Style.fontSizeXS
				color: Style.inactiveTextColor
				elide: Text.ElideRight
			}
		}
	}
}
