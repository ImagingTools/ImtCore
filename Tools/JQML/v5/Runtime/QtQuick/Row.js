const engine = require('../Core/engine')
const { defineType, makeReadonly } = require('../Core/type')
const { Item } = require('./Item')
const { Positioner, relayout } = require('./Positioner')

class Row extends Positioner {
    $polish(){
        let left = this.$padding('leftPadding'), top = this.$padding('topPadding')
        let spacing = this.$v[Positioner.SPACING]
        let x = left, height = 0, count = 0
        for(let child of this.$visibleChildren()){
            if(count) x += spacing
            child.$place(Item.X, x)
            if(!(child.$anchored & 2)) child.$write(Item.Y, child.$v[Item.Y] + top - (child.$posPadding || 0))
            child.$posPadding = top
            x += child.$v[Item.WIDTH]
            height = Math.max(height, child.$v[Item.HEIGHT])
            count++
        }
        this.$write(Item.IMPLICITWIDTH, x + this.$padding('rightPadding'))
        this.$write(Item.IMPLICITHEIGHT, height + top + this.$padding('bottomPadding'))
        this.$mirror(this.$visibleChildren())
        engine.emitArgs(this, Positioner.SIGNAL_positioningComplete, [])
    }
}
defineType(Row, {
    props: {
        layoutDirection: { kind: 'int', effect: (row) => { row.$write(Row.EFFECTIVELAYOUTDIRECTION, row.$v[Row.LAYOUTDIRECTION]); relayout(row) } },
        effectiveLayoutDirection: { kind: 'int', readonly: true },
    },
})

// read-only in Qt, writable in the base class
makeReadonly(Row, 'implicitWidth')
makeReadonly(Row, 'implicitHeight')

module.exports = { Row }
