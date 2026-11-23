// SwipeDelegate of QtQuick.Templates (qquickswipedelegate.cpp, Qt 6.12). Checked against Qt 6.12
// (tests/diff/cases/swipedelegate.qml):
//  - swipe.left / right / behind: components made on demand as children of the delegate (z -2), the one of the side
//    swiped to shown; the content item and the background move by position * the width of that item;
//  - a horizontal drag past startDragDistance takes the mouse (pressed, canceled on the release, no click): position is
//    the distance over the width of the item (from -1 or 1 when it was open); the release opens it past 0.5 (or with a
//    speed over 300 px/s), else closes it, through swipe.transition (the Basic style: SmoothedAnimation velocity 3);
//  - complete when at -1 or 1 (completed, opened), closed when back at 0; swipe.open(side) / close() (none while
//    pressed; close() waits for its transition);
//  - the attached SwipeDelegate.pressed / clicked() of the items in left / right / behind.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { GroupObject } = require('../../Core/QtObject')
const { Item } = require('../Item')
const { Control } = require('./Control')
const { ItemDelegate } = require('./AbstractButton')
const { clock, DRAG_THRESHOLD } = require('../Flickable')

const Left = 1, Right = -1
const EXPOSE_VELOCITY = 300
const fuzzyNull = (v) => Math.abs(v) <= 1e-12
const easeInOutCubic = (t) => (t < 0.5 ? 4 * t * t * t : 1 - Math.pow(-2 * t + 2, 3) / 2)
const isIn = (child, item) => { for(let it = child; it; it = it.$parent) if(it === item) return true; return false }

