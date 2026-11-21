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

// the corners of Rectangle (Qt 6.7) in the order of CSS border-radius
const CORNERS = ['topLeftRadius', 'topRightRadius', 'bottomRightRadius', 'bottomLeftRadius']

class Rectangle extends Item {
    // a corner not set (NaN) is the radius
    $corner(name){
        let value = this.$v[Rectangle[name.toUpperCase()]]
        return value === value ? value : this.$v[Rectangle.RADIUS]
    }

    // QQuickRectangle::setTopLeftRadius: a write sets the corner (its change is emitted the first time even with the
    // value of the radius); undefined resets it to the radius
    $write(id, value){
        let corner = CORNER_IDS.get(id)
        if(corner === undefined){
            super.$write(id, value)
            // the corners are limited by the size
            if(this.$cornersSet && (id === Item.WIDTH || id === Item.HEIGHT)) engine.markDirty(this, STYLE)
            return
        }
        let old = this.$v[id]
        let reset = value === undefined
        if(reset && old !== old) return
        let number = reset ? NaN : Number(value)
        if(!reset && old === number) return
        this.$v[id] = number
        this.$cornersSet = CORNERS.some(c => { let x = this.$v[Rectangle[c.toUpperCase()]]; return x === x })
        engine.markDirty(this, STYLE)
        engine.emit(this, Rectangle.$type.props[id].signal)
    }

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
            if(this.$cornersSet){
                // each corner at most half the shorter side (QQuickRectangle::updatePaintNode), not scaled together as CSS does
                let max = Math.max(0, Math.min(v[Item.WIDTH], v[Item.HEIGHT]) / 2)
                s.borderRadius = CORNERS.map(c => Math.min(max, Math.max(0, this.$corner(c))) + 'px').join(' ')
            } else {
                s.borderRadius = v[Rectangle.RADIUS] + 'px'
            }
            // the border is drawn inside the rectangle above its fill and does not move the children,
            // which a CSS border would
            let border = v[Rectangle.BORDER]
            if(border && border.$valid){
                let width = Math.round(border.$v[Border.WIDTH])
                s.boxShadow = 'inset 0 0 0 ' + width + 'px ' + toCss(border.$v[Border.COLOR])
                // the fill is inside the border (QSGRectangleNode), which shows through a border not opaque: the fill
                // is clipped to the inside (the padding does not move the children, placed from the padding edge)
                let inside = alphaOf(border.$v[Border.COLOR]) < 255
                s.padding = inside ? width + 'px' : ''
                s.boxSizing = inside ? 'border-box' : ''
                s.backgroundClip = inside ? 'content-box' : ''
            } else {
                s.boxShadow = ''
                if(s.backgroundClip){
                    s.padding = ''
                    s.boxSizing = ''
                    s.backgroundClip = ''
                }
            }
        }
    }
}

function cornerGetter(name){
    return function(){
        let id = Rectangle[name.toUpperCase()]
        engine.track(this, Rectangle.$type.props[id].signal)
        return this.$corner(name)
    }
}

defineType(Rectangle, {
    props: {
        color: { kind: 'color', initial: '#ffffff', effect: style },
        // QQuickRectangle::setRadius: the corners not set change with it
        radius: {
            kind: 'real',
            effect(rect){
                style(rect)
                for(let c of CORNERS){
                    let id = Rectangle[c.toUpperCase()]
                    if(rect.$v[id] !== rect.$v[id]) engine.emit(rect, Rectangle.$type.props[id].signal)
                }
            },
        },
        topLeftRadius: { kind: 'real', initial: NaN, get: cornerGetter('topLeftRadius') },
        topRightRadius: { kind: 'real', initial: NaN, get: cornerGetter('topRightRadius') },
        bottomLeftRadius: { kind: 'real', initial: NaN, get: cornerGetter('bottomLeftRadius') },
        bottomRightRadius: { kind: 'real', initial: NaN, get: cornerGetter('bottomRightRadius') },
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
const CORNER_IDS = new Map(CORNERS.map(c => [Rectangle[c.toUpperCase()], c]))
Rectangle.prototype.$cornersSet = false

module.exports = { Border, Rectangle }
