const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { Item } = require('./Item')

// ---- the flick engine (QQuickFlickablePrivate) -------------------------------------------------------------------
//
// Shared by Flickable, ListView and GridView. A view gives
//   $flickBounds(axis) -> { min, max }   the range of contentX / contentY (min <= max): -minXExtent .. -maxXExtent
//   $flickAxis(axis)                     QQuickFlickable::xflick / yflick
//   $contentPos(axis), $setContentPos(axis, value)
//   $fixupTarget(axis, value, state)     optional: where fixup takes the content (ListView snapMode)
// Positions here are content positions (contentX / contentY); Qt's move value is their negative.

const DRAG_THRESHOLD = 10           // QStyleHints::startDragDistance
const FLICK_START_DISTANCE = 15     // QPlatformTheme::FlickStartDistance
const MINIMUM_FLICK_VELOCITY = 75   // _q_MinimumFlickVelocity
const RETAIN_GRAB_VELOCITY = 100
const SAMPLE_BUFFER = 3             // QML_FLICK_SAMPLEBUFFER
const FIXUP_DURATION = 400
const DRAG_OVER_BOUNDS = 1, OVERSHOOT_BOUNDS = 2
const AUTO_FLICK_IF_NEEDED = 0xc

// the clock and the frames of the animations; the tests drive them
const clock = {
    now: () => (typeof performance !== 'undefined' ? performance.now() : Date.now()),
    frame: (fn) => (typeof requestAnimationFrame !== 'undefined' ? requestAnimationFrame(() => fn()) : setTimeout(fn, 16)),
}

const AXES = ['x', 'y']

function flickState(view){
    if(view.$fs) return view.$fs
    let axis = () => ({ pressPos: 0, dragStartOffset: 0, previousDragDelta: 0, velocity: 0, samples: [], moved: false, flicking: false,
        fixingUp: false, animation: null, smoothVelocity: 0, flickingWhenDragBegan: false })
    view.$fs = { pressed: false, stealMouse: false, keepMouseGrab: false, lastPosTime: -1, lastPressTime: 0, pressX: 0, pressY: 0,
        lastX: 0, lastY: 0, dragging: false, moving: false, x: axis(), y: axis() }
    return view.$fs
}

const easeInQuad = (t) => t * t
const easeOutExpo = (t) => t >= 1 ? 1 : 1 - Math.pow(2, -10 * t)

function bounds(view, axis){
    let b = view.$flickBounds(axis)
    return { min: b.min, max: Math.max(b.min, b.max) }
}

function atBeginning(view, axis){ return view.$contentPos(axis) <= bounds(view, axis).min }
function atEnd(view, axis){ return view.$contentPos(axis) >= bounds(view, axis).max }

// ---- signals of the movement: QQuickFlickable::movementStarting, movementEnding, flickingStarted, draggingStarting ----

function setFlag(view, id, value){
    if(view.$v[id] !== value) view.$write(id, value)
}

function movementStarting(view){
    let s = flickState(view)
    if(s.moving) return
    s.moving = true
    let type = view.constructor
    setFlag(view, type.MOVING, true)
    engine.emitArgs(view, type.SIGNAL_movementStarted, [])
}

function animating(s){
    return !!(s.x.animation || s.y.animation)
}

function movementEnding(view){
    let s = flickState(view)
    let type = view.constructor
    let wasFlicking = s.x.flicking || s.y.flicking
    s.x.flicking = s.y.flicking = false
    if(wasFlicking){
        setFlag(view, type.FLICKING, false)
        engine.emitArgs(view, type.SIGNAL_flickEnded, [])
    } else if(s.x.flickingWhenDragBegan || s.y.flickingWhenDragBegan){
        s.x.flickingWhenDragBegan = s.y.flickingWhenDragBegan = false
        engine.emitArgs(view, type.SIGNAL_flickEnded, [])
    }
    if(s.moving && !s.pressed && !s.stealMouse){
        s.moving = false
        s.x.moved = s.y.moved = false
        setFlag(view, type.MOVING, false)
        engine.emitArgs(view, type.SIGNAL_movementEnded, [])
    }
    for(let axis of AXES){
        s[axis].fixingUp = false
        s[axis].smoothVelocity = 0
        s[axis].previousDragDelta = 0
    }
}

