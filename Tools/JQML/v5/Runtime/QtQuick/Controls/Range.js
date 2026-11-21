// The range controls of QtQuick.Templates (Qt 6.12): Slider, RangeSlider, Dial, SpinBox, ProgressBar, BusyIndicator,
// PageIndicator. Checked against Qt 6.12 (tests/diff/cases/range_controls.qml):
//  - Slider: a press off the handle moves it there (moved), a press on it keeps it; a drag moves it, snapped by stepSize
//    with SnapAlways; live: false keeps the value until the release; the arrow keys of its orientation step by stepSize
//    or 0.1 (pressed, value, moved, released); a value is clamped to from..to; valueAt() snaps to stepSize;
//  - RangeSlider: a press takes the nearer handle; first stays below second;
//  - Dial: the angle from the top, startAngle -140 .. endAngle 140; a press sets it (Circular);
//  - SpinBox: the up and down indicators and Up / Down step by stepSize and emit valueModified, the value is clamped
//    (wrap: it wraps); displayText is textFromValue(value, locale).
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { GroupObject } = require('../../Core/QtObject')
const { Item } = require('../Item')
const { Control, withSetters } = require('./Control')
const { buttonFocusPolicy } = require('./AbstractButton')
const QtEnums = require('../../Qt/enums')

const Horizontal = 1, Vertical = 2
const NoSnap = 0, SnapAlways = 1, SnapOnRelease = 2
const clamp = (value, a, b) => Math.min(Math.max(value, Math.min(a, b)), Math.max(a, b))
const fuzzyNull = (v) => Math.abs(v) <= 1e-12
const LeftButton = 1

// ---- Slider (QQuickSlider) ----
class Slider extends Control {
    constructor(parent = null){
        super(parent)
        this.$plain(Slider.TO, 1)
        this.$plain(Slider.ORIENTATION, Horizontal)
        this.$plain(Slider.HORIZONTAL, true)
        this.$plain(Slider.LIVE, true)
        this.$plain(Slider.TOUCHDRAGTHRESHOLD, -1)
        this.$write(Control.FOCUSPOLICY, buttonFocusPolicy())
        this.$pressPoint = null
    }
    // value <-> position
    $setFrom(value){ this.$plain(Slider.FROM, Number(value)); this.$setValue(this.$v[Slider.VALUE]) }
    $setTo(value){ this.$plain(Slider.TO, Number(value)); this.$setValue(this.$v[Slider.VALUE]) }
    $setValue(value){
        let from = this.$v[Slider.FROM], to = this.$v[Slider.TO]
        value = clamp(Number(value), from, to)
        this.$plain(Slider.VALUE, value)
        this.$setPosition(fuzzyNull(to - from) ? 0 : (value - from) / (to - from))
    }
    $setPosition(position){
        position = clamp(position, 0, 1)
        this.$plain(Slider.POSITION, position)
        let visual = this.$v[Slider.ORIENTATION] === Vertical ? 1 - position : (this.$v[Control.MIRRORED] ? 1 - position : position)
        this.$plain(Slider.VISUALPOSITION, visual)
    }
    $setOrientation(value){
        this.$plain(Slider.ORIENTATION, value)
        this.$plain(Slider.HORIZONTAL, value === Horizontal)
        this.$plain(Slider.VERTICAL, value === Vertical)
        this.$setPosition(this.$v[Slider.POSITION])
    }
    // QQuickSlider::valueAt: the value at a position, snapped to stepSize
    valueAt(position){
        let from = this.$v[Slider.FROM], to = this.$v[Slider.TO], step = this.$v[Slider.STEPSIZE]
        let value = from + (to - from) * position
        if(!fuzzyNull(step)){
            let steps = Math.round((value - from) / step)
            value = clamp(from + steps * step, from, to)
        }
        return value
    }
    // QQuickSliderPrivate::snapPosition
    $snap(position){
        let range = this.$v[Slider.TO] - this.$v[Slider.FROM]
        if(fuzzyNull(range)) return position
        let step = this.$v[Slider.STEPSIZE] / range
        if(fuzzyNull(step)) return position
        return Math.round(position / step) * step
    }
    // QQuickSliderPrivate::positionAt: the handle's middle at the point
    $positionAt(x, y){
        let handle = this.$v[Slider.HANDLE]
        let v = this.$v
        if(v[Slider.ORIENTATION] === Horizontal){
            let hw = handle ? handle.$v[Item.WIDTH] : 0
            let extent = v[Control.AVAILABLEWIDTH] - hw
            if(fuzzyNull(extent)) return 0
            return (x - v[Control.LEFTPADDING] - hw / 2) / extent
        }
        let hh = handle ? handle.$v[Item.HEIGHT] : 0
        let extent = v[Control.AVAILABLEHEIGHT] - hh
        if(fuzzyNull(extent)) return 0
        return (v[Item.HEIGHT] - y - v[Control.BOTTOMPADDING] - hh / 2) / extent
    }
    // to a position by the user: the value (live) or only the position, moved when it changes
    $moveTo(position, snap){
        let old = this.$v[Slider.POSITION]
        position = clamp(position, 0, 1)
        if(snap) position = this.$snap(position)
        if(this.$v[Slider.LIVE]) this.$setValue(this.valueAt(position))
        else this.$setPosition(position)
        if(this.$v[Slider.POSITION] !== old) engine.emit(this, Slider.SIGNAL_moved)
    }
    $onHandle(x, y){
        let handle = this.$v[Slider.HANDLE]
        if(!handle) return false
        let hx = x - handle.$v[Item.X], hy = y - handle.$v[Item.Y]
        return hx >= 0 && hy >= 0 && hx <= handle.$v[Item.WIDTH] && hy <= handle.$v[Item.HEIGHT]
    }
    $mousePress(x, y, button){
        if(button !== LeftButton) return false
        this.$clickFocus()
        this.$pressPoint = { x, y }
        this.$plain(Slider.PRESSED, true)
        if(!this.$onHandle(x, y)) this.$moveTo(this.$positionAt(x, y), this.$v[Slider.SNAPMODE] === SnapAlways)
        return true
    }
    $mouseMove(x, y){
        if(!this.$v[Slider.PRESSED]) return
        this.$moveTo(this.$positionAt(x, y), this.$v[Slider.SNAPMODE] === SnapAlways)
    }
    $mouseRelease(x, y){
        if(!this.$v[Slider.PRESSED]) return
        let position = clamp(this.$positionAt(x, y), 0, 1)
        if(this.$v[Slider.SNAPMODE] !== NoSnap) position = this.$snap(position)
        let old = this.$v[Slider.POSITION]
        this.$setValue(this.valueAt(position))
        if(this.$v[Slider.POSITION] !== old) engine.emit(this, Slider.SIGNAL_moved)
        this.$plain(Slider.PRESSED, false)
        this.$pressPoint = null
    }
    $ungrabMouse(){ this.$plain(Slider.PRESSED, false) }
    get $keepMouseGrab(){ return !!this.$v[Slider.PRESSED] }

