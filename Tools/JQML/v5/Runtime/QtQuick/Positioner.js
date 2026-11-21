const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { Item } = require('./Item')

const PADDING_BITS = { leftPadding: 1, topPadding: 2, rightPadding: 4, bottomPadding: 8 }

class Positioner extends Item {
    constructor(parent = null){
        super(parent)
        this.$explicitPadding = 0
        engine.scheduleComponentComplete(this)
    }

    $componentComplete(){
        super.$componentComplete()
        this.$polish()
    }

    $childGeometryChanged(){
        if(this.$f & engine.COMPLETED) engine.queuePolish(this)
    }

    // "Positioner typically positions its children once per frame. This function forces it to reposition immediately."
    forceLayout(){
        if(this.$f & engine.COMPLETED) this.$polish()
    }

    // laid out from the right (layoutDirection Qt.RightToLeft of Row, Grid, Flow)
    $rightToLeft(){
        let id = this.constructor.LAYOUTDIRECTION
        return id !== undefined && this.$v[id] === 1
    }

    // QQuickRow / QQuickGrid / QQuickFlow::doPositioning from the right: the children placed from the left (with the
    // side paddings swapped) are mirrored in the width, or in the implicit width without one
    $mirror(children){
        if(!this.$rightToLeft()) return
        let end = this.$explicit & 1 ? this.$v[Item.WIDTH] : this.$v[Item.IMPLICITWIDTH]
        for(let child of children) child.$place(Item.X, end - child.$v[Item.X] - child.$v[Item.WIDTH])
    }

    // A side padding set explicitly wins over padding, like in Qt; from the right the left and right ones swap
    $padding(name){
        if(this.$rightToLeft() && (name === 'leftPadding' || name === 'rightPadding')) name = name === 'leftPadding' ? 'rightPadding' : 'leftPadding'
        let type = this.constructor.$type
        let own = this.$v[type.propByName.get(name).id]
        return this.$explicitPadding & PADDING_BITS[name] ? own : this.$v[Positioner.PADDING]
    }

    $assign(id, value){
        let bit = PADDING_BITS[this.constructor.$type.props[id].name]
        if(bit) this.$explicitPadding |= bit
        super.$assign(id, value)
    }

    $bind(id, fn, scope, self){
        let bit = PADDING_BITS[this.constructor.$type.props[id].name]
        if(bit) this.$explicitPadding |= bit
        return super.$bind(id, fn, scope, self)
    }

    $visibleChildren(){
        let result = []
        for(let child of this.$children || []){
            // "Items are only omitted from positioning if they are explicitly hidden" (QQuickBasePositioner::prePositioning)
            if(!(child instanceof Item) || !child.$ownVisible) continue
            if(child.$v[Item.WIDTH] <= 0 || child.$v[Item.HEIGHT] <= 0) continue
            result.push(child)
        }
        return result
    }
}

const relayout = (positioner) => { if(positioner.$f & engine.COMPLETED) engine.queuePolish(positioner) }

defineType(Positioner, {
    props: {
        spacing: { kind: 'real', effect: relayout },
        padding: { kind: 'real', effect: relayout },
        leftPadding: { kind: 'real', effect: relayout },
        topPadding: { kind: 'real', effect: relayout },
        rightPadding: { kind: 'real', effect: relayout },
        bottomPadding: { kind: 'real', effect: relayout },
    },
    signals: ['positioningComplete'],
})

module.exports = { Positioner, relayout }
