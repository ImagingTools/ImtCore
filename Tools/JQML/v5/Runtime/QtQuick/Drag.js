// The Drag attached property and the drag events of the scene, after qquickdrag.cpp (QQuickDragAttached) and
// QQuickDeliveryAgentPrivate::deliverDragEvent: an internal drag sends enter, move, leave and drop events to the items
// that accept drops (DropArea) under the hot spot of the dragged item.
const engine = require('../Core/engine')
const { QtObject, GroupObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { Item } = require('./Item')
const { scene, scenePos, contains } = require('./PointerDelivery')

const IgnoreAction = 0x0, CopyAction = 0x1, MoveAction = 0x2, LinkAction = 0x4
const NONE = 0, AUTOMATIC = 1, INTERNAL = 2

// Qt.DropAction, Drag.DragType and the drag axes (MouseArea.drag.axis: Drag.YAxis)
const ENUMS = { XAxis: 1, YAxis: 2, XAndYAxis: 3, XandYAxis: 3, None: NONE, Automatic: AUTOMATIC, Internal: INTERNAL }

// the event a DropArea gets (QQuickDragEvent): accepted unless its handler rejects it
class DragEvent {
    constructor(drag, x, y){
        this.x = x
        this.y = y
        this.source = drag.$v[DragAttached.SOURCE]
        this.keys = drag.$v[DragAttached.KEYS] || []
        this.supportedActions = drag.$v[DragAttached.SUPPORTEDACTIONS]
        this.proposedAction = drag.$v[DragAttached.PROPOSEDACTION]
        this.action = this.proposedAction
        this.accepted = false
        let mime = drag.$v[DragAttached.MIMEDATA]
        this.formats = mime && typeof mime === 'object' ? Object.keys(mime) : []
        this.text = mime && mime['text/plain'] !== undefined ? String(mime['text/plain']) : ''
        this.hasText = this.formats.includes('text/plain')
    }

    accept(action){
        if(action !== undefined) this.action = action
        this.accepted = true
    }

    acceptProposedAction(){
        this.action = this.proposedAction
        this.accepted = true
    }

    getDataAsString(format){
        let mime = this.$mime
        return mime && mime[format] !== undefined ? String(mime[format]) : ''
    }
}

class DragHotSpot extends GroupObject {}
const hotSpotChanged = (hotSpot) => { if(hotSpot.$owner.$v[DragAttached.ACTIVE]) hotSpot.$owner.$updatePosition() }
defineType(DragHotSpot, {
    props: { x: { kind: 'real', effect: hotSpotChanged }, y: { kind: 'real', effect: hotSpotChanged } },
})

class DragAttached extends GroupObject {
    constructor(owner){
        super(owner)
        this.$v[DragAttached.SOURCE] = owner
        this.$grabbed = []          // QQuickDragGrabber: the items that accepted the enter event
        this.$ignored = []          // the items that rejected it, until the hot spot leaves them
        this.$grabberTarget = null
        this.$window = false
        this.$listening = false
        this.$inEvent = false
        this.$moved = false
        this.$restarted = false
        this.$queued = false
        this.$setting = false
    }

    $write(id, value){
        if(id === DragAttached.ACTIVE && !this.$setting) return this.$setActive(!!value)
        let old = this.$v[id]
        super.$write(id, value)
        if(old === this.$v[id] || !this.$v[DragAttached.ACTIVE]) return
        // QQuickDragAttached::setSource, setKeys, setSupportedActions: the sequence starts again
        if(id === DragAttached.SOURCE || id === DragAttached.KEYS || id === DragAttached.SUPPORTEDACTIONS) this.$restartDrag()
        else if(id === DragAttached.PROPOSEDACTION) this.$updatePosition()
    }

    $writeActive(value){
        this.$setting = true
        try {
            this.$write(DragAttached.ACTIVE, value)
        } finally {
            this.$setting = false
        }
    }

    $writeTarget(value){
        if(this.$v[DragAttached.TARGET] !== value) this.$write(DragAttached.TARGET, value)
    }

    // QQuickDragAttached::setActive
    $setActive(active){
        if(this.$v[DragAttached.ACTIVE] === active) return
        if(this.$inEvent){
            console.warn('Drag: active cannot be changed from within a drag event handler')
            return
        }
        if(!active) return this.cancel()
        let type = this.$v[DragAttached.DRAGTYPE]
        if(type === INTERNAL) return this.$start(this.$v[DragAttached.SUPPORTEDACTIONS])
        this.$writeActive(true)
        if(type === AUTOMATIC) this.startDrag()
    }

    // QQuickDragAttachedPrivate::start
    $start(supportedActions){
        this.$actions = supportedActions
        if(!this.$listening){
            this.$listening = true
            let owner = this.$owner
            let moved = () => { if(this.$v[DragAttached.ACTIVE] && !this.$moved) this.$updatePosition() }
            engine.connect(owner, Item.SIGNAL_xChanged, moved)
            engine.connect(owner, Item.SIGNAL_yChanged, moved)
            engine.connect(owner, Item.SIGNAL_parentChanged, () => { if(this.$v[DragAttached.ACTIVE] && !this.$restarted) this.$updatePosition() })
        }
        this.$moved = false
        this.$restarted = false
        this.$writeActive(true)
        this.$deliverEnter()
        this.$writeTarget(this.$grabberTarget)
    }

    // the hot spot in the scene, null when the item is in no window
    $scenePoint(){
        let root = this.$owner
        while(root.$parent instanceof Item) root = root.$parent
        if(!scene.roots.includes(root)) return null
        let p = scenePos(this.$owner)
        let hotSpot = this.$v[DragAttached.HOTSPOT]
        return { x: Math.round(p.x + (hotSpot ? hotSpot.$v[DragHotSpot.X] : 0)), y: Math.round(p.y + (hotSpot ? hotSpot.$v[DragHotSpot.Y] : 0)) }
    }

    // QQuickDragAttachedPrivate::updatePosition, restartDrag: the events go out once the changes are made
    $updatePosition(){
        this.$moved = true
        this.$queue()
    }

    $restartDrag(){
        this.$restarted = true
        this.$queue()
    }

    $queue(){
        if(this.$queued) return
        this.$queued = true
        queueMicrotask(() => {
            this.$queued = false
            if(this.$f & engine.DESTROYED || engine.isStopped()) return
            engine.batch(() => {
                if(this.$restarted){
                    this.$deliverLeave()
                    this.$deliverEnter()
                    this.$writeTarget(this.$grabberTarget)
                } else if(this.$moved) this.$deliverMove()
            })
        })
    }

    $deliverEnter(){
        this.$restarted = false
        this.$moved = false
        let point = this.$scenePoint()
        this.$window = !!point
        if(point) this.$deliver('enter', point)
    }

    $deliverMove(){
        this.$moved = false
        if(!this.$window) return
        let point = this.$scenePoint()
        if(point) this.$deliver('move', point)
        this.$writeTarget(this.$grabberTarget)
    }

    $deliverLeave(){
        if(!this.$window) return
        this.$deliver('leave', null)
        this.$window = false
    }

    $send(item, type, event){
        this.$inEvent = true
        try {
            if(type === 'enter') item.$dragEnter(event)
            else if(type === 'move') item.$dragMove(event)
            else if(type === 'leave') item.$dragLeave()
            else item.$dropEvent(event)
        } finally {
            this.$inEvent = false
        }
    }

    $local(item, point){
        let p = scenePos(item)
        let event = new DragEvent(this, point.x - p.x, point.y - p.y)
        event.$mime = this.$v[DragAttached.MIMEDATA]
        return event
    }

    // QQuickDeliveryAgentPrivate::deliverDragEvent(grabber, event); returns whether the event was accepted
    $deliver(type, point, event = null){
        let former = this.$grabberTarget
        this.$grabberTarget = null
        if(this.$grabbed.length){
            if(type === 'drop'){
                event.accepted = false
                while(!event.accepted && this.$grabbed.length){
                    let item = this.$grabbed.shift()
                    let translated = this.$local(item, point)
                    this.$send(item, 'drop', translated)
                    event.accepted = translated.accepted
                    event.action = translated.action
                    this.$grabberTarget = item
                }
            }
            if(type !== 'move'){
                for(let item of this.$grabbed.splice(0)) this.$send(item, 'leave', null)
                this.$ignored = []
                return !!(event && event.accepted)
            }
            let current = this.$grabbed.splice(0)
            let accepted = this.$deliverRoots(point, current, former)
            for(let item of this.$grabbed){
                let i = current.indexOf(item)
                if(i < 0) continue
                current.splice(i, 1)
                let translated = this.$local(item, point)
                this.$send(item, 'move', translated)
                accepted = translated.accepted
            }
            for(let item of current) this.$send(item, 'leave', null)
            return accepted
        }
        if(type === 'enter' || type === 'move') return this.$deliverRoots(point, null, null)
        this.$ignored = []
        return false
    }

    $deliverRoots(point, current, former){
        for(let i = scene.roots.length - 1; i >= 0; i--){
            if(this.$deliverTo(scene.roots[i], point.x, point.y, point, current, former)) return true
        }
        return false
    }

    // QQuickDeliveryAgentPrivate::deliverDragEvent(grabber, item, event, ...): the children in front first, the
    // item, the children behind; an item that accepts the enter event takes the drag
    $deliverTo(item, x, y, point, current, former){
        if(!item.$v[Item.VISIBLE] || !item.$v[Item.ENABLED] || item.$culled) return false
        let lx = x - item.$v[Item.X], ly = y - item.$v[Item.Y]
        let contained = contains(item, lx, ly)
        let ignoredIndex = this.$ignored.indexOf(item)
        if(!contained){
            if(ignoredIndex >= 0) this.$ignored.splice(ignoredIndex, 1)
            if(item.$v[Item.CLIP]) return false
        }
        let children = (item.$children || []).filter((child) => child instanceof Item)
        let order = children.map((child, i) => [child, i]).sort((a, b) => a[0].$v[Item.Z] - b[0].$v[Item.Z] || a[1] - b[1]).map(([child]) => child)
        for(let i = order.length - 1; i >= 0; i--){
            if(order[i].$v[Item.Z] < 0) continue
            if(this.$deliverTo(order[i], lx, ly, point, current, former)) return true
        }
        if(contained){
            if(current && current.includes(item)){
                this.$grabbed.push(item)
                this.$grabberTarget = item
                return true
            }
            if(item.$acceptsDrops){
                if(former){
                    if(current && current.includes(former)){
                        this.$send(former, 'leave', null)
                        current.splice(current.indexOf(former), 1)
                    }
                } else if(ignoredIndex >= 0) return false
                let event = this.$local(item, point)
                this.$send(item, 'enter', event)
                if(event.accepted){
                    this.$grabbed.push(item)
                    this.$grabberTarget = item
                    return true
                }
                if(ignoredIndex < 0) this.$ignored.push(item)
            }
        }
        for(let i = order.length - 1; i >= 0; i--){
            if(order[i].$v[Item.Z] >= 0) continue
            if(this.$deliverTo(order[i], lx, ly, point, current, former)) return true
        }
        return false
    }

    // Drag.start(supportedActions): an internal drag, the actions given override supportedActions
    start(supportedActions){
        if(this.$inEvent){
            console.warn('Drag: start() cannot be called from within a drag event handler')
            return
        }
        if(this.$v[DragAttached.ACTIVE]) this.cancel()
        this.$start(typeof supportedActions === 'number' ? supportedActions : this.$v[DragAttached.SUPPORTEDACTIONS])
    }

    // Drag.drop(): the drop event to the target; the action it accepted, else Qt.IgnoreAction
    drop(){
        if(this.$inEvent){
            console.warn('Drag: drop() cannot be called from within a drag event handler')
            return IgnoreAction
        }
        if(this.$moved) this.$deliverMove()
        if(!this.$v[DragAttached.ACTIVE]) return IgnoreAction
        let action = IgnoreAction
        let target = null
        this.$setting = true
        this.$v[DragAttached.ACTIVE] = false
        this.$setting = false
        let point = this.$window ? this.$scenePoint() : null
        if(point){
            let event = { accepted: false, action: IgnoreAction }
            this.$deliver('drop', point, event)
            if(event.accepted){
                action = event.action
                target = this.$grabberTarget
            }
        }
        this.$writeTarget(target)
        engine.emit(this, DragAttached.SIGNAL_activeChanged, true, false)
        if(this.$automatic){
            this.$automatic = false
            engine.emitArgs(this, DragAttached.SIGNAL_dragFinished, [action])
        }
        return action
    }

    // Drag.cancel(): the sequence ends, the target gets a leave event
    cancel(){
        if(this.$inEvent){
            console.warn('Drag: cancel() cannot be called from within a drag event handler')
            return
        }
        if(!this.$v[DragAttached.ACTIVE]) return
        this.$setting = true
        this.$v[DragAttached.ACTIVE] = false
        this.$setting = false
        this.$deliverLeave()
        this.$writeTarget(null)
        engine.emit(this, DragAttached.SIGNAL_activeChanged, true, false)
        if(this.$automatic){
            this.$automatic = false
            engine.emitArgs(this, DragAttached.SIGNAL_dragFinished, [IgnoreAction])
        }
    }

    // Drag.startDrag(): the platform drag of Qt; in the page it is the internal drag of the scene, its end reported
    // by dragFinished
    startDrag(supportedActions){
        if(!this.$v[DragAttached.ACTIVE] || this.$v[DragAttached.DRAGTYPE] !== INTERNAL){
            this.$setting = true
            this.$v[DragAttached.ACTIVE] = false
            this.$setting = false
            this.$start(typeof supportedActions === 'number' ? supportedActions : this.$v[DragAttached.SUPPORTEDACTIONS])
        }
        this.$automatic = true
        engine.emit(this, DragAttached.SIGNAL_dragStarted)
    }
}

Object.assign(DragAttached.prototype, ENUMS)

defineType(DragAttached, {
    props: {
        active: 'bool',
        source: 'var',
        target: { kind: 'var', readonly: true },
        hotSpot: { group: DragHotSpot },
        keys: 'var',
        mimeData: 'var',
        dragType: { kind: 'int', initial: INTERNAL },
        supportedActions: { kind: 'int', initial: CopyAction | MoveAction | LinkAction },
        proposedAction: { kind: 'int', initial: MoveAction },
        imageSource: 'string',
        imageSourceSize: 'var',
    },
    signals: ['dragStarted', 'dragFinished(dropAction)'],
})

// the type Drag: its enums (Drag.YAxis, Drag.Internal) where no item has the attached object
class Drag extends QtObject {}
defineType(Drag)
Object.assign(Drag, ENUMS)

// The Drag attached group property of every Item, wired here because it needs DragAttached
{
    let prop = Item.$type.propByName.get('Drag')
    prop.group = DragAttached
    let id = prop.id
    Object.defineProperty(Item.prototype, 'Drag', {
        get(){ return this.$v[id] || (this.$v[id] = new DragAttached(this)) },
        configurable: true,
    })
}

module.exports = { Drag, DragAttached, DragEvent, IgnoreAction, CopyAction, MoveAction, LinkAction }
