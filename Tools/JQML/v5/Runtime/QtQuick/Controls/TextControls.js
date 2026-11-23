// TextField and TextArea of QtQuick.Templates (qquicktextfield.cpp, qquicktextarea.cpp, Qt 6.12): a TextInput / TextEdit
// with the font of the controls above it, a background sized to it less its insets, a placeholder text, hover and the
// reason of its focus. Its implicit size is the style's when the style gives one (Basic: the background's 200 x 40),
// else the text's as a TextInput's.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Item } = require('../Item')
const { TextInput } = require('../TextInput')
const { TextEdit } = require('../TextEdit')
const { installFont, resolveFont } = require('./Control')

const INSETS = ['topInset', 'leftInset', 'rightInset', 'bottomInset']

// the part TextField and TextArea share (QQuickTextFieldPrivate, QQuickTextAreaPrivate)
function textControl(Base, name){
    class TextControl extends Base {
        constructor(parent = null){
            super(parent)
            this.$insets = {}
            this.$hoverSet = false
            this.$styleSize = false     // the style binds the implicit size: the text does not write it
            installFont(this, Base.FONT)
            engine.connect(this, Item.SIGNAL_implicitWidthChanged, () => engine.emit(this, TextControl.SIGNAL_implicitWidthChanged3), null, engine.PRIORITY_NORMAL, this)
            engine.connect(this, Item.SIGNAL_implicitHeightChanged, () => engine.emit(this, TextControl.SIGNAL_implicitHeightChanged3), null, engine.PRIORITY_NORMAL, this)
            this.$v[TextControl.HOVERENABLED] = !!require('../../Qt/Qt').Qt.styleHints.useHoverEffects
        }
        $parentChanged(old, parent){
            super.$parentChanged(old, parent)
            if(this.$controlFont) resolveFont(this)
        }
        // the text sets the content size; the implicit size when the style does not bind it
        $updateImplicitSize(){
            this.$textSizing = true
            try {
                super.$updateImplicitSize()
            } finally {
                this.$textSizing = false
            }
        }
        $setBackground(item){
            let old = this.$v[TextControl.BACKGROUND]
            if(old === item) return
            if(old && !(old.$f & engine.DESTROYED)){
                if(this.$defaults && this.$defaults.has(old)){
                    this.$defaults.delete(old)
                    old.destroy()
                } else if(old.$parent === this) old.$setParent(null)
            }
            Base.prototype.$write.call(this, TextControl.BACKGROUND, item || null)
            if(item){
                if(item.$ownSize === undefined) item.$ownSize = item.$explicit & 3
                if(item.$parent !== this) item.$setParent(this)
                if(item.$v[Item.Z] === 0) item.$write(Item.Z, -1)
                let update = () => this.$updateImplicitBackground()
                engine.connect(item, Item.SIGNAL_implicitWidthChanged, update, null, engine.PRIORITY_NORMAL, this)
                engine.connect(item, Item.SIGNAL_implicitHeightChanged, update, null, engine.PRIORITY_NORMAL, this)
            }
            this.$updateImplicitBackground()
            this.$resizeBackground()
        }
        $updateImplicitBackground(){
            let background = this.$v[TextControl.BACKGROUND]
            Base.prototype.$write.call(this, TextControl.IMPLICITBACKGROUNDWIDTH, background ? background.$v[Item.IMPLICITWIDTH] : 0)
            Base.prototype.$write.call(this, TextControl.IMPLICITBACKGROUNDHEIGHT, background ? background.$v[Item.IMPLICITHEIGHT] : 0)
        }
        // as QQuickControlPrivate::resizeBackground
        $resizeBackground(){
            let background = this.$v[TextControl.BACKGROUND]
            if(!background || background.$f & engine.DESTROYED) return
            let v = this.$v, insets = this.$insets, own = background.$ownSize || 0
            if((!(own & 1) && background.$v[Item.X] === 0) || insets.leftInset !== undefined || insets.rightInset !== undefined){
                background.$write(Item.X, v[TextControl.LEFTINSET])
                background.$explicit |= 1
                background.$write(Item.WIDTH, v[Item.WIDTH] - v[TextControl.LEFTINSET] - v[TextControl.RIGHTINSET])
            }
            if((!(own & 2) && background.$v[Item.Y] === 0) || insets.topInset !== undefined || insets.bottomInset !== undefined){
                background.$write(Item.Y, v[TextControl.TOPINSET])
                background.$explicit |= 2
                background.$write(Item.HEIGHT, v[Item.HEIGHT] - v[TextControl.TOPINSET] - v[TextControl.BOTTOMINSET])
            }
        }
        $write(id, value){
            if(id === TextControl.BACKGROUND) return this.$setBackground(value)
            if((id === Item.IMPLICITWIDTH || id === Item.IMPLICITHEIGHT) && this.$textSizing && this.$styleSize) return
            let inset = INSETS.find(n => TextControl[n.toUpperCase()] === id)
            if(inset){
                if(value === undefined) delete this.$insets[inset]
                else this.$insets[inset] = Number(value)
                super.$write(id, value === undefined ? 0 : Number(value))
                return this.$resizeBackground()
            }
            if(id === TextControl.HOVERENABLED){
                this.$hoverSet = value !== undefined
                super.$write(id, value === undefined ? !!require('../../Qt/Qt').Qt.styleHints.useHoverEffects : !!value)
                if(!this.$v[TextControl.HOVERENABLED]) super.$write(TextControl.HOVERED, false)
                return
            }
            super.$write(id, value)
            if(id === Item.WIDTH || id === Item.HEIGHT) this.$resizeBackground()
        }
        // hover (QQuickTextField::hoverEnterEvent)
        $acceptsHover(){ return this.$v[TextControl.HOVERENABLED] && this.$v[Item.ENABLED] }
        $hoverEnter(){ Base.prototype.$write.call(this, TextControl.HOVERED, true) }
        $hoverMove(){}
        $hoverLeave(){ Base.prototype.$write.call(this, TextControl.HOVERED, false) }
        $focusEvent(active, reason){
            if(active) Base.prototype.$write.call(this, TextControl.FOCUSREASON, reason)
        }
    }
    defineType(TextControl, {
        props: {
            implicitBackgroundWidth: { kind: 'real', readonly: true },
            implicitBackgroundHeight: { kind: 'real', readonly: true },
            background: { kind: 'var', initial: null },
            placeholderText: 'string',
            placeholderTextColor: { kind: 'color', initial: '#80000000' },
            focusReason: { kind: 'int', initial: 7 },
            hovered: { kind: 'bool', readonly: true },
            hoverEnabled: 'bool',
            topInset: 'real', leftInset: 'real', rightInset: 'real', bottomInset: 'real',
        },
        // QQuickTextField / QQuickTextArea declare implicitWidth / implicitHeight again (their notify signals revision 3)
        signals: ['pressAndHold(event)', 'pressed(event)', 'released(event)', 'implicitWidthChanged3', 'implicitHeightChanged3'],
    })
    TextControl.prototype.$theme = 'basic'
    Object.defineProperty(TextControl, 'name', { value: name })
    return TextControl
}

const TextField = textControl(TextInput, 'TextField')
const TextArea = textControl(TextEdit, 'TextArea')

module.exports = { TextField, TextArea }
