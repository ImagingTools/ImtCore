// ScrollBar and ScrollIndicator of QtQuick.Templates (qquickscrollbar.cpp, qquickscrollindicator.cpp, Qt 6.12), and their
// attached properties on a Flickable (ListView, GridView, TableView): ScrollBar.vertical: ScrollBar {}. Checked against
// Qt 6.12 (tests/diff/cases/scrollbar*.qml):
//  - the handle is the content item, placed along the track by visualPosition and visualSize (QQuickScrollBarPrivate::
//    visualArea: minimumSize, a position beyond the ends shrinks it);
//  - a press on the track moves the handle so that its middle is at the press, a press on the handle keeps the offset;
//    snapMode snaps by stepSize * (1 - size); increase() / decrease() move by stepSize, or 0.1;
//  - attached: the bar becomes a child of the view at its right (bottom) edge, as tall (wide) as the view; its size and
//    position are the view's visibleArea, a move of the bar moves the content; it is active while the view moves.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { GroupObject } = require('../../Core/QtObject')
const { Item } = require('../Item')
const { Control, withSetters } = require('./Control')

const Horizontal = 1, Vertical = 2
const NoSnap = 0, SnapAlways = 1, SnapOnRelease = 2
const AsNeeded = 0, AlwaysOff = 1, AlwaysOn = 2

// QQuickScrollBarPrivate::visualArea
function visualArea(position, size, minimumSize){
    let visualPos = position
    if(minimumSize > size) visualPos = position / (1.0 - size) * (1.0 - minimumSize)
    let visualSize = Math.min(Math.max(0, Math.max(size, minimumSize) + Math.min(0, visualPos)), Math.max(0, 1.0 - visualPos))
    visualPos = Math.min(Math.max(0, visualPos), Math.max(0, 1.0 - visualSize))
    return { position: visualPos, size: visualSize }
}

// the part both share: size, position, orientation, minimumSize, the visual area, the handle placed on the track
const indicatorPart = {
    $setSize(value){
        value = Math.max(0, Math.min(1, Number(value)))
        this.$plain(this.constructor.SIZE, value)
        this.$updateVisual()
    },
    $setPosition(value){
        this.$plain(this.constructor.POSITION, Number(value))
        this.$updateVisual()
        // QQuickScrollBarAttachedPrivate::scrollVertical: a scroll bar the view does not move moves the view
        if(this.$attached && !this.$syncing && this.$attached.$scrolls) this.$attached.$barMoved(this)
    },
    $setMinimumSize(value){
        this.$plain(this.constructor.MINIMUMSIZE, Math.max(0, Math.min(1, Number(value))))
        this.$updateVisual()
    },
    $setOrientation(value){
        this.$plain(this.constructor.ORIENTATION, value)
        this.$plain(this.constructor.HORIZONTAL, value === Horizontal)
        this.$plain(this.constructor.VERTICAL, value === Vertical)
        this.$resizeContent()
    },
    $updateVisual(){
        let C = this.constructor
        let area = visualArea(this.$v[C.POSITION], this.$v[C.SIZE], this.$v[C.MINIMUMSIZE])
        this.$plain(C.VISUALPOSITION, area.position)
        this.$plain(C.VISUALSIZE, area.size)
        this.$resizeContent()
    },
    // QQuickScrollBar::resizeContent: the handle along the track
    $resizeContent(){
        let content = this.$v[Control.CONTENTITEM]
        if(!content || content.$f & engine.DESTROYED) return
        let v = this.$v, C = this.constructor
        let left = v[Control.LEFTPADDING], top = v[Control.TOPPADDING]
        let aw = v[Control.AVAILABLEWIDTH], ah = v[Control.AVAILABLEHEIGHT]
        let pos = v[C.VISUALPOSITION], size = v[C.VISUALSIZE]
        let set = (id, bit, value) => { if(bit) content.$explicit |= bit; content.$write(id, value) }
        if(v[C.ORIENTATION] === Horizontal){
            set(Item.X, 0, left + pos * aw)
            set(Item.Y, 0, top)
            set(Item.WIDTH, 1, size * aw)
            set(Item.HEIGHT, 2, ah)
        } else {
            set(Item.X, 0, left)
            set(Item.Y, 0, top + pos * ah)
            set(Item.WIDTH, 1, aw)
            set(Item.HEIGHT, 2, size * ah)
        }
    },
}

