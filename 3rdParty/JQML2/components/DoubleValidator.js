const { QtObject } = require('./QtObject')
const { QReal, QString } = require('../utils/properties')

class DoubleValidator extends QtObject {
    static StandardNotation = 0
    static ScientificNotation = 1
    static StandardRegExp = /^(-|\+)?\s*[0-9]+(\.[0-9]+)?$/
    static ScientificRegExp = /^(-|\+)?\s*[0-9]+(\.[0-9]+)?(E(-|\+)?[0-9]+)?$/

    static defaultProperties = {
        bottom: { type: QReal, value: -Infinity },
        top: { type: QReal, value: Infinity },
        locale: { type: QString, value: '' },
        decimals: { type: QReal, value: 1000 },
        notation: { type: QReal, value: DoubleValidator.ScientificNotation },
    }

    constructor(parent,exCtx,exModel) {
        super(parent,exCtx,exModel)
    }

    getRegExpForNotation() {
        switch (this.getPropertyValue('notation')) {
          case DoubleValidator.ScientificNotation:
            return DoubleValidator.ScientificRegExp
          case DoubleValidator.StandardNotation:
            return DoubleValidator.StandardRegExp
        }
        return null
      }
    getDecimalsForNumber(number) {
        if (Math.round(number) === number) {
            return 0
        }
        let str = number.toString()
        return /\d*$/.exec(str)[0].length
    }
    validate(str) {
        let regExp = this.getRegExpForNotation()
        if (!regExp.test(str.trim())) {
            return false
        }
        let value = parseFloat(str)
        let acceptable = this.getPropertyValue('bottom') <= value && this.getPropertyValue('top') >= value && this.getDecimalsForNumber(value) <= this.getPropertyValue('decimals')
        return acceptable
    }
}

module.exports.DoubleValidator = DoubleValidator