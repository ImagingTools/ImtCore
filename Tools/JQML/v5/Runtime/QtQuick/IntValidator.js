const { defineType } = require('../Core/type')
const { ACCEPTABLE, INTERMEDIATE, INVALID, Validator, numDigits } = require('./Validator')

// QIntValidator::validate
class IntValidator extends Validator {
    $validate(input){
        let buff = String(input)
        if(!/^[+-]?\d*$/.test(buff)) return INVALID
        if(buff === '') return INTERMEDIATE
        let b = this.$v[IntValidator.BOTTOM], t = this.$v[IntValidator.TOP]
        let minus = buff[0] === '-', plus = buff[0] === '+'
        if(b >= 0 && minus) return INVALID
        if(t < 0 && plus) return INVALID
        if(buff.length === 1 && (plus || minus)) return INTERMEDIATE
        let entered = Number(buff)
        if(!Number.isSafeInteger(entered)) return INVALID
        if(entered >= b && entered <= t) return ACCEPTABLE
        if(entered >= 0){
            let tLength = t !== 0 ? Math.floor(Math.log10(Math.abs(t))) + 1 : 1
            return entered > t && -entered < b && numDigits(entered) > tLength ? INVALID : INTERMEDIATE
        }
        return entered < b ? INVALID : INTERMEDIATE
    }

    // "007" -> "7"
    $fixup(input){
        let buff = String(input)
        if(!/^[+-]?\d+$/.test(buff)) return input
        let entered = Number(buff)
        return Number.isSafeInteger(entered) ? String(entered) : input
    }
}
defineType(IntValidator, {
    props: {
        bottom: { kind: 'int', initial: -2147483648 },
        top: { kind: 'int', initial: 2147483647 },
    },
})

module.exports = { IntValidator }