function flickingStarted(view, flickingX, flickingY){
    let s = flickState(view)
    if(!flickingX && !flickingY) return
    let was = s.x.flicking || s.y.flicking
    if(flickingX) s.x.flicking = true
    if(flickingY) s.y.flicking = true
    if(!was){
        setFlag(view, view.constructor.FLICKING, true)
        engine.emitArgs(view, view.constructor.SIGNAL_flickStarted, [])
    }
}

function draggingStarting(view){
    let s = flickState(view)
    if(s.dragging || !s.pressed || !(s.x.moved || s.y.moved)) return
    s.dragging = true
    setFlag(view, view.constructor.DRAGGING, true)
    engine.emitArgs(view, view.constructor.SIGNAL_dragStarted, [])
}

function draggingEnding(view){
    let s = flickState(view)
    if(!s.dragging) return
    s.dragging = false
    setFlag(view, view.constructor.DRAGGING, false)
    engine.emitArgs(view, view.constructor.SIGNAL_dragEnded, [])
}

// ---- the timeline: segments of an axis, one frame after the other ----

function resetTimeline(view, axis){
    let s = flickState(view)[axis]
    s.animation = null
    s.smoothVelocity = 0
}

function runTimeline(view, axis, segments, done){
    let s = flickState(view)[axis]
    let animation = { segments, index: 0, start: clock.now(), done }
    s.animation = animation
    let step = () => {
        if(s.animation !== animation || view.$f & engine.DESTROYED || engine.isStopped()) return
        engine.batch(() => {
            let time = clock.now()
            while(animation.index < animation.segments.length){
                let segment = animation.segments[animation.index]
                let elapsed = time - animation.start
                let value = segment.at(Math.min(elapsed, segment.duration))
                s.smoothVelocity = segment.velocity ? segment.velocity(Math.min(elapsed, segment.duration)) : 0
                view.$setContentPos(axis, value)
                if(elapsed < segment.duration) return
                animation.start += segment.duration
                animation.index++
            }
            s.animation = null
            s.smoothVelocity = 0
            if(animation.done) animation.done()
        })
        if(s.animation === animation) clock.frame(step)
    }
    clock.frame(step)
}

// a decelerating move from the current position with the velocity v (content units per second), stopped after
// maxDistance when given (QQuickTimeLine::accel)
function accelSegment(from, v, decel, maxDistance){
    let duration = Math.abs(v) / decel * 1000
    let distance = v * v / (2 * decel)
    if(maxDistance !== undefined && maxDistance < distance){
        // the time at which the distance is reached
        let t = (Math.abs(v) - Math.sqrt(Math.max(0, v * v - 2 * decel * maxDistance))) / decel
        duration = t * 1000
    }
    let sign = Math.sign(v)
    return {
        duration,
        at: (ms) => { let t = ms / 1000; return from + sign * (Math.abs(v) * t - decel * t * t / 2) },
        velocity: (ms) => sign * Math.max(0, Math.abs(v) - decel * ms / 1000),
    }
}

function moveSegment(from, to, duration, easing){
    return { duration, at: (ms) => duration <= 0 ? to : from + (to - from) * easing(ms / duration) }
}

// QQuickFlickablePrivate::fixup: back into the bounds, animated (the first quarter InQuad to halfway, then OutExpo)
function fixup(view, axis, immediate = false){
    let s = flickState(view)
    if(!view.$flickAxis(axis) && !view.$fixupTarget) return endIfIdle(view)
    let pos = view.$contentPos(axis)
    let { min, max } = bounds(view, axis)
    let target = pos < min ? min : pos > max ? max : Math.round(pos)
    if(view.$fixupTarget) target = view.$fixupTarget(axis, target, s[axis])
    if(target === pos) return endIfIdle(view)
    resetTimeline(view, axis)
    if(immediate){
        view.$setContentPos(axis, target)
        return endIfIdle(view)
    }
    s[axis].fixingUp = true
    let half = target - (target - pos) / 2
    runTimeline(view, axis, [moveSegment(pos, half, FIXUP_DURATION / 4, easeInQuad), moveSegment(half, target, 3 * FIXUP_DURATION / 4, easeOutExpo)], () => {
        s[axis].fixingUp = false
        endIfIdle(view)
    })
}

