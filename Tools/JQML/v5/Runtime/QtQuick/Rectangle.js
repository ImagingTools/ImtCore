const engine = require('../Core/engine')
const { toCss } = require('../Core/color')
const { GroupObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { Item, bits } = require('./Item')
const { style } = require('./TextLayout')

const STYLE = bits.STYLE

const ownerStyle = (group) => engine.markDirty(group.$owner, STYLE)

// ---- Rectangle ----------------------------------------------------------------

class Border extends GroupObject {}
defineType(Border, {
    props: {
        width: { kind: 'real', effect: ownerStyle },
        color: { kind: 'color', initial: '#000000', effect: ownerStyle },
    },
})

class Rectangle extends Item {
    $paint(dirty){
        super.$paint(dirty)
        if(!this.$dom) return
        if(dirty & STYLE){
            let v = this.$v
            let s = this.$dom.style
            let gradient = v[Rectangle.GRADIENT]
            if(gradient && gradient.$css){
                s.backgroundColor = 'transparent'
                s.backgroundImage = gradient.$css()
            } else {
                s.backgroundColor = toCss(v[Rectangle.COLOR])
                s.backgroundImage = ''
            }
            s.borderRadius = v[Rectangle.RADIUS] + 'px'
            // the border is drawn inside the rectangle above its fill and does not move the children,
            // which a CSS border would
            let border = v[Rectangle.BORDER]
            if(border && border.$v[Border.WIDTH] > 0){
                s.boxShadow = 'inset 0 0 0 ' + border.$v[Border.WIDTH] + 'px ' + toCss(border.$v[Border.COLOR])
            } else {
                s.boxShadow = ''
            }
        }
    }
}

defineType(Rectangle, {
    props: {
        color: { kind: 'color', initial: '#ffffff', effect: style },
        radius: { kind: 'real', effect: style },
        border: { group: Border },
        gradient: {
            kind: 'var',
            effect(rect, value, old){
                if(old && old.$users) old.$users.delete(rect)
                if(value && value.$users) value.$users.add(rect)
                engine.markDirty(rect, STYLE)
            },
        },
    },
})

module.exports = { Border, Rectangle }
