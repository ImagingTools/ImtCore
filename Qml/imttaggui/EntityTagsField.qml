// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtauthgui 1.0

/*!
	\qmltype EntityTagsField
	\inqmlmodule imttaggui
	\brief Tags of one entity for an editor or a detail view: chips plus a "Tags" button.

	The button opens a searchable multi-select of the tag catalog; every toggle
	replaces the tag set of the entity on the server. Without the AssignTags
	permission the field is read-only.

	\code
	EntityTagsField {
		entityType: "Devices"
		entityId: deviceEditor.documentId
	}
	\endcode
*/
Row {
	id: entityTagsFieldRoot
	objectName: "EntityTagsField"

	property string context: ""
	property string entityType: ""
	property string entityId: ""
	property bool readOnly: !PermissionsController.checkPermission("AssignTags")

	//! Width available for the chips before they wrap.
	property int chipsWidth: 400

	signal tagsChanged()

	spacing: Style.spacingM

	function refresh(){
		if (entityTagsFieldRoot.entityType !== "" && entityTagsFieldRoot.entityId !== ""){
			entityTagsProvider.load([entityTagsFieldRoot.entityId])
		}
		else{
			tagChipRow.tags = []
		}
	}

	onEntityIdChanged: {
		entityTagsFieldRoot.refresh()
	}

	onEntityTypeChanged: {
		entityTagsFieldRoot.refresh()
	}

	TagChipRow {
		id: tagChipRow
		anchors.verticalCenter: parent.verticalCenter
		width: Math.min(implicitWidth, entityTagsFieldRoot.chipsWidth)
	}

	BaseText {
		anchors.verticalCenter: parent.verticalCenter
		visible: tagChipRow.tags.length === 0
		text: qsTr("No tags")
		color: Style.inactiveTextColor
		font.pixelSize: Style.fontSizeS
	}

	Button {
		anchors.verticalCenter: parent.verticalCenter
		visible: !entityTagsFieldRoot.readOnly && entityTagsFieldRoot.entityId !== ""
		text: qsTr("Tags...")

		onClicked: {
			ModalDialogManager.openDialog(tagSelectComp, {})
		}
	}

	EntityTagsProvider {
		id: entityTagsProvider
		context: entityTagsFieldRoot.context
		entityType: entityTagsFieldRoot.entityType

		onLoaded: {
			tagChipRow.tags = entityTagsProvider.getTags(entityTagsFieldRoot.entityId)
		}
	}

	EntityTagsEditor {
		id: entityTagsEditor
		context: entityTagsFieldRoot.context

		onChanged: {
			entityTagsFieldRoot.refresh()
			entityTagsFieldRoot.tagsChanged()
		}

		onFailed: {
			PopupManager.addErrorMessage(message)
			entityTagsFieldRoot.refresh()
		}
	}

	Component {
		id: tagSelectComp

		FilterableSelectPopup {
			showCheckBox: true
			filterPlaceholder: qsTr("Filter tags...")
			preselectedIds: entityTagsProvider.getTagIds(entityTagsFieldRoot.entityId)

			dataProvider: FilterableSelectGqlDataProvider {
				context: entityTagsFieldRoot.context
				collectionId: "Tags"
				multiSelect: true
			}

			onSelectionChanged: {
				entityTagsEditor.setTags(entityTagsFieldRoot.entityType, entityTagsFieldRoot.entityId, selectedIds)
			}
		}
	}
}
