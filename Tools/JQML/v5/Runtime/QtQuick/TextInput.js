const engine = require('../Core/engine')
const { defineType, makeReadonly } = require('../Core/type')
const { QRectF } = require('../Core/ValueTypes')
const { Item } = require('./Item')
const { TextField, common, text, relayout, parseInputMask, maskString, clearString, maskAcceptable } = require('./TextField')
const { measure } = require('./TextLayout')
const { ACCEPTABLE } = require('./Validator')

class TextInput extends TextField {
    constructor(parent = null){
        super(parent, false)
    }

    $validateText(text){
        let validator = this.$v[TextInput.VALIDATOR]
        return validator && validator.$validate ? validator.$validate(text) : ACCEPTABLE
    }

    // QQuickTextInputPrivate::hasAcceptableInput: the validator, then the input mask
    $updateAcceptable(){
        let content = this.$content()
        let acceptable = this.$validateText(content) === ACCEPTABLE && (!this.$mask || maskAcceptable(this.$mask, content))
        this.$write(TextInput.ACCEPTABLEINPUT, acceptable)
    }

    // QQuickTextInputPrivate::internalSetText: laid over the input mask, else cut to the maximum length
    $normalizeText(value){
        let mask = this.$mask
        if(!mask) return value.slice(0, this.$v[TextInput.MAXIMUMLENGTH])
        let content = maskString(mask, 0, value, true, null)
        return content + clearString(mask, content.length, mask.maxLength - content.length)
    }

    // QQuickTextInputPrivate::parseInputMask: the mask sets the maximum length and takes the text again; a mask
    // removed clears the text
    $setInputMask(value){
        let had = !!this.$mask
        let current = this.$content()
        this.$mask = parseInputMask(value)
        if(this.$mask){
            this.$write(TextInput.MAXIMUMLENGTH, this.$mask.maxLength)
            this.$setText(current)
        } else if(had){
            this.$write(TextInput.MAXIMUMLENGTH, 32767)
            this.$setText('')
        }
    }

    // QQuickTextInput::ensureVisible: the text scrolls so that position shows (QQuickTextInputPrivate::ensureVisible)
    ensureVisible(position){
        let display = this.$layoutText()
        let font = this.$layout().font
        let width = Math.max(0, this.$v[Item.WIDTH] - this.$padding('LEFTPADDING') - this.$padding('RIGHTPADDING'))
        let cix = measure(display.slice(0, Math.max(0, position)), font).advance
        let widthUsed = Math.max(measure(display, font).advance, cix)
        let hscroll = this.$scroll().x
        if(widthUsed <= width) hscroll = 0
        else if(cix - hscroll >= width) hscroll = cix - width
        else if(cix - hscroll < 0 && hscroll < widthUsed) hscroll = cix
        else if(widthUsed - hscroll < width) hscroll = widthUsed - width
        else if(width - hscroll > widthUsed) hscroll = width - widthUsed
        this.$hs = hscroll
        if(this.$input) this.$input.scrollLeft = hscroll
        this.$updateCursorRectangle()
    }

    // QQuickTextInput::positionToRectangle: in the text, the padding not counted
    positionToRectangle(pos){
        let rect = super.positionToRectangle(pos)
        return new QRectF(rect.x - this.$padding('LEFTPADDING'), rect.y - this.$padding('TOPPADDING'), rect.width, rect.height)
    }
}

const echoChanged = (input) => {
    input.$updateDisplayText()
    relayout(input)
}

defineType(TextInput, {
    props: Object.assign({}, common, {
        echoMode: { kind: 'int', effect: echoChanged },
        passwordCharacter: { kind: 'string', initial: '●', effect: echoChanged },
        passwordMaskDelay: 'int',
        autoScroll: { kind: 'bool', initial: true },
        // QQuickTextInput::setMaxLength: not with an input mask; the text is cut to it
        maximumLength: {
            kind: 'int',
            initial: 32767,
            effect(input, value){
                if(!input.$mask && input.$v[TextInput.TEXT].length > value) input.$setText(input.$v[TextInput.TEXT])
                engine.markDirty(input, 64)
            },
        },
        validator: {
            kind: 'var',
            effect(input, value, old){
                if(old && old.$inputs) old.$inputs.delete(input)
                if(value && typeof value === 'object') (value.$inputs || (value.$inputs = new Set())).add(input)
                input.$updateAcceptable()
            },
        },
        inputMask: { kind: 'string', effect: (input, value) => input.$setInputMask(value) },
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
Object.assign(TextInput, { CursorBetweenCharacters: 0, CursorOnCharacter: 1, SelectCharacters: 0, SelectWords: 1 })

// read-only in Qt, writable in the base class
makeReadonly(TextInput, 'implicitWidth')
makeReadonly(TextInput, 'implicitHeight')

module.exports = { TextInput, text }
