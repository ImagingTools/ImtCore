const { defineType } = require('../Core/type')
const { ACCEPTABLE, INTERMEDIATE, INVALID, Validator, numDigits } = require('./Validator')

// QLocaleData::validateChars for doubles in the C locale; null when a character is not allowed
function doubleChars(input, scientific, decimals){
    let decimalSeen = false, exponentSeen = false, decimalDigits = 0, previous = ''
    for(let i = 0; i < input.length; i++){
        let c = input[i]
        if(c >= '0' && c <= '9'){
            if(decimalSeen && !exponentSeen && decimals >= 0 && ++decimalDigits > decimals) return null
        } else if(c === '.'){
            if(decimalSeen || exponentSeen) return null
            decimalSeen = true
        } else if(c === '+' || c === '-'){
            if(i !== 0 && previous !== 'e') return null
        } else if(c === 'e' || c === 'E'){
            if(!scientific || exponentSeen || i === 0) return null
            exponentSeen = true
            c = 'e'
        } else {
            return null
        }
        previous = c
    }
    return input
}

// QDoubleValidatorPrivate::validateWithLocale
class DoubleValidator extends Validator {
    $validate(input){
        let v = this.$v
        let b = v[DoubleValidator.BOTTOM], t = v[DoubleValidator.TOP]
        let scientific = v[DoubleValidator.NOTATION] === DoubleValidator.ScientificNotation
        let buff = doubleChars(String(input), scientific, v[DoubleValidator.DECIMALS])
        if(buff === null) return INVALID
        if(buff === '') return INTERMEDIATE
        if(b >= 0 && buff[0] === '-') return INVALID
        if(t < 0 && buff[0] === '+') return INVALID
        let ok = /^[+-]?(\d+\.?\d*|\.\d+)([eE][+-]?\d+)?$/.test(buff)
        if(!ok) return INTERMEDIATE
        let i = Number(buff)
        if(i !== i) return INVALID
        if(i >= b && i <= t) return ACCEPTABLE
        if(!scientific){
            let max = Math.max(Math.abs(b), Math.abs(t))
            if(Number.isFinite(max) && max < 9.2e18){
                let n = Math.pow(10, numDigits(Math.trunc(max)))
                if(Math.abs(i) > n - Math.pow(10, -v[DoubleValidator.DECIMALS])) return INVALID
            }
        }
        return INTERMEDIATE
    }
}
defineType(DoubleValidator, {
    props: {
        bottom: { kind: 'real', initial: -Infinity },
        top: { kind: 'real', initial: Infinity },
        decimals: { kind: 'int', initial: 1000 },
        notation: { kind: 'int', initial: 1 },
    },
})
Object.assign(DoubleValidator, { StandardNotation: 0, ScientificNotation: 1 })

module.exports = { DoubleValidator }
