const engine = require('../Core/engine')
const { defineType, makeReadonly } = require('../Core/type')
const { Item } = require('./Item')
const { Positioner, relayout } = require('./Positioner')

// Flow.LeftToRight wraps at the width; Flow.TopToBottom wraps at the height
class Flow extends Positioner {
    $polish(){
        let left = this.$padding('leftPadding'), top = this.$padding('topPadding')
        let right = this.$padding('rightPadding'), bottom = this.$padding('bottomPadding')
        let spacing = this.$v[Positioner.SPACING]
        let leftToRight = this.$v[Flow.FLOW] === Flow.LeftToRight
        let limit = leftToRight ? this.$v[Item.WIDTH] - right : this.$v[Item.HEIGHT] - bottom
        let x = left, y = top, line = 0, maxX = left, maxY = top
        for(let child of this.$visibleChildren()){
            let w = child.$v[Item.WIDTH], h = child.$v[Item.HEIGHT]
            if(leftToRight){
                if(x > left && limit > 0 && x + w > limit){
                    x = left
                    y += line + spacing
                    line = 0
                }
                child.$place(Item.X, x)
                child.$place(Item.Y, y)
                x += w + spacing
                line = Math.max(line, h)
                maxX = Math.max(maxX, x - spacing)
                maxY = Math.max(maxY, y + h)
            } else {
                if(y > top && limit > 0 && y + h > limit){
                    y = top
                    x += line + spacing
                    line = 0
                }
                child.$place(Item.X, x)
                child.$place(Item.Y, y)
                y += h + spacing
                line = Math.max(line, w)
                maxX = Math.max(maxX, x + w)
                maxY = Math.max(maxY, y - spacing)
            }
        }
        this.$write(Item.IMPLICITWIDTH, maxX + right)
        this.$write(Item.IMPLICITHEIGHT, maxY + bottom)
        engine.emitArgs(this, Positioner.SIGNAL_positioningComplete, [])
    }
}
defineType(Flow, {
    props: { flow: { kind: 'int', effect: relayout } },
})
Flow.LeftToRight = 0
Flow.TopToBottom = 1

// size changes of a Flow re-wrap it
{
    let base = Flow.prototype.$write
    Flow.prototype.$write = function(id, value){
        base.call(this, id, value)
        if((id === Item.WIDTH || id === Item.HEIGHT) && this.$f & engine.COMPLETED) engine.queuePolish(this)
    }
}

// read-only in Qt, writable in the base class
makeReadonly(Flow, 'implicitWidth')
makeReadonly(Flow, 'implicitHeight')

module.exports = { Flow }