    // QQuickSlider::keyPressEvent: the arrows of the orientation
    $keyEvent(event, kind){
        let horizontal = this.$v[Slider.ORIENTATION] === Horizontal
        let step = 0
        if(horizontal && (event.key === QtEnums.Key_Left || event.key === QtEnums.Key_Right)) step = event.key === QtEnums.Key_Right ? 1 : -1
        if(!horizontal && (event.key === QtEnums.Key_Up || event.key === QtEnums.Key_Down)) step = event.key === QtEnums.Key_Up ? 1 : -1
        if(!step) return 0
        if(kind === 'pressed'){
            this.$plain(Slider.PRESSED, true)
            if(this.$v[Control.MIRRORED] && horizontal) step = -step
            if(step > 0) this.increase()
            else this.decrease()
            engine.emit(this, Slider.SIGNAL_moved)
        } else {
            this.$plain(Slider.PRESSED, false)
        }
        return 1
    }
    increase(){ engine.batch(() => this.$setValue(this.$v[Slider.VALUE] + (this.$v[Slider.STEPSIZE] || 0.1))) }
    decrease(){ engine.batch(() => this.$setValue(this.$v[Slider.VALUE] - (this.$v[Slider.STEPSIZE] || 0.1))) }
}
defineType(Slider, {
    props: {
        from: 'real', to: 'real', value: 'real',
        position: { kind: 'real', readonly: true }, visualPosition: { kind: 'real', readonly: true },
        stepSize: 'real', snapMode: 'int', pressed: 'bool', orientation: 'int',
        handle: { kind: 'var', initial: null }, live: 'bool',
        horizontal: { kind: 'bool', readonly: true }, vertical: { kind: 'bool', readonly: true },
        touchDragThreshold: 'real',
        implicitHandleWidth: { kind: 'real', readonly: true }, implicitHandleHeight: { kind: 'real', readonly: true },
    },
    signals: ['moved'],
})
Slider.$setters = withSetters(Control, [
    [Slider.FROM, (c, v) => c.$setFrom(v)],
    [Slider.TO, (c, v) => c.$setTo(v)],
    [Slider.VALUE, (c, v) => c.$setValue(v)],
    [Slider.ORIENTATION, (c, v) => c.$setOrientation(v)],
    [Slider.HANDLE, (c, v) => c.$setDelegate(Slider.HANDLE, v)],
])
// the handle is a delegate: its implicit size
Slider.prototype.$delegateChanged = function(id){
    if(id === Slider.HANDLE) return this.$delegateImplicitChanged(id)
    Control.prototype.$delegateChanged.call(this, id)
}
Slider.prototype.$delegateImplicitChanged = function(id){
    if(id === Slider.HANDLE){
        let handle = this.$v[Slider.HANDLE]
        this.$plain(Slider.IMPLICITHANDLEWIDTH, handle ? handle.$v[Item.IMPLICITWIDTH] : 0)
        this.$plain(Slider.IMPLICITHANDLEHEIGHT, handle ? handle.$v[Item.IMPLICITHEIGHT] : 0)
        return
    }
    Control.prototype.$delegateImplicitChanged.call(this, id)
}
Object.assign(Slider, { NoSnap, SnapAlways, SnapOnRelease })

