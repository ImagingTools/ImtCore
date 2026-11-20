// SelectionRectangle of QtQuick.Controls (qquickselectionrectangle.cpp and the Basic style's SelectionRectangle.qml,
// Qt 6.12): selects the cells of a TableView by a drag in its viewport or by a press and hold on a cell, and shows two
// handles at the corners of the selection that drag it bigger or smaller. The view does the selecting (QQuickSelectable:
// $startSelection, $setSelectionStartPos, $setSelectionEndPos, $selectionRectangle ...); the control turns the pointer
// into those calls. Its handlers sit on the view's content item in Qt (a TapHandler and a DragHandler): here the view
// hands the presses, moves and releases of its content to the control ($pointerPress / $pointerMove / $pointerRelease)
// in content item coordinates, after its own tap handler (made first in Qt).
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { GroupObject } = require('../../Core/QtObject')
const { Item } = require('../Item')
const { Rectangle, Border } = require('../Rectangle')
const { Component } = require('../../QtQml/Component')
const { Flickable, DRAG_THRESHOLD } = require('../Flickable')
const QtEnums = require('../../Qt/enums')

const ENUMS = { Drag: 0, PressAndHold: 1, Auto: 2 }
// QStyleHints::mousePressAndHoldInterval: the long press of a TapHandler (longPressThreshold)
const LONG_PRESS_MS = 800
const MODIFIERS = QtEnums.ControlModifier | QtEnums.ShiftModifier

// ---- the attached object of a handle (QQuickSelectionRectangleAttached) ----
class SelectionRectangleAttached extends GroupObject {}
defineType(SelectionRectangleAttached, {
    props: {
        control: { kind: 'var', readonly: true, initial: null },
        dragging: { kind: 'bool', readonly: true },
    },
})
Object.assign(SelectionRectangleAttached.prototype, ENUMS)
{
    let prop = Item.$type.propByName.get('SelectionRectangle')
    prop.group = SelectionRectangleAttached
    let id = prop.id
    Object.defineProperty(Item.prototype, 'SelectionRectangle', {
        get(){ return this.$v[id] || (this.$v[id] = new SelectionRectangleAttached(this)) },
        configurable: true,
    })
}
// qmlAttachedPropertiesObject(object): the attached object of a handle
const attachedOf = (item) => (item ? item.SelectionRectangle : null)

// the Basic style's handle: a circle of 28, shown while the selection is active
function basicHandle(){
    return new Component(null, (parent) => {
        let handle = new Rectangle(parent)
        let attached = handle.SelectionRectangle
        handle.$bind(Item.WIDTH, () => 28, null)
        handle.$bind(Item.HEIGHT, () => handle.width, null)
        handle.$bind(Rectangle.RADIUS, () => handle.width / 2, null)
        handle.$bind(Rectangle.COLOR, () => (attached.dragging ? '#f6f6f6' : '#ffffff'), null)
        handle.border.$bind(Border.WIDTH, () => 1, null)
        handle.border.$bind(Border.COLOR, () => {
            let control = attached.control
            return control && control.enabled ? '#bdbdbd' : '#e4e4e4'
        }, null)
        handle.$bind(Item.VISIBLE, () => {
            let control = attached.control
            return !!(control && control.active)
        }, null)
        return handle
    })
}

const isSelectionRectValid = (rect) => rect.width !== 0 || rect.height !== 0

class SelectionRectangle extends Item {
    constructor(parent = null){
        super(parent)
        this.$target = null
        this.$selectable = null
        this.$targetLinks = []
        this.$effectiveSelectionMode = ENUMS.Drag
        this.$tapEnabled = true
        this.$dragEnabled = true
        this.$topLeft = null
        this.$bottomRight = null
        this.$draggedHandle = null
        this.$scrollTimer = null
        this.$scrollToPoint = { x: 0, y: 0 }
        this.$scrollSpeed = { width: 1, height: 1 }
        // the pointer of the content item: the press of the tap handler and the drag of the drag handler
        this.$point = null
        this.$v[SelectionRectangle.TOPLEFTHANDLE] = basicHandle()
        this.$v[SelectionRectangle.BOTTOMRIGHTHANDLE] = basicHandle()
    }

