const QtObject = require("../QtQml/QtObject")
const String = require("../QtQml/String")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class DoubleValidator extends QtObject {
    static StandardNotation = 0
    static ScientificNotation = 1
    static StandardRegExp = /^(-|\+)?\s*[0-9]+(\.[0-9]+)?$/
    static ScientificRegExp = /^(-|\+)?\s*[0-9]+(\.[0-9]+)?(E(-|\+)?[0-9]+)?$/

    static meta = Object.assign({}, QtObject.meta, {
        bottom: { type: Real, value: -Infinity, signalName: 'bottomChanged' },
        top: { type: Real, value: Infinity, signalName: 'topChanged' },
        locale: { type: String, value: false, signalName: 'localeChanged' },
        decimals: { type: Real, value: 1000, signalName: 'decimalsChanged' },
        notation: { type: Real, value: DoubleValidator.ScientificNotation, signalName: 'notationChanged' },

        bottomChanged: { type:Signal, slotName:'onBottomChanged', args:[] },
        topChanged: { type:Signal, slotName:'onTopChanged', args:[] },
        localeChanged: { type:Signal, slotName:'onLocaleChanged', args:[] },
        decimalsChanged: { type:Signal, slotName:'onDecimalsChanged', args:[] },
        notationChanged: { type:Signal, slotName:'onNotationChanged', args:[] },
    })

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
        if(!str) return false

        let locale = this.getPropertyValue('locale').replaceAll('_', '-')
        let delimiter = this.getPropertyValue('context').getPropertyValue('delimiter')

        let regexp = new RegExp(`^-*[0-9]*[${delimiter}]*[0-9]*$`)
        if(!regexp.test(str)) return false

        let value = Number(str.replaceAll(delimiter,'.'))
        if(isNaN(value)) return false

        let decimals = str.indexOf(delimiter) >= 0 ? str.split(delimiter)[1] : []

        return (this.getPropertyValue('bottom') <= value && this.getPropertyValue('top') >= value) && decimals.length <= this.getPropertyValue('decimals')
    }

    hasPartialMatch(str){
        if(!str) return true

        let locale = this.getPropertyValue('locale').replaceAll('_', '-')
        let delimiter = this.getPropertyValue('context').getPropertyValue('delimiter')

        if(this.getPropertyValue('bottom') < 0 || this.getPropertyValue('top') < 0){
            if(str === '-') return true
        }

        let regexp = new RegExp(`^-*[0-9]*[${delimiter}]*[0-9]*$`)
        if(!regexp.test(str)) return false

        let value = Number(str.replaceAll(delimiter,'.'))
        if(isNaN(value)) return false
        let decimals = str.indexOf(delimiter) >= 0 ? str.split(delimiter)[1] : []
        return decimals.length <= this.getPropertyValue('decimals')
    }
}

DoubleValidator.initialize()

module.exports = DoubleValidator