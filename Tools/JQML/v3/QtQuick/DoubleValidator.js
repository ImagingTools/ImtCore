const QtObject = require("../QtQml/QtObject")
const String = require("../QtQml/String")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")
const QtFunctions = require("../Qt/functions")

class DoubleValidator extends QtObject {
    static StandardNotation = 0
    static ScientificNotation = 1
    static StandardRegExp = /^(-|\+)?\s*[0-9]+(\.[0-9]+)?$/
    static ScientificRegExp = /^(-|\+)?\s*[0-9]+(\.[0-9]+)?(E(-|\+)?[0-9]+)?$/

    static meta = Object.assign({}, QtObject.meta, {
        bottom: { type: Real, value: -Infinity},
        top: { type: Real, value: Infinity},
        locale: { type: String, value: false},
        decimals: { type: Real, value: 1000},
        notation: { type: Real, value: DoubleValidator.ScientificNotation},

        bottomChanged: { type:Signal, args:[] },
        topChanged: { type:Signal, args:[] },
        localeChanged: { type:Signal, args:[] },
        decimalsChanged: { type:Signal, args:[] },
        notationChanged: { type:Signal, args:[] },
    })

    getRegExpForNotation() {
        switch (this.notation) {
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
        if(!str) return false

        let locale = this.locale.replaceAll('_', '-')
        let decimalPoint = QtFunctions.locale().decimalPoint

        let regexp = new RegExp(`^-*[0-9]*[${decimalPoint}]*[0-9]*$`)
        if(!regexp.test(str)) return false

        let value = Number(str.replaceAll(decimalPoint,'.'))
        if(isNaN(value)) return false

        let decimals = str.indexOf(decimalPoint) >= 0 ? str.split(decimalPoint)[1] : []

        return (this.bottom <= value && top >= value) && decimals.length <= decimals
    }

    hasPartialMatch(str){
        if(!str) return true

        let locale = this.locale.replaceAll('_', '-')
        let decimalPoint = QtFunctions.locale().decimalPoint

        if(this.bottom < 0 || top < 0){
            if(str === '-') return true
        }

        let regexp = new RegExp(`^-*[0-9]*[${decimalPoint}]*[0-9]*$`)
        if(!regexp.test(str)) return false

        let value = Number(str.replaceAll(decimalPoint,'.'))
        if(isNaN(value)) return false
        let decimals = str.indexOf(decimalPoint) >= 0 ? str.split(decimalPoint)[1] : []
        return decimals.length <= decimals
    }
}



module.exports = DoubleValidator