    // QQuickItem::enabledChanged: the selection stops
    $enabledChanged(){
        super.$enabledChanged()
        this.$stopScrollTimer()
        this.$updateSelectionMode()
        this.$updateDraggingState(false)
        this.$updateActiveState(false)
    }

    // ---- target (QQuickSelectionRectangle::setTarget) ----
    $setTargetValue(target){
        target = target || null
        if(this.$target === target) return
        if(this.$selectable){
            this.$stopScrollTimer()
            this.$cancelPointer()
            for(let [object, signal, fn] of this.$targetLinks) engine.disconnect(object, signal, fn)
            this.$targetLinks = []
            if(this.$selectable.$selectionControl === this) this.$selectable.$selectionControl = null
            this.$selectable.$selectableCallback = null
        }
        this.$target = target
        this.$selectable = null
        if(target){
            if(typeof target.$startSelection === 'function') this.$selectable = target
            else console.warn('SelectionRectangle: the assigned target is not supported by the control')
        }
        if(this.$selectable){
            this.$selectable.$selectionControl = this
            this.$connectToTarget()
            this.$updateSelectionMode()
        }
    }

    $connectToTarget(){
        let target = this.$target
        if(target instanceof Flickable){
            let fn = () => this.$updateSelectionMode()
            engine.connect(target, Flickable.SIGNAL_interactiveChanged, fn)
            this.$targetLinks.push([target, Flickable.SIGNAL_interactiveChanged, fn])
        }
        this.$selectable.$selectableCallback = (flag) => {
            if(flag === 'cancel') this.$updateActiveState(false)
            else this.$updateHandles()
        }
    }

    // QQuickSelectionRectanglePrivate::updateSelectionMode (the input is a mouse: the devices are no matter here)
    $updateSelectionMode(){
        let enabled = this.$v[Item.ENABLED]
        this.$tapEnabled = enabled
        let mode = this.$v[SelectionRectangle.SELECTIONMODE]
        let target = this.$target
        if(mode === ENUMS.Auto){
            let parentItem = target && target.parent
            if(parentItem && parentItem.constructor && parentItem.constructor.name === 'ScrollView'){
                this.$effectiveSelectionMode = ENUMS.Drag
                this.$dragEnabled = enabled
            } else if(target instanceof Flickable){
                // Flickable takes the left button (acceptedButtons): a drag selects only when it does not flick
                if(enabled && !target.$v[Flickable.INTERACTIVE]){
                    this.$effectiveSelectionMode = ENUMS.Drag
                    this.$dragEnabled = true
                } else {
                    this.$effectiveSelectionMode = ENUMS.PressAndHold
                    this.$dragEnabled = false
                }
            } else {
                this.$effectiveSelectionMode = ENUMS.Drag
                this.$dragEnabled = enabled
            }
        } else if(mode === ENUMS.Drag){
            this.$effectiveSelectionMode = ENUMS.Drag
            this.$dragEnabled = enabled
        } else {
            this.$effectiveSelectionMode = ENUMS.PressAndHold
            this.$dragEnabled = false
        }
        if(!this.$dragEnabled && this.$point && this.$point.dragActive) this.$endDrag()
    }

    // ---- state ----
    $updateDraggingState(dragging){
        if(dragging !== this.$v[SelectionRectangle.DRAGGING]) this.$write(SelectionRectangle.DRAGGING, dragging)
        let handle = this.$draggedHandle
        if(handle && !(handle.$f & engine.DESTROYED)) attachedOf(handle).$write(SelectionRectangleAttached.DRAGGING, dragging)
    }

    $updateActiveState(active){
        if(active === this.$v[SelectionRectangle.ACTIVE]) return
        let target = this.$target
        if(target && target.$startSelection){
            const { TableView } = require('../TableView')
            if(active){
                this.$layoutLink = () => this.$updateHandles()
                engine.connect(target, TableView.SIGNAL_layoutChanged, this.$layoutLink)
            } else if(this.$layoutLink){
                engine.disconnect(target, TableView.SIGNAL_layoutChanged, this.$layoutLink)
                this.$layoutLink = null
            }
        }
        this.$write(SelectionRectangle.ACTIVE, active)
    }

