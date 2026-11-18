const { defineType } = require('../Core/type')
const { ACCEPTABLE, INTERMEDIATE, INVALID, Validator, numDigits, DOUBLE_STANDARD, DOUBLE_SCIENTIFIC, validateChars, initialResultCheck, groupingValid, formatDouble } = require('./Validator')

const NUMBER = /^[+-]?(\d+\.?\d*|\.\d+)(e[+-]?\d+)?$/

// QLocale::toDouble of the C locale: a whole number, its grouping right
function toDouble(input, buff){
    if(!NUMBER.test(buff) || !groupingValid(input)) return null
    let value = Number(buff)
    return Number.isFinite(value) ? value : null
}

// QDoubleValidator (C locale): validateWithLocale, fixupWithLocale
class DoubleValidator extends Validator {
    $mode(){
        return this.$v[DoubleValidator.NOTATION] === DoubleValidator.StandardNotation ? DOUBLE_STANDARD : DOUBLE_SCIENTIFIC
    }

    $validate(input){
        let v = this.$v
        let b = v[DoubleValidator.BOTTOM], t = v[DoubleValidator.TOP], dec = v[DoubleValidator.DECIMALS]
        let mode = this.$mode()
        let result = validateChars(input, mode, dec)
        let initial = initialResultCheck(b, t, result)
        if(initial !== null) return initial
        let i = toDouble(input, result.buff)
        if(i === null) return INTERMEDIATE
        if(i >= b && i <= t) return ACCEPTABLE
        if(mode === DOUBLE_STANDARD){
            // with dec decimals at most 9.99 is in reach of a range of 0 - 9.0: 9.999 and 10.0 are Invalid
            let max = Math.max(Math.abs(b), Math.abs(t))
            if(Number.isFinite(max) && Math.floor(max) <= Number.MAX_SAFE_INTEGER){
                let n = Math.pow(10, numDigits(Math.floor(max)))
                if(Math.abs(i) > n - Math.pow(10, -dec)) return INVALID
            }
        }
        return INTERMEDIATE
    }

    // the number written again in the notation, no more decimals than allowed
    $fixup(input){
        let mode = this.$mode()
        let result = validateChars(input, mode, -1)
        if(result.state === INVALID) return input
        let entered = Number(result.buff)
        if(!NUMBER.test(result.buff) || !Number.isFinite(entered)) return input
        let dec = this.$v[DoubleValidator.DECIMALS]
        let format = mode === DOUBLE_STANDARD ? 'f' : String(input).indexOf('E') >= 0 ? 'E' : 'e'
        let precision = -1
        if(dec >= 0){
            let buff = result.buff
            if(format === 'f'){
                let point = buff.indexOf('.')
                precision = point >= 0 ? buff.length - point - 1 : 0
            } else {
                let e = buff.indexOf('e')
                if(e < 0) e = buff.length
                precision = e - (buff.indexOf('.') >= 0 ? 1 : 0) - (buff[0] === '-' || buff[0] === '+' ? 1 : 0)
            }
            precision = Math.min(precision, dec)
        }
        return formatDouble(entered, format, precision)
    }
}
defineType(DoubleValidator, {
    props: {
        bottom: { kind: 'real', initial: -Infinity },
        top: { kind: 'real', initial: Infinity },
        decimals: { kind: 'int', initial: -1 },
        notation: { kind: 'int', initial: 1 },
    },
})
Object.assign(DoubleValidator, { StandardNotation: 0, ScientificNotation: 1 })

module.exports = { DoubleValidator }
