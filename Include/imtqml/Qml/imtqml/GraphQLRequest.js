var Field = function(){
    return {
        id: "",
        value: "",
        objectPtr: undefined
    }  
}

var GqlObject = function(objectId){
    return {
        m_fieldsMap: {},
        m_objectId: objectId,
        m_parentPtr: {},

        IsObject: function(fieldId){
            var retVal = false
            if (this.m_fieldsMap.hasOwnProperty(fieldId)){
                if (this.m_fieldsMap[fieldId].objectPtr !== undefined){
                    retVal = true
                }
            }
            return retVal
        },

        GetFieldIds: function() {
            var keys = []
    
            for (var key in this.m_fieldsMap){
                if (this.m_fieldsMap.hasOwnProperty(key)) {
                    keys.push(key)
                }
            }
            return keys
        },

        GetFieldArgumentObjectPtr: function(fieldId){
            var retVal = ""
    
            if (this.m_fieldsMap.hasOwnProperty(fieldId)){
                retVal = this.m_fieldsMap[fieldId].objectPtr
            }
            return retVal
        },
    
        GetFieldArgumentValue: function(fieldId){
            var retVal = "";
            if (this.m_fieldsMap.hasOwnProperty(fieldId)){
                retVal = this.m_fieldsMap[fieldId]; // .value undefined
            }
            return retVal;
        },
    
        InsertField: function(fieldId){
            if (!this.m_fieldsMap.hasOwnProperty(fieldId)) {
                  this.m_fieldsMap[fieldId] = fieldId
            }
        },
    
        InsertFieldArgument: function(fieldId, value){
            if (this.m_fieldsMap.hasOwnProperty(fieldId)){
                this.m_fieldsMap[fieldId] = value;
            }
        },
    
        InsertFieldObject: function(objectPtr){
            fieldId = objectPtr.m_objectId
            if (!this.m_fieldsMap.hasOwnProperty(fieldId)){
                var newField
                newField.id = fieldId
                newField.objectPtr = objectPtr
    
                this.m_fieldsMap[fieldId] = newField
                objectPtr.m_parentPtr = this
            }
        }
    }  
}

var GqlRequest = function(requestType, commandId){
    return {
        m_requestType: requestType,
        m_commandId: commandId,
        m_fields: [],
        m_params: [],

        AddField: function(field){
            this.m_fields.push(field)
        },

        AddParam:function (param){
            this.m_params.push(param)
        },

        AddObjectFieldPart: function(gqlObject){
            var retVal = ""
            retVal += gqlObject.m_objectId

            var fieldIds = gqlObject.GetFieldIds()

            if (fieldIds.length > 0){
                retVal += " {"

                for (var i = 0; i < fieldIds.length; ++i){
                    var fieldId = fieldIds[i]
                    if (gqlObject.IsObject(fieldId)){
                        retVal += this.AddObjectFieldPart(gqlObject.GetFieldArgumentObjectPtr(fieldId))
                    } else {
                        retVal += fieldId
                    }
                    if (i < fieldIds.length - 1){
                        retVal += " "
                    }
                }

                retVal += "}"
            }

            return retVal
        },

        AddObjectParamPart: function(gqlObject){
            var retVal = ""
            var objectId = gqlObject.m_objectId

            if (objectId.length > 0) {
                retVal += objectId
                retVal += ": {"
            }

            var fieldIds = gqlObject.GetFieldIds()
            for (var i = 0; i < fieldIds.length; ++i) {
                var fieldId = fieldIds[i]

                if (gqlObject.IsObject(fieldId)) {
                    retVal += this.AddObjectParamPart(gqlObject.GetFieldArgumentObjectPtr(fieldId))
                } else {
                    retVal += fieldId
                    retVal += ": "
                    var value = gqlObject.GetFieldArgumentValue(fieldId)
                    var isString = (typeof value === 'string' || value instanceof String)
                    if (isString) {
                        retVal += "\\\""
                    }
                    retVal += value
                    if (isString) {
                        retVal += "\\\""
                    }

                }
                if (i < fieldIds.length - 1) {
                    retVal += ", "
                }
            }

            if (objectId.length > 0) {
                retVal += "}"
            }

            return retVal
        },

        CreateQueryFields: function() {
            var retVal = ""

            for (var i = 0; i < this.m_fields.length; ++i){
                this.object = this.m_fields[i]
                retVal += this.AddObjectFieldPart(this.object)

                if (i < this.m_fields.length - 1){
                    retVal += " "
                }
            }
            return retVal
        },

        CreateQueryParams: function() {
            var retVal = ""

            for (var i = 0; i < this.m_params.length; ++i){
                this.object = this.m_params[i]
                retVal += this.AddObjectParamPart(this.object)

                if (i < this.m_fields - 1){
                    retVal += ", "
                }
            }
            return retVal
        },

        GetQuery: function(){
            var fields = this.CreateQueryFields()
            var params = this.CreateQueryParams()

            var type = this.m_requestType

            if (params.length > 0) {
                params = "(" + params
                params = params + ")"
            }

            var queryData = "{\"query\": \"" + type + " " + this.m_commandId + " {" + this.m_commandId + params + " {" + fields + "}" + "}\"}"

            return queryData
        }
    }  
}