    // ---- handles ----
    // QQuickSelectionRectanglePrivate::createHandle: made in the control's context in the content item, its attached
    // control set before its bindings run (beginCreate / completeCreate), above the cells (z 100 when it has none)
    $createHandle(delegate, topLeft){
        let content = this.$selectable.contentItem
        let control = this
        let handle = delegate.createObject(content, null, {
            $attachRequired(obj){
                if(obj instanceof Item) attachedOf(obj).$write(SelectionRectangleAttached.CONTROL, control)
            },
        })
        if(!(handle instanceof Item)) return null
        if(handle.z === 0) handle.z = 100
        this.$installHandleInput(handle, topLeft)
        return handle
    }

    // the handlers of a handle: a DragHandler that takes over from anything, a blocking HoverHandler with the
    // diagonal cursor and a TapHandler (DragWithinBounds) that keeps the taps from the view
    $installHandleInput(handle, topLeft){
        let drag = null
        let positionOf = (x, y) => ({ x: handle.$v[Item.X] + x, y: handle.$v[Item.Y] + y })
        let moveCorner = (pos) => {
            if(topLeft) this.$selectable.$setSelectionStartPos(pos)
            else this.$selectable.$setSelectionEndPos(pos)
        }
        let deactivate = () => {
            let wasActive = drag && drag.active
            drag = null
            handle.$keepMouseGrab = false
            if(!wasActive) return
            this.$stopScrollTimer()
            this.$selectable.$normalizeSelection()
            this.$updateDraggingState(false)
            require('../PointerDelivery').setOverrideCursor(this, null)
        }
        handle.$keepMouseGrab = false
        handle.$mousePress = (x, y, button) => {
            if(button !== 1 || !this.$selectable) return false
            drag = { pressX: x, pressY: y, active: false }
            handle.$keepMouseGrab = true
            return true
        }
        handle.$mouseMove = (x, y) => {
            if(!drag) return
            if(!drag.active){
                if(Math.abs(x - drag.pressX) <= DRAG_THRESHOLD && Math.abs(y - drag.pressY) <= DRAG_THRESHOLD) return
                drag.active = true
                moveCorner(positionOf(x, y))
                this.$draggedHandle = handle
                this.$updateHandles()
                this.$updateDraggingState(true)
                require('../PointerDelivery').setOverrideCursor(this, QtEnums.SizeFDiagCursor)
                return
            }
            if(!this.$v[SelectionRectangle.DRAGGING]) return
            let pos = positionOf(x, y)
            moveCorner(pos)
            this.$updateHandles()
            this.$scrollTowardsPos(pos)
        }
        handle.$mouseRelease = () => deactivate()
        handle.$ungrabMouse = () => deactivate()
        handle.$acceptsHover = () => true
        handle.$hoverEnter = () => {}
        handle.$hoverMove = () => {}
        handle.$hoverLeave = () => {}
        handle.$cursor = () => QtEnums.SizeFDiagCursor
    }

    // QQuickSelectionRectanglePrivate::updateHandles: the handles made on first use, centered on the corners
    $updateHandles(){
        let selectable = this.$selectable
        if(!selectable) return
        let r = selectable.$selectionRectangle()
        let rect = { x: Math.min(r.x, r.x + r.width), y: Math.min(r.y, r.y + r.height), width: Math.abs(r.width), height: Math.abs(r.height) }
        let tlDelegate = this.$v[SelectionRectangle.TOPLEFTHANDLE]
        let brDelegate = this.$v[SelectionRectangle.BOTTOMRIGHTHANDLE]
        if(!this.$topLeft && tlDelegate) this.$topLeft = this.$createHandle(tlDelegate, true)
        if(!this.$bottomRight && brDelegate) this.$bottomRight = this.$createHandle(brDelegate, false)
        let tl = this.$topLeft, br = this.$bottomRight
        if(tl){
            tl.x = rect.x - tl.width / 2
            tl.y = rect.y - tl.height / 2
        }
        if(br){
            br.x = rect.x + rect.width - br.width / 2
            br.y = rect.y + rect.height - br.height / 2
        }
    }

    // QQuickSelectionRectanglePrivate::handleUnderPos: a handle that contains the point of the content item
    $handleUnderPos(pos){
        for(let handle of [this.$topLeft, this.$bottomRight]){
            if(!handle || handle.$f & engine.DESTROYED) continue
            let x = pos.x - handle.$v[Item.X], y = pos.y - handle.$v[Item.Y]
            if(x >= 0 && y >= 0 && x <= handle.$v[Item.WIDTH] && y <= handle.$v[Item.HEIGHT]) return handle
        }
        return null
    }

