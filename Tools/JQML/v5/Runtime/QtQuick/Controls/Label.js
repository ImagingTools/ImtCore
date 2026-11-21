// QQuickLabel (QtQuick.Templates Label, Qt 6.12): a Text whose font passes from the controls above it (as a Control's),
// with a background sized to the label less its insets (implicitBackgroundWidth / Height)
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Item } = require('../Item')
const { Text } = require('../Text')
const { installFont, resolveFont } = require('./Control')

class Label extends Text {
    constructor(parent = null){
        super(parent)
        this.$insets = {}
        installFont(this, Text.FONT)
    }
    $parentChanged(old, parent){
        super.$parentChanged(old, parent)
        if(this.$controlFont) resolveFont(this)
    }
    $setBackground(item){
        let old = this.$v[Label.BACKGROUND]
        if(old === item) return
        if(old && !(old.$f & engine.DESTROYED) && old.$parent === this) old.$setParent(null)
        super.$write(Label.BACKGROUND, item || null)
        if(item){
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
        let background = this.$v[Label.BACKGROUND]
        super.$write(Label.IMPLICITBACKGROUNDWIDTH, background ? background.$v[Item.IMPLICITWIDTH] : 0)
        super.$write(Label.IMPLICITBACKGROUNDHEIGHT, background ? background.$v[Item.IMPLICITHEIGHT] : 0)
    }
    // QQuickLabelPrivate::resizeBackground, as a Control's
    $resizeBackground(){
        let background = this.$v[Label.BACKGROUND]
        if(!background || background.$f & engine.DESTROYED) return
        let v = this.$v, insets = this.$insets
        if((!(background.$explicit & 1) && background.$v[Item.X] === 0) || insets.leftInset !== undefined || insets.rightInset !== undefined){
            background.$write(Item.X, v[Label.LEFTINSET])
            background.$write(Item.WIDTH, v[Item.WIDTH] - v[Label.LEFTINSET] - v[Label.RIGHTINSET])
        }
        if((!(background.$explicit & 2) && background.$v[Item.Y] === 0) || insets.topInset !== undefined || insets.bottomInset !== undefined){
            background.$write(Item.Y, v[Label.TOPINSET])
            background.$write(Item.HEIGHT, v[Item.HEIGHT] - v[Label.TOPINSET] - v[Label.BOTTOMINSET])
        }
    }
    $write(id, value){
        if(id === Label.BACKGROUND) return this.$setBackground(value)
        if(id === Label.TOPINSET || id === Label.LEFTINSET || id === Label.RIGHTINSET || id === Label.BOTTOMINSET){
            let name = this.constructor.$type.props[id].name
            if(value === undefined) delete this.$insets[name]
            else this.$insets[name] = Number(value)
            super.$write(id, value === undefined ? 0 : Number(value))
            this.$resizeBackground()
            return
        }
        super.$write(id, value)
        if(id === Item.WIDTH || id === Item.HEIGHT) this.$resizeBackground()
    }
}
defineType(Label, {
    props: {
        background: { kind: 'var', initial: null },
        implicitBackgroundWidth: { kind: 'real', readonly: true },
        implicitBackgroundHeight: { kind: 'real', readonly: true },
        topInset: 'real',
        leftInset: 'real',
        rightInset: 'real',
        bottomInset: 'real',
    },
})

// the palette of a Label defaults to the theme's, as a control's
Label.prototype.$theme = 'basic'

module.exports = { Label }
