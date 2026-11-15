const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { Item } = require('./Item')
const { Positioner } = require('./Positioner')

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
        engine.emitArgs(this, Positioner.SIGNAL_positioningComplete, [])
    }
}
defineType(Row)

module.exports = { Row }
