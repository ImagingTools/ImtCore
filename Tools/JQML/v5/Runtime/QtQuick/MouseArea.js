const engine = require('../Core/engine')
const { GroupObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { Item } = require('./Item')
const { LeftButton, NoButton, contains, scenePos, targetsAt, ungrab } = require('./PointerDelivery')

const XAxis = 1, YAxis = 2, XAndYAxis = 3
const START_DRAG_DISTANCE = 10        // QStyleHints::startDragDistance
const PRESS_AND_HOLD_INTERVAL = 800   // QStyleHints::mousePressAndHoldInterval

function connected(obj, signal){
    return engine.hasSubscribers(obj, signal)
}

// ---- events ---------------------------------------------------------------------------------

class MouseEvent {
    constructor(x, y, button, buttons, modifiers, isClick, wasHeld){
        this.x = x
        this.y = y
        this.button = button
        this.buttons = buttons
        this.modifiers = modifiers
        this.isClick = isClick
        this.wasHeld = wasHeld
        this.accepted = true
        this.source = 0
        this.flags = 0
    }
}

class WheelEvent {
    constructor(x, y, angleDelta, pixelDelta, buttons, modifiers, inverted){
        this.x = x
        this.y = y
        this.angleDelta = angleDelta
        this.pixelDelta = pixelDelta
        this.buttons = buttons
        this.modifiers = modifiers
        this.inverted = inverted
        this.accepted = true
    }
}

// ---- MouseArea ------------------------------------------------------------------------------

class MouseDrag extends GroupObject {}

defineType(MouseDrag, {
    props: {
        target: 'var',
        axis: { kind: 'int', initial: XAndYAxis },
        minimumX: { kind: 'real', initial: -3.4028234663852886e38 },
        maximumX: { kind: 'real', initial: 3.4028234663852886e38 },
        minimumY: { kind: 'real', initial: -3.4028234663852886e38 },
        maximumY: { kind: 'real', initial: 3.4028234663852886e38 },
        active: 'bool',
        filterChildren: 'bool',
        smoothed: { kind: 'bool', initial: true },
        threshold: { kind: 'real', initial: START_DRAG_DISTANCE },
    },
})

class MouseArea extends Item {
    constructor(parent = null){
        super(null)
        this.$pressed = NoButton
        this.$hovered = false
        this.$lastX = 0
        this.$lastY = 0
        this.$lastButton = NoButton
        this.$lastButtons = NoButton
        this.$lastModifiers = 0
        this.$longPress = false
        this.$doubleClick = false
        this.$moved = false
        this.$stealMouse = false
        this.$keepMouseGrab = false
        this.$overThreshold = false
        this.$startX = 0
        this.$startY = 0
        this.$targetStart = null
        this.$holdTimer = 0
        this.$cursorSet = false
        if(parent) this.$setParent(parent)
    }

    $acceptsHover(){
        return this.$v[MouseArea.HOVERENABLED] && this.$v[MouseArea.ENABLED]
    }

    $cursor(){
        return this.$cursorSet ? this.$v[MouseArea.CURSORSHAPE] : null
    }

    $event(isClick){
        return new MouseEvent(this.$lastX, this.$lastY, this.$lastButton, this.$lastButtons, this.$lastModifiers, isClick, this.$longPress)
    }

    $save(x, y, button, buttons, modifiers){
        this.$lastX = x
        this.$lastY = y
        this.$lastButton = button
        this.$lastButtons = buttons
        this.$lastModifiers = modifiers
    }

    // mouseX / mouseY notify on every event, changed or not
    $emitPosition(me){
        this.$v[MouseArea.MOUSEX] = this.$lastX
        this.$v[MouseArea.MOUSEY] = this.$lastY
        engine.emit(this, MouseArea.SIGNAL_mouseXChanged, me)
        engine.emit(this, MouseArea.SIGNAL_mouseYChanged, me)
    }

    $emitPressed(){
        this.$v[MouseArea.PRESSED] = !!this.$pressed
        this.$v[MouseArea.PRESSEDBUTTONS] = this.$pressed
        this.$v[MouseArea.CONTAINSPRESS] = !!this.$pressed && this.$hovered
    }

    $setHovered(hovered){
        if(this.$hovered === hovered) return
        this.$hovered = hovered
        this.$v[MouseArea.CONTAINSMOUSE] = hovered
        engine.emit(this, MouseArea.SIGNAL_containsMouseChanged, !hovered, hovered)
        engine.emit(this, hovered ? MouseArea.SIGNAL_entered : MouseArea.SIGNAL_exited)
        if(this.$pressed){
            this.$emitPressed()
            engine.emit(this, MouseArea.SIGNAL_containsPressChanged)
        }
    }

    $setDragActive(active){
        let drag = this.$v[MouseArea.DRAG]
        if(drag) drag.$write(MouseDrag.ACTIVE, active)
    }

    $dragActive(){
        let drag = this.$v[MouseArea.DRAG]
        return !!(drag && drag.$v[MouseDrag.ACTIVE])
    }

    $setPressed(button, p){
        if(p && !this.$v[Item.VISIBLE]) return false
        let dragged = this.$dragActive()
        let wasPressed = !!(this.$pressed & button)
        let isClick = wasPressed && !p && !dragged && this.$hovered
        let oldPressed = this.$pressed
        if(wasPressed === p) return false

        let me = this.$event(isClick)
        if(p){
            this.$pressed |= button
            this.$emitPressed()
            if(!this.$doubleClick) engine.emitArgs(this, MouseArea.SIGNAL_pressed, [me])
            this.$emitPosition(me)
            if(!me.accepted){
                this.$pressed = NoButton
                this.$emitPressed()
                if(!this.$v[MouseArea.HOVERENABLED]) this.$setHovered(false)
            }
            if(!oldPressed){
                engine.emit(this, MouseArea.SIGNAL_pressedChanged, false, !!this.$pressed)
                engine.emit(this, MouseArea.SIGNAL_containsPressChanged)
            }
            engine.emit(this, MouseArea.SIGNAL_pressedButtonsChanged)
        } else {
            this.$pressed &= ~button
            this.$emitPressed()
            engine.emitArgs(this, MouseArea.SIGNAL_released, [me])
            if(!this.$pressed){
                engine.emit(this, MouseArea.SIGNAL_pressedChanged, true, false)
                engine.emit(this, MouseArea.SIGNAL_containsPressChanged)
            }
            engine.emit(this, MouseArea.SIGNAL_pressedButtonsChanged)
            if(isClick && !this.$longPress && !this.$doubleClick){
                me.accepted = connected(this, MouseArea.SIGNAL_clicked)
                engine.emitArgs(this, MouseArea.SIGNAL_clicked, [me])
                if(!me.accepted) this.$propagate(me, MouseArea.SIGNAL_clicked)
            }
        }
        return me.accepted
    }

    // propagateComposedEvents: the composed event goes to the MouseAreas below, topmost first
    $propagate(me, signal){
        if(!this.$v[MouseArea.PROPAGATECOMPOSEDEVENTS]) return
        let origin = scenePos(this)
        let x = origin.x + me.x, y = origin.y + me.y
        for(let target of targetsAt(x, y)){
            let area = target.item
            if(area === this || !(area instanceof MouseArea) || !area.$v[MouseArea.ENABLED]) continue
            if(!(area.$v[MouseArea.ACCEPTEDBUTTONS] & me.button)) continue
            let event = new MouseEvent(target.x, target.y, me.button, me.buttons, me.modifiers, me.isClick, me.wasHeld)
            event.accepted = connected(area, signal)
            engine.emitArgs(area, signal, [event])
            if(event.accepted){
                me.accepted = true
                return
            }
        }
    }

    $mousePress(x, y, button, buttons, modifiers){
        this.$moved = false
        this.$stealMouse = this.$v[MouseArea.PREVENTSTEALING]
        this.$overThreshold = false
        if(!this.$v[MouseArea.ENABLED] || !(button & this.$v[MouseArea.ACCEPTEDBUTTONS])) return false
        this.$longPress = false
        this.$save(x, y, button, buttons, modifiers)
        this.$setDragActive(false)
        this.$setHovered(true)
        let pos = scenePos(this)
        this.$startX = pos.x + x
        this.$startY = pos.y + y
        this.$keepMouseGrab = this.$stealMouse
        let accepted = this.$setPressed(button, true)
        if(accepted) this.$startHoldTimer()
        return accepted
    }

    $startHoldTimer(){
        if(this.$holdTimer) clearTimeout(this.$holdTimer)
        this.$holdTimer = setTimeout(() => {
            this.$holdTimer = 0
            if(this.$f & engine.DESTROYED || engine.isStopped()) return
            engine.batch(() => {
                if(!this.$pressed || this.$dragActive() || !this.$hovered) return
                this.$longPress = true
                let me = this.$event(false)
                me.accepted = connected(this, MouseArea.SIGNAL_pressAndHold)
                engine.emitArgs(this, MouseArea.SIGNAL_pressAndHold, [me])
                if(!me.accepted) this.$propagate(me, MouseArea.SIGNAL_pressAndHold)
                if(!me.accepted) this.$longPress = false
            })
        }, this.$v[MouseArea.PRESSANDHOLDINTERVAL])
    }

    $mouseMove(x, y, buttons, modifiers){
        if(!this.$v[MouseArea.ENABLED] && !this.$pressed) return
        this.$setHovered(contains(this, x, y))
        if(!(buttons & this.$v[MouseArea.ACCEPTEDBUTTONS])) return
        this.$save(x, y, NoButton, buttons, modifiers)
        let drag = this.$v[MouseArea.DRAG]
        let target = drag && drag.$v[MouseDrag.TARGET]
        if(target instanceof Item) this.$drag(drag, target, x, y)
        let me = this.$event(false)
        this.$emitPosition(me)
        engine.emitArgs(this, MouseArea.SIGNAL_positionChanged, [me])
    }

    $drag(drag, target, x, y){
        let parent = target.$parent instanceof Item ? target.$parent : null
        let parentPos = parent ? scenePos(parent) : { x: 0, y: 0 }
        if(!this.$moved) this.$targetStart = { x: parentPos.x + target.$v[Item.X], y: parentPos.y + target.$v[Item.Y] }
        let origin = scenePos(this)
        let sceneX = origin.x + x, sceneY = origin.y + y
        let startLocalX = this.$startX - parentPos.x, startLocalY = this.$startY - parentPos.y
        let curLocalX = sceneX - parentPos.x, curLocalY = sceneY - parentPos.y

        if(this.$keepMouseGrab && this.$stealMouse && this.$overThreshold && !drag.$v[MouseDrag.ACTIVE]) drag.$write(MouseDrag.ACTIVE, true)

        let startX = this.$targetStart.x - parentPos.x, startY = this.$targetStart.y - parentPos.y
        let axis = drag.$v[MouseDrag.AXIS]
        let dragX = target.$v[Item.X], dragY = target.$v[Item.Y]
        let boundedX = dragX, boundedY = dragY
        if(axis & XAxis){
            dragX = startX + curLocalX - startLocalX
            boundedX = Math.max(drag.$v[MouseDrag.MINIMUMX], Math.min(dragX, drag.$v[MouseDrag.MAXIMUMX]))
        }
        if(axis & YAxis){
            dragY = startY + curLocalY - startLocalY
            boundedY = Math.max(drag.$v[MouseDrag.MINIMUMY], Math.min(dragY, drag.$v[MouseDrag.MAXIMUMY]))
        }
        let targetX = target.$v[Item.X], targetY = target.$v[Item.Y]
        if(drag.$v[MouseDrag.ACTIVE]){
            target.x = boundedX
            target.y = boundedY
            let now = scenePos(this)
            this.$lastX = sceneX - now.x
            this.$lastY = sceneY - now.y
        }
        let threshold = drag.$v[MouseDrag.THRESHOLD]
        let overX = Math.abs(dragX - startX) > threshold
        let overY = Math.abs(dragY - startY) > threshold
        if(!this.$overThreshold && ((targetX !== boundedX && overX) || (targetY !== boundedY && overY))){
            this.$overThreshold = true
            if(drag.$v[MouseDrag.SMOOTHED]){
                this.$startX = sceneX
                this.$startY = sceneY
            }
        }
        if(!this.$keepMouseGrab && this.$overThreshold){
            this.$keepMouseGrab = true
            this.$stealMouse = true
        }
        this.$moved = true
    }

    $mouseRelease(x, y, button, buttons, modifiers){
        this.$stealMouse = false
        this.$overThreshold = false
        if(this.$v[MouseArea.ENABLED] || this.$pressed){
            this.$save(x, y, button, buttons, modifiers)
            this.$setPressed(button, false)
            if(!this.$pressed){
                this.$setDragActive(false)
                if(!this.$v[MouseArea.HOVERENABLED]) this.$setHovered(false)
                if(!this.$v[MouseArea.PREVENTSTEALING]) this.$keepMouseGrab = false
                if(this.$holdTimer){
                    clearTimeout(this.$holdTimer)
                    this.$holdTimer = 0
                }
            }
        }
        this.$doubleClick = false
    }

    $mouseDoubleClick(x, y, button, buttons, modifiers){
        if(!this.$v[MouseArea.ENABLED]) return
        this.$save(x, y, button, buttons, modifiers)
        let me = new MouseEvent(x, y, button, buttons, modifiers, true, false)
        let isConnected = connected(this, MouseArea.SIGNAL_doubleClicked)
        me.accepted = isConnected
        engine.emitArgs(this, MouseArea.SIGNAL_doubleClicked, [me])
        if(!me.accepted) this.$propagate(me, MouseArea.SIGNAL_doubleClicked)
        if(this.$pressed) this.$doubleClick = isConnected || me.accepted
    }

    $hoverEnter(x, y, modifiers){
        if(!this.$v[MouseArea.ENABLED] && !this.$pressed) return
        this.$lastX = x
        this.$lastY = y
        this.$lastModifiers = modifiers
        this.$setHovered(true)
        let me = new MouseEvent(x, y, NoButton, NoButton, modifiers, false, false)
        this.$emitPosition(me)
        engine.emitArgs(this, MouseArea.SIGNAL_positionChanged, [me])
    }

    $hoverMove(x, y, modifiers){
        if(!this.$v[MouseArea.ENABLED] && !this.$pressed) return
        if(x === this.$lastX && y === this.$lastY) return
        this.$lastX = x
        this.$lastY = y
        this.$lastModifiers = modifiers
        let me = new MouseEvent(x, y, NoButton, NoButton, modifiers, false, false)
        this.$emitPosition(me)
        engine.emitArgs(this, MouseArea.SIGNAL_positionChanged, [me])
    }

    $hoverLeave(){
        this.$setHovered(false)
    }

    $wheel(x, y, angleDelta, pixelDelta, buttons, modifiers){
        if(!this.$v[MouseArea.ENABLED]) return false
        let we = new WheelEvent(x, y, angleDelta, pixelDelta, buttons, modifiers, false)
        we.accepted = connected(this, MouseArea.SIGNAL_wheel)
        engine.emitArgs(this, MouseArea.SIGNAL_wheel, [we])
        return we.accepted
    }

    $ungrabMouse(){
        ungrab(this)
        if(!this.$pressed) return
        this.$pressed = NoButton
        this.$stealMouse = false
        this.$doubleClick = false
        this.$overThreshold = false
        this.$keepMouseGrab = false
        this.$setDragActive(false)
        this.$emitPressed()
        engine.emit(this, MouseArea.SIGNAL_canceled)
        engine.emit(this, MouseArea.SIGNAL_pressedChanged, true, false)
        engine.emit(this, MouseArea.SIGNAL_containsPressChanged)
        engine.emit(this, MouseArea.SIGNAL_pressedButtonsChanged)
        if(this.$hovered){
            this.$hovered = false
            this.$v[MouseArea.CONTAINSMOUSE] = false
            engine.emit(this, MouseArea.SIGNAL_containsMouseChanged, true, false)
        }
    }

    $disposeNative(){
        if(this.$holdTimer) clearTimeout(this.$holdTimer)
        ungrab(this)
        super.$disposeNative()
    }
}

// pressed / containsMouse / mouseX ... are read only for QML and written by the area itself
const readonly = (kind, initial) => ({ kind, initial, readonly: true })

defineType(MouseArea, {
    props: {
        // "not equivalent to Item.enabled: it only affects mouse events, and does not propagate to child items"
        enabled: { kind: 'bool', initial: true, effect(area, value){ if(!value && area.$pressed) area.$ungrabMouse() } },
        hoverEnabled: 'bool',
        containsMouse: readonly('bool', false),
        containsPress: readonly('bool', false),
        pressed: readonly('bool', false),
        pressedButtons: readonly('int', 0),
        mouseX: readonly('real', 0),
        mouseY: readonly('real', 0),
        acceptedButtons: { kind: 'int', initial: LeftButton },
        propagateComposedEvents: 'bool',
        preventStealing: 'bool',
        scrollGestureEnabled: { kind: 'bool', initial: true },
        pressAndHoldInterval: { kind: 'int', initial: PRESS_AND_HOLD_INTERVAL },
        cursorShape: { kind: 'var', initial: 'default', effect(area){ area.$cursorSet = true } },
        drag: { group: MouseDrag },
    },
    signals: ['pressed(mouse)', 'released(mouse)', 'clicked(mouse)', 'doubleClicked(mouse)', 'pressAndHold(mouse)',
        'positionChanged(mouse)', 'wheel(wheel)', 'entered', 'exited', 'canceled'],
})

// an area that hides itself (in its press handler, say) or is disabled through a parent drops the press
{
    let base = MouseArea.prototype.$write
    MouseArea.prototype.$write = function(id, value){
        base.call(this, id, value)
        if((id === Item.VISIBLE && !this.$v[Item.VISIBLE]) || (id === Item.ENABLED && !this.$v[Item.ENABLED])){
            if(this.$pressed) this.$ungrabMouse()
            else if(this.$hovered) this.$setHovered(false)
        }
    }
}

module.exports = { MouseEvent, WheelEvent, MouseDrag, MouseArea }
