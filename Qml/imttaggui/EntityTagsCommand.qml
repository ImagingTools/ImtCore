// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0

/*!
	\qmltype EntityTagsCommand
	\inqmlmodule imttaggui
	\brief Handles the "AssignTags" command of a collection view or a document editor.

	The server side of the command is ImtTagVoce/AssignTagsCommand in the view's command list.
	In a collection view the command works on the selected objects; an editor sets \c entityIds
	to its saved document. The command is enabled while there is something to tag.

	\code
	EntityTagsCommand {
		view: collectionView
		entityType: "Devices"
	}

	EntityTagsCommand {
		view: deviceEditor
		entityType: "Devices"
		entityIds: deviceEditor.isNewDocument ? [] : [deviceEditor.documentObjectId]
	}
	\endcode
*/
Item {
	id: entityTagsCommandRoot

	property string context: ""
	property string commandId: "AssignTags"
	property string entityType: ""

	//! Collection view or document view whose command bar holds the command.
	property Item view: null

	//! Objects the command works on; a collection view keeps it in sync with its selection.
	property var entityIds: []

	signal tagsChanged(var entityIds)

	onEntityIdsChanged: {
		entityTagsCommandRoot.updateCommandState()
	}

	onViewChanged: {
		entityTagsCommandRoot.updateCommandState()
	}

	function updateCommandState(){
		if (entityTagsCommandRoot.view && entityTagsCommandRoot.view.commandsController){
			entityTagsCommandRoot.view.commandsController.setCommandIsEnabled(entityTagsCommandRoot.commandId, entityTagsCommandRoot.entityIds.length > 0)
		}
	}

	function openDialog(){
		if (entityTagsCommandRoot.entityType === "" || entityTagsCommandRoot.entityIds.length === 0){
			return
		}

		ModalDialogManager.openDialog(entityTagsDialogComp, {
			entityType: entityTagsCommandRoot.entityType,
			entityIds: entityTagsCommandRoot.entityIds
		})
	}

	Connections {
		target: entityTagsCommandRoot.view
		ignoreUnknownSignals: true

		function onCommandActivated(commandId){
			if (commandId === entityTagsCommandRoot.commandId){
				entityTagsCommandRoot.openDialog()
			}
		}

		function onSelectionChanged(selectedIds, selectedIndexes){
			entityTagsCommandRoot.entityIds = selectedIds
		}
	}

	Connections {
		target: entityTagsCommandRoot.view ? entityTagsCommandRoot.view.commandsController : null

		function onCommandsReceived(typeId, commands){
			entityTagsCommandRoot.updateCommandState()
		}
	}

	Component {
		id: entityTagsDialogComp

		EntityTagsDialog {
			id: entityTagsDialogItem
			context: entityTagsCommandRoot.context

			onTagsChanged: {
				entityTagsCommandRoot.tagsChanged(entityTagsDialogItem.entityIds)
			}
		}
	}
}
