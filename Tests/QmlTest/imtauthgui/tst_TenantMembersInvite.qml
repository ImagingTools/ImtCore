import QtQuick 2.12
import QtTest 1.12
import Acf 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtauthgui 1.0

TestCase {
	id: testCase

	name: "TenantMembersInvite"
	when: windowShown

	// The application installs the button decorator; the test host has none
	property var previousButtonDecorator: null

	Component {
		id: buttonDecoratorComp

		ButtonDecorator {
		}
	}

	function initTestCase(){
		previousButtonDecorator = Style.buttonDecorator
		Style.buttonDecorator = buttonDecoratorComp
	}

	// A failed test must not leave its popup to the next one
	function cleanup(){
		while (ModalDialogManager.count > 0){
			ModalDialogManager.closeDialog()
		}
	}

	function cleanupTestCase(){
		Style.buttonDecorator = previousButtonDecorator
	}

	Component {
		id: stateManagerComp

		QtObject {
			property bool canInviteOrganizationMember: true
			property bool canExcludeOrganizationMember: false
			property bool canChangeOrganizationMember: false
			property bool canChangeOrganizationMemberRole: false
			property bool canManageOrganizationMembers: true
			property var pendingMembers: [{"id": "member", "name": "Member"}]
			property var pendingInvitations: []
			property bool __membersModifiedLocally: false
			property var invitations: []

			function hasAnyPermission(ids){ return true }
			function hasPermission(id){ return true }
			function scopeHolds(id){ return true }
			function isOwner(userId){ return false }
			function isInvitationExpired(invitation){ return false }
			function formatInvitationInfo(invitedByName, expiresAt){ return "" }
			function loadMembersFromModel(){}
			function requestMembersRefresh(){}
			function inviteSelectedUsers(selectedItems){ invitations.push(selectedItems) }
		}
	}

	Component {
		id: apiClientComp

		QtObject {
			property FilterableSelectDataProvider invitableUsersListDataProvider: FilterableSelectDataProvider {
				multiSelect: true

				function executeRequest(requestId, count, offset, filter){
					onRequestSuccess(requestId, [{"id": "u1", "title": "User 1"}, {"id": "u2", "title": "User 2"}])
				}

				function executeFetchByIds(requestId, ids){
					onFetchByIdsSuccess(requestId, [])
				}
			}
		}
	}

	Component {
		id: dialogViewComp

		DialogManagerView {
			width: 800
			height: 600
		}
	}

	Component {
		id: pageComp

		TenantMembersPage {
			width: 800
			height: 600
		}
	}

	function findItem(item, check){
		if (check(item)){
			return item
		}

		for (let i = 0; i < item.children.length; ++i){
			let found = findItem(item.children[i], check)
			if (found){
				return found
			}
		}

		return null
	}

	function openInvitePopup(){
		let dialogView = createTemporaryObject(dialogViewComp, testCase.parent)
		let stateManager = createTemporaryObject(stateManagerComp, testCase)
		let page = createTemporaryObject(pageComp, testCase.parent, {"stateManager": stateManager, "apiClient": createTemporaryObject(apiClientComp, testCase)})
		wait(100)

		let inviteLink = findItem(page, function(item){ return item.text === "+ Create Invitation" && item.visible })
		verify(inviteLink, "The invite link is shown")
		mouseClick(inviteLink)

		let popup = null
		tryVerify(function(){
			popup = findItem(dialogView, function(item){ return item.newSelection !== undefined && item.closePopup !== undefined })
			return popup !== null
		}, 3000, "The invite popup opens")
		wait(100)

		return {"dialogView": dialogView, "stateManager": stateManager, "popup": popup}
	}

	function inviteButton(popup){
		return findItem(popup, function(item){ return item.text !== undefined && String(item.text).indexOf("Invite (") === 0 })
	}

	// Ticking users sends nothing; the Invite button sends the new ones once
	function test_inviteButtonSendsTheTickedUsers(){
		let context = openInvitePopup()

		context.popup.selectionChanged(["member", "u1"])
		context.popup.selectionChanged(["member", "u1", "u2"])

		compare(context.stateManager.invitations.length, 0, "A tick does not invite")
		let button = inviteButton(context.popup)
		verify(button)
		compare(button.text, "Invite (2)", "Members already in the organization are not counted")

		mouseClick(button)

		compare(context.stateManager.invitations.length, 1)
		compare(context.stateManager.invitations[0].length, 2)
		compare(context.stateManager.invitations[0][0].id, "u1")
		compare(context.stateManager.invitations[0][1].id, "u2")
		tryCompare(ModalDialogManager, "count", 0, 3000, "The popup closes")
	}

	function test_closedPopupInvitesNobody(){
		let context = openInvitePopup()

		context.popup.selectionChanged(["member", "u1"])
		context.popup.closePopup()

		tryCompare(ModalDialogManager, "count", 0, 3000)
		compare(context.stateManager.invitations.length, 0)
	}

	function test_inviteButtonWaitsForASelection(){
		let context = openInvitePopup()

		let button = inviteButton(context.popup)
		verify(button)
		compare(button.text, "Invite (0)")
		verify(!button.enabled)

		context.popup.closePopup()
		tryCompare(ModalDialogManager, "count", 0, 3000)
	}
}
