.pragma library

// Builds the changeSet of BaseClass.modelChanged, see Docs/Development/ModelChangeSet.md
// Kept out of BaseClass: every function of BaseClass makes each SDL object slower to create.

function createChange(path, operation, name, sender){
	return {"path": path, "operation": operation, "name": name, "sender": sender}
}

// Path step of a property: its JSON key, as in the SDL schema.
function getPathKey(object, propertyId){
	let key = object.getJSONKeyForProperty(propertyId)
	if (key === undefined || key === null || key === ""){
		return propertyId
	}

	return key
}

// Reports changes of object at once, or adds them to its open transaction.
function reportChanges(object, changes){
	if (object._internal.isTransaction){
		let changeList = object._internal.changeList
		for (let i = 0; i < changes.length; ++i){
			changeList.push(changes[i])
		}
		object._internal.countChanges++

		return
	}

	object.modelChanged(changes)
}

// The list element found last: edits of many elements of a list go in order, so the next one is found next to it.
// No map keyed by objects: a WeakMap with QObject keys breaks the V4 heap (assert in qv4heap_p.h, Qt 6.8).
let lastListOwner = null
let lastListName = ""
let lastListIndex = -1

function findNearLastElement(object, child){
	if (object !== lastListOwner){
		return -1
	}

	let list = object[lastListName]
	if (list === null || list === undefined || list.getItemsCount === undefined){
		return -1
	}

	for (let delta = 1; delta >= -1; --delta){
		let index = lastListIndex + delta
		if (index >= 0 && index < list.count && list.get(index).item === child){
			return index
		}
	}

	return -1
}

// Property of object holding child directly or as a list element: {"name": "m_items", "index": 2}, index -1 for a direct child.
function findChild(object, child){
	let nearIndex = findNearLastElement(object, child)
	if (nearIndex >= 0){
		lastListIndex = nearIndex

		return {"name": lastListName, "index": nearIndex}
	}

	// A list is a property value itself, never an element: its owner's lists need no scan
	let isList = child.getItemsCount !== undefined
	let keys = object.getProperties()
	for (let i = 0; i < keys.length; ++i){
		let value = object[keys[i]]
		if (value === null || value === undefined || typeof value !== "object"){
			continue
		}

		if (value === child){
			return {"name": keys[i], "index": -1}
		}

		if (!isList && value.getItemsCount !== undefined){
			for (let k = 0; k < value.count; ++k){
				if (value.get(k).item === child){
					lastListOwner = object
					lastListName = keys[i]
					lastListIndex = k

					return {"name": keys[i], "index": k}
				}
			}
		}
	}

	return null
}

// The changes of child with the path of child in object in front; the path is null when child is not found.
function prependChildPath(object, child, changeSet){
	let location = findChild(object, child)
	let step = null
	if (location !== null){
		step = location.index < 0 ? [getPathKey(object, location.name)] : [getPathKey(object, location.name), location.index]
	}

	let result = []
	for (let i = 0; i < changeSet.length; ++i){
		let change = changeSet[i]
		let path = step !== null && change.path ? step.concat(change.path) : null
		result.push(createChange(path, change.operation ? change.operation : "set", change.name, change.sender))
	}

	return result
}

// Change of a list of object: operation "insert", "remove" or "set" at the element index.
function createListChange(object, list, operation, index){
	let location = findChild(object, list)
	if (location === null || location.index >= 0){
		return createChange(null, operation, "", list)
	}

	return createChange([getPathKey(object, location.name), index], operation, location.name, list)
}