// QQuickFlickable::geometryChange: "Make sure that we're entirely in view" - a new width or height puts the content
// back within its bounds at once, unless it is pressed or moving
function geometryFixup(view, axis){
    let s = view.$fs
    if(s && (s.pressed || s.moving)) return
    let { min, max } = bounds(view, axis)
    let pos = view.$contentPos(axis)
    if(pos < min || pos > max) fixup(view, axis, true)
}

function endIfIdle(view){
    let s = flickState(view)
    if(!s.pressed && !animating(s)) movementEnding(view)
}

// QQuickFlickablePrivate::flick: v is the velocity of the pointer (positive down / right), the content goes the other way
function flick(view, axis, v){
    let s = flickState(view)
    let pos = view.$contentPos(axis)
    let { min, max } = bounds(view, axis)
    let contentVelocity = -v
    let maxDistance = contentVelocity < 0 ? Math.abs(pos - min) : Math.abs(max - pos)
    let overshoot = view.$v[view.constructor.BOUNDSBEHAVIOR] & OVERSHOOT_BOUNDS
    if(!(maxDistance > 0 || overshoot)){
        resetTimeline(view, axis)
        fixup(view, axis)
        return false
    }
    let maxVelocity = view.$v[view.constructor.MAXIMUMFLICKVELOCITY]
    if(maxVelocity !== -1 && maxVelocity < Math.abs(contentVelocity)) contentVelocity = Math.sign(contentVelocity) * maxVelocity
    let decel = view.$v[view.constructor.FLICKDECELERATION]
    // adjust the deceleration so that the flick ends on a whole pixel
    let dist = contentVelocity * contentVelocity / (2 * decel)
    let target = Math.round(pos + Math.sign(contentVelocity) * dist)
    if(view.$flickTarget) target = view.$flickTarget(axis, target, contentVelocity, s[axis])
    dist = Math.abs(target - pos)
    if(dist === 0){
        fixup(view, axis)
        return false
    }
    decel = contentVelocity * contentVelocity / (2 * dist)
    resetTimeline(view, axis)
    let segment = accelSegment(pos, contentVelocity, decel, overshoot ? undefined : maxDistance)
    runTimeline(view, axis, [segment], () => fixup(view, axis))
    return !s[axis].flicking && view.$flickAxis(axis)
}

// ---- the pointer: QQuickFlickablePrivate::handlePressEvent, handleMoveEvent (drag), handleReleaseEvent ----

function pressFlick(view, x, y){
    let s = flickState(view)
    let now = clock.now()
    let fast = AXES.some((axis) => s[axis].animation && Math.abs(s[axis].smoothVelocity) > RETAIN_GRAB_VELOCITY && !s[axis].fixingUp)
    // pressed again while flicking: the press stops the flick and is not given to the children
    s.stealMouse = view.$v[view.constructor.INTERACTIVE] && fast
    s.keepMouseGrab = s.stealMouse
    s.pressed = true
    for(let axis of AXES){
        let a = s[axis]
        if(!a.fixingUp) resetTimeline(view, axis)
        a.pressPos = view.$contentPos(axis)
        a.dragStartOffset = 0
        a.previousDragDelta = 0
        a.velocity = 0
        a.samples = []
        a.flickingWhenDragBegan = a.flicking
        a.flicking = false
    }
    if(s.x.flickingWhenDragBegan || s.y.flickingWhenDragBegan) setFlag(view, view.constructor.FLICKING, false)
    s.pressX = s.lastX = x
    s.pressY = s.lastY = y
    s.lastPosTime = s.lastPressTime = now
    return s.stealMouse
}

