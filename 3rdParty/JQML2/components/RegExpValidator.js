const { QtObject } = require('./QtObject')
const { QVar } = require('../utils/properties')

class RegExpValidator extends QtObject {
    static defaultProperties = {
        regularExpression: { type: QVar, value: undefined },
        regExp: { type: QVar, value: undefined },
    }

    constructor(args) {
        super(args)
    }

    validate(str){
        if(this.getPropertyValue('regularExpression')){
            return this.getPropertyValue('regularExpression').test(str)
        } else if(this.getPropertyValue('regExp')){
            return this.getPropertyValue('regExp').test(str)
        } else {
            return true
        }
    }
}

module.exports.RegExpValidator = RegExpValidator