// ---- swipe (QQuickSwipe) ----
class Swipe extends GroupObject {
    constructor(owner){
        super(owner)
        this.$v[Swipe.ENABLED] = true
        this.$positionBeforePress = 0
        this.$wasComplete = false
        this.$waitForTransition = false
        this.$animation = null
    }
    get $control(){ return this.$owner }
    $write(id, value){
        let d = this.$v
        if(id === Swipe.LEFT || id === Swipe.RIGHT || id === Swipe.BEHIND){
            if(value === d[id]) return
            // left / right and behind do not mix; the delegates change only while closed
            if((id === Swipe.BEHIND && (d[Swipe.LEFT] || d[Swipe.RIGHT])) || (id !== Swipe.BEHIND && d[Swipe.BEHIND])){
                console.warn('SwipeDelegate: cannot set both behind and left/right properties')
                return
            }
            if(!fuzzyNull(d[Swipe.POSITION])){
                console.warn('SwipeDelegate: left/right/behind properties may only be set when swipe.position is 0')
                return
            }
            super.$write(id, value || null)
            let itemId = id === Swipe.LEFT ? Swipe.LEFTITEM : id === Swipe.RIGHT ? Swipe.RIGHTITEM : Swipe.BEHINDITEM
            if(!value && d[itemId]) this.$setItem(itemId, null)
            return
        }
        if(id === Swipe.LEFTITEM || id === Swipe.RIGHTITEM || id === Swipe.BEHINDITEM) return this.$setItem(id, value)
        if(id === Swipe.POSITION) return this.$setPosition(value)
        super.$write(id, value)
    }
    // QQuickSwipe::setLeftItem...: the old one deleted, the new one a child of the delegate, z -2 unless set
    $setItem(id, item){
        let old = this.$v[id]
        if(old === item) return
        if(old && !(old.$f & engine.DESTROYED)) old.destroy()
        super.$write(id, item || null)
        if(item){
            if(item.$parent !== this.$control) item.$setParent(this.$control)
            if(item.$v[Item.Z] === 0) item.$write(Item.Z, -2)
        }
    }
    $createItem(id, componentId){
        if(this.$v[id] || !this.$v[componentId]) return this.$v[id]
        let item = this.$v[componentId].createObject(this.$control)
        if(item) this.$setItem(id, item)
        return this.$v[id]
    }
    $show(id, hide){
        let item = this.$v[id]
        if(item) item.$write(Item.VISIBLE, true)
        let other = hide && this.$v[hide]
        if(other) other.$write(Item.VISIBLE, false)
        return item
    }
    // QQuickSwipePrivate::showRelevantItemForPosition
    $relevantItemForPosition(position){
        let d = this.$v
        if(fuzzyNull(position)) return null
        if(d[Swipe.BEHIND]){ this.$createItem(Swipe.BEHINDITEM, Swipe.BEHIND); return this.$show(Swipe.BEHINDITEM) }
        if(d[Swipe.RIGHT] && position < 0){ this.$createItem(Swipe.RIGHTITEM, Swipe.RIGHT); return this.$show(Swipe.RIGHTITEM, Swipe.LEFTITEM) }
        if(d[Swipe.LEFT] && position > 0){ this.$createItem(Swipe.LEFTITEM, Swipe.LEFT); return this.$show(Swipe.LEFTITEM, Swipe.RIGHTITEM) }
        return null
    }
    // QQuickSwipePrivate::createRelevantItemForDistance
    $relevantItemForDistance(distance){
        let d = this.$v, before = this.$positionBeforePress
        if(fuzzyNull(distance)) return null
        if(d[Swipe.BEHIND]) return this.$createItem(Swipe.BEHINDITEM, Swipe.BEHIND)
        let leftItem = d[Swipe.LEFTITEM], rightItem = d[Swipe.RIGHTITEM]
        if(d[Swipe.RIGHT] && ((distance < 0 && before === 0) || (rightItem && before === -1 && distance < rightItem.width)
            || (leftItem && before === 1 && Math.abs(distance) > leftItem.width))) return this.$createItem(Swipe.RIGHTITEM, Swipe.RIGHT)
        if(d[Swipe.LEFT] && ((distance > 0 && before === 0) || (leftItem && before === 1 && Math.abs(distance) < leftItem.width)
            || (rightItem && before === -1 && Math.abs(distance) > rightItem.width))) return this.$createItem(Swipe.LEFTITEM, Swipe.LEFT)
        return null
    }
    // QQuickSwipePrivate::reposition: the content and the background by position * the width of the item shown
    $reposition(){
        let control = this.$control
        let item = this.$relevantItemForPosition(this.$v[Swipe.POSITION])
        let width = item ? item.$v[Item.WIDTH] : 0
        let x = this.$v[Swipe.POSITION] * width
        let content = control.$v[Control.CONTENTITEM], background = control.$v[Control.BACKGROUND]
        if(content) content.$write(Item.X, x + control.$v[Control.LEFTPADDING])
        if(background) background.$write(Item.X, x)
    }
    $setPosition(position){
        position = Math.max(-1, Math.min(1, Number(position) || 0))
        if(position === this.$v[Swipe.POSITION]) return
        super.$write(Swipe.POSITION, position)
        this.$reposition()
    }
    $setComplete(complete){
        if(complete === this.$v[Swipe.COMPLETE]) return
        super.$write(Swipe.COMPLETE, complete)
        if(complete) engine.emit(this, Swipe.SIGNAL_completed)
    }
    $hasDelegates(){ return !!(this.$v[Swipe.LEFT] || this.$v[Swipe.RIGHT] || this.$v[Swipe.BEHIND]) }
    $isTransitioning(){ return !!this.$animation }
    // QQuickSwipePrivate::beginTransition: the transition of the swipe (its SmoothedAnimation velocity), else at once
    $beginTransition(position){
        if(this.$waitForTransition) return
        let transition = this.$v[Swipe.TRANSITION]
        if(!transition){
            this.$setPosition(position)
            this.$finishTransition()
            return
        }
        let velocity = this.$transitionVelocity(transition)
        let from = this.$v[Swipe.POSITION]
        let duration = velocity > 0 ? Math.abs(position - from) / velocity * 1000 : 250
        let animation = { start: clock.now() }
        this.$animation = animation
        let step = () => {
            if(this.$animation !== animation || this.$control.$f & engine.DESTROYED || engine.isStopped()) return
            let t = duration > 0 ? Math.min(1, (clock.now() - animation.start) / duration) : 1
            engine.batch(() => this.$setPosition(from + (position - from) * easeInOutCubic(t)))
            if(t < 1) return clock.frame(step)
            this.$animation = null
            engine.batch(() => this.$finishTransition())
        }
        clock.frame(step)
    }
    $transitionVelocity(transition){
        let animations = transition.$children || []
        for(let a of animations) if(a.velocity !== undefined) return a.velocity
        return 3
    }
    $cancelTransition(){ this.$animation = null }
    // QQuickSwipePrivate::finishTransition
    $finishTransition(){
        this.$waitForTransition = false
        let position = this.$v[Swipe.POSITION]
        this.$setComplete(Math.abs(Math.abs(position) - 1) < 1e-12)
        if(this.$v[Swipe.COMPLETE]) engine.emit(this, Swipe.SIGNAL_opened)
        else {
            if(fuzzyNull(position)) this.$wasComplete = false
            engine.emit(this, Swipe.SIGNAL_closed)
        }
    }
    open(side){
        let d = this.$v
        if(Math.abs(Math.abs(d[Swipe.POSITION]) - 1) < 1e-12) return
        if((side !== Left && side !== Right) || (!d[Swipe.LEFT] && !d[Swipe.BEHIND] && side === Left) || (!d[Swipe.RIGHT] && !d[Swipe.BEHIND] && side === Right)) return
        engine.batch(() => this.$beginTransition(side))
        this.$wasComplete = true
        this.$positionBeforePress = d[Swipe.POSITION]
    }
    close(){
        if(fuzzyNull(this.$v[Swipe.POSITION]) || this.$control.pressed) return
        engine.batch(() => this.$beginTransition(0))
        this.$waitForTransition = true
        this.$wasComplete = false
        this.$positionBeforePress = 0
    }
}
defineType(Swipe, {
    props: {
        position: 'real', complete: { kind: 'bool', readonly: true }, enabled: 'bool', transition: { kind: 'var', initial: null },
        left: { kind: 'component', initial: null }, behind: { kind: 'component', initial: null }, right: { kind: 'component', initial: null },
        leftItem: { kind: 'var', initial: null }, behindItem: { kind: 'var', initial: null }, rightItem: { kind: 'var', initial: null },
    },
    signals: ['completed', 'opened', 'closed'],
})

