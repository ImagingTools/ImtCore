function GqlIsObject(value){
	return value !== null && typeof value === "object" && value.m_isGqlObject === true
}


function GqlIsPlainObject(value){
	if (value === null || typeof value !== "object"){
		return false
	}

	// Qt QML wraps QObjects into objects with the default prototype
	if (typeof Qt !== "undefined" && Qt.isQtObject && Qt.isQtObject(value)){
		return false
	}

	let prototype = Object.getPrototypeOf(value)

	return prototype === null || Object.getPrototypeOf(prototype) === null
}


function GqlCreateString(value){
	let retVal = value.replace(/\\/g, "\\\\")
	retVal = retVal.replace(/\"/g, "\\\"")
	retVal = retVal.replace(/\n/g, "\\n")
	retVal = retVal.replace(/\r/g, "\\r")
	retVal = retVal.replace(/\t/g, "\\t")
	retVal = retVal.replace(/\x08/g, "\\b")
	retVal = retVal.replace(/\f/g, "\\f")

	return "\"" + retVal + "\""
}


function GqlCreateArguments(gqlObject){
	let parts = []
	let fieldIds = gqlObject.GetFieldIds()
	for (let i = 0; i < fieldIds.length; ++i){
		let fieldId = fieldIds[i]
		let value = gqlObject.IsObject(fieldId) ? gqlObject.GetFieldArgumentObjectPtr(fieldId) : gqlObject.GetFieldArgumentValue(fieldId)

		parts.push(fieldId + ": " + GqlCreateValue(value, fieldId))
	}

	return parts.join(", ")
}


function GqlCreateValue(value, fieldId){
	if (value === null || value === undefined){
		return "null"
	}

	if (typeof value === "string" || value instanceof String){
		return GqlCreateString(String(value))
	}

	if (typeof value === "number"){
		return isFinite(value) ? String(value) : "null"
	}

	if (typeof value === "boolean"){
		return value ? "true" : "false"
	}

	if (Array.isArray(value)){
		let items = []
		for (let i = 0; i < value.length; ++i){
			items.push(GqlCreateValue(value[i], fieldId))
		}

		return "[" + items.join(", ") + "]"
	}

	if (GqlIsObject(value)){
		return "{" + GqlCreateArguments(value) + "}"
	}

	if (typeof value === "object" && typeof value.toGraphQL === "function"){
		let retVal = value.toGraphQL()
		if (typeof retVal !== "string" || retVal === ""){
			throw new Error("Unable to create GraphQL value of the field '" + fieldId + "'")
		}

		return retVal
	}

	if (GqlIsPlainObject(value)){
		let parts = []
		for (let key in value){
			parts.push(key + ": " + GqlCreateValue(value[key], key))
		}

		return "{" + parts.join(", ") + "}"
	}

	throw new Error("Unsupported GraphQL value of the field '" + fieldId + "'")
}


function GqlCreateSelectionSet(gqlObject){
	let parts = []
	let fieldIds = gqlObject.GetFieldIds()
	for (let i = 0; i < fieldIds.length; ++i){
		let fieldId = fieldIds[i]
		let part = gqlObject.IsObject(fieldId) ? GqlCreateSelection(gqlObject.GetFieldArgumentObjectPtr(fieldId)) : fieldId
		if (part !== ""){
			parts.push(part)
		}
	}

	return parts.join(" ")
}


// An anonymous object is a plain container: only its fields are selected
function GqlCreateSelection(gqlObject){
	let selectionSet = GqlCreateSelectionSet(gqlObject)
	if (gqlObject.m_objectId === ""){
		return selectionSet
	}

	if (selectionSet === ""){
		return gqlObject.m_objectId
	}

	return gqlObject.m_objectId + " {" + selectionSet + "}"
}