    // ---- auto scroll while dragging outside the viewport (m_scrollTimer, 1 ms) ----
    $scrollTowardsPos(pos){
        this.$scrollToPoint = { x: pos.x, y: pos.y }
        if(this.$scrollTimer) return
        let dist = this.$selectable.$scrollTowardsPoint(this.$scrollToPoint, this.$scrollSpeed)
        if(dist.width !== 0 || dist.height !== 0) this.$scrollTimer = setInterval(() => engine.batch(() => this.$scrollTick()), 1)
    }

    $scrollTick(){
        let selectable = this.$selectable
        if(!selectable || this.$f & engine.DESTROYED){
            this.$stopScrollTimer()
            return
        }
        if(this.$topLeft && this.$draggedHandle === this.$topLeft) selectable.$setSelectionStartPos(this.$scrollToPoint)
        else selectable.$setSelectionEndPos(this.$scrollToPoint)
        this.$updateHandles()
        let speed = this.$scrollSpeed
        let dist = selectable.$scrollTowardsPoint(this.$scrollToPoint, speed)
        this.$scrollToPoint.x += dist.width > 0 ? speed.width : -speed.width
        this.$scrollToPoint.y += dist.height > 0 ? speed.height : -speed.height
        this.$scrollSpeed = { width: Math.abs(dist.width * 0.007), height: Math.abs(dist.height * 0.007) }
    }

    $stopScrollTimer(){
        if(this.$scrollTimer) clearInterval(this.$scrollTimer)
        this.$scrollTimer = null
    }

    // ---- the pointer of the content item (the view calls these) ----

    // a left press: the tap handler is pressed. True when the drag handler may take the drag (the view does not flick)
    $pointerPress(pos, modifiers){
        this.$cancelPointer()
        if(!this.$selectable || !this.$tapEnabled) return false
        let point = this.$point = { pressX: pos.x, pressY: pos.y, x: pos.x, y: pos.y, modifiers, dragActive: false, longTimer: null }
        point.longTimer = setTimeout(() => engine.batch(() => {
            if(this.$point !== point) return
            point.longTimer = null
            this.$longPressed(point)
        }), LONG_PRESS_MS)
        this.$tapPressed(point)
        return this.$dragEnabled
    }

    // the tap handler's pressedChanged on a press (only with Drag: shift extends, control adds a cell)
    $tapPressed(point){
        if(this.$effectiveSelectionMode !== ENUMS.Drag) return
        let modifiers = point.modifiers
        if(modifiers & ~MODIFIERS) return
        let selectable = this.$selectable
        let pos = { x: point.pressX, y: point.pressY }
        if(modifiers & QtEnums.ShiftModifier){
            if(!this.$v[SelectionRectangle.ACTIVE]){
                if(!selectable.$startSelection(pos, modifiers)) return
                selectable.$setSelectionStartPos({ x: -1, y: -1 })
            }
            selectable.$setSelectionEndPos(pos)
            if(!isSelectionRectValid(selectable.$selectionRectangle())) return
            this.$updateHandles()
            this.$updateActiveState(true)
        } else if(modifiers & QtEnums.ControlModifier){
            if(this.$handleUnderPos(pos)) return
            if(!selectable.$startSelection(pos, modifiers)) return
            selectable.$setSelectionStartPos(pos)
            selectable.$setSelectionEndPos(pos)
            if(!isSelectionRectValid(selectable.$selectionRectangle())) return
            this.$updateHandles()
            this.$updateActiveState(true)
        }
    }

    // the tap handler's longPressed
    $longPressed(point){
        if(this.$effectiveSelectionMode !== ENUMS.PressAndHold) return
        let selectable = this.$selectable
        let pos = { x: point.pressX, y: point.pressY }
        let modifiers = point.modifiers
        point.longPressed = true
        if(this.$handleUnderPos(pos)) return
        if(modifiers === QtEnums.ShiftModifier){
            if(!this.$v[SelectionRectangle.ACTIVE]){
                if(!selectable.$startSelection(pos, modifiers)) return
                selectable.$setSelectionStartPos({ x: -1, y: -1 })
            }
            selectable.$setSelectionEndPos(pos)
        } else {
            if(!selectable.$startSelection(pos, modifiers)) return
            selectable.$setSelectionStartPos(pos)
            selectable.$setSelectionEndPos(pos)
        }
        if(!isSelectionRectValid(selectable.$selectionRectangle())) return
        this.$updateHandles()
        this.$updateActiveState(true)
    }