// ---- RangeSlider (QQuickRangeSlider) and its first / second nodes (QQuickRangeSliderNode) ----
class RangeSliderNode extends GroupObject {
    $slider(){ return this.$owner }
    $isFirst(){ return this.$slider().$v[RangeSlider.FIRST] === this }
    $setValue(value){
        let s = this.$slider()
        let from = s.$v[RangeSlider.FROM], to = s.$v[RangeSlider.TO]
        value = clamp(Number(value), from, to)
        // first stays at or below second, second at or above first (unless the handles may cross)
        let other = this.$isFirst() ? s.$v[RangeSlider.SECOND] : s.$v[RangeSlider.FIRST]
        if(other && other.$ready && !s.$v[RangeSlider.CROSSINGENABLED]){
            if(this.$isFirst()) value = Math.min(value, other.$v[RangeSliderNode.VALUE])
            else value = Math.max(value, other.$v[RangeSliderNode.VALUE])
        }
        this.$ready = true
        super.$write(RangeSliderNode.VALUE, value)
        this.$setPosition(fuzzyNull(to - from) ? 0 : (value - from) / (to - from))
        s.$updateEffective()
    }
    $setPosition(position){
        let s = this.$slider()
        position = clamp(position, 0, 1)
        super.$write(RangeSliderNode.POSITION, position)
        let vertical = s.$v[RangeSlider.ORIENTATION] === Vertical
        super.$write(RangeSliderNode.VISUALPOSITION, vertical ? 1 - position : position)
    }
    $write(id, value){
        if(id === RangeSliderNode.VALUE) return this.$setValue(value)
        if(id === RangeSliderNode.HANDLE) return this.$setHandle(value)
        super.$write(id, value)
    }
    $setHandle(item){
        let s = this.$slider()
        let old = this.$v[RangeSliderNode.HANDLE]
        if(old === item) return
        if(old && !(old.$f & engine.DESTROYED)){
            if(s.$defaults && s.$defaults.has(old)){
                s.$defaults.delete(old)
                old.destroy()
            } else if(old.$parent === s) old.$setParent(null)
        }
        super.$write(RangeSliderNode.HANDLE, item || null)
        if(item){
            if(item.$parent !== s) item.$setParent(s)
            let update = () => {
                super.$write(RangeSliderNode.IMPLICITHANDLEWIDTH, item.$v[Item.IMPLICITWIDTH])
                super.$write(RangeSliderNode.IMPLICITHANDLEHEIGHT, item.$v[Item.IMPLICITHEIGHT])
            }
            engine.connect(item, Item.SIGNAL_implicitWidthChanged, update, null, engine.PRIORITY_NORMAL, this)
            engine.connect(item, Item.SIGNAL_implicitHeightChanged, update, null, engine.PRIORITY_NORMAL, this)
            update()
        }
    }
    increase(){ engine.batch(() => this.$setValue(this.$v[RangeSliderNode.VALUE] + (this.$slider().$v[RangeSlider.STEPSIZE] || 0.1))) }
    decrease(){ engine.batch(() => this.$setValue(this.$v[RangeSliderNode.VALUE] - (this.$slider().$v[RangeSlider.STEPSIZE] || 0.1))) }
}
defineType(RangeSliderNode, {
    props: {
        value: 'real', position: { kind: 'real', readonly: true }, visualPosition: { kind: 'real', readonly: true },
        handle: { kind: 'var', initial: null }, pressed: 'bool', hovered: 'bool',
        implicitHandleWidth: { kind: 'real', readonly: true }, implicitHandleHeight: { kind: 'real', readonly: true },
    },
    signals: ['moved'],
})
for(let name of ['value', 'handle']){
    let id = RangeSliderNode[name.toUpperCase()]
    let get = Object.getOwnPropertyDescriptor(RangeSliderNode.prototype, name).get
    Object.defineProperty(RangeSliderNode.prototype, name, { get, set(value){ engine.batch(() => this.$write(id, value)) }, configurable: true })
}

