// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

/**
 * ChoiceModel
 *
 * The model of a ComboBox, built from plain JS objects: it holds the choices only, the data stay in the objects.
 * An item without an id is no choice and is skipped; "nothing chosen" is the index -1 of a clearable combo box.
 */
TreeItemModel {
	id: choiceModel

	// The items as [{<idKey>, <nameKey>}]; returns the index of selectedId, -1 when it is not among them
	function fill(items, idKey, nameKey, selectedId) {
		choiceModel.clear()
		var selectedIndex = -1
		var list = items || []
		for (var i = 0; i < list.length; i++) {
			var id = list[i][idKey] ? String(list[i][idKey]) : ""
			if (id === "")
				continue
			var index = choiceModel.insertNewItem()
			choiceModel.setData("id", id, index)
			choiceModel.setData("name", list[i][nameKey] ? String(list[i][nameKey]) : id, index)
			if (id === selectedId)
				selectedIndex = index
		}
		return selectedIndex
	}

	// The id of the choice at the index, empty for none
	function idAt(index) {
		if (index < 0 || index >= choiceModel.getItemsCount())
			return ""
		return String(choiceModel.getData("id", index))
	}

	function nameAt(index) {
		if (index < 0 || index >= choiceModel.getItemsCount())
			return ""
		return String(choiceModel.getData("name", index))
	}

	function indexOfId(id) {
		for (var i = 0; i < choiceModel.getItemsCount(); i++) {
			if (String(choiceModel.getData("id", i)) === id)
				return i
		}
		return -1
	}
}
