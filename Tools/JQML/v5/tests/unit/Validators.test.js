// Run: node --test tests/unit
// IntValidator, DoubleValidator, RegularExpressionValidator as QIntValidator, QDoubleValidator and
// QRegularExpressionValidator (qtbase 6.8, the C locale: QLocaleData::validateChars), and a validator together with
// an input mask in TextInput: the validator sees the text with the blanks of the mask
// (QQuickTextInputPrivate::finishChange), an Invalid edit is undone, acceptableInput needs both, Return fixes up.
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

const INVALID = 0, INTERMEDIATE = 1, ACCEPTABLE = 2
const Key_Return = 'Enter'  // QtEnums.Key_Return of the runtime

function cases(name, runtime){
    const { engine, TextInput, types } = runtime
    const { IntValidator, DoubleValidator, RegularExpressionValidator } = types

    function make(Type, setup){
        let item
        engine.batch(() => {
            item = new Type(null)
            if(setup) setup(item)
        })
        return item
    }

    function states(validator, inputs){
        let out = {}
        for(let input of inputs) out[input] = validator.$validate(input)
        return out
    }

    test(name + ': IntValidator: signs, digits typed beyond top, leading zeros, group separators, spaces', () => {
        let v = make(IntValidator, (it) => { it.bottom = 0; it.top = 150 })
        assert.deepEqual(states(v, ['', '12', '+', '-1', '200', '0200', '1500', 'a', '1 ', '1,0', '12,', '007']), {
            '': INTERMEDIATE, '12': ACCEPTABLE, '+': INTERMEDIATE, '-1': INVALID, '200': INTERMEDIATE,
            '0200': INVALID, '1500': INVALID, 'a': INVALID, '1 ': INVALID, '1,0': INTERMEDIATE, '12,': INTERMEDIATE, '007': ACCEPTABLE,
        })
        let wide = make(IntValidator, (it) => { it.top = 5000 })
        assert.equal(wide.$validate('1,000'), ACCEPTABLE)
        assert.equal(wide.$validate('-'), INTERMEDIATE)
        assert.equal(v.$fixup('007'), '7')
        assert.equal(wide.$fixup('1,000'), '1000')
    })

    test(name + ': DoubleValidator: decimals, the reach of the range, notation, -1 decimals by default', () => {
        let v = make(DoubleValidator, (it) => { it.bottom = 0; it.top = 9; it.decimals = 2; it.notation = DoubleValidator.StandardNotation })
        assert.deepEqual(states(v, ['5.5', '9.99', '9.999', '10.0', '5,5', '1e2', '.', '']), {
            '5.5': ACCEPTABLE, '9.99': INTERMEDIATE, '9.999': INVALID, '10.0': INVALID, '5,5': INTERMEDIATE, '1e2': INVALID, '.': INTERMEDIATE, '': INTERMEDIATE,
        })
        let scientific = make(DoubleValidator)
        assert.equal(scientific.decimals, -1)
        assert.equal(scientific.notation, DoubleValidator.ScientificNotation)
        assert.deepEqual(states(scientific, ['1e', 'e5', '1.5e+3', '1e2e3']), { '1e': INTERMEDIATE, 'e5': INTERMEDIATE, '1.5e+3': ACCEPTABLE, '1e2e3': INVALID })
        assert.equal(scientific.$fixup('150'), '1.5e+02')
        assert.equal(v.$fixup('3.14159'), '3.14')
    })

    test(name + ': RegularExpressionValidator: a full match, a prefix, anything else', () => {
        let v = make(RegularExpressionValidator, (it) => { it.regularExpression = /\d{3}-\d{2}/ })
        assert.deepEqual(states(v, ['123-45', '12', '123-', '', '12a', '123-456']), {
            '123-45': ACCEPTABLE, '12': INTERMEDIATE, '123-': INTERMEDIATE, '': INTERMEDIATE, '12a': INVALID, '123-456': INVALID,
        })
    })

    test(name + ': a validator with an input mask sees the blanks: an IntValidator rejects every edit of "000"', () => {
        let input = make(TextInput, (it) => {
            it.inputMask = '000'
            it.validator = make(IntValidator, (v) => { v.bottom = 0; v.top = 150 })
        })
        engine.batch(() => { input.cursorPosition = 0 })
        engine.batch(() => input.insert(0, '1'))
        assert.equal(input.text, '')
        assert.equal(input.acceptableInput, false)
    })

    test(name + ': a validator that takes the blank character works with the mask; acceptableInput needs both', () => {
        let input = make(TextInput, (it) => {
            it.inputMask = '999;_'
            it.validator = make(RegularExpressionValidator, (v) => { v.regularExpression = /[0-4_][0-9_]*/ })
        })
        engine.batch(() => input.$userEdit(0, 0, '7', 'insert'))
        assert.equal(input.displayText, '___')
        engine.batch(() => input.$userEdit(0, 0, '3', 'insert'))
        assert.equal(input.displayText, '3__')
        assert.equal(input.acceptableInput, false)
        engine.batch(() => input.$userEdit(1, 1, '12', 'insert'))
        assert.equal(input.text, '312')
        assert.equal(input.acceptableInput, true)
    })

    test(name + ': a text set in code is kept even when the validator rejects it, acceptableInput is false', () => {
        let input = make(TextInput, (it) => {
            it.validator = make(IntValidator, (v) => { v.bottom = 0; v.top = 10 })
            it.text = 'abc'
        })
        assert.equal(input.text, 'abc')
        assert.equal(input.acceptableInput, false)
    })

    test(name + ': Return fixes up an Intermediate text the validator can fix, then accepted is emitted', () => {
        let input = make(TextInput, (it) => {
            it.validator = make(IntValidator, (v) => { v.bottom = 0; v.top = 5000 })
            it.text = '1,00'
        })
        assert.equal(input.acceptableInput, false)
        let accepted = 0
        engine.connect(input, TextInput.SIGNAL_accepted, () => accepted++)
        engine.batch(() => input.$keyEvent({ key: Key_Return, modifiers: 0, text: '' }, 'pressed'))
        assert.equal(input.text, '100')
        assert.equal(input.acceptableInput, true)
        assert.equal(accepted, 1)
    })

    test(name + ': changing the range of a validator updates acceptableInput', () => {
        let validator
        let input = make(TextInput, (it) => {
            validator = make(IntValidator, (v) => { v.bottom = 0; v.top = 10 })
            it.validator = validator
            it.text = '50'
        })
        assert.equal(input.acceptableInput, false)
        engine.batch(() => { validator.top = 100 })
        assert.equal(input.acceptableInput, true)
    })
}

for(let [name, load] of runtimes) cases(name, load())