class RangeSlider extends Control {
    constructor(parent = null){
        super(parent)
        this.$plain(RangeSlider.TO, 1)
        this.$plain(RangeSlider.ORIENTATION, Horizontal)
        this.$plain(RangeSlider.HORIZONTAL, true)
        this.$plain(RangeSlider.LIVE, true)
        this.$plain(RangeSlider.TOUCHDRAGTHRESHOLD, -1)
        this.$v[RangeSlider.FIRST] = new RangeSliderNode(this)
        this.$v[RangeSlider.SECOND] = new RangeSliderNode(this)
        this.$v[RangeSlider.FIRST].$setValue(0)
        this.$v[RangeSlider.SECOND].$setValue(1)
        this.$write(Control.FOCUSPOLICY, buttonFocusPolicy())
        this.$active = null
    }
    $nodes(){ return [this.$v[RangeSlider.FIRST], this.$v[RangeSlider.SECOND]] }
    // handlesCrossed, effectiveFirstValue / effectiveSecondValue: the lower and the higher value
    $updateEffective(){
        let [first, second] = this.$nodes()
        if(!first || !second) return
        let a = first.$v[RangeSliderNode.VALUE], b = second.$v[RangeSliderNode.VALUE]
        this.$plain(RangeSlider.HANDLESCROSSED, a > b)
        this.$plain(RangeSlider.EFFECTIVEFIRSTVALUE, Math.min(a, b))
        this.$plain(RangeSlider.EFFECTIVESECONDVALUE, Math.max(a, b))
    }
    $relimit(){
        let [first, second] = this.$nodes()
        second.$setValue(second.$v[RangeSliderNode.VALUE])
        first.$setValue(first.$v[RangeSliderNode.VALUE])
    }
    setValues(firstValue, secondValue){
        engine.batch(() => {
            let [first, second] = this.$nodes()
            let from = this.$v[RangeSlider.FROM], to = this.$v[RangeSlider.TO]
            let a = clamp(firstValue, from, to), b = clamp(secondValue, from, to)
            if(b < a) b = a
            second.$ready = false
            first.$setValue(a)
            second.$setValue(b)
        })
    }
    valueAt(position){ return Slider.prototype.valueAt.call(this, position) }
    $snap(position){ return Slider.prototype.$snap.call(this, position) }
    $positionAt(x, y, handle){
        let v = this.$v
        if(v[RangeSlider.ORIENTATION] === Horizontal){
            let hw = handle ? handle.$v[Item.WIDTH] : 0
            let extent = v[Control.AVAILABLEWIDTH] - hw
            return fuzzyNull(extent) ? 0 : (x - v[Control.LEFTPADDING] - hw / 2) / extent
        }
        let hh = handle ? handle.$v[Item.HEIGHT] : 0
        let extent = v[Control.AVAILABLEHEIGHT] - hh
        return fuzzyNull(extent) ? 0 : (v[Item.HEIGHT] - y - v[Control.BOTTOMPADDING] - hh / 2) / extent
    }
    // QQuickRangeSliderPrivate::pressedNode: the handle under the press, else the nearer one
    $nodeAt(x, y){
        let nodes = this.$nodes()
        for(let node of nodes.slice().reverse()){
            let h = node.$v[RangeSliderNode.HANDLE]
            if(h && x >= h.$v[Item.X] && y >= h.$v[Item.Y] && x <= h.$v[Item.X] + h.$v[Item.WIDTH] && y <= h.$v[Item.Y] + h.$v[Item.HEIGHT]) return { node, onHandle: true }
        }
        let [first, second] = nodes
        let pos = this.$positionAt(x, y, first.$v[RangeSliderNode.HANDLE])
        let d1 = Math.abs(pos - first.$v[RangeSliderNode.POSITION]), d2 = Math.abs(pos - second.$v[RangeSliderNode.POSITION])
        return { node: d2 < d1 || (d2 === d1 && pos > second.$v[RangeSliderNode.POSITION]) ? second : first, onHandle: false }
    }
    $moveNode(node, position, snap){
        let old = node.$v[RangeSliderNode.POSITION]
        position = clamp(position, 0, 1)
        if(snap) position = this.$snap(position)
        if(this.$v[RangeSlider.LIVE]) node.$setValue(this.valueAt(position))
        else node.$setPosition(position)
        if(node.$v[RangeSliderNode.POSITION] !== old) engine.emit(node, RangeSliderNode.SIGNAL_moved)
    }
    $mousePress(x, y, button){
        if(button !== LeftButton) return false
        this.$clickFocus()
        let { node, onHandle } = this.$nodeAt(x, y)
        this.$active = node
        node.$write(RangeSliderNode.PRESSED, true)
        if(!onHandle) this.$moveNode(node, this.$positionAt(x, y, node.$v[RangeSliderNode.HANDLE]), this.$v[RangeSlider.SNAPMODE] === SnapAlways)
        return true
    }
    $mouseMove(x, y){
        let node = this.$active
        if(node) this.$moveNode(node, this.$positionAt(x, y, node.$v[RangeSliderNode.HANDLE]), this.$v[RangeSlider.SNAPMODE] === SnapAlways)
    }
    $mouseRelease(x, y){
        let node = this.$active
        if(!node) return
        let position = clamp(this.$positionAt(x, y, node.$v[RangeSliderNode.HANDLE]), 0, 1)
        if(this.$v[RangeSlider.SNAPMODE] !== NoSnap) position = this.$snap(position)
        node.$setValue(this.valueAt(position))
        node.$write(RangeSliderNode.PRESSED, false)
        this.$active = null
    }
    $ungrabMouse(){
        if(this.$active) this.$active.$write(RangeSliderNode.PRESSED, false)
        this.$active = null
    }
    get $keepMouseGrab(){ return !!this.$active }
    $setOrientation(value){
        this.$plain(RangeSlider.ORIENTATION, value)
        this.$plain(RangeSlider.HORIZONTAL, value === Horizontal)
        this.$plain(RangeSlider.VERTICAL, value === Vertical)
        for(let node of this.$nodes()) node.$setPosition(node.$v[RangeSliderNode.POSITION])
    }
}
defineType(RangeSlider, {
    props: {
        from: 'real', to: 'real', first: { group: RangeSliderNode }, second: { group: RangeSliderNode },
        stepSize: 'real', snapMode: 'int', orientation: 'int', live: 'bool',
        horizontal: { kind: 'bool', readonly: true }, vertical: { kind: 'bool', readonly: true }, touchDragThreshold: 'real',
        crossingEnabled: 'bool', handlesCrossed: { kind: 'bool', readonly: true },
        effectiveFirstValue: { kind: 'real', readonly: true }, effectiveSecondValue: { kind: 'real', readonly: true },
    },
})
RangeSlider.$setters = withSetters(Control, [
    [RangeSlider.FROM, (c, v) => { c.$plain(RangeSlider.FROM, Number(v)); c.$relimit() }],
    [RangeSlider.TO, (c, v) => { c.$plain(RangeSlider.TO, Number(v)); c.$relimit() }],
    [RangeSlider.ORIENTATION, (c, v) => c.$setOrientation(v)],
])
Object.assign(RangeSlider, { NoSnap, SnapAlways, SnapOnRelease })

