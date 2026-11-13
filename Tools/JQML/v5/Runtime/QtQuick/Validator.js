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

module.exports = { INVALID, INTERMEDIATE, ACCEPTABLE, Validator, numDigits }