    // the tap handler is released or canceled: an active control without a selection is no longer active
    $tapReleased(){
        if(this.$v[SelectionRectangle.ACTIVE] && !this.$selectable.$hasSelection()) this.$updateActiveState(false)
    }

    $pointerMove(pos){
        let point = this.$point
        if(!point || !this.$selectable) return
        point.x = pos.x
        point.y = pos.y
        if(!point.dragActive){
            if(Math.abs(pos.x - point.pressX) <= DRAG_THRESHOLD && Math.abs(pos.y - point.pressY) <= DRAG_THRESHOLD) return
            // past the drag threshold the tap handler lets go (DragThreshold), and the drag handler takes the point
            if(point.longTimer) clearTimeout(point.longTimer)
            point.longTimer = null
            if(!point.tapReleased){
                point.tapReleased = true
                this.$tapReleased()
            }
            if(!this.$dragEnabled) return
            point.dragActive = true
            this.$dragActivated(point)
            return
        }
        // centroidChanged
        if(!this.$v[SelectionRectangle.DRAGGING]) return
        this.$selectable.$setSelectionEndPos(pos)
        this.$updateHandles()
        this.$scrollTowardsPos(pos)
    }

    // the drag handler's activeChanged (active)
    $dragActivated(point){
        let modifiers = point.modifiers
        if(modifiers & ~MODIFIERS) return
        let selectable = this.$selectable
        let startPos = { x: point.pressX, y: point.pressY }
        let modifiersHeld = !!(modifiers & MODIFIERS)
        if(!this.$v[SelectionRectangle.ACTIVE] || !modifiersHeld){
            if(!selectable.$startSelection(startPos, modifiers)) return
            selectable.$setSelectionStartPos(startPos)
        }
        selectable.$setSelectionEndPos({ x: point.x, y: point.y })
        this.$draggedHandle = null
        if(!isSelectionRectValid(selectable.$selectionRectangle())) return
        this.$updateHandles()
        this.$updateActiveState(true)
        this.$updateDraggingState(true)
    }

    // the drag handler's activeChanged (inactive)
    $endDrag(){
        let point = this.$point
        if(point) point.dragActive = false
        if(point && (point.modifiers & ~MODIFIERS)) return
        this.$stopScrollTimer()
        this.$selectable.$normalizeSelection()
        this.$updateDraggingState(false)
    }

    $pointerRelease(){
        let point = this.$point
        if(!point) return
        this.$point = null
        if(point.longTimer) clearTimeout(point.longTimer)
        if(point.dragActive) this.$endDrag()
        else if(!point.tapReleased) this.$tapReleased()
    }

    // the pointer is taken away (the grab goes elsewhere): the handlers let go
    $cancelPointer(){
        let point = this.$point
        if(!point) return
        this.$pointerRelease()
    }

    // did the press of the content item end in a long press (the tap handler emits no tap then)
    $longPressedNow(){ return !!(this.$point && this.$point.longPressed) }

    $disposeNative(){
        this.$stopScrollTimer()
        this.$cancelPointer()
        this.$setTargetValue(null)
        require('../PointerDelivery').setOverrideCursor(this, null)
        super.$disposeNative && super.$disposeNative()
    }
}
defineType(SelectionRectangle, {
    props: {
        target: { kind: 'var', initial: null, effect: (control, value) => control.$setTargetValue(value) },
        active: { kind: 'bool', readonly: true },
        dragging: { kind: 'bool', readonly: true },
        selectionMode: { kind: 'int', initial: ENUMS.Auto, effect: (control) => { if(control.$target) control.$updateSelectionMode() } },
        topLeftHandle: 'component',
        bottomRightHandle: 'component',
    },
})
Object.assign(SelectionRectangle, ENUMS)

module.exports = { SelectionRectangle, SelectionRectangleAttached }
