// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imttagTagsSdl 1.0

/*!
	\qmltype EntityTagsEditor
	\inqmlmodule imttaggui
	\brief Changes the tags of entities: add, remove, replace or clear.

	The add/remove/clear calls take several entity IDs, so a collection view can
	apply them to its whole selection. \c changed() is emitted after the server
	accepted the change.
*/
Item {
	id: entityTagsEditorRoot

	property string context: ""

	signal changed(string entityType)
	signal failed(string message)

	function addTags(entityType, entityIds, tagIds){
		addInput.m_entityType = entityType
		addInput.m_entityIds = entityIds
		addInput.m_tagIds = tagIds

		addRequest.send(addInput)
	}

	function removeTags(entityType, entityIds, tagIds){
		removeInput.m_entityType = entityType
		removeInput.m_entityIds = entityIds
		removeInput.m_tagIds = tagIds

		removeRequest.send(removeInput)
	}

	function clearTags(entityType, entityIds){
		clearInput.m_entityType = entityType
		clearInput.m_entityIds = entityIds
		clearInput.m_tagIds = []

		clearRequest.send(clearInput)
	}

	function setTags(entityType, entityId, tagIds){
		setInput.m_entityType = entityType
		setInput.m_entityId = entityId
		setInput.m_tagIds = tagIds

		setRequest.send(setInput)
	}

	EntityTagsChangeInput {
		id: addInput
	}

	EntityTagsChangeInput {
		id: removeInput
	}

	EntityTagsChangeInput {
		id: clearInput
	}

	EntityTagsSetInput {
		id: setInput
	}

	Component {
		id: changedPayloadComp

		EntityTagsChangedPayload {
			onFinished: {
				entityTagsEditorRoot.changed(m_entityType)
			}
		}
	}

	GqlSdlRequestSender {
		id: addRequest
		context: entityTagsEditorRoot.context
		requestType: 1
		gqlCommandId: ImttagTagsSdlCommandIds.s_entityTagsAdd
		sdlObjectComp: changedPayloadComp

		function onError(message, type){
			entityTagsEditorRoot.failed(message)
		}
	}

	GqlSdlRequestSender {
		id: removeRequest
		context: entityTagsEditorRoot.context
		requestType: 1
		gqlCommandId: ImttagTagsSdlCommandIds.s_entityTagRemove
		sdlObjectComp: changedPayloadComp

		function onError(message, type){
			entityTagsEditorRoot.failed(message)
		}
	}

	GqlSdlRequestSender {
		id: clearRequest
		context: entityTagsEditorRoot.context
		requestType: 1
		gqlCommandId: ImttagTagsSdlCommandIds.s_entityTagsClear
		sdlObjectComp: changedPayloadComp

		function onError(message, type){
			entityTagsEditorRoot.failed(message)
		}
	}

	GqlSdlRequestSender {
		id: setRequest
		context: entityTagsEditorRoot.context
		requestType: 1
		gqlCommandId: ImttagTagsSdlCommandIds.s_entityTagsSet
		sdlObjectComp: changedPayloadComp

		function onError(message, type){
			entityTagsEditorRoot.failed(message)
		}
	}
}
