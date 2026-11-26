// Pointer handlers of QtQuick (qquickpointerhandler.cpp, qquickhoverhandler.cpp, qquickdraghandler.cpp, Qt 6.12).
// A handler is no item: it is declared in an item (its parent) and gets the pointer events of that item before the
// item does (QQuickDeliveryAgentPrivate::deliverMatchingPointsToItem); the page's pointer is a mouse (device Mouse,
// pointer type Generic).
//  - HoverHandler: hovered while the pointer is over its parent (with margin), whether or not the item takes hover
//    events; a hovered one makes its item the hovered leaf, a blocking one stops the hover there.
//  - DragHandler: a press over its parent is a passive grab; a move past the drag threshold along an enabled axis takes
//    the grab (the item that had it is canceled, unless it keeps it: MouseArea.preventStealing) and the handler is
//    active: its target follows the pointer within the limits of the axes.
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { QtObject, GroupObject } = require('../Core/QtObject')
const { QPointF } = require('../Core/ValueTypes')
const { Item } = require('./Item')

// QInputDevice::DeviceType, QPointingDevice::PointerType (PointerDevice of QML)
const PointerDevice = {
    Unknown: 0, Mouse: 0x1, TouchScreen: 0x2, TouchPad: 0x4, Puck: 0x8, Stylus: 0x10, Airbrush: 0x20, Keyboard: 0x1000,
    AllDevices: 0x7fffffff,
    GenericPointer: 0x1, Finger: 0x2, Pen: 0x4, Eraser: 0x8, Cursor: 0x10, AllPointerTypes: 0x7fff,
}
const KeyboardModifierMask = 0xfe000000
const START_DRAG_DISTANCE = 10      // QStyleHints::startDragDistance

// QQuickPointerHandler::GrabPermission
const GRAB = {
    TakeOverForbidden: 0x0, CanTakeOverFromHandlersOfSameType: 0x01, CanTakeOverFromHandlersOfDifferentType: 0x02,
    CanTakeOverFromItems: 0x04, CanTakeOverFromAnything: 0x0f, ApprovesTakeOverByHandlersOfSameType: 0x10,
    ApprovesTakeOverByHandlersOfDifferentType: 0x20, ApprovesTakeOverByItems: 0x40, ApprovesCancellation: 0x80,
    ApprovesTakeOverByAnything: 0xf0,
}

// QQuickHandlerPoint: the point a handler reports, in the coordinates of its parent and of the scene
function handlerPoint(x, y, sx, sy, press, buttons, modifiers){
    return {
        id: 0, uniqueId: null,
        position: new QPointF(x, y), scenePosition: new QPointF(sx, sy),
        pressPosition: new QPointF(press ? press.x : 0, press ? press.y : 0),
        scenePressPosition: new QPointF(press ? press.sx : 0, press ? press.sy : 0),
        sceneGrabPosition: new QPointF(press ? press.sx : 0, press ? press.sy : 0),
        pressedButtons: buttons, modifiers, velocity: { x: 0, y: 0 }, rotation: 0, pressure: 0,
        ellipseDiameters: { width: 0, height: 0 },
    }
}

class PointerHandler extends QtObject {
    // the handlers of an item, the last declared first (QQuickItemPrivate::addPointerHandler prepends)
    $parentChanged(old, parent){
        super.$parentChanged(old, parent)
        if(old && old.$handlers){
            let index = old.$handlers.indexOf(this)
            if(index >= 0) old.$handlers.splice(index, 1)
        }
        if(parent instanceof Item) (parent.$handlers || (parent.$handlers = [])).unshift(this)
    }

    // a target written (null too) replaces the parent; a cursor shape written is shown over the parent
    $write(id, value){
        if(id === PointerHandler.TARGET){
            this.$targetSet = true
            if(value === undefined) value = null
        }
        if(id === PointerHandler.CURSORSHAPE) this.$cursorSet = value !== undefined
        super.$write(id, value)
    }

