const engine = require('../Core/engine')
const { defineType, makeReadonly } = require('../Core/type')
const { Item } = require('./Item')
const { Positioner } = require('./Positioner')

class Column extends Positioner {
    $polish(){
        let left = this.$padding('leftPadding'), top = this.$padding('topPadding')
        let spacing = this.$v[Positioner.SPACING]
        let y = top, width = 0, count = 0
        for(let child of this.$visibleChildren()){
            if(count) y += spacing
            // the child keeps its own x, moved by a padding change (QQuickColumn::doPositioning)
            if(!(child.$anchored & 1)) child.$write(Item.X, child.$v[Item.X] + left - (child.$posPadding || 0))
            child.$posPadding = left
            child.$place(Item.Y, y)
            y += child.$v[Item.HEIGHT]
            width = Math.max(width, child.$v[Item.WIDTH])
            count++
        }
        this.$write(Item.IMPLICITWIDTH, width + left + this.$padding('rightPadding'))
        this.$write(Item.IMPLICITHEIGHT, y + this.$padding('bottomPadding'))
        engine.emitArgs(this, Positioner.SIGNAL_positioningComplete, [])
    }
}
defineType(Column)

// read-only in Qt, writable in the base class
makeReadonly(Column, 'implicitWidth')
makeReadonly(Column, 'implicitHeight')

module.exports = { Column }
