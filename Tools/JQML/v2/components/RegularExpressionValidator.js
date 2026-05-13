const { QtObject } = require('./QtObject')
const { QVar } = require('../utils/properties')

class RegularExpressionValidator extends QtObject {
    static defaultProperties = {
        regularExpression: { type: QVar, value: undefined, changed: '$regularExpressionChanged' },
        regExp: { type: QVar, value: undefined, changed: '$regExpChanged' },
    }

    constructor(parent,exCtx,exModel) {
        super(parent,exCtx,exModel)
    }

    $makeRegExp(regExp){
        delete this.$matchRegex
        delete this.$partialMatchRegex

        if(regExp){
            this.$matchRegex = regExp
            this.$partialMatchRegex = this.$matchRegex.toPartialMatchRegex()
        }
    }

    $regularExpressionChanged(){
        this.$makeRegExp(this.getPropertyValue('regularExpression'))
    }

    $regExpChanged(){
        this.$makeRegExp(this.getPropertyValue('regExp'))
    }

    validate(str){
        if(this.$matchRegex){
            let result = this.$matchRegex.exec(str)
            return result ? result[0] === str ? true : false : false
        } else {
            return true
        }
    }

    hasPartialMatch(str){
        if(!str) return true

        if(this.$partialMatchRegex){
            let result = this.$partialMatchRegex.exec(str)
            return result ? result[0] === str ? true : false : false
        } else {
            return true
        }
    }
}

module.exports.RegularExpressionValidator = RegularExpressionValidator