function dragAxis(view, axis, delta, overThreshold, elapsedSincePress){
    let s = flickState(view)
    let a = s[axis]
    let result = { reject: false, steal: false, keep: true }
    if(!(overThreshold || elapsedSincePress > 200)){
        a.previousDragDelta = delta
        return result
    }
    if(!a.moved) a.dragStartOffset = delta
    let pos = a.pressPos - (delta - a.dragStartOffset)
    let { min, max } = bounds(view, axis)
    let current = view.$contentPos(axis)
    if(!(view.$v[view.constructor.BOUNDSBEHAVIOR] & DRAG_OVER_BOUNDS)){
        if(pos >= max){
            pos = max
            result.reject = a.pressPos === max && current === max && delta < 0
        }
        if(pos <= min){
            pos = min
            result.reject = result.reject || (a.pressPos === min && current === min && delta > 0)
        }
    } else if(pos < min) pos = min - (min - pos) / 2
    else if(pos > max) pos = max + (pos - max) / 2
    if(!result.reject && s.stealMouse && delta !== a.previousDragDelta){
        resetTimeline(view, axis)
        view.$setContentPos(axis, pos)
        a.moved = true
    }
    if(!result.reject && overThreshold) result.steal = true
    if((pos <= min && a.pressPos === min && current === min && delta > 0) || (pos >= max && a.pressPos === max && current === max && delta < 0)) result.keep = false
    a.previousDragDelta = delta
    return result
}

// returns whether the view takes the mouse (stealMouse)
function moveFlick(view, x, y, buttons){
    let s = flickState(view)
    if(!view.$v[view.constructor.INTERACTIVE] || s.lastPosTime === -1 || !(buttons & 1)) return false
    let now = clock.now()
    let dx = x - s.pressX, dy = y - s.pressY
    let overThreshold = s.moving
        || (view.$flickAxis('y') && Math.abs(dy) > DRAG_THRESHOLD) || (view.$flickAxis('x') && Math.abs(dx) > DRAG_THRESHOLD)
    let dt = Math.max(1, now - s.lastPosTime)
    let velocity = { x: (x - s.lastX) / dt * 1000, y: (y - s.lastY) / dt * 1000 }
    let wasMoved = s.x.moved || s.y.moved
    let elapsed = now - s.lastPressTime
    let steal = false, keep = false
    let rejected = { x: false, y: false }
    for(let axis of AXES){
        if(!view.$flickAxis(axis)) continue
        let r = dragAxis(view, axis, axis === 'x' ? dx : dy, overThreshold, elapsed)
        rejected[axis] = r.reject
        if(r.steal){
            steal = true
            if(r.keep) keep = true
        }
    }
    s.stealMouse = s.stealMouse || steal
    if(s.stealMouse && keep) s.keepMouseGrab = true
    let maxVelocity = view.$v[view.constructor.MAXIMUMFLICKVELOCITY]
    for(let axis of AXES){
        let a = s[axis]
        if(rejected[axis]){
            a.samples = []
            a.velocity = 0
        } else if(view.$flickAxis(axis)){
            // QQuickFlickablePrivate::AxisData::addVelocitySample
            let v = velocity[axis]
            if(maxVelocity > 0) v = Math.max(-maxVelocity, Math.min(maxVelocity, v))
            a.samples.push(v)
            if(a.samples.length > SAMPLE_BUFFER) a.samples.shift()
        }
    }
    draggingStarting(view)
    if(!wasMoved && (s.x.moved || s.y.moved)) movementStarting(view)
    s.lastPosTime = now
    s.lastX = x
    s.lastY = y
    return s.stealMouse
}

