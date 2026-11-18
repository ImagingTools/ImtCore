const { defineType } = require('../Core/type')
const { ACCEPTABLE, INTERMEDIATE, INVALID, Validator, INTEGER, validateChars, initialResultCheck, groupingValid } = require('./Validator')

const INT64_DIGITS = /^[+-]?\d+$/

// QIntValidator::validate (C locale)
class IntValidator extends Validator {
    $validate(input){
        let b = this.$v[IntValidator.BOTTOM], t = this.$v[IntValidator.TOP]
        let result = validateChars(input, INTEGER, -1)
        let initial = initialResultCheck(b, t, result)
        if(initial !== null) return initial
        let buff = result.buff
        // QLocaleData::bytearrayToLongLong: a 64-bit integer
        if(!INT64_DIGITS.test(buff)) return INVALID
        let big = BigInt(buff)
        if(big > 9223372036854775807n || big < -9223372036854775808n) return INVALID
        let entered = Number(big)
        // QLocale::toInt of the input: its grouping must be right
        if(entered >= b && entered <= t) return groupingValid(input) ? ACCEPTABLE : INTERMEDIATE
        if(entered >= 0){
            // the -entered < b condition lets the minus be typed last; up to as many digits as top is Intermediate
            let buffLength = buff.length - (buff[0] === '+' ? 1 : 0)
            let tLength = t !== 0 ? Math.floor(Math.log10(Math.abs(t))) + 1 : 1
            return entered > t && -entered < b && buffLength > tLength ? INVALID : INTERMEDIATE
        }
        return entered < b ? INVALID : INTERMEDIATE
    }

    // QIntValidator::fixup: the number written again, "007" is "7", "1,000" is "1000"
    $fixup(input){
        let result = validateChars(input, INTEGER, -1)
        if(result.state === INVALID || !INT64_DIGITS.test(result.buff)) return input
        let big = BigInt(result.buff)
        if(big > 9223372036854775807n || big < -9223372036854775808n) return input
        return big.toString()
    }
}
defineType(IntValidator, {
    props: {
        bottom: { kind: 'int', initial: -2147483648 },
        top: { kind: 'int', initial: 2147483647 },
    },
})

module.exports = { IntValidator }