class ScrollBar extends Control {
    constructor(parent = null){
        super(parent)
        this.$plain(ScrollBar.ORIENTATION, Vertical)
        this.$plain(ScrollBar.VERTICAL, true)
        this.$plain(ScrollBar.INTERACTIVE, true)
        this.$offset = 0
        this.$hoverActive = false
        this.$attachedActive = false
    }
    // QQuickScrollBarPrivate::positionAt: the position under a point of the track, a logical one (a handle larger than size
    // by minimumSize covers more of the track: QQuickScrollBarPrivate::logicalPosition)
    $positionAt(x, y){
        let v = this.$v
        let visual
        if(v[ScrollBar.ORIENTATION] === Horizontal){
            let aw = v[Control.AVAILABLEWIDTH]
            visual = aw > 0 ? (x - v[Control.LEFTPADDING]) / aw : 0
        } else {
            let ah = v[Control.AVAILABLEHEIGHT]
            visual = ah > 0 ? (y - v[Control.TOPPADDING]) / ah : 0
        }
        return this.$logical(visual)
    }
    $logical(position){
        let size = this.$v[ScrollBar.SIZE], visualSize = this.$v[ScrollBar.VISUALSIZE]
        if(visualSize !== size && visualSize !== 1.0) return position * (1.0 - size) / (1.0 - visualSize)
        return position
    }
    // QQuickScrollBarPrivate::snapPosition
    $snap(position){
        let step = this.$v[ScrollBar.STEPSIZE] * (1.0 - this.$v[ScrollBar.SIZE])
        if(!step) return position
        return Math.round(position / step) * step
    }
    $moveTo(x, y){
        let size = this.$v[ScrollBar.SIZE]
        let pos = Math.min(Math.max(0, this.$positionAt(x, y) - this.$offset), 1.0 - size)
        if(this.$v[ScrollBar.SNAPMODE] === SnapAlways) pos = this.$snap(pos)
        this.$setPosition(pos)
    }
    $updateActive(){
        this.$plain(ScrollBar.ACTIVE, !!(this.$v[ScrollBar.PRESSED] || this.$hoverActive || this.$attachedActive))
    }
    $setActiveFromView(value){
        this.$attachedActive = !!value
        this.$updateActive()
    }
    $setPressed(value){
        this.$plain(ScrollBar.PRESSED, !!value)
        this.$updateActive()
    }
    $setInteractive(value){
        this.$plain(ScrollBar.INTERACTIVE, !!value)
        if(!value && this.$v[ScrollBar.PRESSED]) this.$setPressed(false)
    }