    $cursor(){
        return this.$cursorSet ? this.$v[PointerHandler.CURSORSHAPE] : null
    }

    $item(){
        return this.$parent instanceof Item ? this.$parent : null
    }

    $target(){
        return this.$targetSet ? this.$v[PointerHandler.TARGET] : this.$item()
    }

    // QQuickPointerHandler::wantsPointerEvent / parentContains: enabled, the mouse among the accepted devices and
    // pointer types, the modifiers, the point over the parent widened by the margin
    $wants(x, y, modifiers){
        let v = this.$v
        if(!v[PointerHandler.ENABLED]) return false
        if(!(v[PointerHandler.ACCEPTEDDEVICES] & PointerDevice.Mouse)) return false
        if(!(v[PointerHandler.ACCEPTEDPOINTERTYPES] & PointerDevice.GenericPointer)) return false
        let accepted = v[PointerHandler.ACCEPTEDMODIFIERS]
        if(accepted !== KeyboardModifierMask && (modifiers || 0) !== accepted) return false
        let item = this.$item()
        if(!item) return false
        let m = v[PointerHandler.MARGIN]
        return x >= -m && y >= -m && x <= item.$v[Item.WIDTH] + m && y <= item.$v[Item.HEIGHT] + m
    }

    $setActive(active){
        if(this.$v[PointerHandler.ACTIVE] === active) return
        this.$write(PointerHandler.ACTIVE, active)
    }
}
defineType(PointerHandler, {
    props: {
        enabled: { kind: 'bool', initial: true },
        active: { kind: 'bool', readonly: true },
        target: {
            kind: 'var',
            initial: null,
            get(){
                engine.track(this, PointerHandler.SIGNAL_targetChanged)
                return this.$target()
            },
        },
        cursorShape: { kind: 'int', initial: 0 },
        margin: 'real',
        dragThreshold: {
            kind: 'int',
            initial: -1,
            get(){
                engine.track(this, PointerHandler.SIGNAL_dragThresholdChanged)
                let value = this.$v[PointerHandler.DRAGTHRESHOLD]
                return value < 0 ? START_DRAG_DISTANCE : value
            },
        },
        grabPermissions: { kind: 'int', initial: GRAB.CanTakeOverFromItems | GRAB.CanTakeOverFromHandlersOfDifferentType | GRAB.ApprovesTakeOverByAnything },
        acceptedDevices: { kind: 'int', initial: PointerDevice.AllDevices },
        acceptedPointerTypes: { kind: 'int', initial: PointerDevice.AllPointerTypes },
        acceptedModifiers: { kind: 'int', initial: KeyboardModifierMask },
    },
    signals: ['canceled(point)', 'grabChanged(transition, point)'],
})
Object.assign(PointerHandler, GRAB)
PointerHandler.prototype.$targetSet = false
PointerHandler.prototype.$cursorSet = false
PointerHandler.prototype.$isHandler = true

// ---- HoverHandler ----------------------------------------------------------------------------

class HoverHandler extends PointerHandler {
    // a hover pass over its parent at (x, y) of the parent: true when it is hovered now
    $hover(x, y, sx, sy, modifiers){
        if(!this.$wants(x, y, modifiers)){
            this.$leave()
            return false
        }
        this.$write(HoverHandler.POINT, handlerPoint(x, y, sx, sy, null, 0, modifiers || 0))
        if(!this.$v[HoverHandler.HOVERED]){
            this.$write(HoverHandler.HOVERED, true)
            this.$setActive(true)
        }
        return true
    }

    $leave(){
        if(!this.$v[HoverHandler.HOVERED]) return
        this.$write(HoverHandler.HOVERED, false)
        this.$setActive(false)
    }
}
defineType(HoverHandler, {
    props: {
        hovered: { kind: 'bool', readonly: true },
        blocking: 'bool',
        point: { kind: 'var', readonly: true, initial: handlerPoint(0, 0, 0, 0, null, 0, 0) },
        acceptedButtons: { kind: 'int', initial: 1 },
    },
})