// ---- Dial (QQuickDial) ----
const Circular = 0, InputHorizontal = 1, InputVertical = 2
class Dial extends Control {
    constructor(parent = null){
        super(parent)
        this.$plain(Dial.TO, 1)
        this.$plain(Dial.STARTANGLE, -140)
        this.$plain(Dial.ENDANGLE, 140)
        this.$plain(Dial.LIVE, true)
        this.$plain(Dial.ANGLE, -140)
        this.$write(Control.FOCUSPOLICY, buttonFocusPolicy())
    }
    $setValue(value){
        let from = this.$v[Dial.FROM], to = this.$v[Dial.TO]
        value = clamp(Number(value), from, to)
        this.$plain(Dial.VALUE, value)
        this.$setPosition(fuzzyNull(to - from) ? 0 : (value - from) / (to - from))
    }
    $setPosition(position){
        position = clamp(position, 0, 1)
        this.$plain(Dial.POSITION, position)
        let start = this.$v[Dial.STARTANGLE], end = this.$v[Dial.ENDANGLE]
        this.$plain(Dial.ANGLE, start + position * (end - start))
    }
    valueAt(position){ return Slider.prototype.valueAt.call(this, position) }
    $snap(position){ return Slider.prototype.$snap.call(this, position) }
    // QQuickDialPrivate::positionAt (Circular): the angle of the point from the top, clockwise
    $positionAt(x, y){
        let yy = this.$v[Item.HEIGHT] / 2 - y, xx = x - this.$v[Item.WIDTH] / 2
        let alpha = (xx || yy) ? Math.atan2(yy, xx) * 180 / Math.PI : 0
        let angle = 90 - alpha
        if(angle > 180) angle -= 360
        let start = this.$v[Dial.STARTANGLE], end = this.$v[Dial.ENDANGLE]
        return (angle - start) / (end - start)
    }
    $moveTo(position){
        let old = this.$v[Dial.POSITION]
        if(!this.$v[Dial.WRAP]){
            // no jump across the gap at the bottom
            if(position < 0 || position > 1) position = old < 0.5 ? 0 : 1
        } else {
            let wrappedTo = position < 0 ? 0 : position > 1 ? 1 : -1
            position = ((position % 1) + 1) % 1
            // QQuickDial::wrapped(direction): Clockwise 0 past the end, CounterClockwise 1 past the start
            if(wrappedTo >= 0) engine.emitArgs(this, Dial.SIGNAL_wrapped, [wrappedTo === 1 ? 0 : 1])
        }
        if(this.$v[Dial.SNAPMODE] === SnapAlways) position = this.$snap(position)
        if(this.$v[Dial.LIVE]) this.$setValue(this.valueAt(position))
        else this.$setPosition(position)
        if(this.$v[Dial.POSITION] !== old) engine.emit(this, Dial.SIGNAL_moved)
    }
    $mousePress(x, y, button){
        if(button !== LeftButton) return false
        this.$clickFocus()
        this.$plain(Dial.PRESSED, true)
        this.$pressPoint = { x, y, position: this.$v[Dial.POSITION] }
        if(this.$v[Dial.INPUTMODE] === Circular) this.$moveTo(this.$positionAt(x, y))
        return true
    }
    $mouseMove(x, y){
        if(!this.$v[Dial.PRESSED]) return
        let mode = this.$v[Dial.INPUTMODE]
        if(mode === Circular) return this.$moveTo(this.$positionAt(x, y))
        // the horizontal / vertical input modes: a drag along the axis, the size of the dial for the whole range
        let p = this.$pressPoint
        let delta = mode === InputHorizontal ? (x - p.x) / this.$v[Item.WIDTH] : (p.y - y) / this.$v[Item.HEIGHT]
        this.$moveTo(p.position + delta)
    }
    $mouseRelease(x, y){
        if(!this.$v[Dial.PRESSED]) return
        if(this.$v[Dial.SNAPMODE] === SnapOnRelease) this.$setPosition(this.$snap(this.$v[Dial.POSITION]))
        this.$plain(Dial.PRESSED, false)
    }
    $ungrabMouse(){ this.$plain(Dial.PRESSED, false) }
    get $keepMouseGrab(){ return !!this.$v[Dial.PRESSED] }
    $keyEvent(event, kind){
        let K = QtEnums
        if(kind !== 'pressed') return [K.Key_Left, K.Key_Right, K.Key_Up, K.Key_Down, K.Key_Home, K.Key_End].includes(event.key) ? 1 : 0
        if(event.key === K.Key_Left || event.key === K.Key_Down) this.decrease()
        else if(event.key === K.Key_Right || event.key === K.Key_Up) this.increase()
        else if(event.key === K.Key_Home) this.$setValue(this.$v[Dial.FROM])
        else if(event.key === K.Key_End) this.$setValue(this.$v[Dial.TO])
        else return 0
        engine.emit(this, Dial.SIGNAL_moved)
        return 1
    }
    increase(){ engine.batch(() => this.$setValue(this.$v[Dial.VALUE] + (this.$v[Dial.STEPSIZE] || 0.1))) }
    decrease(){ engine.batch(() => this.$setValue(this.$v[Dial.VALUE] - (this.$v[Dial.STEPSIZE] || 0.1))) }
}
defineType(Dial, {
    props: {
        from: 'real', to: 'real', value: 'real', position: { kind: 'real', readonly: true }, angle: { kind: 'real', readonly: true },
        stepSize: 'real', snapMode: 'int', wrap: 'bool', pressed: { kind: 'bool', readonly: true }, handle: { kind: 'var', initial: null },
        live: 'bool', inputMode: 'int', startAngle: 'real', endAngle: 'real',
    },
    signals: ['moved', 'wrapped(direction)'],
})
Dial.$setters = withSetters(Control, [
    [Dial.FROM, (c, v) => { c.$plain(Dial.FROM, Number(v)); c.$setValue(c.$v[Dial.VALUE]) }],
    [Dial.TO, (c, v) => { c.$plain(Dial.TO, Number(v)); c.$setValue(c.$v[Dial.VALUE]) }],
    [Dial.VALUE, (c, v) => c.$setValue(v)],
    [Dial.STARTANGLE, (c, v) => { c.$plain(Dial.STARTANGLE, Number(v)); c.$setPosition(c.$v[Dial.POSITION]) }],
    [Dial.ENDANGLE, (c, v) => { c.$plain(Dial.ENDANGLE, Number(v)); c.$setPosition(c.$v[Dial.POSITION]) }],
    [Dial.HANDLE, (c, v) => c.$setDelegate(Dial.HANDLE, v)],
])
Object.assign(Dial, { NoSnap, SnapAlways, SnapOnRelease, Circular, Horizontal: InputHorizontal, Vertical: InputVertical, Clockwise: 0, CounterClockwise: 1 })

