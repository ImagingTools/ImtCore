// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imttagTagsSdl 1.0

/*!
	\qmltype EntityTagsProvider
	\inqmlmodule imttaggui
	\brief Loads the tags of several entities of one type in a single request.

	Load the IDs of the visible collection page with \c load(), then read the tags
	of each row with \c getTags(entityId) after \c loaded().
*/
Item {
	id: entityTagsProviderRoot

	property string context: ""

	//! ID of the taggable collection, e.g. "Devices".
	property string entityType: ""

	property var tagsByEntity: null

	signal loaded()
	signal failed(string message)

	function load(entityIds){
		if (entityTagsProviderRoot.entityType === "" || !entityIds || entityIds.length === 0){
			entityTagsProviderRoot.tagsByEntity = null
			entityTagsProviderRoot.loaded()

			return
		}

		entityTagsInput.m_entityType = entityTagsProviderRoot.entityType
		entityTagsInput.m_entityIds = entityIds

		entityTagsRequest.send(entityTagsInput)
	}

	function getTags(entityId){
		if (!entityTagsProviderRoot.tagsByEntity){
			return []
		}

		var tags = entityTagsProviderRoot.tagsByEntity[String(entityId)]

		return tags ? tags : []
	}

	function getTagIds(entityId){
		var tags = entityTagsProviderRoot.getTags(entityId)
		var tagIds = []
		for (var i = 0; i < tags.length; i++){
			tagIds.push(tags[i].id)
		}

		return tagIds
	}

	function normalizeItems(itemsModel){
		var result = {}
		if (!itemsModel){
			return result
		}

		var count = itemsModel.getItemsCount()
		for (var i = 0; i < count; i++){
			var entityId = String(itemsModel.getData("m_entityId", i) || "")
			var tagsModel = itemsModel.getData("m_tags", i)

			var tags = []
			var tagCount = tagsModel ? tagsModel.getItemsCount() : 0
			for (var j = 0; j < tagCount; j++){
				tags.push({
					id: String(tagsModel.getData("m_id", j) || ""),
					name: String(tagsModel.getData("m_name", j) || ""),
					color: String(tagsModel.getData("m_color", j) || "ededed"),
					description: String(tagsModel.getData("m_description", j) || ""),
					isSystem: tagsModel.getData("m_isSystem", j) === true
				})
			}

			result[entityId] = tags
		}

		return result
	}

	EntityTagsInput {
		id: entityTagsInput
	}

	GqlSdlRequestSender {
		id: entityTagsRequest
		context: entityTagsProviderRoot.context
		gqlCommandId: ImttagTagsSdlCommandIds.s_entityTagsGet

		sdlObjectComp: Component {
			EntityTagsPayload {
				onFinished: {
					entityTagsProviderRoot.tagsByEntity = entityTagsProviderRoot.normalizeItems(m_items)
					entityTagsProviderRoot.loaded()
				}
			}
		}

		function onError(message, type){
			entityTagsProviderRoot.failed(message)
		}
	}
}
