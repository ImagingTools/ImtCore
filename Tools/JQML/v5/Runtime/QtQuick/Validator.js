const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

const INVALID = 0, INTERMEDIATE = 1, ACCEPTABLE = 2

// QValidator::changed: inputs using the validator recheck acceptableInput
class Validator extends QtObject {
    $write(id, value){
        super.$write(id, value)
        if(this.$inputs) for(let input of this.$inputs) input.$updateAcceptable()
    }

    $validate(){ return ACCEPTABLE }
    $fixup(text){ return text }
}
defineType(Validator, { props: { locale: 'string' } })

function numDigits(n){
    n = Math.abs(n)
    let digits = 1
    while(n >= 10){
        n = Math.floor(n / 10)
        digits++
    }
    return digits
}

const INTEGER = 0, DOUBLE_STANDARD = 1, DOUBLE_SCIENTIFIC = 2

// QLocaleData::validateChars in the C locale: the characters of a number in their places; buff is the number
// without group separators. Anything else (a space, a letter) is Invalid; a number that ends in a group separator,
// a sign or the exponent is Intermediate
function validateChars(str, mode, decDigits){
    let buff = ''
    let state = 'whole'
    let scientific = mode === DOUBLE_SCIENTIFIC
    let last = ''
    for(let ch of String(str)){
        let c = ch === 'E' ? 'e' : ch
        if(c >= '0' && c <= '9'){
            if(state === 'fractional' && decDigits-- === 0) return { state: INVALID, buff: '' }
        } else if(c === '.'){
            if(mode === INTEGER || state !== 'whole') return { state: INVALID, buff: '' }
            state = 'fractional'
        } else if(c === '+' || c === '-'){
            if(last !== '' && !(scientific && last === 'e')) return { state: INVALID, buff: '' }
        } else if(c === ','){
            if(state !== 'whole' || !(last >= '0' && last <= '9')) return { state: INVALID, buff: '' }
        } else if(c === 'e'){
            if(!scientific || state === 'exponent') return { state: INVALID, buff: '' }
            state = 'exponent'
        } else {
            return { state: INVALID, buff: '' }
        }
        last = c
        if(c !== ',') buff += c
    }
    let intermediate = last === ',' || last === '-' || last === '+' || last === 'e'
    return { state: intermediate ? INTERMEDIATE : ACCEPTABLE, buff }
}

// QValidator's initialResultCheck: Invalid characters, an empty number, a sign the range excludes
function initialResultCheck(min, max, result){
    if(result.state === INVALID) return INVALID
    if(result.buff === '') return INTERMEDIATE
    let c = result.buff[0]
    if((min >= 0 && c === '-') || (max < 0 && c === '+')) return INVALID
    if(result.state === INTERMEDIATE) return INTERMEDIATE
    return null
}

// QLocaleData::numberToCLocale: group separators only between groups of three digits of the whole part
function groupingValid(str){
    let whole = String(str).replace(/^[+-]/, '').split(/[.e]/i)[0]
    return whole.indexOf(',') < 0 || /^\d{1,3}(,\d{3})+$/.test(whole)
}

// QLocale::toString(double, 'f' / 'e', precision) of the C locale: the exponent has at least two digits;
// precision < 0 is QLocale::FloatingPointShortest
function formatDouble(value, mode, precision){
    if(mode === 'f'){
        if(precision >= 0) return value.toFixed(Math.min(precision, 100))
        let text = String(value)
        if(!/e/i.test(text)) return text
        return value.toLocaleString('en-US', { useGrouping: false, maximumFractionDigits: 20 })
    }
    let text = precision >= 0 ? value.toExponential(Math.min(precision, 100)) : value.toExponential()
    text = text.replace(/e([+-])(\d)$/, 'e$10$2')
    return mode === 'E' ? text.toUpperCase() : text
}

module.exports = { INVALID, INTERMEDIATE, ACCEPTABLE, Validator, numDigits, INTEGER, DOUBLE_STANDARD, DOUBLE_SCIENTIFIC, validateChars, initialResultCheck, groupingValid, formatDouble }