// ---- ProgressBar, BusyIndicator ----
class ProgressBar extends Control {
    constructor(parent = null){
        super(parent)
        this.$plain(ProgressBar.TO, 1)
    }
    $update(){
        let from = this.$v[ProgressBar.FROM], to = this.$v[ProgressBar.TO]
        let value = clamp(this.$v[ProgressBar.VALUE], from, to)
        this.$plain(ProgressBar.VALUE, value)
        let position = fuzzyNull(to - from) ? 0 : (value - from) / (to - from)
        this.$plain(ProgressBar.POSITION, position)
        this.$plain(ProgressBar.VISUALPOSITION, this.$v[Control.MIRRORED] ? 1 - position : position)
    }
}
defineType(ProgressBar, {
    props: { from: 'real', to: 'real', value: 'real', position: { kind: 'real', readonly: true }, visualPosition: { kind: 'real', readonly: true }, indeterminate: 'bool' },
})
ProgressBar.$setters = withSetters(Control, [
    [ProgressBar.FROM, (c, v) => { c.$plain(ProgressBar.FROM, Number(v)); c.$update() }],
    [ProgressBar.TO, (c, v) => { c.$plain(ProgressBar.TO, Number(v)); c.$update() }],
    [ProgressBar.VALUE, (c, v) => { c.$plain(ProgressBar.VALUE, Number(v)); c.$update() }],
])

class BusyIndicator extends Control {
    constructor(parent = null){
        super(parent)
        this.$plain(BusyIndicator.RUNNING, true)
    }
}
defineType(BusyIndicator, { props: { running: 'bool' } })

// ---- PageIndicator (QQuickPageIndicator): a delegate per page, a press on one makes it current (interactive) ----
class PageIndicator extends Control {
    // the delegates of the pages: those of the Repeater of the content (the style's), else the content's children in order
    $pages(){
        let content = this.$v[Control.CONTENTITEM]
        if(!content) return []
        let repeater = (content.$children || []).find(c => typeof c.itemAt === 'function' && c.count !== undefined)
        if(repeater){
            let items = []
            for(let i = 0; i < repeater.count; i++) items.push(repeater.itemAt(i))
            return items
        }
        return (content.$children || []).filter(c => c.$isItem)
    }
    $pageAt(x, y){
        let pages = this.$pages()
        for(let index = 0; index < pages.length; index++){
            let item = pages[index]
            if(!item) continue
            // the place of the page in the indicator
            let px = 0, py = 0
            for(let it = item; it && it !== this; it = it.$parent){
                px += it.$v[Item.X]
                py += it.$v[Item.Y]
            }
            if(x >= px && y >= py && x <= px + item.$v[Item.WIDTH] && y <= py + item.$v[Item.HEIGHT]) return index
        }
        return -1
    }
    $mousePress(x, y, button){
        if(!this.$v[PageIndicator.INTERACTIVE] || button !== LeftButton) return false
        this.$pressedPage = this.$pageAt(x, y)
        this.$markPressed()
        return true
    }
    $mouseMove(x, y){
        if(this.$pressedPage === undefined) return
        this.$pressedPage = this.$pageAt(x, y)
        this.$markPressed()
    }
    $mouseRelease(x, y){
        let page = this.$pageAt(x, y)
        if(page >= 0 && page === this.$pressedPage) this.currentIndex = page
        this.$pressedPage = undefined
        this.$markPressed()
    }
    $ungrabMouse(){
        this.$pressedPage = undefined
        this.$markPressed()
    }
    // the delegate of the pressed page has pressed (QQuickPageIndicatorPrivate::updatePressed)
    $markPressed(){
        this.$pages().forEach((item, index) => {
            if(item && item.pressed !== undefined) item.pressed = index === this.$pressedPage
        })
    }
}
defineType(PageIndicator, { props: { count: 'int', currentIndex: 'int', interactive: 'bool', delegate: { kind: 'var', initial: null } } })

