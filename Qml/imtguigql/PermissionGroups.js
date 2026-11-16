// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
.pragma library

// The permission groups of an SDL list as plain objects: {groupId, groupName, entries: [{permissionId, displayName, description, requirements}]},
// the form PermissionsTableView.rebuildFromFlatArray takes.
function toFlatArray(groupsList){
	var result = []
	if (!groupsList){
		return result
	}

	for (var groupIndex = 0; groupIndex < groupsList.count; ++groupIndex){
		var group = groupsList.get(groupIndex).item
		if (!group){
			continue
		}

		var groupObject = {
			"groupId": group.m_groupId ? String(group.m_groupId) : "",
			"groupName": group.m_groupName ? String(group.m_groupName) : "",
			"entries": []
		}

		var entries = group.m_entries
		if (entries){
			for (var entryIndex = 0; entryIndex < entries.count; ++entryIndex){
				var entry = entries.get(entryIndex).item
				if (!entry){
					continue
				}

				groupObject.entries.push({
					"permissionId": entry.m_permissionId ? String(entry.m_permissionId) : "",
					"displayName": entry.m_displayName ? String(entry.m_displayName) : "",
					"description": entry.m_description ? String(entry.m_description) : "",
					"requirements": Array.isArray(entry.m_requirements) ? entry.m_requirements : []
				})
			}
		}

		result.push(groupObject)
	}

	return result
}
