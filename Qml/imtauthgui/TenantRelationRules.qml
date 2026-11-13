// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12

/**
 * What the current user may do with an organization of the list, by the relation the server reports for it:
 * "Owner", "Creator", "Member", "Invited", "Delegated", or none (e.g. the superuser seeing every organization).
 */
QtObject {
	id: rules

	// The server takes changes of an organization from its owner, creator and members only
	function isEditable(scope) {
		return scope === "Owner" || scope === "Creator" || scope === "Member"
	}

	// An organization is entered by a relation: an invitation is to be accepted first
	function canSwitch(scope, isCurrent) {
		return !isCurrent && scope !== "" && scope !== "Invited"
	}

	function canLeave(scope) {
		return scope === "Member"
	}
}
