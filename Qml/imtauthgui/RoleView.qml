import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthRolesSdl 1.0
import imtcolgui 1.0
import imtdocgui 1.0
import imtauthgui 1.0
import imtguigql 1.0

ViewBase {
	id: container;
	
	anchors.fill: parent;
	contentColor: Style.baseColor
	
	property string productId: "";
	property string tenantId: "";
	property var permissionsProvider: null;

	property bool __permissionsRequested: false
	property var __receivedPermissions: null
	property bool __completed: false

	property RoleData roleData: model;

	Component.onCompleted: {
		container.__completed = true
		container.__requestPermissionsOnce()
	}

	onProductIdChanged: {
		if (!container.__completed)
			return
		container.__permissionsRequested = false
		container.__receivedPermissions = null
		container.__requestPermissionsOnce()
	}

	onTenantIdChanged: {
		if (!container.__completed)
			return
		container.__permissionsRequested = false
		container.__receivedPermissions = null
		container.__requestPermissionsOnce()
	}

	onPermissionsProviderChanged: {
		if (!container.__completed)
			return
		container.__permissionsRequested = false
		container.__receivedPermissions = null
		container.__requestPermissionsOnce()
	}

	Connections {
		target: container.permissionsProvider

		function onPermissionsReceived(permissions, sourceTenantId) {
			var expectedTenantId = container.tenantId || ""
			var actualTenantId = sourceTenantId || ""
			if (expectedTenantId !== actualTenantId)
				return
			container.__receivedPermissions = permissions
			container.__populatePermissionsTree()
		}
	}

	function __requestPermissionsOnce() {
		if (!container.permissionsProvider)
			return
		if (container.productId === "")
			return
		if (container.__permissionsRequested)
			return
		container.__permissionsRequested = true
		container.permissionsProvider.productId = container.productId
		var requestTenantId = container.tenantId || ""
		container.permissionsProvider.requestPermissions(requestTenantId)
	}

	function __populatePermissionsTree() {
		var permissionPageInstance = multiPageView.getPageById("Permission")
		if (!permissionPageInstance || !permissionPageInstance.bottomItem)
			return
		var perms = container.__receivedPermissions
		if (!perms)
			return
		permissionPageInstance.bottomItem.rebuildFromFlatArray(perms)
		if (container.roleData)
			container.doUpdateGuiPermissions()
	}

	function updateGui(){
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateGui()
		var parentRolesPageInstance = multiPageView.getPageById("ParentRoles")
		if (parentRolesPageInstance)
			parentRolesPageInstance.updateGui()
		container.doUpdateGuiPermissions()
		container.updateBadges()
	}
	
	function updateModel(){
		if (!container.roleData){
			return
		}

		if (container.productId === ""){
			console.error("Unable to update a role model. Product-ID is empty");
			return;
		}
		
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateModel()
		var parentRolesPageInstance = multiPageView.getPageById("ParentRoles")
		if (parentRolesPageInstance)
			parentRolesPageInstance.updateModel()
		container.doUpdateModelPermissions()
		
		roleData.m_productId = container.productId;
		container.updateBadges()
	}

	// Emitted after a change of the direct assignments that other ones are inherited through.
	signal assignmentsChanged()

	// The inherited assignments are being recalculated.
	property bool assignmentsUpdating: false

	// Shows the recalculated assignment lists on the loaded pages.
	function updateAssignments(){
		container.setBlockingUpdateModel(true)
		var parentRolesPageInstance = multiPageView.getPageById("ParentRoles")
		if (parentRolesPageInstance)
			parentRolesPageInstance.updateGui()
		container.doUpdateGuiPermissions()
		container.updateBadges()
		container.setBlockingUpdateModel(false)
	}

	// Item counts next to the page names.
	function updateBadges(){
		if (!container.roleData){
			return
		}

		multiPageView.setPageBadge("ParentRoles", String(container.roleData.m_parentRoles ? container.roleData.m_parentRoles.count : 0))
		multiPageView.setPageBadge("Permission", String(container.roleData.m_permissions ? container.roleData.m_permissions.count : 0))
	}

	function getHeaders(){
		return {};
	}

	function doUpdateGuiPermissions() {
		if (!container.roleData){
			return
		}

		var permissionPageInstance = multiPageView.getPageById("Permission")
		if (permissionPageInstance && permissionPageInstance.bottomItem){
			// Unlock first: unchecking skips locked nodes.
			permissionPageInstance.bottomItem.setLockedPermissions({})
			permissionPageInstance.bottomItem.applySelection(container.ownPermissionIds())
			permissionPageInstance.bottomItem.setLockedPermissions(container.inheritedPermissionSources())
		}
	}

	// Ids of the permissions assigned to the role itself.
	function ownPermissionIds() {
		var ids = []
		var permissions = container.roleData ? container.roleData.m_permissions : null
		for (var i = 0; permissions && i < permissions.count; i++){
			var item = permissions.get(i).item
			if (item.m_direct){
				ids.push(String(item.m_id))
			}
		}
		return ids
	}

	// Permissions inherited from parent roles {permissionId: "Parent > Grandparent"}.
	function inheritedPermissionSources() {
		var result = ({})
		var permissions = container.roleData ? container.roleData.m_permissions : null
		for (var i = 0; permissions && i < permissions.count; i++){
			var item = permissions.get(i).item
			var sources = item.m_sources
			var paths = []
			for (var k = 0; sources && k < sources.count; k++){
				var steps = sources.get(k).item.m_steps
				var names = []
				for (var s = 0; steps && s < steps.count; s++){
					var step = steps.get(s).item
					names.push(step.m_name ? String(step.m_name) : String(step.m_id))
				}
				if (names.length > 0){
					paths.push(names.join(" " + String.fromCharCode(0x2192) + " "))
				}
			}
			if (paths.length > 0){
				result[String(item.m_id)] = paths.join("; ")
			}
		}
		return result
	}

	function doUpdateModelPermissions() {
		if (!container.roleData){
			return
		}

		var permissionPageInstance = multiPageView.getPageById("Permission")
		if (!permissionPageInstance || !permissionPageInstance.bottomItem){
			return
		}

		// Only leaf permission IDs are stored. Locked permissions come from parent roles;
		// the ones the role also owned before stay its own.
		var ownIds = permissionPageInstance.bottomItem.getCheckedIds(true)
		var lockedPermissions = permissionPageInstance.bottomItem.lockedPermissions
		var previousIds = container.ownPermissionIds()
		for (var i = 0; i < previousIds.length; i++){
			if (lockedPermissions[previousIds[i]] && ownIds.indexOf(previousIds[i]) < 0){
				ownIds.push(previousIds[i])
			}
		}

		if (!container.roleData.hasPermissions()){
			container.roleData.emplacePermissions()
		}

		var permissions = container.roleData.m_permissions
		var present = []
		for (var k = permissions.count - 1; k >= 0; k--){
			var item = permissions.get(k).item
			var itemId = String(item.m_id)
			var own = ownIds.indexOf(itemId) >= 0
			var inherited = item.m_sources && item.m_sources.count > 0
			if (!own && !inherited){
				permissions.removeElement(k)
				continue
			}
			item.m_direct = own
			present.push(itemId)
		}

		for (var n = 0; n < ownIds.length; n++){
			if (present.indexOf(ownIds[n]) < 0){
				var assignment = container.roleData.createPermissionsArrayElement()
				assignment.m_id = ownIds[n]
				assignment.m_name = ownIds[n]
				assignment.m_direct = true
				permissions.addElement(assignment)
			}
		}
	}

	MultiPageView {
		id: multiPageView
		objectName: "RoleEditorPages"
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS

		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
			multiPageView.addPage("ParentRoles", qsTr("Parent Roles"), parentRolesPageComp, "Icons/Role")
			multiPageView.addPage("Permission", qsTr("Permissions"), permissionPageComp, "Icons/Key")
			if (PermissionsController.checkPermission("ViewRevisions")){
				multiPageView.addPage("History", qsTr("History"), historyPageComp, "Icons/History")
			}
			multiPageView.currentIndex = 0
			container.updateBadges()
		}

		Component.onCompleted: {
			multiPageView.updatePages()
		}
	}

	Component {
		id: generalPageComp

		Item {
			id: generalPage
			anchors.fill: parent

			function updateGui(){
				generalGroup.updateGui();
			}

			function updateModel(){
				generalGroup.updateModel();
			}

			CustomScrollbar {
				id: scrollbar;
				z: parent.z + 1;
				anchors.right: parent.right;
				anchors.top: flickable.top;
				anchors.bottom: flickable.bottom;
				secondSize: Style.marginM;
				targetItem: flickable;
			}
			
			Flickable {
				id: flickable;
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.bottom: parent.bottom;
				anchors.bottomMargin: Style.marginXL;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginXL;
				anchors.right: scrollbar.left;
				anchors.rightMargin: Style.marginXL;
				contentHeight: bodyColumn.height + 2 * Style.marginXL;
				
				boundsBehavior: Flickable.StopAtBounds;
				clip: true;
				
				Column {
					id: bodyColumn;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: Math.min(parent.width, Style.contentWidthMax);
					spacing: Style.marginXL;
					
					GroupHeaderView {
						width: parent.width;
						title: qsTr("General");
						groupView: generalGroup;
					}
					
					GroupElementView {
						id: generalGroup;
						width: parent.width;
						
						TextInputElementView {
							id: roleNameInput;

							// Test instrumentation: matches the AccountEditor/DeviceEditor/etc. convention of
							// an explicit per-field objectName on the ElementView usage site (the shared
							// TextInputElementView/CustomTextField components only ever carry the generic
							// "TextField"/"TextInput"). Inert - no runtime/visual effect.
							objectName: "RoleNameInput";

							name: qsTr("Role Name");
							placeHolderText: qsTr("Enter the role name");
							
							onEditingFinished: {
								if (!container.roleData){
									return
								}

								let oldText = container.roleData.m_name;
								if (oldText && oldText !== roleNameInput.text || !oldText && roleNameInput.text !== ""){
									roleIdInput.text = roleNameInput.text.replace(/\s+/g, '');
									container.doUpdateModel();
								}
							}
							
							KeyNavigation.tab: roleIdInput;
						}
						
						TextInputElementView {
							id: roleIdInput;

							// Test instrumentation - see roleNameInput's comment above. Inert.
							objectName: "RoleIdInput";

							readOnly: true;

							name: qsTr("Role-ID");
							
							KeyNavigation.tab: descriptionInput;
							KeyNavigation.backtab: roleNameInput;
						}
						
						TextInputElementView {
							id: descriptionInput;

							// Test instrumentation - see roleNameInput's comment above. Inert.
							objectName: "RoleDescriptionInput";

							name: qsTr("Description");
							placeHolderText: qsTr("Enter the description");

							onEditingFinished: {
								let oldText = container.roleData.m_description;
								if (oldText && oldText !== descriptionInput.text || !oldText && descriptionInput.text !== ""){
									container.doUpdateModel();
								}
							}
							
							KeyNavigation.backtab: roleIdInput;
						}

						function updateGui(){
							if (!container.roleData){
								return
							}

							roleIdInput.text = container.roleData.m_roleId;
							roleNameInput.text = container.roleData.m_name;
							descriptionInput.text = container.roleData.m_description;
						}
						
						function updateModel(){
							if (!container.roleData){
								return
							}

							container.roleData.m_roleId = roleIdInput.text;
							container.roleData.m_name = roleNameInput.text;
							container.roleData.m_description = descriptionInput.text;
						}
					}
				}
			}
		}
	}

	Component {
		id: parentRolesPageComp

		Item {
			id: parentRolesPage
			anchors.fill: parent

			function updateGui(){
				if (!container.roleData){
					return
				}

				parentRolesTable.loadAssignments(container.roleData.m_parentRoles)
			}

			function updateModel(){
				if (!container.roleData){
					return
				}

				if (!container.roleData.hasParentRoles()){
					container.roleData.emplaceParentRoles()
				}

				var added = parentRolesTable.syncAssignments(container.roleData.m_parentRoles)
				for (var i = 0; i < added.length; i++){
					var assignment = container.roleData.createParentRolesArrayElement()
					assignment.m_id = added[i].id
					assignment.m_name = added[i].title
					assignment.m_direct = true
					container.roleData.m_parentRoles.addElement(assignment)
				}
			}

			Component.onCompleted: {
				parentRolesPage.updateGui();
			}

			Column {
				id: parentRolesHeader
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				x: Math.max(0, (parentRolesPage.width - width) / 2)
				width: Math.max(0, Math.min(Style.contentWidthMax, parentRolesPage.width - 2 * Style.marginXL))
				spacing: Style.marginM

				GroupHeaderView {
					width: parent.width
					title: qsTr("Parent Roles") + " (" + parentRolesTable.itemsCount + ")" + (container.assignmentsUpdating ? "   " + qsTr("Updating...") : "")
					controlComp: Component {
						Row {
							anchors.verticalCenter: parent.verticalCenter
							spacing: Style.marginL

							Text {
								objectName: "RemoveParentRoleLink"
								anchors.verticalCenter: parent.verticalCenter
								text: qsTr("Remove") + " (" + parentRolesTable.selectedCount + ")"
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: parentRolesTable.selectedCount > 0 ? Style.linkColor : Style.inactiveTextColor

								MouseArea {
									objectName: "MouseArea"
									anchors.fill: parent
									enabled: parentRolesTable.selectedCount > 0
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: {
										parentRolesTable.removeSelected()
									}
								}
							}

							Text {
								id: parentRolesTableAddLink
								objectName: "AddParentRoleLink"
								anchors.verticalCenter: parent.verticalCenter
								text: "+ " + qsTr("Add Parent Role")
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: Style.linkColor

								MouseArea {
									objectName: "MouseArea"
									anchors.fill: parent
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: {
										roleSelectableCollectionEditor.items = parentRolesTable.directItems()
										roleSelectableCollectionEditor.openSelector(parentRolesTableAddLink)
									}
								}
							}
						}
					}
				}

				CollectionItemSelectElementView {
					id: roleSelectableCollectionEditor
					width: parent.width
					commandId: ImtauthRolesSdlCommandIds.s_rolesList
					fields: [RoleItemDataTypeMetaInfo.s_id, RoleItemDataTypeMetaInfo.s_roleName]
					titleField: RoleItemDataTypeMetaInfo.s_roleName
					textFilterFieldIds: [RoleItemDataTypeMetaInfo.s_roleName]
					sortByField: RoleItemDataTypeMetaInfo.s_roleName
					label: qsTr("Parent Roles")
					addButtonText: qsTr("Add Parent Role")
					// A role cannot be its own parent, so it is not offered at all.
					excludeIds: container.roleData && container.roleData.m_id
						? [container.roleData.m_id]
						: []
					visible: false

					// The role list is scoped by product, as a header and as an input field.
					function getHeaders(){
						let headers = {}
						headers["productId"] = container.productId
						return headers
					}

					function setCustomInputParams(inputParams){
						if (container.productId){
							inputParams.InsertField(RoleItemInputTypeMetaInfo.s_productId, container.productId)
						}
					}

					onSelectionChanged: {
						parentRolesTable.setDirectItems(roleSelectableCollectionEditor.items)
						container.doUpdateModel()
						container.assignmentsChanged()
					}
				}
			}

			AssignmentsTable {
				id: parentRolesTable
				anchors.top: parentRolesHeader.bottom
				anchors.topMargin: Style.marginM
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				x: parentRolesHeader.x
				width: parentRolesHeader.width
				warnings: container.roleData ? container.roleData.m_accessWarnings : null
				editable: true
				navigationPath: "Administration/Roles/Role/"
				nameTitle: qsTr("Role")
				emptyText: qsTr("The role has no parent roles.")
				filterPlaceholder: qsTr("Filter roles...")


				onRemoved: {
					container.doUpdateModel()
					container.assignmentsChanged()
				}
			}
		}
	}

	Component {
		id: permissionPageComp

		Item {
			id: permissionPage
			anchors.fill: parent

			property alias bottomItem: permissionsTableElementView.bottomItem

			// Deferred: multiPageView.getPageById() needs the Loader to have
			// published itemAt().item, which isn't true yet during this page's own completion.
			Timer {
				id: populatePermissionsTimer
				interval: 0
				repeat: false
				onTriggered: container.__populatePermissionsTree()
			}

			Connections {
				target: permissionsTableElementView

				function onBottomItemChanged() {
					populatePermissionsTimer.restart()
				}
			}

			Component.onCompleted: {
				populatePermissionsTimer.restart();
			}

			Column {
				id: permissionsHeader
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				x: Math.max(0, (permissionPage.width - width) / 2)
				width: Math.max(0, Math.min(Style.contentWidthMax, permissionPage.width - 2 * Style.marginXL))
				spacing: Style.marginM

				GroupHeaderView {
					width: parent.width
					title: qsTr("Permissions") + " (" + (permissionPage.bottomItem ? permissionPage.bottomItem.checkedCount : 0) + ")" + (container.assignmentsUpdating ? "   " + qsTr("Updating...") : "")
					controlComp: Component {
						CheckBox {
							objectName: "SelectedPermissionsOnlyCheckBox"
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Selected only")

							onCheckStateChanged: {
								if (permissionPage.bottomItem){
									permissionPage.bottomItem.selectedOnly = checkState === Qt.Checked
								}
							}
						}
					}
				}
			}

			CustomScrollbar {
				id: scrollbar;
				z: parent.z + 1;
				anchors.right: parent.right;
				anchors.top: flickable.top;
				anchors.bottom: flickable.bottom;
				secondSize: Style.marginM;
				targetItem: flickable;
			}

			Flickable {
				id: flickable;
				anchors.top: permissionsHeader.bottom;
				anchors.topMargin: Style.marginM;
				anchors.bottom: parent.bottom;
				anchors.bottomMargin: Style.marginXL;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginXL;
				anchors.right: scrollbar.left;
				anchors.rightMargin: Style.marginXL;
				contentHeight: bodyColumn.height + Style.marginXL;

				boundsBehavior: Flickable.StopAtBounds;
				clip: true;

				Column {
					id: bodyColumn;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: Math.min(parent.width, Style.contentWidthMax);
					spacing: Style.marginXL;

					GroupElementView {
						id: permissionsGroupElement
						width: parent.width

						ElementView {
							id: permissionsTableElementView
							width: parent.width
							border.width: 0
							color: "transparent"
							radius: 0
							contentMargin: Style.marginL
							contentSpacing: 0
							height: contentHeight
							bottomComp: Component {
								PermissionsTableView {
									id: permissionsTableView
									width: parent.width
									height: preferredHeight
									showControlPanel: true
									showSourceColumn: true
									treeToScrollbarSpacing: 0
									controlPanelTopMargin: Style.marginL
									treeTopMargin: Style.marginL
									treeBottomMargin: Style.marginL

									onSelectionChanged: {
										container.doUpdateModel()
									}
								}
							}
						}
					}
				}
			}
		}
	}

	Component {
		id: historyPageComp

		Item {
			id: historyPage
			anchors.fill: parent

			Item {
				id: centeredContainer
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				anchors.horizontalCenter: parent.horizontalCenter
				width: Math.min(parent.width - Style.marginXL * 2, Style.contentWidthMax)
			}

			GroupHeaderView {
				id: historyHeader
				anchors.left: centeredContainer.left
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.right: centeredContainer.right
				title: qsTr("History") + " (" + historyView.revisionsCount + ")"
			}

			DocumentHistoryView {
				id: historyView
				anchors.left: centeredContainer.left
				anchors.top: historyHeader.bottom
				anchors.topMargin: Style.marginM
				anchors.right: centeredContainer.right
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				documentId: container.roleData ? container.roleData.m_id : "";
				collectionId: "Roles";

				function getHeaders(){
					return container.getHeaders();
				}
			}
		}
	}
}
