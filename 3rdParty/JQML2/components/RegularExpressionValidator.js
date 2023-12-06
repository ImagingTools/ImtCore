const { QtObject } = require('./QtObject')
const { QVar } = require('../utils/properties')

class RegularExpressionValidator extends QtObject {
    static defaultProperties = {
        regularExpression: { type: QVar, value: undefined },
        regExp: { type: QVar, value: undefined },
    }

    constructor(parent,exCtx,exModel) {
        super(parent,exCtx,exModel)
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

module.exports.RegularExpressionValidator = RegularExpressionValidator