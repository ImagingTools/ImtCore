import QtQuick 2.12
import QtTest 1.12
import imtauthgui 1.0

TestCase {
	id: testCase

	name: "TenantRelationRules"

	TenantRelationRules {
		id: rules
	}

	function test_rules_data(){
		return [
			{tag: "owner", scope: "Owner", editable: true, switchable: true, leavable: false},
			{tag: "creator", scope: "Creator", editable: true, switchable: true, leavable: false},
			{tag: "member", scope: "Member", editable: true, switchable: true, leavable: true},
			{tag: "delegated", scope: "Delegated", editable: false, switchable: true, leavable: false},
			{tag: "invited", scope: "Invited", editable: false, switchable: false, leavable: false},
			{tag: "no relation (superuser)", scope: "", editable: false, switchable: false, leavable: false}
		]
	}

	function test_rules(data){
		compare(rules.isEditable(data.scope), data.editable)
		compare(rules.canSwitch(data.scope, false), data.switchable)
		compare(rules.canLeave(data.scope), data.leavable)
	}

	function test_theCurrentOrganizationIsNotSwitchedTo(){
		verify(!rules.canSwitch("Owner", true))
		verify(!rules.canSwitch("Member", true))
	}
}