function releaseFlick(view, x, y, grabbed){
    let s = flickState(view)
    s.stealMouse = false
    s.keepMouseGrab = false
    s.pressed = false
    let elapsed = clock.now() - s.lastPosTime
    for(let axis of AXES){
        let a = s[axis]
        a.velocity = a.samples.length ? a.samples.reduce((sum, v) => sum + v, 0) / a.samples.length : 0
    }
    draggingEnding(view)
    if(s.lastPosTime === -1) return endIfIdle(view)
    let flicked = { x: false, y: false }
    for(let axis of AXES){
        let a = s[axis]
        let v = elapsed < 100 && a.velocity !== 0 ? a.velocity : 0
        if((atBeginning(view, axis) && v > 0) || (atEnd(view, axis) && v < 0)) v /= 2
        let distance = axis === 'x' ? x - s.pressX : y - s.pressY
        if(grabbed && view.$flickAxis(axis) && Math.abs(v) > MINIMUM_FLICK_VELOCITY && Math.abs(distance) > FLICK_START_DISTANCE){
            flicked[axis] = flick(view, axis, v)
        }
    }
    s.lastPosTime = -1
    if(!flicked.y) fixup(view, 'y')
    if(!flicked.x) fixup(view, 'x')
    flickingStarted(view, flicked.x, flicked.y)
    endIfIdle(view)
}

// the mouse went elsewhere (QQuickFlickable::mouseUngrabEvent, cancelInteraction)
function cancelFlick(view){
    let s = flickState(view)
    if(!s.pressed) return
    s.pressed = false
    s.stealMouse = false
    s.keepMouseGrab = false
    s.lastPosTime = -1
    draggingEnding(view)
    fixup(view, 'x')
    fixup(view, 'y')
    endIfIdle(view)
}

// QQuickFlickable::childMouseEventFilter (filterPointerEvent): the view watches the presses, moves and releases of its
// children; it takes the mouse from the child once a drag goes past the threshold (not from a MouseArea that
// prevents stealing or drags something, nor from an item that keeps the grab). grab: the view takes the mouse from
// the receiver; filter: the event does not go to the receiver
const NO_FILTER = { filter: false, grab: false }
function filterPointer(view, kind, receiver, x, y, buttons){
    if(!view.$v[Item.VISIBLE] || !view.$v[Item.ENABLED] || !view.$v[view.constructor.INTERACTIVE]){
        cancelFlick(view)
        return NO_FILTER
    }
    let s = flickState(view)
    let receiverKeepsGrab = !!(receiver && receiver.$keepMouseGrab)
    let receiverRelinquishGrab = false
    if(receiver && receiver.$isMouseArea){
        let drag = receiver.$v[receiver.constructor.DRAG]
        let preventStealing = receiver.$v[receiver.constructor.PREVENTSTEALING] || !!(drag && drag.$v[drag.constructor.TARGET])
        if(!preventStealing && receiverKeepsGrab){
            receiverRelinquishGrab = receiver.$v[Item.ENABLED]
            if(receiverRelinquishGrab) receiverKeepsGrab = false
        }
    }
    let receiverDisabled = !!(receiver && !receiver.$v[Item.ENABLED])
    let stealThisEvent = s.stealMouse
    let inside = x >= 0 && y >= 0 && x <= view.$v[Item.WIDTH] && y <= view.$v[Item.HEIGHT]
    if((stealThisEvent || inside) && (!receiverKeepsGrab || receiverDisabled)){
        if(kind === 'press'){
            pressFlick(view, x, y)
            s.stealMouse = false
            stealThisEvent = false
        } else if(kind === 'move'){
            moveFlick(view, x, y, buttons)
        } else if(kind === 'release'){
            releaseFlick(view, x, y, false)
            stealThisEvent = s.stealMouse
        }
        return {
            filter: s.moving || (!receiverRelinquishGrab && (stealThisEvent || receiverDisabled)),
            grab: (stealThisEvent && !receiverKeepsGrab) || receiverDisabled,
        }
    } else if(s.lastPosTime !== -1){
        s.lastPosTime = -1
        fixup(view, 'x')
        fixup(view, 'y')
    }
    if(kind === 'release' || (receiverKeepsGrab && !receiverDisabled)){
        s.lastPosTime = -1
        s.stealMouse = false
        s.pressed = false
    }
    return NO_FILTER
}