var GqlObject = function(objectId){
	return {
		m_isGqlObject: true,
		m_objectId: (objectId === undefined || objectId === null) ? "" : String(objectId),
		m_fieldIds: [],
		m_fields: Object.create(null),
		m_parentPtr: null,

		HasField: function(fieldId){
			return fieldId in this.m_fields
		},

		IsObject: function(fieldId){
			return this.HasField(fieldId) && this.m_fields[fieldId].objectPtr !== null
		},

		GetFieldIds: function(){
			return this.m_fieldIds.slice()
		},

		GetFieldArgumentObjectPtr: function(fieldId){
			return this.HasField(fieldId) ? this.m_fields[fieldId].objectPtr : null
		},

		GetFieldArgumentValue: function(fieldId){
			return this.HasField(fieldId) ? this.m_fields[fieldId].value : null
		},

		// Without a value the field is only selected; as an argument it is null
		InsertField: function(fieldId, value){
			if (!this.HasField(fieldId)){
				this.m_fieldIds.push(fieldId)
				this.m_fields[fieldId] = {value: null, objectPtr: null}
			}

			if (value !== undefined){
				this.m_fields[fieldId] = {value: value, objectPtr: null}
			}
		},

		InsertFieldObject: function(objectPtr){
			let fieldId = objectPtr.m_objectId
			if (!this.HasField(fieldId)){
				this.m_fieldIds.push(fieldId)
			}

			this.m_fields[fieldId] = {value: null, objectPtr: objectPtr}
			objectPtr.m_parentPtr = this
		},

		RemoveField: function(fieldId){
			if (!this.HasField(fieldId)){
				return
			}

			delete this.m_fields[fieldId]
			this.m_fieldIds.splice(this.m_fieldIds.indexOf(fieldId), 1)
		},

		Clear: function(){
			this.m_fieldIds = []
			this.m_fields = Object.create(null)
		},

		// getProperties() is an array in Qt QML and a Set in JQML
		fromObject: function(object){
			for (let key of object.getProperties()){
				let value = object[key]
				if (object.isArrayValueValid && !object.isArrayValueValid(key, value)){
					throw new Error("Invalid array value of the field '" + object.getJSONKeyForProperty(key) + "'")
				}

				this.InsertField(object.getJSONKeyForProperty(key), value === undefined ? null : value)
			}
		},

		fromJson: function(json){
			let fillObject = function(gqlObject, jsonObject){
				for (let key in jsonObject){
					let value = jsonObject[key]
					if (GqlIsPlainObject(value)){
						let objectParam = GqlObject(key)
						fillObject(objectParam, value)
						gqlObject.InsertFieldObject(objectParam)
					}
					else{
						gqlObject.InsertField(key, value)
					}
				}
			}

			fillObject(this, JSON.parse(json))
		}
	}
}


var GqlRequest = function(requestType, commandId){
	return {
		m_requestType: requestType,
		m_commandId: commandId,
		m_fields: [],
		m_params: [],

		SetRequestType: function(requestType){
			this.m_requestType = requestType
		},

		GetRequestType: function(){
			return this.m_requestType
		},

		SetCommandId: function(commandId){
			this.m_commandId = commandId
		},

		GetCommandId: function(){
			return this.m_commandId
		},

		AddField: function(field){
			this.m_fields.push(field)
		},

		AddParam: function(param){
			this.m_params.push(param)
		},

		RemoveField: function(field){
			let index = this.m_fields.indexOf(field)
			if (index >= 0){
				this.m_fields.splice(index, 1)
			}
		},

		RemoveParam: function(param){
			let index = this.m_params.indexOf(param)
			if (index >= 0){
				this.m_params.splice(index, 1)
			}
		},

		Clear: function(){
			this.m_fields = []
			this.m_params = []
		},

		CreateQueryFields: function(){
			let parts = []
			for (let i = 0; i < this.m_fields.length; ++i){
				let part = GqlCreateSelection(this.m_fields[i])
				if (part !== ""){
					parts.push(part)
				}
			}

			return parts.join(" ")
		},

		// An anonymous parameter object contributes its fields as separate arguments
		CreateQueryParams: function(){
			let parts = []
			for (let i = 0; i < this.m_params.length; ++i){
				let param = this.m_params[i]
				if (param.m_objectId === ""){
					let paramArguments = GqlCreateArguments(param)
					if (paramArguments !== ""){
						parts.push(paramArguments)
					}
				}
				else{
					parts.push(param.m_objectId + ": " + GqlCreateValue(param, param.m_objectId))
				}
			}

			return parts.join(", ")
		},

		GetQuery: function(){
			let params = this.CreateQueryParams()
			if (params !== ""){
				params = "(" + params + ")"
			}

			let query = this.m_requestType + " " + this.m_commandId + " {" + this.m_commandId + params + " {" + this.CreateQueryFields() + "}}"

			return JSON.stringify({"query": query})
		}
	}
}


var Gql = {
	GqlObject: GqlObject,
	GqlRequest: GqlRequest
}
