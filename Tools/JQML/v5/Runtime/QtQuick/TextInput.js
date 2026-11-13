const { defineType, makeReadonly } = require('../Core/type')
const { TextField, common, text } = require('./TextField')
const { ACCEPTABLE } = require('./Validator')

class TextInput extends TextField {
    constructor(parent = null){
        super(parent, false)
    }

    $validateText(text){
        let validator = this.$v[TextInput.VALIDATOR]
        return validator && validator.$validate ? validator.$validate(text) : ACCEPTABLE
    }

    $updateAcceptable(){
        this.$write(TextInput.ACCEPTABLEINPUT, this.$validateText(this.$v[TextInput.TEXT]) === ACCEPTABLE)
    }
}

defineType(TextInput, {
    props: Object.assign({}, common, {
        echoMode: { kind: 'int', effect: text },
        maximumLength: { kind: 'int', initial: 32767, effect: text },
        validator: {
            kind: 'var',
            effect(input, value, old){
                if(old && old.$inputs) old.$inputs.delete(input)
                if(value && typeof value === 'object') (value.$inputs || (value.$inputs = new Set())).add(input)
                input.$updateAcceptable()
            },
        },
        inputMask: 'string',
        acceptableInput: { kind: 'bool', initial: true, readonly: true },
        displayText: { kind: 'string', readonly: true },
    }),
    signals: ['accepted', 'editingFinished', 'textEdited'],
})
TextInput.Normal = 0
TextInput.NoEcho = 1
TextInput.Password = 2
TextInput.PasswordEchoOnEdit = 3

Object.assign(TextInput, { AlignLeft: 0x1, AlignRight: 0x2, AlignHCenter: 0x4, AlignJustify: 0x8, AlignTop: 0x20, AlignBottom: 0x40, AlignVCenter: 0x80 })

// read-only in Qt, writable in the base class
makeReadonly(TextInput, 'implicitWidth')
makeReadonly(TextInput, 'implicitHeight')

module.exports = { TextInput }
