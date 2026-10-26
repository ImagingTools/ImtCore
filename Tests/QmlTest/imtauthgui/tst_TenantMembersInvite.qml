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
					onRequestSuccess(requestId, [{"id": "u1", "title": "User 1"}, {"id": "u2", "title": "User 2"}, {"id": "u3", "title": "User 3"}, {"id": "u4", "title": "User 4"}, {"id": "u5", "title": "User 5"}])
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

	function openInvitePopup(windowHeight, members){
		let dialogView = createTemporaryObject(dialogViewComp, testCase.parent, windowHeight ? {"height": windowHeight} : {})
		let stateManager = createTemporaryObject(stateManagerComp, testCase, members ? {"pendingMembers": members} : {})
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

	// On a short window the list gives way: the Invite button, the only way to send, stays inside the popup
	function test_inviteButtonStaysReachableOnAShortWindow(){
		let context = openInvitePopup(360)
		// Ticked rows fill the group of the selected users above the list
		for (let i = 0; i < 4; ++i){
			context.popup.handleItemClick(context.popup.getItemId(i), i)
		}
		wait(100)
		compare(context.popup.newSelection.length, 4)

		let button = inviteButton(context.popup)
		verify(button)
		let bottomInPopup = button.mapToItem(context.popup, 0, button.height).y
		let bottomInWindow = button.mapToItem(context.dialogView, 0, button.height).y
		verify(bottomInPopup <= context.popup.height, "In the popup: " + bottomInPopup + " of " + context.popup.height)
		verify(bottomInWindow <= context.dialogView.height, "In the window: " + bottomInWindow + " of " + context.dialogView.height)

		mouseClick(button)
		compare(context.stateManager.invitations.length, 1)
		compare(context.stateManager.invitations[0].length, 4)
	}

	// Keyboard only: the Tab cycle reaches the Invite button after the list, and Return presses it
	function test_keyboardReachesTheInviteButton(){
		let context = openInvitePopup()
		context.popup.handleItemClick(context.popup.getItemId(0), 0)
		wait(100)
		let button = inviteButton(context.popup)
		verify(button.enabled)

		// filter → "Clear all" → selected users → list → Invite
		for (let i = 0; i < 4; ++i){
			keyClick(Qt.Key_Tab)
			wait(50)
		}
		verify(button.activeFocus, "Tab reaches the Invite button")

		keyClick(Qt.Key_Return)
		compare(context.stateManager.invitations.length, 1)
		compare(context.stateManager.invitations[0].length, 1)
	}

	// Tab to the Invite button with one user ticked
	function tabToTheInviteButton(context){
		context.popup.handleItemClick(context.popup.getItemId(0), 0)
		wait(100)
		for (let i = 0; i < 4; ++i){
			keyClick(Qt.Key_Tab)
			wait(50)
		}
		verify(inviteButton(context.popup).activeFocus, "Tab reaches the Invite button")
	}

	// Mouse after keyboard: the hovered row takes over, Return toggles it instead of sending
	function test_hoverAfterTheButtonTogglesTheRow(){
		let context = openInvitePopup()
		tabToTheInviteButton(context)

		let list = findItem(context.popup, function(item){ return item.itemCount !== undefined && item.contentY !== undefined })
		verify(list)
		// The pointer comes from outside the list onto the second row
		mouseMove(list, 20, -10)
		wait(50)
		mouseMove(list, 20, context.popup.itemHeight * 1.5)
		wait(50)
		mouseMove(list, 22, context.popup.itemHeight * 1.5)
		wait(100)
		keyClick(Qt.Key_Return)
		wait(100)

		compare(context.stateManager.invitations.length, 0, "Return toggles the hovered row")
		compare(context.popup.newSelection.length, 2)
	}

	// A click into the filter takes the focus: the next Tab starts the cycle again
	function test_filterClickAfterTheButtonRestartsTheCycle(){
		let context = openInvitePopup()
		tabToTheInviteButton(context)

		let filter = findItem(context.popup, function(item){ return item.placeHolderText !== undefined && item.textInputFocus !== undefined })
		verify(filter)
		mouseClick(filter)
		wait(100)
		keyClick(Qt.Key_Tab)
		wait(50)

		verify(!inviteButton(context.popup).activeFocus)
		verify(context.popup.__internal.clearAllFocused, "Tab from the filter goes to \"Clear all\"")
	}

	// Up from the button goes to the last row, which Return toggles
	function test_upFromTheButtonGoesToTheLastRow(){
		let context = openInvitePopup()
		tabToTheInviteButton(context)

		keyClick(Qt.Key_Up)
		wait(50)

		verify(!inviteButton(context.popup).activeFocus)
		compare(context.popup.focusedIndex, 4)
		keyClick(Qt.Key_Return)
		wait(100)
		compare(context.stateManager.invitations.length, 0)
		compare(context.popup.newSelection.length, 2)
	}
}