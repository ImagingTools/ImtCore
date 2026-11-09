// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0

Item {
	id: root

	property string productId: AuthorizationController.productId
	property var groups: []
	property var roles: []
	property var mappings: ({})
	property string selectedGroup: ""
	property var selectedRoles: []
	property string command: ""
	property string requestProductId: ""
	property bool loaded: false
	property bool initialized: false
	property bool updatingRoles: false
	property bool busy: request.state === "Loading"

	Component.onCompleted: {
		root.initialized = true
		root.load()
	}
	onProductIdChanged: {
		if (root.initialized)
			root.load()
	}

	function load() {
		root.loaded = false
		root.selectedGroup = ""
		root.selectedRoles = []
		root.groups = []
		root.roles = []
		root.mappings = ({})
		synchronization.text = ""
		if (root.productId !== "" && AuthorizationController.loggedUserIsSuperuser())
			root.send("LdapGroupSettings", {})
	}

	function send(command, values) {
		if (root.busy || !AuthorizationController.loggedUserIsSuperuser())
			return
		root.command = command
		root.requestProductId = root.productId
		let query = Gql.GqlRequest(command === "SaveLdapGroupSettings" ? "mutation" : "query", command)
		let input = Gql.GqlObject("input")
		input.InsertField("productId", root.productId)
		for (let key in values)
			input.InsertField(key, values[key])
		query.AddParam(input)
		query.AddField(Gql.GqlObject("settings"))
		query.AddField(Gql.GqlObject("ok"))
		let groups = Gql.GqlObject("groups")
		groups.InsertField("id")
		groups.InsertField("name")
		query.AddField(groups)
		let roles = Gql.GqlObject("roles")
		roles.InsertField("id")
		roles.InsertField("name")
		query.AddField(roles)
		query.AddField(Gql.GqlObject("notificationState"))
		query.AddField(Gql.GqlObject("synchronizedAt"))
		query.AddField(Gql.GqlObject("synchronizationError"))
		status.text = ""
		let headers = {}
		headers["productId"] = root.productId
		headers[AuthorizationController.authenticationTokenHeaderId] = AuthorizationController.userTokenProvider.accessToken
		request.setGqlQuery(query.GetQuery(), headers)
	}

	function receive(json) {
		if (root.requestProductId !== root.productId) {
			root.load()
			return
		}
		try {
			let response = JSON.parse(json)
			if (!response.data || !response.data[root.command]) {
				status.text = qsTr("LDAP group request failed. Check directory access and server configuration.")
				return
			}
			let data = response.data[root.command]
			if (root.command === "LdapGroupSettings") {
				let settings = JSON.parse(data.settings)
				enabledSwitch.checkState = settings.enabled ? Qt.Checked : Qt.Unchecked
				pattern.text = settings.pattern
				root.mappings = settings.mappings
				root.roles = data.roles
				root.showGroups(data.groups ? data.groups : [])
				root.showSynchronization(data)
				root.loaded = true
			}
			else if (root.command === "DiscoverLdapGroups") {
				root.showGroups(data.groups)
			}
			else if (data.ok === true) {
				root.showGroups(data.groups ? data.groups : [])
				root.showSynchronization(data)
				status.text = qsTr("Saved. Directory roles are refreshed at the next sign-in.")
			}
			else {
				status.text = qsTr("LDAP group settings were not saved")
			}
		}
		catch (error) {
			status.text = qsTr("Invalid LDAP group response")
		}
	}

	function showGroups(rows) {
		let ids = []
		for (let index = 0; index < rows.length; ++index)
			ids.push(rows[index].id)
		for (let sid in root.mappings) {
			if (ids.indexOf(sid) < 0)
				rows.push({id: sid, name: sid + " " + qsTr("(unavailable; clear its roles before saving)")})
		}
		root.groups = rows
		root.selectGroup("")
	}

	function showSynchronization(data) {
		let text = qsTr("Directory change notifications: ") + data.notificationState
		if (data.synchronizedAt)
			text += ". " + qsTr("Group list synchronized: ") + data.synchronizedAt
		if (data.synchronizationError)
			text += ". " + data.synchronizationError
		synchronization.text = text
	}

	function selectGroup(sid) {
		root.updatingRoles = true
		root.selectedGroup = sid
		root.selectedRoles = root.mappings[sid] ? root.mappings[sid].slice() : []
		root.updatingRoles = false
	}

	function setRole(roleId, assigned) {
		if (root.selectedGroup === "" || root.updatingRoles)
			return
		let roles = root.selectedRoles.slice()
		let index = roles.indexOf(roleId)
		if (assigned && index < 0)
			roles.push(roleId)
		if (!assigned && index >= 0)
			roles.splice(index, 1)
		if (roles.length > 0)
			root.mappings[root.selectedGroup] = roles
		else
			delete root.mappings[root.selectedGroup]
		root.selectedRoles = roles
	}

	function save() {
		let settings = {enabled: enabledSwitch.checkState === Qt.Checked, pattern: pattern.text, mappings: root.mappings}
		root.send("SaveLdapGroupSettings", {settings: JSON.stringify(settings)})
	}

	function clearMappings() {
		root.mappings = ({})
		root.selectGroup(root.selectedGroup)
	}

	GqlRequest {
		id: request
		onStateChanged: {
			if (request.state === "Ready")
				root.receive(request.json)
			else if (request.state === "Error" || request.state === "Unauthorized" || request.state === "Forbidden")
				status.text = qsTr("LDAP group request failed")
		}
	}

	Column {
		id: body
		anchors.fill: root
		anchors.margins: Style.marginM
		spacing: Style.marginM
		enabled: !root.busy && AuthorizationController.loggedUserIsSuperuser()

		CheckBox {
			id: enabledSwitch
			width: body.width
			text: qsTr("Map Active Directory security groups to product roles")
			isActive: root.loaded
		}
		TextField {
			id: pattern
			width: body.width
			height: Style.controlHeightM
			maximumLength: 256
			placeHolderText: qsTr("Group-name pattern, for example Product-* (only * is a wildcard)")
		}
		Row {
			id: buttons
			spacing: Style.marginM
			Button {
				id: discover
				width: 180
				height: Style.controlHeightM
				text: qsTr("Discover groups")
				enabled: root.loaded && pattern.text !== ""
				onClicked: root.send("DiscoverLdapGroups", {pattern: pattern.text})
			}
			Button {
				id: saveButton
				width: 120
				height: Style.controlHeightM
				text: qsTr("Save")
				enabled: root.loaded
				onClicked: root.save()
			}
			Button {
				id: reload
				width: 120
				height: Style.controlHeightM
				text: qsTr("Reload")
				onClicked: root.load()
			}
			Button {
				id: clearButton
				width: 160
				height: Style.controlHeightM
				text: qsTr("Clear mappings")
				enabled: root.loaded
				onClicked: root.clearMappings()
			}
		}
		Text {
			id: synchronization
			width: body.width
			wrapMode: Text.WordWrap
			color: Style.textColor
		}
		Text {
			id: groupTitle
			text: qsTr("Select a directory group")
			color: Style.textColor
		}
		ListView {
			id: groupList
			width: body.width
			height: 160
			clip: true
			model: root.groups
			delegate: Button {
				id: groupButton
				width: groupList.width
				height: Style.controlHeightM
				text: modelData.name
				checked: root.selectedGroup === modelData.id
				onClicked: root.selectGroup(modelData.id)
			}
		}
		Text {
			id: rolesTitle
			text: qsTr("Grant ALL selected product roles")
			color: Style.textColor
		}
		ListView {
			id: roleList
			width: body.width
			height: Math.max(80, root.height - 440)
			clip: true
			model: root.roles
			delegate: CheckBox {
				id: roleBox
				width: roleList.width
				text: modelData.name
				isActive: root.selectedGroup !== ""
				checkState: root.selectedRoles.indexOf(modelData.id) >= 0 ? Qt.Checked : Qt.Unchecked

				function nextCheckState(){
					root.setRole(modelData.id, roleBox.checkState !== Qt.Checked)
				}
			}
		}
		Text {
			id: status
			width: body.width
			wrapMode: Text.WordWrap
			color: Style.textColor
		}
	}
}