    // QQuickScrollBarPrivate::handlePress / handleMove / handleRelease
    $mousePress(x, y, button){
        if(button !== 1 || !this.$v[ScrollBar.INTERACTIVE]) return false
        this.$offset = this.$positionAt(x, y) - this.$v[ScrollBar.POSITION]
        // the handle in logical units: a press outside it takes its middle
        let size = this.$logical(this.$v[ScrollBar.VISUALSIZE])
        if(this.$offset < 0 || this.$offset > size) this.$offset = size / 2
        this.$setPressed(true)
        this.$moveTo(x, y)
        this.$userMoved()
        return true
    }
    $mouseMove(x, y){
        if(!this.$v[ScrollBar.PRESSED]) return
        this.$moveTo(x, y)
        this.$userMoved()
    }
    $mouseRelease(x, y){
        if(!this.$v[ScrollBar.PRESSED]) return
        let size = this.$v[ScrollBar.SIZE]
        let pos = Math.min(Math.max(0, this.$positionAt(x, y) - this.$offset), 1.0 - size)
        if(this.$v[ScrollBar.SNAPMODE] !== NoSnap) pos = this.$snap(pos)
        this.$setPosition(pos)
        this.$userMoved()
        this.$setPressed(false)
    }
    $ungrabMouse(){ if(this.$v[ScrollBar.PRESSED]) this.$setPressed(false) }
    // a pressed bar keeps the mouse: the view it is on does not take the drag for a flick (QQuickItem::keepMouseGrab)
    get $keepMouseGrab(){ return !!this.$v[ScrollBar.PRESSED] }
    $hoverEnter(){ super.$hoverEnter(); this.$hoverChanged() }
    $hoverLeave(){ super.$hoverLeave(); this.$hoverChanged() }
    // QQuickScrollBar::hoverChange: an interactive bar is active while hovered
    $hoverChanged(){
        this.$hoverActive = !!(this.$v[ScrollBar.INTERACTIVE] && this.$v[Control.HOVERED])
        this.$updateActive()
    }
    // the bar moved by the user: the view follows ($setPosition tells the attached part)
    $userMoved(){}

    // QQuickScrollBar::increase / decrease: by stepSize, or 0.1; active while it moves
    increase(){ this.$step(1) }
    decrease(){ this.$step(-1) }
    $step(sign){
        let step = this.$v[ScrollBar.STEPSIZE] || 0.1
        engine.batch(() => {
            let was = this.$attachedActive
            this.$setActiveFromView(true)
            this.$setPosition(this.$v[ScrollBar.POSITION] + sign * step)
            this.$userMoved()
            this.$setActiveFromView(was)
        })
    }
    setSize(value){ engine.batch(() => this.$setSize(value)) }
    setPosition(value){ engine.batch(() => this.$setPosition(value)) }
}
Object.assign(ScrollBar.prototype, indicatorPart)
defineType(ScrollBar, {
    props: {
        size: 'real',
        position: 'real',
        stepSize: 'real',
        active: 'bool',
        pressed: 'bool',
        orientation: 'int',
        snapMode: 'int',
        interactive: 'bool',
        policy: 'int',
        horizontal: { kind: 'bool', readonly: true },
        vertical: { kind: 'bool', readonly: true },
        minimumSize: 'real',
        visualSize: { kind: 'real', readonly: true },
        visualPosition: { kind: 'real', readonly: true },
    },
})
ScrollBar.$setters = withSetters(Control, [
    [ScrollBar.SIZE, (c, v) => c.$setSize(v)],
    [ScrollBar.POSITION, (c, v) => c.$setPosition(v)],
    [ScrollBar.MINIMUMSIZE, (c, v) => c.$setMinimumSize(v)],
    [ScrollBar.ORIENTATION, (c, v) => c.$setOrientation(v)],
    [ScrollBar.PRESSED, (c, v) => c.$setPressed(v)],
    [ScrollBar.INTERACTIVE, (c, v) => c.$setInteractive(v)],
    [ScrollBar.ACTIVE, (c, v) => c.$setActiveFromView(v)],
])
Object.assign(ScrollBar, { NoSnap, SnapAlways, SnapOnRelease, AsNeeded, AlwaysOff, AlwaysOn })

