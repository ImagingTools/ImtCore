const { QtObject } = require('./QtObject')
const { QReal, QString } = require('../utils/properties')

class IntValidator extends QtObject {
    static defaultProperties = {
        bottom: { type: QReal, value: -2147483647 },
        top: { type: QReal, value: 2147483647 },
        locale: { type: QString, value: '' },
    }

    constructor(parent,exCtx,exModel) {
        super(parent,exCtx,exModel)
    }

    validate(str) {
        if(!str) return false

        let value = Number(str)
        if(isNaN(value)) return false

        return (this.getPropertyValue('bottom') <= value && this.getPropertyValue('top') >= value)
    }

    hasPartialMatch(str){
        if(!str) return true

        if(this.getPropertyValue('bottom') < 0 || this.getPropertyValue('top') < 0){
            if(str === '-') return true
        }
        let value = Number(str)
        if(isNaN(value)) return false

        return (this.getPropertyValue('bottom') <= value && this.getPropertyValue('top') >= value) || (str.length - (this.getPropertyValue('top')-1).toString().length === 1)
    }
}

module.exports.IntValidator = IntValidator