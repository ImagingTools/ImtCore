// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
pragma Singleton
import QtQuick 2.12
import imtnotifygui 1.0

/**
 * NotificationTypeRegistry
 *
 * What the client knows about the notification types. The labels and icons come from the server in the catalog
 * (NotificationCenter loads it, in the language of the client); the product registers the pages of the objects:
 *
 *   NotificationTypeRegistry.registerRoute("SupportTicket", "Tickets/Ticket/{id}")
 *
 * A notification is shown as the message of its type with its values ('Anna is no longer a member of "Test"'),
 * or, for a type without a message, as its object ('Organization "Test"'), with the label of its type below.
 * A type the catalog does not have shows its id; an object type without a route opens nothing.
 */
QtObject {
	id: root

	property var __routes: ({})
	property var __types: ({})
	property var __objectTypes: ({})

	// The language of the catalog: the plural forms of the messages are chosen by it
	property string __languageId: ""

	// Changes with every catalog: bindings that show labels depend on it
	property int typesRevision: 0

	// Page of an object type. "{id}" is replaced by the id of the subject.
	function registerRoute(subjectTypeId, routeTemplate){
		root.__routes[subjectTypeId] = routeTemplate
	}

	// The catalog of the server: the notification types [{"id", "name", "icon", "message"}], the types of their objects
	// [{"id", "name"}] and the language of both
	function setTypes(types, objectTypes, languageId){
		var typesMap = {}
		for (var i = 0; i < types.length; i++){
			typesMap[types[i].id] = types[i]
		}

		var objectTypesMap = {}
		for (var j = 0; j < objectTypes.length; j++){
			objectTypesMap[objectTypes[j].id] = objectTypes[j]
		}

		root.__types = typesMap
		root.__objectTypes = objectTypesMap
		root.__languageId = languageId
		root.typesRevision++
	}

	function getTypeName(typeId){
		var typeInfo = root.__types[String(typeId)]

		return typeInfo && typeInfo.name !== "" ? typeInfo.name : String(typeId)
	}

	function getRoute(item){
		var routeTemplate = root.__routes[String(item.subjectTypeId)]
		if (!routeTemplate || item.subjectId === ""){
			return ""
		}

		return routeTemplate.replace("{id}", item.subjectId)
	}

	// The message of the type with the values of the notification; without a message, or when a value is missing, the
	// object with its type ('Organization "Test"') or the label of the type for a notification without an object
	function getTitle(item){
		var typeInfo = root.__types[String(item.typeId)]
		if (typeInfo && typeInfo.message !== ""){
			var message = NotificationMessageFormat.format(typeInfo.message, root.__getArguments(item), root.__languageId)
			if (message !== null){
				return message
			}
		}

		var title = item.subjectName
		if (item.subjectName === ""){
			title = root.getTypeName(item.typeId)
		}
		else{
			var objectType = root.__objectTypes[String(item.subjectTypeId)]
			if (objectType && objectType.name !== ""){
				title = qsTr("%1 \"%2\"").arg(objectType.name).arg(item.subjectName)
			}
		}

		return item.groupCount > 1 ? title + " (" + item.groupCount + ")" : title
	}

	// The label of the type and who caused the latest events
	function getDetails(item){
		var typeName = root.getTypeName(item.typeId)
		var actorNames = root.__getActorNames(item)

		return actorNames !== "" ? typeName + " · " + actorNames : typeName
	}

	// A list of a notification is an array, or a ListModel when the notification is an entry of a ListModel
	function __getCount(list){
		if (!list){
			return 0
		}

		return list.count !== undefined ? list.count : list.length
	}

	function __getAt(list, index){
		return list.get !== undefined ? list.get(index) : list[index]
	}

	function __getActorNames(item){
		var names = []
		for (var i = 0; i < root.__getCount(item.actors); i++){
			var actor = root.__getAt(item.actors, i)
			if (actor.name !== ""){
				names.push(actor.name)
			}
		}

		return names.join(", ")
	}

	// The values of the message: the standard ones and the arguments of the notification
	function __getArguments(item){
		var argumentsMap = {
			"subject": {"kind": "Text", "value": item.subjectName},
			"actor": {"kind": "Text", "value": root.__getCount(item.actors) > 0 ? root.__getAt(item.actors, 0).name : ""},
			"actors": {"kind": "Text", "value": root.__getActorNames(item)},
			"count": {"kind": "Number", "value": String(item.groupCount)}
		}

		for (var i = 0; i < root.__getCount(item.arguments); i++){
			var argument = root.__getAt(item.arguments, i)
			argumentsMap[argument.id] = {"kind": argument.kind, "value": argument.value}
		}

		return argumentsMap
	}

	function getIcon(item){
		var typeInfo = root.__types[String(item.typeId)]
		if (typeInfo && typeInfo.icon !== ""){
			return typeInfo.icon
		}

		if (item.severity === "Critical" || item.severity === "Error"){
			return "Icons/Error"
		}

		if (item.severity === "Warning"){
			return "Icons/Warning"
		}

		return "Icons/Notification"
	}
}