class ScrollIndicator extends Control {
    constructor(parent = null){
        super(parent)
        this.$plain(ScrollIndicator.ORIENTATION, Vertical)
        this.$plain(ScrollIndicator.VERTICAL, true)
    }
    $setActiveFromView(value){ this.$plain(ScrollIndicator.ACTIVE, !!value) }
    setSize(value){ engine.batch(() => this.$setSize(value)) }
    setPosition(value){ engine.batch(() => this.$setPosition(value)) }
}
Object.assign(ScrollIndicator.prototype, indicatorPart)
defineType(ScrollIndicator, {
    props: {
        size: 'real',
        position: 'real',
        active: 'bool',
        orientation: 'int',
        horizontal: { kind: 'bool', readonly: true },
        vertical: { kind: 'bool', readonly: true },
        minimumSize: 'real',
        visualSize: { kind: 'real', readonly: true },
        visualPosition: { kind: 'real', readonly: true },
    },
})
ScrollIndicator.$setters = withSetters(Control, [
    [ScrollIndicator.SIZE, (c, v) => c.$setSize(v)],
    [ScrollIndicator.POSITION, (c, v) => c.$setPosition(v)],
    [ScrollIndicator.MINIMUMSIZE, (c, v) => c.$setMinimumSize(v)],
    [ScrollIndicator.ORIENTATION, (c, v) => c.$setOrientation(v)],
])
// the Control part writes the content item by $resizeContent: the handle's place
for(let Class of [ScrollBar, ScrollIndicator]){
    let base = Class.prototype.$updatePaddings
    Class.prototype.$updatePaddings = function(){
        base.call(this)
        this.$resizeContent()
    }
}

// ---- the attached properties (QQuickScrollBarAttached, QQuickScrollIndicatorAttached) ----
// the view the bars of an item are attached to: a Flickable, ListView, GridView or TableView (has visibleArea)
const isView = (item) => item && typeof item.$updateVisibleArea === 'function'

class BarsAttached extends GroupObject {
    constructor(owner){
        super(owner)
        this.$installed = false
    }
    $view(){ return isView(this.$owner) ? this.$owner : null }
    // QQuickScrollBarAttached::setVertical / setHorizontal
    $setBar(id, bar){
        let old = this.$v[id]
        if(old === bar) return
        let view = this.$view()
        if(old && !(old.$f & engine.DESTROYED)){
            old.$attached = null
            if(old.$layoutLink) for(let link of old.$layoutLink) engine.unsubscribe(link)
            old.$layoutLink = null
        }
        super.$write(id, bar || null)
        if(!bar || !view) return
        let horizontal = id === this.constructor.HORIZONTAL
        bar.$attached = this
        bar.$attachedAxis = horizontal ? 'x' : 'y'
        if(!bar.$parent || !bar.$parent.$isItem) bar.$setParent(view)
        bar.orientation = horizontal ? Horizontal : Vertical
        let layout = () => this.$layout(bar)
        bar.$layoutLink = [
            engine.connect(view, Item.SIGNAL_widthChanged, layout, null, engine.PRIORITY_NORMAL, bar),
            engine.connect(view, Item.SIGNAL_heightChanged, layout, null, engine.PRIORITY_NORMAL, bar),
            engine.connect(bar, Item.SIGNAL_widthChanged, layout, null, engine.PRIORITY_NORMAL, bar),
            engine.connect(bar, Item.SIGNAL_heightChanged, layout, null, engine.PRIORITY_NORMAL, bar),
        ]
        let moving = view.constructor.SIGNAL_movingChanged
        if(moving !== undefined) bar.$layoutLink.push(engine.connect(view, moving, () => bar.$setActiveFromView(view.moving), null, engine.PRIORITY_NORMAL, bar))
        view.$scrollBars = view.$scrollBars || new ScrollBarsOfView(view)
        view.$scrollBars.$add(this)
        this.$layout(bar)
        view.$updateVisibleArea()
    }
    // QQuickScrollBarAttachedPrivate::layoutVertical / layoutHorizontal: along the right (bottom) edge of the view
    $layout(bar){
        let view = this.$view()
        if(!view || bar.$parent !== view) return
        if(bar.$attachedAxis === 'x'){
            bar.$explicit |= 1
            bar.$write(Item.WIDTH, view.$v[Item.WIDTH])
            bar.$write(Item.Y, view.$v[Item.HEIGHT] - bar.$v[Item.HEIGHT])
        } else {
            bar.$explicit |= 2
            bar.$write(Item.HEIGHT, view.$v[Item.HEIGHT])
            bar.$write(Item.X, view.$v[Item.WIDTH] - bar.$v[Item.WIDTH])
        }
    }
    // the view moved: the size and position of its bars
    $areaChanged(values){
        for(let id of [this.constructor.HORIZONTAL, this.constructor.VERTICAL]){
            let bar = this.$v[id]
            if(!bar || bar.$f & engine.DESTROYED || bar.$moving) continue
            let area = values[bar.$attachedAxis]
            bar.$syncing = true
            try {
                bar.$setSize(area.size)
                bar.$setPosition(area.pos)
            } finally {
                bar.$syncing = false
            }
        }
    }
    // QQuickScrollBarAttachedPrivate::scrollVertical / scrollHorizontal: the bar moved, the content follows
    $barMoved(bar){
        let view = this.$view()
        if(!view) return
        let axis = bar.$attachedAxis
        let b = view.$flickBounds(axis)
        let viewSize = view.$v[axis === 'x' ? Item.WIDTH : Item.HEIGHT]
        let range = b.max - b.min + viewSize
        bar.$moving = true
        try {
            view.$setContentPos(axis, b.min + bar.$v[bar.constructor.POSITION] * range)
        } finally {
            bar.$moving = false
        }
    }
    $write(id, value){
        if(id === this.constructor.HORIZONTAL || id === this.constructor.VERTICAL) return this.$setBar(id, value)
        super.$write(id, value)
    }
}

