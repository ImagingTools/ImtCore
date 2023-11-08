const { QtObject } = require('./QtObject')
const { QVar } = require('../utils/properties')

class ListElement extends QtObject {
    constructor(parent){
        super(parent)
    }
    
    getStatement(name){
        if(this.$properties[name]){
            return this.$properties[name]
        } else {
            this.$properties[name] = new QVar()
            return this.$properties[name]
        }
    }
    
}

module.exports.ListElement = ListElement