// ---- DragHandler -----------------------------------------------------------------------------

// QQuickDragAxis: the limits of the target along an axis, its value in the drag (activeValue) and the accumulated
// one (the persistentTranslation of the handler, kept within the limits as QQuickDragAxis::updateValue does)
class DragAxis extends GroupObject {
    constructor(owner){
        super(owner)
        this.$accumulated = 0
    }

    // QQuickDragAxis::updateValue
    $update(active, accumulated, delta){
        if(!this.$v[DragAxis.ENABLED]) return
        this.$write(DragAxis.ACTIVEVALUE, active)
        this.$accumulated = Math.min(this.$v[DragAxis.MAXIMUM], Math.max(this.$v[DragAxis.MINIMUM], accumulated))
        engine.emitArgs(this, DragAxis.SIGNAL_activeValueChanged, [delta])
    }
}
defineType(DragAxis, {
    props: {
        minimum: { kind: 'real', initial: -Number.MAX_VALUE },
        maximum: { kind: 'real', initial: Number.MAX_VALUE },
        enabled: { kind: 'bool', initial: true },
        activeValue: { kind: 'real', readonly: true },
    },
    signals: ['activeValueChanged(delta)'],
})

class DragHandler extends PointerHandler {
    // a press at (x, y) of the parent: the passive grab (true) when the handler wants it
    $press(x, y, sx, sy, button, modifiers){
        if(!(button & this.$v[DragHandler.ACCEPTEDBUTTONS])) return false
        if(!this.$wants(x, y, modifiers)) return false
        let target = this.$target()
        this.$pressAt = { x, y, sx, sy, modifiers, button }
        this.$startPos = target ? { x: target.$v[Item.X], y: target.$v[Item.Y] } : null
        this.$centroid(sx, sy, button, modifiers)
        return true
    }

    // the centroid in the coordinates of the parent as it is now
    $centroid(sx, sy, buttons, modifiers){
        let item = this.$item()
        let origin = item ? require('./PointerDelivery').scenePos(item) : { x: 0, y: 0 }
        this.$write(DragHandler.CENTROID, handlerPoint(sx - origin.x, sy - origin.y, sx, sy, this.$pressAt, buttons, modifiers || 0))
    }

    // a move while the press lasts (QQuickDragHandler::handlePointerEventImpl). grab(): takes the grab of the point,
    // false when the grabber keeps it. True when the handler takes the grab now
    $drag(sx, sy, buttons, modifiers, grab){
        let press = this.$pressAt
        if(!press) return false
        let xOn = this.xAxis.$v[DragAxis.ENABLED], yOn = this.yAxis.$v[DragAxis.ENABLED]
        let dx = sx - press.sx, dy = sy - press.sy
        let took = false
        if(this.$v[DragHandler.ACTIVE]){
            this.$setActiveTranslation(xOn ? dx : 0, yOn ? dy : 0)
        } else {
            // a disabled axis: no movement along it, and none at all while the drag is mostly along it
            let mx = dx, my = dy
            if(!xOn){
                if(Math.abs(mx) > Math.abs(my)) my = 0
                mx = 0
            }
            if(!yOn){
                if(Math.abs(my) > Math.abs(mx)) mx = 0
                my = 0
            }
            let threshold = this.dragThreshold
            let over = Math.round(Math.abs(mx)) > threshold || Math.round(Math.abs(my)) > threshold
            if(over && (!grab || grab())){
                took = true
                // QQuickDragHandler::onActiveChanged: the axes start from their accumulated values
                this.xAxis.$write(DragAxis.ACTIVEVALUE, 0)
                this.yAxis.$write(DragAxis.ACTIVEVALUE, 0)
                this.$setActive(true)
            }
        }
        let target = this.$target()
        if(this.$v[DragHandler.ACTIVE] && target && target.$parent instanceof Item && this.$startPos){
            // the target keeps the point where it was pressed under the pointer, within the limits of the axes
            let x = xOn ? Math.min(this.xAxis.$v[DragAxis.MAXIMUM], Math.max(this.xAxis.$v[DragAxis.MINIMUM], this.$startPos.x + dx)) : target.$v[Item.X]
            let y = yOn ? Math.min(this.yAxis.$v[DragAxis.MAXIMUM], Math.max(this.yAxis.$v[DragAxis.MINIMUM], this.$startPos.y + dy)) : target.$v[Item.Y]
            target.x = x
            target.y = y
        }
        this.$centroid(sx, sy, buttons, modifiers)
        return took
    }

