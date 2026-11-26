const engine = require('../../Core/engine')
const { toCss } = require('../../Core/color')
const { defineType } = require('../../Core/type')
const { Item, bits } = require('../../QtQuick/Item')

const STYLE = bits.STYLE

// ---- DropShadow (Qt5Compat.GraphicalEffects) ----------------------------------------------------

// The effect draws its source with a shadow; with the source visible (how ImtCore uses it) the same
// picture is a CSS drop-shadow on the source. A hidden source is not drawn by the effect here.
class DropShadow extends Item {
    constructor(parent = null){
        super(parent)
        this.$styled = null
    }

    $apply(){
        let source = this.$v[DropShadow.SOURCE]
        if(this.$styled && this.$styled !== source) this.$clear()
        if(!(source instanceof Item) || !source.$dom) return
        let v = this.$v
        if(!v[Item.VISIBLE]){
            this.$clear()
            return
        }
        // Qt's blur deviation is (radius + 1) / 3.3333; CSS blur is twice the deviation
        let blur = 2 * (v[DropShadow.RADIUS] + 1) / 3.3333
        source.$dom.style.filter = 'drop-shadow(' + v[DropShadow.HORIZONTALOFFSET] + 'px ' + v[DropShadow.VERTICALOFFSET] + 'px ' +
            blur.toFixed(2) + 'px ' + toCss(v[DropShadow.COLOR]) + ')'
        this.$styled = source
    }

    $clear(){
        if(this.$styled && this.$styled.$dom) this.$styled.$dom.style.filter = ''
        this.$styled = null
    }

    $paint(dirty){
        super.$paint(dirty)
        if(dirty & (STYLE | bits.VISIBILITY)) this.$apply()
    }

    $disposeNative(){
        this.$clear()
        super.$disposeNative()
    }
}
const shadowChanged = (effect) => engine.markDirty(effect, STYLE)
defineType(DropShadow, {
    props: {
        source: { kind: 'var', effect: shadowChanged },
        horizontalOffset: { kind: 'real', effect: shadowChanged },
        verticalOffset: { kind: 'real', effect: shadowChanged },
        radius: { kind: 'real', initial: 4, effect: shadowChanged },
        samples: { kind: 'int', initial: 9 },
        color: { kind: 'color', initial: '#000000', effect: shadowChanged },
        spread: 'real',
        transparentBorder: { kind: 'bool', initial: true },
        cached: 'bool',
    },
})

module.exports = { DropShadow }