// the mouse of the view itself (QQuickFlickable::mousePressEvent and on): it takes a left press when interactive
const flickMouse = {
    $mousePress(x, y, button){
        if(!this.$v[this.constructor.INTERACTIVE] || button !== 1) return false
        pressFlick(this, x, y)
        return true
    },
    $mouseMove(x, y, buttons){
        let s = flickState(this)
        if(s.pressed) moveFlick(this, x, y, buttons | 1)
    },
    $mouseRelease(x, y){
        if(flickState(this).pressed) releaseFlick(this, x, y, true)
    },
    $ungrabMouse(){
        cancelFlick(this)
    },
    $filterPointer(kind, receiver, x, y, buttons){
        return filterPointer(this, kind, receiver, x, y, buttons)
    },
    get $keepMouseGrab(){
        return !!(this.$fs && this.$fs.keepMouseGrab)
    },
}

function installFlick(Class){
    for(let name of Object.getOwnPropertyNames(flickMouse)){
        Object.defineProperty(Class.prototype, name, Object.getOwnPropertyDescriptor(flickMouse, name))
    }
    // QQuickFlickable::returnToBounds
    if(!Class.prototype.returnToBounds) Class.prototype.returnToBounds = function(){ fixup(this, 'x'); fixup(this, 'y') }
    // QQuickFlickable::flick, cancelFlick
    Class.prototype.flick = function(xVelocity, yVelocity){
        // a positive velocity moves the content towards its start, like a pointer moving down / right
        let flickedX = this.$flickAxis('x') && xVelocity ? flick(this, 'x', xVelocity) : false
        let flickedY = this.$flickAxis('y') && yVelocity ? flick(this, 'y', yVelocity) : false
        if(flickedX || flickedY) movementStarting(this)
        flickingStarted(this, flickedX, flickedY)
    }
    Class.prototype.cancelFlick = function(){
        resetTimeline(this, 'x')
        resetTimeline(this, 'y')
        movementEnding(this)
    }
}

// QQuickFlickable::xflick / yflick: AutoFlickDirection flicks along an axis where the content size differs from the
// view's, AutoFlickIfNeeded where it is larger; else the direction set
function flickDirection(direction, contentSize, viewSize, axis){
    if((direction & AUTO_FLICK_IF_NEEDED) && contentSize > viewSize) return true
    if(direction === 0) return Math.floor(Math.abs(contentSize - viewSize)) !== 0
    return !!(direction & (axis === 'x' ? 1 : 2))
}

// ---- Flickable ------------------------------------------------------------------------------

// "Items declared as children of a Flickable are automatically parented to the Flickable's contentItem."
// contentWidth / contentHeight -1: the content item follows the Flickable's size
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

    // QQuickFlickable::minXExtent = 0, maxXExtent = min(0, width - contentWidth)
    $flickBounds(axis){
        let content = this.$content ? this.$content.$v[axis === 'x' ? Item.WIDTH : Item.HEIGHT] : 0
        let size = this.$v[axis === 'x' ? Item.WIDTH : Item.HEIGHT]
        return { min: 0, max: Math.max(0, content - size) }
    }

    $flickAxis(axis){
        let content = this.$content ? this.$content.$v[axis === 'x' ? Item.WIDTH : Item.HEIGHT] : 0
        return flickDirection(this.$v[Flickable.FLICKABLEDIRECTION], content, this.$v[axis === 'x' ? Item.WIDTH : Item.HEIGHT], axis)
    }

    $contentPos(axis){ return this.$v[axis === 'x' ? Flickable.CONTENTX : Flickable.CONTENTY] }

    $setContentPos(axis, value){
        if(axis === 'x') this.contentX = value
        else this.contentY = value
    }

    $updateBounds(){
        let x = this.$v[Flickable.CONTENTX], y = this.$v[Flickable.CONTENTY]
        let bx = this.$flickBounds('x'), by = this.$flickBounds('y')
        this.$write(Flickable.ATXBEGINNING, x <= bx.min)
        this.$write(Flickable.ATXEND, x >= bx.max)
        this.$write(Flickable.ATYBEGINNING, y <= by.min)
        this.$write(Flickable.ATYEND, y >= by.max)
    }

    resizeContent(width, height, center){
        this.contentWidth = width
        this.contentHeight = height
    }

    $wheel(x, y, angleDelta, pixelDelta){
        if(!this.$v[Flickable.INTERACTIVE]) return false
        return wheelScroll(this, angleDelta, pixelDelta)
    }
}
installFlick(Flickable)

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