    // QQuickDragHandler::setActiveTranslation
    $setActiveTranslation(x, y){
        let ax = this.xAxis.$v[DragAxis.ACTIVEVALUE], ay = this.yAxis.$v[DragAxis.ACTIVEVALUE]
        if(ax === x && ay === y) return
        let dx = x - ax, dy = y - ay
        this.xAxis.$update(x, this.xAxis.$accumulated + dx, dx)
        this.yAxis.$update(y, this.yAxis.$accumulated + dy, dy)
        engine.emitArgs(this, DragHandler.SIGNAL_translationChanged, [{ x: dx, y: dy }])
    }

    // the release (or the end of the grab)
    $end(canceled){
        if(!this.$pressAt) return
        this.$pressAt = null
        if(this.$v[DragHandler.ACTIVE]){
            this.xAxis.$write(DragAxis.ACTIVEVALUE, 0)
            this.yAxis.$write(DragAxis.ACTIVEVALUE, 0)
            this.$setActive(false)
        }
        if(canceled) engine.emitArgs(this, PointerHandler.SIGNAL_canceled, [this.$v[DragHandler.CENTROID]])
    }
}
const translationGetter = (read) => function(){
    engine.track(this, DragHandler.SIGNAL_translationChanged)
    engine.track(this, PointerHandler.SIGNAL_activeChanged)
    return read(this)
}
defineType(DragHandler, {
    props: {
        xAxis: { group: DragAxis },
        yAxis: { group: DragAxis },
        snapMode: { kind: 'int', initial: 1 },
        acceptedButtons: { kind: 'int', initial: 1 },
        centroid: { kind: 'var', readonly: true, initial: handlerPoint(0, 0, 0, 0, null, 0, 0) },
        activeTranslation: { kind: 'var', readonly: true, get: translationGetter((h) => new QPointF(h.xAxis.$v[DragAxis.ACTIVEVALUE], h.yAxis.$v[DragAxis.ACTIVEVALUE])) },
        translation: { kind: 'var', readonly: true, get: translationGetter((h) => new QPointF(h.xAxis.$v[DragAxis.ACTIVEVALUE], h.yAxis.$v[DragAxis.ACTIVEVALUE])) },
        persistentTranslation: {
            kind: 'var',
            get: translationGetter((h) => new QPointF(h.xAxis.$accumulated, h.yAxis.$accumulated)),
            // QQuickDragHandler::setPersistentTranslation
            set(value){
                let x = value ? Number(value.x) || 0 : 0, y = value ? Number(value.y) || 0 : 0
                if(x === this.xAxis.$accumulated && y === this.yAxis.$accumulated) return
                this.xAxis.$update(this.xAxis.$v[DragAxis.ACTIVEVALUE], x, 0)
                this.yAxis.$update(this.yAxis.$v[DragAxis.ACTIVEVALUE], y, 0)
                engine.emitArgs(this, DragHandler.SIGNAL_translationChanged, [{ x: 0, y: 0 }])
            },
        },
    },
    signals: ['translationChanged(delta)'],
})
Object.assign(DragHandler, { NoSnap: 0, SnapAuto: 1, SnapIfPressedOutsideTarget: 2, SnapAlways: 3 })

module.exports = { PointerDevice, PointerHandler, HoverHandler, DragHandler, DragAxis }
