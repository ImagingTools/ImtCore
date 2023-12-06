const { QtObject } = require('./QtObject')
const { QVar } = require('../utils/properties')

class ListElement extends QtObject {
    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
    }
    
    getStatement(name){
        if(this.$properties[name]){
            return this.$properties[name]
        } else {
            this.$properties[name] = new QVar()
            return this.$properties[name]
        }
    }

    createProperty(name, type, value){
        if(name in this.$properties){
            this.$properties[name].reset(value)
        } else {
            super.createProperty(name, type, value)
        }
    }
    
}

module.exports.ListElement = ListElement