function wheelScroll(view, angleDelta, pixelDelta){
    let dx = pixelDelta.x || angleDelta.x / 120 * 100
    let dy = pixelDelta.y || angleDelta.y / 120 * 100
    let bx = bounds(view, 'x'), by = bounds(view, 'y')
    let x = view.$contentPos('x'), y = view.$contentPos('y')
    let nx = view.$flickAxis('x') ? Math.max(bx.min, Math.min(bx.max, x - dx)) : x
    let ny = view.$flickAxis('y') ? Math.max(by.min, Math.min(by.max, y - dy)) : y
    if(nx === x && ny === y) return false
    wheelMovement(view)
    if(nx !== x) view.$setContentPos('x', nx)
    if(ny !== y) view.$setContentPos('y', ny)
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

const FLICK_PROPS = {
    interactive: { kind: 'bool', initial: true },
    boundsBehavior: { kind: 'int', initial: 3 },
    boundsMovement: { kind: 'int', initial: 1 },
    moving: { kind: 'bool', readonly: true },
    flicking: { kind: 'bool', readonly: true },
    dragging: { kind: 'bool', readonly: true },
    maximumFlickVelocity: { kind: 'real', initial: 2500 },
    flickDeceleration: { kind: 'real', initial: 1500 },
    pressDelay: 'int',
}
const FLICK_SIGNALS = ['movementStarted', 'movementEnded', 'flickStarted', 'flickEnded', 'dragStarted', 'dragEnded']

defineType(Flickable, {
    props: Object.assign({
        contentX: { kind: 'real', effect: moved },
        contentY: { kind: 'real', effect: moved },
        contentWidth: { kind: 'real', initial: -1, effect: contentSize },
        contentHeight: { kind: 'real', initial: -1, effect: contentSize },
        originX: { kind: 'real', readonly: true },
        originY: { kind: 'real', readonly: true },
        flickableDirection: 'int',
        atXBeginning: { kind: 'bool', initial: true, readonly: true },
        atXEnd: { kind: 'bool', initial: true, readonly: true },
        atYBeginning: { kind: 'bool', initial: true, readonly: true },
        atYEnd: { kind: 'bool', initial: true, readonly: true },
    }, FLICK_PROPS),
    signals: FLICK_SIGNALS,
})
const FLICK_ENUMS = {
    AutoFlickDirection: 0, AutoFlickIfNeeded: 0xc, HorizontalFlick: 1, VerticalFlick: 2, HorizontalAndVerticalFlick: 3,
    StopAtBounds: 0, DragOverBounds: 1, OvershootBounds: 2, DragAndOvershootBounds: 3, FollowBoundsBehavior: 1, StopAtBoundsMovement: 0,
}
Object.assign(Flickable, FLICK_ENUMS)

{
    let base = Flickable.prototype.$write
    Flickable.prototype.$write = function(id, value){
        let old = this.$v[id]
        base.call(this, id, value)
        if(id === Item.WIDTH || id === Item.HEIGHT){
            this.$syncContentSize()
            if(old !== this.$v[id]) geometryFixup(this, id === Item.WIDTH ? 'x' : 'y')
        }
    }
}

module.exports = { Flickable, wheelScroll, installFlick, flickDirection, flickState, fixup, FLICK_PROPS, FLICK_SIGNALS, FLICK_ENUMS, clock, DRAG_THRESHOLD }