// ---- the attached SwipeDelegate.pressed, clicked() ----
class SwipeDelegateAttached extends GroupObject {}
defineType(SwipeDelegateAttached, { props: { pressed: 'bool' }, signals: ['clicked'] })
const attachedProp = Item.$type.propByName.get('SwipeDelegate')
attachedProp.group = SwipeDelegateAttached
Object.defineProperty(Item.prototype, 'SwipeDelegate', {
    get(){ return this.$v[attachedProp.id] || (this.$v[attachedProp.id] = new SwipeDelegateAttached(this)) },
    configurable: true,
})
Object.assign(SwipeDelegateAttached.prototype, { Left, Right })

// the point (x, y of the delegate) in an item
function containsPoint(delegate, item, x, y){
    let p = delegate.mapToItem(item, x, y)
    return p.x >= 0 && p.y >= 0 && p.x <= item.$v[Item.WIDTH] && p.y <= item.$v[Item.HEIGHT]
}

// ---- SwipeDelegate (QQuickSwipeDelegate) ----
class SwipeDelegate extends ItemDelegate {
    constructor(parent = null){
        super(parent)
        this.$v[SwipeDelegate.SWIPE] = new Swipe(this)
    }
    get swipe(){ return this.$v[SwipeDelegate.SWIPE] }
    // QQuickSwipeDelegatePrivate::resizeContent: complete, the content keeps its x
    $resizeContent(){
        let swipe = this.$v[SwipeDelegate.SWIPE]
        if(!swipe || !swipe.$v[Swipe.COMPLETE]) super.$resizeContent()
        else {
            let content = this.$v[Control.CONTENTITEM]
            if(content){
                content.$write(Item.Y, this.$v[Control.TOPPADDING])
                content.$write(Item.WIDTH, this.$v[Control.AVAILABLEWIDTH])
                content.$write(Item.HEIGHT, this.$v[Control.AVAILABLEHEIGHT])
            }
        }
        if(swipe && !fuzzyNull(swipe.$v[Swipe.POSITION])) swipe.$reposition()
    }
    $write(id, value){
        let old = this.$v[Item.WIDTH]
        super.$write(id, value)
        // QQuickSwipeDelegate::geometryChange: a new width repositions
        let swipe = this.$v[SwipeDelegate.SWIPE]
        if(id === Item.WIDTH && old !== this.$v[Item.WIDTH] && swipe && this.$f & engine.COMPLETED) swipe.$reposition()
    }
    $componentComplete(){
        super.$componentComplete()
        this.$v[SwipeDelegate.SWIPE].$reposition()
    }
    get $keepMouseGrab(){ return !!this.$keepGrab }

