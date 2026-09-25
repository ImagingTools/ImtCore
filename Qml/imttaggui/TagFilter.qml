// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0

/*!
	\qmltype TagFilter
	\inqmlmodule imttaggui
	\brief Puts a tag condition into a CollectionFilter, like GitHub's label search.

	Modes: "any" (label:a,b), "all" (label:a label:b), "exclude" (-label:a,b) and
	"none" (no:label). The collection's SQL delegate must set TaggableEntityType.
*/
QtObject {
	id: tagFilterRoot

	property CollectionFilter collectionFilter: null

	//! Reserved filter field of taggable collections.
	property string fieldId: "Tags"

	property var tagIds: []
	property string mode: "any"

	//! Picked tags as select items ({ id, title, color }), for showing them; may be partial.
	property var tags: []

	readonly property bool isActive: tagFilterRoot.mode === "none" || tagFilterRoot.tagIds.length > 0

	function setFilter(tagIds, mode, beQuiet){
		tagFilterRoot.tagIds = tagIds ? tagIds : []
		tagFilterRoot.mode = mode ? mode : "any"

		tagFilterRoot.apply(beQuiet)
	}

	function setMode(mode, beQuiet){
		tagFilterRoot.setFilter(mode === "none" ? [] : tagFilterRoot.tagIds, mode, beQuiet)
	}

	//! Names of the picked tags, or an empty list when some of them are unknown.
	function getTagNames(){
		var names = []
		for (var i = 0; i < tagFilterRoot.tagIds.length; i++){
			var name = ""
			for (var j = 0; j < tagFilterRoot.tags.length; j++){
				if (String(tagFilterRoot.tags[j].id) === String(tagFilterRoot.tagIds[i])){
					name = String(tagFilterRoot.tags[j].title)
				}
			}

			if (name === ""){
				return []
			}

			names.push(name)
		}

		return names
	}

	function clear(beQuiet){
		tagFilterRoot.tagIds = []
		tagFilterRoot.tags = []
		tagFilterRoot.mode = "any"

		tagFilterRoot.apply(beQuiet)
	}

	function apply(beQuiet){
		if (!tagFilterRoot.collectionFilter){
			return
		}

		var filter = tagFilterRoot.collectionFilter
		filter.removeFilterByFieldId(tagFilterRoot.fieldId)

		if (tagFilterRoot.isActive){
			var operations = [filter.filterOperation.ARRAY_HAS_ANY]
			var values = tagFilterRoot.tagIds

			if (tagFilterRoot.mode === "all"){
				operations = [filter.filterOperation.ARRAY_HAS_ALL]
			}
			else if (tagFilterRoot.mode === "exclude"){
				operations = [filter.filterOperation.ARRAY_HAS_ANY, filter.filterOperation.NOT]
			}
			else if (tagFilterRoot.mode === "none"){
				operations = [filter.filterOperation.ARRAY_IS_EMPTY]
				values = []
			}

			filter.addFieldFilter(filter.createArrayFieldFilter(tagFilterRoot.fieldId, values, filter.valueType.STRING, operations))
		}

		if (!beQuiet){
			filter.filterChanged()
		}
	}
}
