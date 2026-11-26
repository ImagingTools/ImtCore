const engine = require('../Core/engine')
const { toCss } = require('../Core/color')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { bits } = require('./Item')

const STYLE = bits.STYLE

// ---- Gradient: "the gradient overrides color"; orientation is Gradient.Vertical by default ----------

// a stop tells the gradient it is in (Gradient, or a ShapeGradient of QtQuick.Shapes)
class GradientStop extends QtObject {
    $parentChanged(old, parent){
        if(old && old.$changed) old.$changed()
        if(parent && parent.$changed) parent.$changed()
    }
}
const stopChanged = (stop) => { if(stop.$parent && stop.$parent.$changed) stop.$parent.$changed() }
defineType(GradientStop, {
    props: {
        position: { kind: 'real', effect: stopChanged },
        color: { kind: 'color', effect: stopChanged },
    },
})

class Gradient extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$users = new Set()
    }

    $stops(){
        let list = this.$v[Gradient.STOPS]
        if(Array.isArray(list)) return list
        return (this.$children || []).filter(child => child instanceof GradientStop)
    }

    $changed(){
        for(let item of this.$users) engine.markDirty(item, STYLE)
    }

    $css(){
        let stops = this.$stops().slice().sort((a, b) => a.$v[GradientStop.POSITION] - b.$v[GradientStop.POSITION])
        if(!stops.length) return 'none'
        let direction = this.$v[Gradient.ORIENTATION] === Gradient.Horizontal ? 'to right' : 'to bottom'
        let parts = stops.map(stop => toCss(stop.$v[GradientStop.COLOR]) + ' ' + stop.$v[GradientStop.POSITION] * 100 + '%')
        if(parts.length === 1) parts.push(parts[0])
        return 'linear-gradient(' + direction + ',' + parts.join(',') + ')'
    }
}
defineType(Gradient, {
    props: {
        orientation: { kind: 'int', initial: 1, effect: (g) => g.$changed() },
        stops: { kind: 'var', effect: (g) => g.$changed() },
    },
})
Object.assign(Gradient, { Vertical: 1, Horizontal: 2 })

module.exports = { GradientStop, Gradient }