// ---- SpinBox (QQuickSpinBox) and its up / down buttons (QQuickIndicatorButton) ----
class IndicatorButton extends GroupObject {}
defineType(IndicatorButton, {
    props: {
        pressed: { kind: 'bool' }, indicator: { kind: 'var', initial: null }, hovered: { kind: 'bool', readonly: true },
        implicitIndicatorWidth: { kind: 'real', readonly: true }, implicitIndicatorHeight: { kind: 'real', readonly: true },
    },
})
IndicatorButton.prototype.$write = function(id, value){
    if(id !== IndicatorButton.INDICATOR) return GroupObject.prototype.$write.call(this, id, value)
    let spin = this.$owner
    let old = this.$v[IndicatorButton.INDICATOR]
    if(old === value) return
    if(old && !(old.$f & engine.DESTROYED)){
        if(spin.$defaults && spin.$defaults.has(old)){
            spin.$defaults.delete(old)
            old.destroy()
        } else if(old.$parent === spin) old.$setParent(null)
    }
    GroupObject.prototype.$write.call(this, id, value || null)
    if(value){
        if(value.$parent !== spin) value.$setParent(spin)
        let update = () => {
            GroupObject.prototype.$write.call(this, IndicatorButton.IMPLICITINDICATORWIDTH, value.$v[Item.IMPLICITWIDTH])
            GroupObject.prototype.$write.call(this, IndicatorButton.IMPLICITINDICATORHEIGHT, value.$v[Item.IMPLICITHEIGHT])
        }
        engine.connect(value, Item.SIGNAL_implicitWidthChanged, update, null, engine.PRIORITY_NORMAL, this)
        engine.connect(value, Item.SIGNAL_implicitHeightChanged, update, null, engine.PRIORITY_NORMAL, this)
        update()
    }
}
{
    let get = Object.getOwnPropertyDescriptor(IndicatorButton.prototype, 'indicator').get
    Object.defineProperty(IndicatorButton.prototype, 'indicator', { get, set(value){ engine.batch(() => this.$write(IndicatorButton.INDICATOR, value)) }, configurable: true })
}

// QQuickSpinBox's textFromValue / valueFromText: the number in the locale (Number.toLocaleString(locale, 'f', 0) of QML:
// with the group separator), and back
function defaultTextFromValue(value, locale){
    let n = Math.round(Number(value))
    let text = String(Math.abs(n))
    let group = locale && locale.groupSeparator !== undefined ? locale.groupSeparator : ','
    text = text.replace(/\B(?=(\d{3})+(?!\d))/g, group)
    return (n < 0 ? (locale && locale.negativeSign || '-') : '') + text
}
function defaultValueFromText(text, locale){
    let group = locale && locale.groupSeparator !== undefined ? locale.groupSeparator : ','
    let n = Number(String(text).split(group).join('').replace(locale && locale.negativeSign || '-', '-'))
    return Number.isFinite(n) ? Math.round(n) : 0
}