class ScrollBarAttached extends BarsAttached {}
// a scroll bar moves its view, an indicator only shows where it is
ScrollBarAttached.prototype.$scrolls = true
defineType(ScrollBarAttached, { props: { horizontal: { kind: 'var', initial: null }, vertical: { kind: 'var', initial: null } } })
class ScrollIndicatorAttached extends BarsAttached {}
defineType(ScrollIndicatorAttached, { props: { horizontal: { kind: 'var', initial: null }, vertical: { kind: 'var', initial: null } } })
for(let Class of [ScrollBarAttached, ScrollIndicatorAttached]){
    for(let name of ['horizontal', 'vertical']){
        let id = Class[name.toUpperCase()]
        let get = Object.getOwnPropertyDescriptor(Class.prototype, name).get
        Object.defineProperty(Class.prototype, name, { get, set(value){ this.$setBar(id, value) }, configurable: true })
    }
}

// ScrollBar.SnapAlways written in an item: the name is its attached object (as Keys.BeforeItem), which has the enums too
for(let name of ['NoSnap', 'SnapAlways', 'SnapOnRelease', 'AsNeeded', 'AlwaysOff', 'AlwaysOn']){
    Object.defineProperty(ScrollBarAttached.prototype, name, { value: ScrollBar[name], configurable: true })
}

// the attached bar groups of a view, told when its visible area changes
class ScrollBarsOfView {
    constructor(view){ this.view = view; this.groups = new Set() }
    $add(group){ this.groups.add(group) }
    $visibleAreaChanged(values){ for(let group of this.groups) group.$areaChanged(values) }
}

// ScrollBar.vertical / ScrollIndicator.vertical on an Item
function installAttached(Item){
    let set = (name, Group) => {
        let prop = Item.$type.propByName.get(name)
        prop.group = Group
        let id = prop.id
        Object.defineProperty(Item.prototype, name, {
            get(){ return this.$v[id] || (this.$v[id] = new Group(this)) },
            configurable: true,
        })
    }
    set('ScrollBar', ScrollBarAttached)
    set('ScrollIndicator', ScrollIndicatorAttached)
}
installAttached(Item)

module.exports = { ScrollBar, ScrollIndicator, ScrollBarAttached, ScrollIndicatorAttached, visualArea, Horizontal, Vertical }
