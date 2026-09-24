// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imttagTagsSdl 1.0

/*!
	\qmltype TagCatalogProvider
	\inqmlmodule imttaggui
	\brief Loads the tag catalog visible to the current tenant: its own tags and the system tags.

	After \c load() finishes, \c tags holds objects with \c id, \c name, \c color,
	\c description, \c isSystem and \c usageCount, and \c loaded() is emitted.
*/
Item {
	id: tagCatalogProviderRoot

	property string context: ""
	property int maxCount: 1000
	property var tags: []
	property bool isLoading: false

	signal loaded()
	signal failed(string message)

	function load(){
		tagCatalogProviderRoot.isLoading = true

		if (!tagsListInput.hasViewParams()){
			tagsListInput.emplaceViewParams()
		}

		tagsListInput.m_viewParams.m_offset = 0
		tagsListInput.m_viewParams.m_count = tagCatalogProviderRoot.maxCount

		tagsListRequest.send(tagsListInput)
	}

	function findTag(tagId){
		for (var i = 0; i < tagCatalogProviderRoot.tags.length; i++){
			if (tagCatalogProviderRoot.tags[i].id === tagId){
				return tagCatalogProviderRoot.tags[i]
			}
		}

		return null
	}

	function normalizeTags(itemsModel){
		var normalized = []
		if (!itemsModel){
			return normalized
		}

		var count = itemsModel.getItemsCount()
		for (var i = 0; i < count; i++){
			var tagId = itemsModel.getData("m_id", i)
			if (!tagId){
				continue
			}

			var usageCount = itemsModel.getData("m_usageCount", i)

			normalized.push({
				id: String(tagId),
				name: String(itemsModel.getData("m_name", i) || ""),
				color: String(itemsModel.getData("m_color", i) || "ededed"),
				description: String(itemsModel.getData("m_description", i) || ""),
				isSystem: itemsModel.getData("m_isSystem", i) === true,
				usageCount: usageCount ? Number(usageCount) : 0
			})
		}

		return normalized
	}

	TagsListInput {
		id: tagsListInput
	}

	GqlSdlRequestSender {
		id: tagsListRequest
		context: tagCatalogProviderRoot.context
		gqlCommandId: ImttagTagsSdlCommandIds.s_tagsList

		sdlObjectComp: Component {
			TagsListPayload {
				onFinished: {
					tagCatalogProviderRoot.tags = tagCatalogProviderRoot.normalizeTags(m_items)
					tagCatalogProviderRoot.isLoading = false
					tagCatalogProviderRoot.loaded()
				}
			}
		}

		function onError(message, type){
			tagCatalogProviderRoot.isLoading = false
			tagCatalogProviderRoot.failed(message)
		}
	}
}
