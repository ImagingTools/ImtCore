const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { Item } = require('./Item')

// ---- Flickable ------------------------------------------------------------------------------

// "Items declared as children of a Flickable are automatically parented to the Flickable's contentItem."
// contentWidth / contentHeight -1: the content item follows the Flickable's size. contentX / contentY are not clamped.
class Flickable extends Item {
    constructor(parent = null){
        super(parent)
        this.$content = new Item(this)
        this.$syncContentSize()
    }

    get contentItem(){ return this.$content }

    $syncContentSize(){
        if(!this.$content) return
        let w = this.$v[Flickable.CONTENTWIDTH], h = this.$v[Flickable.CONTENTHEIGHT]
        this.$content.$write(Item.WIDTH, w < 0 ? this.$v[Item.WIDTH] : w)
        this.$content.$write(Item.HEIGHT, h < 0 ? this.$v[Item.HEIGHT] : h)
        this.$updateBounds()
    }

    $updateBounds(){
        let x = this.$v[Flickable.CONTENTX], y = this.$v[Flickable.CONTENTY]
        let cw = this.$content ? this.$content.$v[Item.WIDTH] : 0, ch = this.$content ? this.$content.$v[Item.HEIGHT] : 0
        let w = this.$v[Item.WIDTH], h = this.$v[Item.HEIGHT]
        this.$write(Flickable.ATXBEGINNING, x <= 0)
        this.$write(Flickable.ATXEND, x >= cw - w)
        this.$write(Flickable.ATYBEGINNING, y <= 0)
        this.$write(Flickable.ATYEND, y >= ch - h)
    }

    returnToBounds(){
        let cw = this.$content.$v[Item.WIDTH], ch = this.$content.$v[Item.HEIGHT]
        this.contentX = Math.max(0, Math.min(this.$v[Flickable.CONTENTX], cw - this.$v[Item.WIDTH]))
        this.contentY = Math.max(0, Math.min(this.$v[Flickable.CONTENTY], ch - this.$v[Item.HEIGHT]))
    }

    resizeContent(width, height, center){
        this.contentWidth = width
        this.contentHeight = height
    }

    $wheel(x, y, angleDelta, pixelDelta){
        if(!this.$v[Flickable.INTERACTIVE]) return false
        return wheelScroll(this, Flickable.CONTENTX, Flickable.CONTENTY, this.$content.$v[Item.WIDTH], this.$content.$v[Item.HEIGHT], angleDelta, pixelDelta)
    }
}

// Moves the content by the wheel delta within its bounds; at a bound the event goes on to the items below.
// QQuickFlickable::wheelEvent: moving starts with the first step and ends after MovementEndingTimerInterval (100 ms)
function wheelMovement(view){
    let type = view.constructor
    if(!view.$v[type.MOVING]){
        view.$write(type.MOVING, true)
        engine.emitArgs(view, type.SIGNAL_movementStarted, [])
    }
    if(view.$movementTimer) clearTimeout(view.$movementTimer)
    view.$movementTimer = setTimeout(() => {
        view.$movementTimer = null
        if(view.$f & engine.DESTROYED) return
        engine.batch(() => {
            view.$write(type.MOVING, false)
            engine.emitArgs(view, type.SIGNAL_movementEnded, [])
        })
    }, 100)
}

function wheelScroll(view, xId, yId, contentWidth, contentHeight, angleDelta, pixelDelta){
    let dx = pixelDelta.x || angleDelta.x / 120 * 100
    let dy = pixelDelta.y || angleDelta.y / 120 * 100
    let maxX = Math.max(0, contentWidth - view.$v[Item.WIDTH]), maxY = Math.max(0, contentHeight - view.$v[Item.HEIGHT])
    let x = view.$v[xId], y = view.$v[yId]
    let nx = Math.max(0, Math.min(maxX, x - dx)), ny = Math.max(0, Math.min(maxY, y - dy))
    if(nx === x && ny === y) return false
    wheelMovement(view)
    if(nx !== x) view[view.constructor.$type.props[xId].name] = nx
    if(ny !== y) view[view.constructor.$type.props[yId].name] = ny
    return true
}
Flickable.$defaultParent = (flickable) => flickable.$content

const contentSize = (flickable) => flickable.$syncContentSize()
// "the content item's position is -contentX, -contentY"
const moved = (flickable) => {
    flickable.$content.$write(Item.X, -flickable.$v[Flickable.CONTENTX])
    flickable.$content.$write(Item.Y, -flickable.$v[Flickable.CONTENTY])
    flickable.$updateBounds()
}

defineType(Flickable, {
    props: {
        contentX: { kind: 'real', effect: moved },
        contentY: { kind: 'real', effect: moved },
        contentWidth: { kind: 'real', initial: -1, effect: contentSize },
        contentHeight: { kind: 'real', initial: -1, effect: contentSize },
        originX: 'real',
        originY: 'real',
        interactive: { kind: 'bool', initial: true },
        flickableDirection: 'int',
        boundsBehavior: { kind: 'int', initial: 3 },
        boundsMovement: { kind: 'int', initial: 1 },
        moving: 'bool',
        flicking: 'bool',
        dragging: 'bool',
        atXBeginning: { kind: 'bool', initial: true },
        atXEnd: { kind: 'bool', initial: true },
        atYBeginning: { kind: 'bool', initial: true },
        atYEnd: { kind: 'bool', initial: true },
        maximumFlickVelocity: { kind: 'real', initial: 2500 },
        flickDeceleration: { kind: 'real', initial: 1500 },
        pressDelay: 'int',
    },
    signals: ['movementStarted', 'movementEnded', 'flickStarted', 'flickEnded', 'dragStarted', 'dragEnded'],
})
Object.assign(Flickable, {
    AutoFlickDirection: 0, AutoFlickIfNeeded: 3, HorizontalFlick: 1, VerticalFlick: 2, HorizontalAndVerticalFlick: 4,
    StopAtBounds: 0, DragOverBounds: 1, OvershootBounds: 2, DragAndOvershootBounds: 3,
})

{
    let base = Flickable.prototype.$write
    Flickable.prototype.$write = function(id, value){
        base.call(this, id, value)
        if(id === Item.WIDTH || id === Item.HEIGHT) this.$syncContentSize()
    }
}

module.exports = { Flickable, wheelScroll }
