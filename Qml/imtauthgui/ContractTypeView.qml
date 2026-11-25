// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractTypeView
 *
 * The editor of a type of contracts: its name and the links of the parties, the periods it offers,
 * the parties it takes by role and the documents a contract of it needs before it is sent.
 * A system type and a type of another organization are shown read only. Its parts are the pages of a MultiPageView.
 * The form of ContractTypeDialog.
 */
Item {
	id: typeView

	property string tenantId: ""
	// The type edited, null for a new one
	property var contractType: null
	// ContractsApiClient
	property var contractsApiClient: null
	property bool canEdit: false

	// The working copy: the fields of ContractTypeInput, the required documents as kinds of ContractTexts
	property var typeData: ({})
	property var roleSlots: []
	property var requiredKinds: []

	readonly property bool isNew: typeView.contractType === null
	readonly property bool isEditable: typeView.canEdit
		&& (typeView.isNew || (!typeView.contractType.isSystem && typeView.contractType.ownerTenantId === typeView.tenantId))
	readonly property bool isArchived: typeView.contractType !== null && typeView.contractType.isArchived === true
	// An archived type is restored before it is changed
	readonly property bool isChangeable: typeView.isEditable && !typeView.isArchived

	Component.onCompleted: {
		typeView.fill()
	}

	// A dialog gives the type after the form is created
	onContractTypeChanged: {
		typeView.fill()
		typeView.__updatePagesGui()
	}

	function __updatePagesGui() {
		for (var i = 0; i < multiPageView.pagesModel.count; i++) {
			var page = multiPageView.getPageByIndex(i)
			if (page && typeof page.updateGui === "function")
				page.updateGui()
		}
	}

	function fill() {
		var type = typeView.contractType
		typeView.typeData = {"typeId": type ? type.typeId : "", "name": type ? (type.name || "") : "", "description": type ? (type.description || "") : "",
							 "numberPrefix": type ? (type.numberPrefix || "") : "", "linkScheme": type ? (type.linkScheme || "Mesh") : "Mesh",
							 "starRoleId": type ? (type.starRoleId || "") : "", "termDays": type ? (type.termDays || 0) : 0,
							 "autoRenew": type ? type.autoRenew === true : false, "renewalPeriodDays": type ? (type.renewalPeriodDays || 0) : 0,
							 "renewalNoticeDays": type ? (type.renewalNoticeDays || 0) : 0, "terminationNoticeDays": type ? (type.terminationNoticeDays || 0) : 0,
							 "isSequential": type ? type.isSequential === true : false, "requiredSignatures": type ? Math.max(1, type.requiredSignatures || 1) : 1,
							 "allocationTemplates": type ? (type.allocationTemplates || []) : []}
		typeView.roleSlots = type ? (type.roleSlots || []).slice() : []
		var kinds = []
		var required = type ? (type.requiredAttachmentKinds || []) : []
		for (var i = 0; i < required.length; i++)
			kinds.push(ContractTexts.attachmentKindAt(required[i]))
		typeView.requiredKinds = kinds
	}

	readonly property string hintText: !typeView.isEditable
		? (typeView.contractType && typeView.contractType.isSystem
		   ? qsTr("A system type is the same for every organization and is read only. Create your own type if you need other terms.")
		   : qsTr("You can read the type but not change it."))
		: typeView.isArchived
		  ? qsTr("The type is archived: its contracts keep it, new contracts are not offered it. Restore it to use it again.")
		  : qsTr("A new contract of this type gets these terms; the initiator can still change them in the contract.")

	// The roles of the parties offered
	function partyRoles() {
		var result = []
		var roles = typeView.contractsApiClient ? typeView.contractsApiClient.partyRoles : []
		for (var i = 0; i < roles.length; i++) {
			if (roles[i].isPartyRole && !roles[i].isArchived)
				result.push({"roleId": roles[i].roleId, "name": roles[i].name})
		}
		return result
	}

	function roleName(roleId) {
		var roles = typeView.contractsApiClient ? typeView.contractsApiClient.partyRoles : []
		for (var i = 0; i < roles.length; i++) {
			if (roles[i].roleId === roleId)
				return roles[i].name
		}
		return roleId
	}

	// The roles without a slot yet, and the one of the slot edited
	function rolesForSlot(roleId) {
		var result = []
		var roles = typeView.partyRoles()
		for (var i = 0; i < roles.length; i++) {
			var isTaken = false
			for (var j = 0; j < typeView.roleSlots.length; j++)
				isTaken = isTaken || (typeView.roleSlots[j].roleId === roles[i].roleId && roles[i].roleId !== roleId)
			if (!isTaken)
				result.push(roles[i])
		}
		return result
	}

	function setSlot(roleId, minCount, maxCount) {
		var slots = []
		var isFound = false
		for (var i = 0; i < typeView.roleSlots.length; i++) {
			if (typeView.roleSlots[i].roleId === roleId) {
				slots.push({"roleId": roleId, "minCount": minCount, "maxCount": maxCount})
				isFound = true
			}
			else {
				slots.push(typeView.roleSlots[i])
			}
		}
		if (!isFound)
			slots.push({"roleId": roleId, "minCount": minCount, "maxCount": maxCount})
		typeView.roleSlots = slots
	}

	function removeSlot(roleId) {
		var slots = []
		for (var i = 0; i < typeView.roleSlots.length; i++) {
			if (typeView.roleSlots[i].roleId !== roleId)
				slots.push(typeView.roleSlots[i])
		}
		typeView.roleSlots = slots
	}

	function setKindRequired(kind, isRequired) {
		var kinds = []
		for (var i = 0; i < typeView.requiredKinds.length; i++) {
			if (typeView.requiredKinds[i] !== kind)
				kinds.push(typeView.requiredKinds[i])
		}
		if (isRequired)
			kinds.push(kind)
		typeView.requiredKinds = kinds
	}

	function __updateFromPages() {
		for (var i = 0; i < multiPageView.pagesModel.count; i++) {
			var page = multiPageView.getPageByIndex(i)
			if (page && typeof page.updateModel === "function")
				page.updateModel()
		}
	}

	// Sends the type; false, with the errors shown, when the form is not valid
	function save() {
		if (!typeView.contractsApiClient || !typeView.isChangeable)
			return false
		typeView.__updateFromPages()
		var generalPage = multiPageView.getPageById("General")
		if (!typeView.typeData.name || typeView.typeData.name.trim() === "") {
			if (generalPage)
				generalPage.revealErrors()
			PopupManager.addWarningMessage(qsTr("A type needs a name."), true)
			multiPageView.activatePage(0)
			return false
		}
		if (typeView.typeData.linkScheme === "Star" && (typeView.typeData.starRoleId || "") === "") {
			PopupManager.addWarningMessage(qsTr("Choose the role linked with every other party."), true)
			multiPageView.activatePage(0)
			return false
		}
		var type = {}
		for (var key in typeView.typeData)
			type[key] = typeView.typeData[key]
		type.name = type.name.trim()
		type.numberPrefix = (type.numberPrefix || "").trim()
		type.roleSlots = typeView.roleSlots
		var kinds = []
		for (var k = 0; k < typeView.requiredKinds.length; k++)
			kinds.push(ContractTexts.attachmentKinds.indexOf(typeView.requiredKinds[k]))
		type.requiredAttachmentKinds = kinds
		typeView.contractsApiClient.saveContractType(typeView.tenantId, type)
		return true
	}

	BaseText {
		id: hintLabel

		anchors.top: parent.top
		anchors.topMargin: Style.marginL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL

		wrapMode: Text.WordWrap
		text: typeView.hintText
		font.pixelSize: Style.fontSizeS
		color: Style.inactiveTextColor
	}

	MultiPageView {
		id: multiPageView

		anchors.top: hintLabel.bottom
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		panelWidth: Style.sizeHintXXS

		onPageLoaded: {
			if (pageItem && typeof pageItem.updateGui === "function")
				pageItem.updateGui()
		}

		Component.onCompleted: {
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
			multiPageView.addPage("Period", qsTr("Period"), periodPageComp, "Icons/Calendar")
			multiPageView.addPage("Parties", qsTr("Parties by role"), slotsPageComp, "Icons/Organizations")
			multiPageView.addPage("Documents", qsTr("Documents"), documentsPageComp, "Icons/Attachment")
			multiPageView.activatePage(0)
		}
	}

	// --- General ---
	Component {
		id: generalPageComp

		Item {
			id: generalPage

			anchors.fill: parent

			function revealErrors() {
				nameInput.revealError()
			}

			function updateGui() {
				nameInput.text = typeView.typeData.name || ""
				descriptionInput.text = typeView.typeData.description || ""
				prefixInput.text = typeView.typeData.numberPrefix || ""
				schemeCB.currentIndex = schemeModel.fill(ContractTexts.linkSchemeItems(), "id", "name", typeView.typeData.linkScheme || "Mesh")
				starCB.currentIndex = starModel.fill(typeView.partyRoles(), "roleId", "name", typeView.typeData.starRoleId || "")
			}

			function updateModel() {
				typeView.typeData.name = nameInput.text
				typeView.typeData.description = descriptionInput.text
				typeView.typeData.numberPrefix = prefixInput.text
				typeView.typeData.linkScheme = schemeModel.idAt(schemeCB.currentIndex) !== "" ? schemeModel.idAt(schemeCB.currentIndex) : "Mesh"
				typeView.typeData.starRoleId = typeView.typeData.linkScheme === "Star" ? starModel.idAt(starCB.currentIndex) : ""
			}

			ChoiceModel {
				id: schemeModel
			}

			ChoiceModel {
				id: starModel
			}

			CustomScrollbar {
				z: generalFlickable.z + 1
				anchors.right: parent.right
				anchors.top: generalFlickable.top
				anchors.bottom: generalFlickable.bottom
				secondSize: Style.marginM
				targetItem: generalFlickable
			}

			Flickable {
				id: generalFlickable

				anchors.fill: parent
				anchors.margins: Style.marginXL
				contentHeight: generalColumn.height
				boundsBehavior: Flickable.StopAtBounds
				clip: true

				Column {
					id: generalColumn

					width: Math.min(generalFlickable.width - Style.marginM, Style.contentWidthMax)
					spacing: Style.marginL

					RegularExpressionValidator {
						id: nameValidator
						regularExpression: /^(?!\s*$).+/
					}

					GroupElementView {
						width: parent.width

						TextInputElementView {
							id: nameInput
							isRequired: true
							showErrorWhenInvalid: true
							deferErrorUntilEdited: true
							errorText: qsTr("Enter the name of the type")
							textInputValidator: nameValidator
							name: qsTr("Name")
							readOnly: !typeView.isChangeable
							placeHolderText: qsTr("e.g. Supply agreement")
						}

						TextInputElementView {
							id: descriptionInput
							name: qsTr("Description")
							readOnly: !typeView.isChangeable
							placeHolderText: qsTr("Optional")
						}

						TextInputElementView {
							id: prefixInput
							name: qsTr("Prefix of the numbers")
							readOnly: !typeView.isChangeable
							placeHolderText: qsTr("e.g. SUP")
						}

						ComboBoxElementView {
							id: schemeCB
							name: qsTr("Links")
							model: schemeModel
							changeable: typeView.isChangeable
						}

						ComboBoxElementView {
							id: starCB
							isRequired: true
							isSelectionRequired: true
							errorText: qsTr("Choose the role linked with every other party")
							visible: schemeModel.idAt(schemeCB.currentIndex) === "Star"
							name: qsTr("Linked with every other party")
							model: starModel
							changeable: typeView.isChangeable
						}
					}
				}
			}
		}
	}

	// --- Period ---
	Component {
		id: periodPageComp

		Item {
			id: periodPage

			anchors.fill: parent

			function updateGui() {
				termDaysInput.value = typeView.typeData.termDays || 0
				autoRenewSwitch.checked = typeView.typeData.autoRenew === true
				renewalPeriodInput.value = typeView.typeData.renewalPeriodDays || 0
				renewalNoticeInput.value = typeView.typeData.renewalNoticeDays || 0
				terminationNoticeInput.value = typeView.typeData.terminationNoticeDays || 0
				sequentialSwitch.checked = typeView.typeData.isSequential === true
				signaturesInput.value = Math.max(1, typeView.typeData.requiredSignatures || 1)
			}

			function updateModel() {
				typeView.typeData.termDays = termDaysInput.value
				typeView.typeData.autoRenew = autoRenewSwitch.checked
				typeView.typeData.renewalPeriodDays = renewalPeriodInput.value
				typeView.typeData.renewalNoticeDays = renewalNoticeInput.value
				typeView.typeData.terminationNoticeDays = terminationNoticeInput.value
				typeView.typeData.isSequential = sequentialSwitch.checked
				typeView.typeData.requiredSignatures = Math.max(1, signaturesInput.value)
			}

			CustomScrollbar {
				z: periodFlickable.z + 1
				anchors.right: parent.right
				anchors.top: periodFlickable.top
				anchors.bottom: periodFlickable.bottom
				secondSize: Style.marginM
				targetItem: periodFlickable
			}

			Flickable {
				id: periodFlickable

				anchors.fill: parent
				anchors.margins: Style.marginXL
				contentHeight: periodColumn.height
				boundsBehavior: Flickable.StopAtBounds
				clip: true

				Column {
					id: periodColumn

					width: Math.min(periodFlickable.width - Style.marginM, Style.contentWidthMax)
					spacing: Style.marginL

					GroupElementView {
						width: parent.width

						SpinBoxElementView {
							id: termDaysInput
							name: qsTr("Term")
							description: qsTr("0 for no term")
							readOnly: !typeView.isChangeable
							from: 0
							units: qsTr("days")
							to: 36500
						}

						SwitchElementView {
							id: autoRenewSwitch
							name: qsTr("Renewed automatically")
							readOnly: !typeView.isChangeable
						}

						SpinBoxElementView {
							id: renewalPeriodInput
							visible: autoRenewSwitch.checked
							name: qsTr("Renewal period")
							readOnly: !typeView.isChangeable
							from: 0
							units: qsTr("days")
							to: 36500
						}

						SpinBoxElementView {
							id: renewalNoticeInput
							visible: autoRenewSwitch.checked
							name: qsTr("Stop the renewal at least")
							description: qsTr("Days before the end of the term")
							readOnly: !typeView.isChangeable
							from: 0
							units: qsTr("days")
							to: 36500
						}

						SpinBoxElementView {
							id: terminationNoticeInput
							name: qsTr("Notice period")
							description: qsTr("A party leaves this long after its notice; 0: at once")
							readOnly: !typeView.isChangeable
							from: 0
							units: qsTr("days")
							to: 36500
						}

						SwitchElementView {
							id: sequentialSwitch
							name: qsTr("The parties sign in their order")
							readOnly: !typeView.isChangeable
						}

						SpinBoxElementView {
							id: signaturesInput
							name: qsTr("Signatures of each party")
							readOnly: !typeView.isChangeable
							from: 1
							to: 10
						}
					}
				}
			}
		}
	}

	// --- Parties by role ---
	Component {
		id: slotsPageComp

		Item {
			id: slotsPage

			anchors.fill: parent

			readonly property var slotRows: slotsPage.__slotRows(typeView.roleSlots)

			function __slotRows(slots) {
				var rows = []
				for (var i = 0; i < slots.length; i++) {
					rows.push({"id": slots[i].roleId,
							   "role": typeView.roleName(slots[i].roleId),
							   "minCount": String(slots[i].minCount || 0),
							   "maxCount": slots[i].maxCount ? String(slots[i].maxCount) : qsTr("Any"),
							   "minValue": slots[i].minCount || 0,
							   "maxValue": slots[i].maxCount || 0})
				}
				return rows
			}

			Item {
				id: slotsHeader

				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL

				height: Math.max(slotsDescription.height, addSlotButton.height)

				BaseText {
					id: slotsDescription
					anchors.left: parent.left
					anchors.right: addSlotButton.left
					anchors.rightMargin: Style.marginL
					anchors.verticalCenter: parent.verticalCenter
					wrapMode: Text.WordWrap
					text: qsTr("How many parties of a role a contract of this type takes. A role without a slot is free.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				Button {
					id: addSlotButton
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					visible: typeView.isChangeable
					variant: "primary"
					text: qsTr("Add role")
					onClicked: {
						ModalDialogManager.openDialog(slotDialogComp, {"roles": typeView.rolesForSlot("")})
					}
				}
			}

			Table {
				id: slotsTable

				anchors.top: slotsHeader.bottom
				anchors.topMargin: Style.marginM
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL

				visible: slotsPage.slotRows.length > 0
				selectable: false
				elements: slotsPage.slotRows
			}

			TableHeaders {
				id: slotsHeaders

				Component.onCompleted: {
					addHeader("role", qsTr("Role"), false, false)
					addHeader("minCount", qsTr("At least"), false, false)
					addHeader("maxCount", qsTr("At most"), false, false)
					addHeader("actions", "", false, false)
					slotsTable.setColumnContentById("actions", slotActionsComp)
					slotsTable.headers = slotsHeaders
				}
			}

			Component {
				id: slotActionsComp

				TableCellDelegateBase {
					id: slotActions

					readonly property var rowData: slotActions.getRowData()

					Row {
						anchors.verticalCenter: parent.verticalCenter
						anchors.right: parent.right
						anchors.rightMargin: Style.marginM
						spacing: Style.marginS
						visible: typeView.isChangeable

						Button {
							text: qsTr("Edit")
							onClicked: {
								ModalDialogManager.openDialog(slotDialogComp, {"roles": typeView.rolesForSlot(slotActions.rowData.id),
																			   "initialRoleId": slotActions.rowData.id,
																			   "initialMinCount": slotActions.rowData.minValue,
																			   "initialMaxCount": slotActions.rowData.maxValue})
							}
						}

						Button {
							text: qsTr("Remove")
							onClicked: {
								typeView.removeSlot(slotActions.rowData.id)
							}
						}
					}
				}
			}
		}
	}

	// --- Documents ---
	Component {
		id: documentsPageComp

		Item {
			id: documentsPage

			anchors.fill: parent

			readonly property var kindRows: documentsPage.__kindRows(typeView.requiredKinds)

			function __kindRows(requiredKinds) {
				var rows = []
				for (var i = 0; i < ContractTexts.attachmentKinds.length; i++) {
					var kind = ContractTexts.attachmentKinds[i]
					rows.push({"id": kind,
							   "kind": ContractTexts.attachmentKindText(kind),
							   "isRequired": requiredKinds.indexOf(kind) >= 0})
				}
				return rows
			}

			BaseText {
				id: documentsDescription

				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL

				wrapMode: Text.WordWrap
				text: qsTr("The documents a contract of this type needs before it is sent to the parties.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Table {
				id: kindsTable

				anchors.top: documentsDescription.bottom
				anchors.topMargin: Style.marginM
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL

				selectable: false
				elements: documentsPage.kindRows
			}

			TableHeaders {
				id: kindsHeaders

				Component.onCompleted: {
					addHeader("kind", qsTr("Document"), false, false)
					addHeader("required", qsTr("Needed"), false, false)
					kindsTable.setColumnContentById("required", requiredCellComp)
					kindsTable.headers = kindsHeaders
				}
			}

			Component {
				id: requiredCellComp

				TableCellDelegateBase {
					id: requiredCell

					readonly property var rowData: requiredCell.getRowData()

					CheckBox {
						anchors.verticalCenter: parent.verticalCenter
						anchors.left: parent.left
						anchors.leftMargin: Style.marginM
						enabled: typeView.isChangeable
						checkState: requiredCell.rowData && requiredCell.rowData.isRequired ? Qt.Checked : Qt.Unchecked
						onCheckStateChanged: {
							if (requiredCell.rowData && (checkState === Qt.Checked) !== requiredCell.rowData.isRequired)
								typeView.setKindRequired(requiredCell.rowData.id, checkState === Qt.Checked)
						}
					}
				}
			}
		}
	}

	Component {
		id: slotDialogComp

		ContractRoleSlotDialog {
			id: slotDialog
			title: slotDialog.initialRoleId !== "" ? qsTr("Parties of the role") : qsTr("Add role")
			onFinished: {
				var content = slotDialog.contentItem
				if (buttonId == Enums.ok && content && content.roleId !== "")
					typeView.setSlot(content.roleId, content.minCount, Math.max(content.maxCount, 0))
			}
		}
	}
}