class SpinBox extends Control {
    constructor(parent = null){
        super(parent)
        this.$plain(SpinBox.TO, 99)
        this.$plain(SpinBox.STEPSIZE, 1)
        this.$v[SpinBox.UP] = new IndicatorButton(this)
        this.$v[SpinBox.DOWN] = new IndicatorButton(this)
        this.$plain(SpinBox.TEXTFROMVALUE, defaultTextFromValue)
        this.$plain(SpinBox.VALUEFROMTEXT, defaultValueFromText)
        this.$plain(SpinBox.DISPLAYTEXT, '0')
        this.$plain(SpinBox.INPUTMETHODHINTS, 0x10000)     // Qt.ImhDigitsOnly
        this.$repeat = null
    }
    $locale(){ return this.$v[Control.LOCALE] || require('../../Qt/Locale').locale() }
    $updateDisplayText(){
        let fn = this.$v[SpinBox.TEXTFROMVALUE]
        let text = typeof fn === 'function' ? fn.call(this, this.$v[SpinBox.VALUE], this.$locale()) : defaultTextFromValue(this.$v[SpinBox.VALUE], this.$locale())
        this.$plain(SpinBox.DISPLAYTEXT, String(text))
    }
    // QQuickSpinBoxPrivate::boundValue: from..to, or wrapped
    $bound(value, wrap){
        let from = this.$v[SpinBox.FROM], to = this.$v[SpinBox.TO]
        let lo = Math.min(from, to), hi = Math.max(from, to)
        if(wrap){
            if(value > hi) return lo
            if(value < lo) return hi
            return value
        }
        return clamp(value, lo, hi)
    }
    $setValue(value, modified, wrap = false){
        value = this.$bound(Math.round(Number(value)), wrap)
        let changed = this.$v[SpinBox.VALUE] !== value
        this.$plain(SpinBox.VALUE, value)
        this.$updateDisplayText()
        if(changed && modified) engine.emit(this, SpinBox.SIGNAL_valueModified)
        return changed
    }
    increase(){ engine.batch(() => this.$setValue(this.$v[SpinBox.VALUE] + this.$v[SpinBox.STEPSIZE], false, this.$v[SpinBox.WRAP])) }
    decrease(){ engine.batch(() => this.$setValue(this.$v[SpinBox.VALUE] - this.$v[SpinBox.STEPSIZE], false, this.$v[SpinBox.WRAP])) }
    $step(sign){ this.$setValue(this.$v[SpinBox.VALUE] + sign * this.$v[SpinBox.STEPSIZE], true, this.$v[SpinBox.WRAP]) }
    $buttonAt(x, y){
        for(let [id, sign] of [[SpinBox.UP, 1], [SpinBox.DOWN, -1]]){
            let button = this.$v[id]
            let i = button.$v[IndicatorButton.INDICATOR]
            if(i && i.$v[Item.VISIBLE] && x >= i.$v[Item.X] && y >= i.$v[Item.Y] && x <= i.$v[Item.X] + i.$v[Item.WIDTH] && y <= i.$v[Item.Y] + i.$v[Item.HEIGHT]) return { button, sign }
        }
        return null
    }
    $mousePress(x, y, button){
        if(button !== LeftButton) return false
        let hit = this.$buttonAt(x, y)
        this.$clickFocus()
        if(!hit) return true
        this.$pressedButton = hit
        hit.button.$write(IndicatorButton.PRESSED, true)
        // QQuickSpinBoxPrivate::startRepeatDelay: held, it steps again after 300 ms, every 100 ms
        this.$repeat = setTimeout(() => {
            this.$repeat = setInterval(() => {
                if(this.$f & engine.DESTROYED || !this.$pressedButton) return this.$stopRepeat()
                engine.batch(() => this.$step(this.$pressedButton.sign))
            }, 100)
        }, 300)
        return true
    }
    $mouseMove(x, y){
        let hit = this.$pressedButton
        if(!hit) return
        let over = this.$buttonAt(x, y)
        hit.button.$write(IndicatorButton.PRESSED, !!over && over.button === hit.button)
    }
    $mouseRelease(x, y){
        let hit = this.$pressedButton
        this.$stopRepeat()
        this.$pressedButton = null
        if(!hit) return
        let over = this.$buttonAt(x, y)
        let repeated = this.$repeated
        hit.button.$write(IndicatorButton.PRESSED, false)
        if(over && over.button === hit.button) this.$step(hit.sign)
    }
    $ungrabMouse(){
        this.$stopRepeat()
        if(this.$pressedButton) this.$pressedButton.button.$write(IndicatorButton.PRESSED, false)
        this.$pressedButton = null
    }
    $stopRepeat(){
        if(this.$repeat){
            clearTimeout(this.$repeat)
            clearInterval(this.$repeat)
            this.$repeat = null
        }
    }
    // QQuickSpinBox::keyPressEvent: Up and Down step (the button looks pressed while the key is down)
    $keyEvent(event, kind){
        let sign = event.key === QtEnums.Key_Up ? 1 : event.key === QtEnums.Key_Down ? -1 : 0
        if(!sign) return 0
        let button = this.$v[sign > 0 ? SpinBox.UP : SpinBox.DOWN]
        if(kind === 'pressed'){
            button.$write(IndicatorButton.PRESSED, true)
            this.$step(sign)
        } else {
            button.$write(IndicatorButton.PRESSED, false)
        }
        return 1
    }
    $wheel(x, y, angleDelta){
        if(!this.$v[Control.WHEELENABLED]) return false
        let delta = angleDelta.y || angleDelta.x
        if(delta) this.$step(delta > 0 ? 1 : -1)
        return true
    }
}
defineType(SpinBox, {
    props: {
        from: 'int', to: 'int', value: 'int', stepSize: 'int', editable: 'bool', live: 'bool',
        validator: { kind: 'var', initial: null }, textFromValue: 'var', valueFromText: 'var',
        up: { group: IndicatorButton }, down: { group: IndicatorButton },
        inputMethodHints: 'int', inputMethodComposing: { kind: 'bool', readonly: true }, wrap: 'bool',
        displayText: { kind: 'string', readonly: true }, acceptableInput: { kind: 'bool', readonly: true, initial: true },
    },
    signals: ['valueModified'],
})
SpinBox.$setters = withSetters(Control, [
    [SpinBox.FROM, (c, v) => { c.$plain(SpinBox.FROM, Math.round(v)); c.$setValue(c.$v[SpinBox.VALUE], false) }],
    [SpinBox.TO, (c, v) => { c.$plain(SpinBox.TO, Math.round(v)); c.$setValue(c.$v[SpinBox.VALUE], false) }],
    [SpinBox.VALUE, (c, v) => c.$setValue(v, false)],
    [SpinBox.TEXTFROMVALUE, (c, v) => { c.$plain(SpinBox.TEXTFROMVALUE, v); c.$updateDisplayText() }],
])

module.exports = { Slider, RangeSlider, RangeSliderNode, Dial, ProgressBar, BusyIndicator, PageIndicator, SpinBox, IndicatorButton }
