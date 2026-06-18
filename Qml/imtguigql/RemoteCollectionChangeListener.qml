import QtQuick 2.12
import imtguigql 1.0

/*!
	\qmltype RemoteCollectionChangeListener
	\inqmlmodule imtguigql
	\brief Parses collection change subscriptions and exposes typed change signals.

	RemoteCollectionChangeListener listens to the collection change subscription
	for a specific \c collectionId and converts raw WebSocket payloads into a
	structured change object.

	\sa SubscriptionClient
	\sa RemoteCollectionView
*/
SubscriptionClient {
	id: root

	/*! \qmlproperty string collectionId
	Target collection identifier. The subscription command is derived from it.
	*/
	property string collectionId: ""

	/*! \qmlproperty string currentUserId
	Current user identifier used to classify changes as local or remote.
	*/
	property string currentUserId: ""

	/*! \qmlsignal changed(var changeInfo, var rawData)
	Emitted for every incoming message after it has been parsed.

	\a changeInfo is the parsed change object. It contains:
	\c valid, \c collectionId, \c operation, \c itemIds, \c itemId,
	\c ownerId, \c ownerName, \c isLocalChange, and \c isRemoteChange.

	\c itemIds is the canonical list of affected identifiers.
	\c itemId is a convenience alias and is set only when exactly one identifier
	is affected, otherwise it is an empty string.

	\a rawData is the original server payload that was received by
	SubscriptionClient before parsing.
	*/
	signal changed(var changeInfo, var rawData)

	/*! \qmlsignal inserted(var changeInfo, var rawData)
	Emitted when the server reports an inserted item.

	\a changeInfo contains the parsed insert event.
	\a rawData contains the unmodified incoming payload.
	*/
	signal inserted(var changeInfo, var rawData)

	/*! \qmlsignal updated(var changeInfo, var rawData)
	Emitted when the server reports an updated item.

	\a changeInfo contains the parsed update event.
	\a rawData contains the unmodified incoming payload.
	*/
	signal updated(var changeInfo, var rawData)

	/*! \qmlsignal removed(var changeInfo, var rawData)
	Emitted when the server reports a removed item.

	\a changeInfo contains the parsed remove event.
	Use \c itemIds as the source of truth for affected identifiers.
	\c itemId is filled only for single-item events.

	\a rawData contains the unmodified incoming payload.
	*/
	signal removed(var changeInfo, var rawData)

	/*! \qmlsignal localChanged(var changeInfo, var rawData)
	Emitted when the change was performed by the current user.

	\a changeInfo contains the parsed event with \c isLocalChange set to true.
	\a rawData contains the unmodified incoming payload.
	*/
	signal localChanged(var changeInfo, var rawData)

	/*! \qmlsignal remoteChanged(var changeInfo, var rawData)
	Emitted when the change was performed by another user.

	\a changeInfo contains the parsed event with \c isRemoteChange set to true.
	\a rawData contains the unmodified incoming payload.
	*/
	signal remoteChanged(var changeInfo, var rawData)

	/*! \qmlsignal parseError(var changeInfo, var rawData)
	Emitted when the payload cannot be interpreted as a valid collection change.

	\a changeInfo contains the partial parse result. In this case
	\c valid is false and \c operation is empty or unknown.

	\a rawData contains the unmodified incoming payload.
	*/
	signal parseError(var changeInfo, var rawData)

	/*! \qmlsignal unknownOperation(var changeInfo, var rawData)
	Emitted when the payload is valid but the operation type is unknown.

	\a changeInfo contains the parsed payload with \c valid set to false for
	unsupported operation types.

	\a rawData contains the unmodified incoming payload.
	*/
	signal unknownOperation(var changeInfo, var rawData)

	gqlCommandId: collectionId !== "" ? "On" + collectionId + "CollectionChanged" : ""

	onMessageReceived: {
		console.log("RemoteCollectionChangeListener onMessageReceived", (data.toJson()))
		
		var changeInfo = _parse(data)

		console.log("RemoteCollectionChangeListener changeInfo", changeInfo)
		root.changed(changeInfo, data)

		if (!changeInfo.valid) {
			root.parseError(changeInfo, data)
			return
		}

		if (changeInfo.isRemoteChange)
			root.remoteChanged(changeInfo, data)
		else
			root.localChanged(changeInfo, data)

		if (changeInfo.operation === "inserted")
			root.inserted(changeInfo, data)
		else if (changeInfo.operation === "updated")
			root.updated(changeInfo, data)
		else if (changeInfo.operation === "removed")
			root.removed(changeInfo, data)
		else
			root.unknownOperation(changeInfo, data)
	}

	/*! \qmlmethod var _parse(var dataModel)
	Internal parser that converts the raw payload into a changeInfo object.
	*/
	function _parse(dataModel) {
		var info = {
			"valid": false,
			"collectionId": root.collectionId,
			"operation": "",
			"itemIds": [],
			"itemId": "",
			"ownerId": "",
			"ownerName": "",
			"isLocalChange": true,
			"isRemoteChange": false
		}

		if (!dataModel)
			return info

		info.operation = _str(dataModel, "typeOperation")
		info.itemIds = _normalizeItemIds(
			_str(dataModel, "itemId"),
			_strArray(dataModel, "itemIds"))
		info.itemId = info.itemIds.length === 1 ? info.itemIds[0] : ""

		var ctx = _field(dataModel, "operationContext")
		if (ctx) {
			info.ownerId = _str(ctx, "ownerId")
			info.ownerName = _str(ctx, "ownerName")
		}

		if (info.operation !== "inserted"
				&& info.operation !== "updated"
				&& info.operation !== "removed")
			return info

		info.valid = true

		if (info.ownerId !== "" && root.currentUserId !== "")
			info.isLocalChange = info.ownerId === root.currentUserId

		info.isRemoteChange = !info.isLocalChange
		return info
	}

	/*! \qmlmethod var _field(var model, string key)
	Returns a field from either a plain JavaScript object or a Qt data model.
	*/
	function _field(model, key) {
		if (!model)
			return undefined
		if (model.containsKey)
			return model.containsKey(key) ? model.getData(key) : undefined
		return model[key]
	}

	/*! \qmlmethod string _str(var model, string key)
	Returns a field value as a string or an empty string when missing.
	*/
	function _str(model, key) {
		var val = _field(model, key)
		return val !== undefined && val !== null ? String(val) : ""
	}

	/*! \qmlmethod var _strArray(var model, string key)
	Returns a field value as a string array.
	Handles both plain JS arrays and Qt data models (TreeItemModel).
	*/
	function _strArray(model, key) {
		var val = _field(model, key)
		if (!val)
			return []

		var result = []

		if (val.getItemsCount) {
			var count = val.getItemsCount()
			for (var j = 0; j < count; j++)
				result.push(String(val.getData("", j)))
			return result
		}

		if (val.length !== undefined) {
			for (var i = 0; i < val.length; i++)
				result.push(String(val[i]))
		}

		return result
	}

	/*! \qmlmethod var _normalizeItemIds(string singleId, var ids)
	Builds a canonical unique list of affected identifiers.
	*/
	function _normalizeItemIds(singleId, ids) {
		var result = []

		if (singleId !== "")
			result.push(singleId)

		for (var i = 0; i < ids.length; i++) {
			var candidate = String(ids[i])
			if (candidate === "")
				continue
			if (result.indexOf(candidate) >= 0)
				continue
			result.push(candidate)
		}

		return result
	}
}
