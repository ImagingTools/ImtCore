const engine = require('../Core/engine')
const { toCss } = require('../Core/color')
const { GroupObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { Item, bits } = require('./Item')
const { style } = require('./TextLayout')

const STYLE = bits.STYLE

const ownerStyle = (group) => engine.markDirty(group.$owner, STYLE)

// ---- Rectangle ----------------------------------------------------------------

// QQuickPen: 1 px and black, and not drawn until its color or width is written (the same value too). A write makes it
// valid while the color is visible and the width rounds to 1 or more; the width is drawn rounded (pixelAligned).
class Border extends GroupObject {
    $write(id, value){
        super.$write(id, value)
        if(id !== Border.WIDTH && id !== Border.COLOR) return
        let valid = alphaOf(this.$v[Border.COLOR]) > 0 && Math.round(this.$v[Border.WIDTH]) >= 1
        if(valid !== this.$valid){
            this.$valid = valid
            ownerStyle(this)
        }
    }
}
Border.prototype.$valid = false
defineType(Border, {
    props: {
        width: { kind: 'real', initial: 1, effect: ownerStyle },
        color: { kind: 'color', initial: '#000000', effect: ownerStyle },
    },
})

// the alpha of a color value (#rrggbb, or #aarrggbb)
const alphaOf = (hex) => (typeof hex === 'string' && hex.length === 9 ? parseInt(hex.slice(1, 3), 16) : 255)

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
            if(border && border.$valid){
                s.boxShadow = 'inset 0 0 0 ' + Math.round(border.$v[Border.WIDTH]) + 'px ' + toCss(border.$v[Border.COLOR])
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