    // QQuickSwipeDelegate::mousePressEvent
    $mousePress(x, y, button){
        if(!super.$mousePress(x, y, button)) return false
        let swipe = this.$v[SwipeDelegate.SWIPE]
        if(!swipe.$v[Swipe.ENABLED]) return true
        swipe.$positionBeforePress = swipe.$v[Swipe.POSITION]
        this.$swipePress = { x, y, samples: [{ x, time: clock.now() }] }
        // open: the press on the right or left item (getPressedItem) goes to its attached objects
        this.$pressedItem = null
        if(swipe.$v[Swipe.COMPLETE]){
            for(let item of [swipe.$v[Swipe.RIGHTITEM], swipe.$v[Swipe.LEFTITEM]]){
                if(item && item.$v[Item.VISIBLE] && containsPoint(this, item, x, y)){ this.$pressedItem = item; break }
            }
            if(this.$pressedItem) this.$setAttachedPressed(this.$pressedItem, x, y, true)
        }
        return true
    }
    $mouseMove(x, y){
        if(this.$v[SwipeDelegate.SWIPE].$hasDelegates()) this.$handleSwipeMove(this, x, y)
        else super.$mouseMove(x, y)
    }
    $mouseRelease(x, y, button){
        if(!this.$v[SwipeDelegate.SWIPE].$hasDelegates() || !this.$handleSwipeRelease(this, x, y)) super.$mouseRelease(x, y, button)
        if(this.$pressedItem) this.$setAttachedPressed(this.$pressedItem, x, y, false)
        this.$pressedItem = null
        this.$swipePress = null
    }
    $ungrabMouse(){
        super.$ungrabMouse()
        this.$keepGrab = false
        let swipe = this.$v[SwipeDelegate.SWIPE]
        for(let item of [swipe.$v[Swipe.RIGHTITEM], swipe.$v[Swipe.LEFTITEM]]){
            if(!item) continue
            let attached = item.$v[attachedProp.id]
            if(attached) attached.pressed = false
            break
        }
    }
    // QQuickSwipeDelegatePrivate::handleMouseMoveEvent (x, y of the delegate)
    $handleSwipeMove(item, x, y){
        let swipe = this.$v[SwipeDelegate.SWIPE]
        let press = this.$swipePress
        if(press && this.$holdTimer && Math.hypot(x - press.x, y - press.y) > DRAG_THRESHOLD) this.$stopPressAndHold()
        if(!swipe.$v[Swipe.ENABLED] || !this.$v[Item.WIDTH] || !swipe.$hasDelegates() || !press) return false
        if(item === this && !this.$v[ItemDelegate.PRESSED] && !this.$keepGrab) return false
        let distance = x - press.x
        press.samples.push({ x, time: clock.now() })
        if(press.samples.length > 5) press.samples.shift()
        if(!this.$keepGrab && Math.abs(distance) > DRAG_THRESHOLD){
            this.$keepGrab = true
            this.$setPressed(true)
            swipe.$setComplete(false)
            this.$setAttachedPressed(item, x, y, false, true)
        }
        if(this.$keepGrab){
            let before = swipe.$positionBeforePress
            if(swipe.$v[Swipe.BEHIND] || ((swipe.$v[Swipe.LEFT] || swipe.$v[Swipe.RIGHT])
                && (fuzzyNull(before) || (before === -1 && distance >= 0) || (before === 1 && distance <= 0)))){
                let relevant = swipe.$relevantItemForDistance(distance)
                let normalized = relevant && relevant.$v[Item.WIDTH] ? distance / relevant.$v[Item.WIDTH] : 0
                let position
                if(fuzzyNull(normalized)) position = fuzzyNull(distance) ? before : 0
                else if(!swipe.$wasComplete) position = normalized
                else position = distance > 0 ? normalized - 1 : normalized + 1
                swipe.$cancelTransition()
                swipe.$setPosition(position)
            }
        } else if(y < 0 || y > this.$v[Item.HEIGHT]){
            this.$setPressed(false)
        }
        return this.$keepGrab
    }
    // QQuickSwipeDelegatePrivate::handleMouseReleaseEvent: a drag is canceled; open past 0.5 or fast enough, else close
    $handleSwipeRelease(item, x, y){
        let swipe = this.$v[SwipeDelegate.SWIPE]
        let press = this.$swipePress
        let grabbed = !!this.$keepGrab
        this.$keepGrab = false
        let position = swipe.$v[Swipe.POSITION]
        if(!fuzzyNull(position)) this.$setPressed(false)
        if(grabbed){
            this.$setPressed(false)
            this.$pressButtons = 0
            this.$stopPressRepeat()
            this.$stopPressAndHold()
            engine.emit(this, ItemDelegate.SIGNAL_canceled)
        }
        // QQuickVelocityCalculator: from the press to the release
        let velocity = 0
        if(press){
            let elapsed = clock.now() - press.samples[0].time
            if(elapsed > 0) velocity = (x - press.x) / elapsed * 1000
        }
        if(position > 0.5 || (position > 0 && velocity > EXPOSE_VELOCITY)){
            swipe.$beginTransition(1)
            swipe.$wasComplete = true
        } else if(position < -0.5 || (position < 0 && velocity < -EXPOSE_VELOCITY)){
            swipe.$beginTransition(-1)
            swipe.$wasComplete = true
        } else if(!swipe.$isTransitioning()){
            if(!fuzzyNull(position)) swipe.$beginTransition(0)
            swipe.$wasComplete = false
        }
        this.$setAttachedPressed(item, x, y, false)
        return grabbed
    }
    // QQuickSwipeDelegatePrivate::attachedObjectsSetPressed: the attached objects of the item and its children at the point
    $setAttachedPressed(item, x, y, pressed, cancel = false){
        let found = false
        let visit = (it) => {
            if(containsPoint(this, it, x, y)){
                let attached = it.$v[attachedProp.id]
                if(attached){
                    let was = attached.pressed
                    attached.pressed = pressed
                    if(was && !pressed && !cancel) engine.emit(attached, SwipeDelegateAttached.SIGNAL_clicked)
                    found = true
                }
            }
            for(let child of it.$children || []) if(child.$isItem) visit(child)
        }
        visit(item)
        return found
    }
    // QQuickSwipeDelegate::childMouseEventFilter: the events of the items of left / right / behind
    $filterPointer(kind, receiver, x, y, buttons){
        let swipe = this.$v[SwipeDelegate.SWIPE]
        if(!swipe.$hasDelegates()) return {}
        let owner = [swipe.$v[Swipe.LEFTITEM], swipe.$v[Swipe.BEHINDITEM], swipe.$v[Swipe.RIGHTITEM]].find(it => it && isIn(receiver, it))
        if(!owner) return {}
        if(kind === 'press'){
            if(fuzzyNull(swipe.$v[Swipe.POSITION])){
                this.$mousePress(x, y, 1)
                return { grab: true, filter: true }
            }
            // forwardMouseEvent: the item does not accept the press, the delegate looks pressed
            if(swipe.$v[Swipe.COMPLETE]) this.$setPressed(true)
            swipe.$positionBeforePress = swipe.$v[Swipe.POSITION]
            this.$swipePress = { x, y, samples: [{ x, time: clock.now() }] }
            let delivered = this.$setAttachedPressed(receiver, x, y, true)
            return delivered ? { grab: true, filter: true } : {}
        }
        if(kind === 'move'){
            let grab = this.$handleSwipeMove(receiver, x, y)
            return grab ? { grab: true, filter: true } : {}
        }
        if(kind === 'release'){
            let grabbed = this.$handleSwipeRelease(receiver, x, y)
            this.$swipePress = null
            return grabbed ? { filter: true } : {}
        }
        return {}
    }
}
defineType(SwipeDelegate, { props: { swipe: { group: Swipe } } })
Object.defineProperty(SwipeDelegate.prototype, 'swipe', { get(){ return this.$v[SwipeDelegate.SWIPE] }, configurable: true })
SwipeDelegate.Left = Left
SwipeDelegate.Right = Right

module.exports = { SwipeDelegate, Swipe, SwipeDelegateAttached }
