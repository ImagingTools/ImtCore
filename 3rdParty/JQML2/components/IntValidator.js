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
        let regExp = /^(-|\+)?\s*[0-9]+$/

        if (regExp.test(str.trim())) {
            let value = parseInt(str, 10)
            return this.getPropertyValue('bottom') <= value && this.getPropertyValue('top') >= value
        }
        return false
    }
}

module.exports.IntValidator = IntValidator