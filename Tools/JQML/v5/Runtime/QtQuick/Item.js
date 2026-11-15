const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { QtObject, GroupObject } = require('../Core/QtObject')

// DOM write bits, flushed once per update phase in $paint
const GEOMETRY = 1
const VISIBILITY = 2
const OPACITY = 4
const Z = 8
const CLIP = 16
const STYLE = 32
const ALL = 0xffff

// explicit size bits: an explicitly set (or bound, or anchored) size is not driven by the implicit size
const EXPLICIT_WIDTH = 1
const EXPLICIT_HEIGHT = 2

// anchored directions: the item's own x / y assignments are ignored while anchored
const ANCHOR_H = 1
const ANCHOR_V = 2

const LEFT = 0, RIGHT = 1, HCENTER = 2, TOP = 3, BOTTOM = 4, VCENTER = 5, BASELINE = 6

// Item.TransformOrigin: TopLeft, Top, TopRight, Left, Center, Right, BottomLeft, Bottom, BottomRight
const ORIGIN = ['0 0', '50% 0', '100% 0', '0 50%', '50% 50%', '100% 50%', '0 100%', '50% 100%', '100% 100%']

class AnchorLine {
    constructor(item, line){
        this.item = item
        this.line = line
    }
}

let styleInjected = false
function injectStyle(){
    if(styleInjected || typeof document === 'undefined') return
    styleInjected = true
    let style = document.createElement('style')
    style.textContent = `
        html, body { margin: 0; padding: 0; width: 100%; height: 100%; overflow: hidden; }
        .jq2 { position: absolute; left: 0; top: 0; z-index: 0; box-sizing: border-box; overflow: visible; pointer-events: none; user-select: none; }
        .jq2-input { user-select: text; }
        .jq2-text { display: flex; font-family: 'Segoe UI'; font-size: 12px; }
        .jq2-text-inner { white-space: pre; line-height: normal; }
        .jq2-text-inner p { margin: 0; }
    `
    document.head.appendChild(style)
}

const geometry = (bits) => (item) => engine.markDirty(item, bits)

function sizeChanged(item){
    engine.markDirty(item, GEOMETRY)
    if(item.$parent && item.$parent.$childGeometryChanged) item.$parent.$childGeometryChanged(item)
}

class Item extends QtObject {
    constructor(parent = null){
        super(null)
        this.$explicit = 0
        this.$anchored = 0
        this.$anchorsReady = false
        this.$ownVisible = true
        this.$dirty = 0
        this.$deferred = 0          // dirty bits held while the item is hidden
        this.$polishQueued = false
        this.$lines = null
        this.$paintOrder = null     // child items in paint order, kept until the children or their z change
        this.$subFocusItem = null   // the item that has focus below this one in its focus scope (KeyDelivery.js)
        this.$notifiedFocus = false
        this.$notifiedActiveFocus = false
        injectStyle()
        this.$dom = typeof document !== 'undefined' ? document.createElement('div') : null
        if(this.$dom) this.$dom.className = 'jq2'
        if(parent) this.$setParent(parent)
        // first paint writes everything, defaults included (a Rectangle is white until told otherwise)
        engine.markDirty(this, ALL)
    }

    $parentChanged(old, parent){
        if(this.$dom){
            if(parent && parent.$dom) parent.$dom.appendChild(this.$dom)
            else if(this.$dom.parentNode) this.$dom.parentNode.removeChild(this.$dom)
        }
        this.$write(Item.VISIBLE, this.$ownVisible)
        // QQuickItemPrivate::addChild / removeChild
        if(old instanceof Item){
            old.$paintOrder = null
            engine.emit(old, Item.SIGNAL_childrenChanged)
        }
        if(parent instanceof Item){
            parent.$paintOrder = null
            engine.emit(parent, Item.SIGNAL_childrenChanged)
        }
        if(old && old.$childGeometryChanged) old.$childGeometryChanged(this)
        if(parent && parent.$childGeometryChanged) parent.$childGeometryChanged(this)
    }

    $assign(id, value){
        if(this.$anchored & ANCHOR_H && (id === Item.X || (id === Item.WIDTH && this.$anchorsFillWidth))) return
        if(this.$anchored & ANCHOR_V && (id === Item.Y || (id === Item.HEIGHT && this.$anchorsFillHeight))) return
        if(id === Item.WIDTH) this.$explicit |= EXPLICIT_WIDTH
        if(id === Item.HEIGHT) this.$explicit |= EXPLICIT_HEIGHT
        super.$assign(id, value)
    }

    $bind(id, fn, scope, self){
        if(id === Item.WIDTH) this.$explicit |= EXPLICIT_WIDTH
        if(id === Item.HEIGHT) this.$explicit |= EXPLICIT_HEIGHT
        return super.$bind(id, fn, scope, self)
    }

    // visible holds the effective visibility: own flag and the parent's
    $write(id, value){
        if(id === Item.STATE && Item.$states){
            Item.$states.write(this, value)
            return
        }
        if(id === Item.FOCUS){
            this.$setFocus(value)
            return
        }
        if(id === Item.VISIBLE){
            if(value === undefined){
                engine.reportError('Cannot assign [undefined] to Bool', this.constructor.name + '.visible')
                return
            }
            // the DOM shows the own flag, which can change while the effective value stays false
            if(this.$ownVisible !== !!value) engine.markDirty(this, VISIBILITY)
            this.$ownVisible = !!value
            let parent = this.$parent
            value = this.$ownVisible && (!parent || !(parent instanceof Item) || parent.$v[Item.VISIBLE])
        }
        super.$write(id, value)
    }

    $componentComplete(){
        super.$componentComplete()
        this.$anchorsReady = true
        let anchors = this.$v[Item.ANCHORS]
        if(anchors) setupAnchors(anchors)
        if(this.$stateMachine) Item.$states.componentComplete(this)
    }

    // Item.children holds the visual children; resources the other objects; data both
    get children(){
        engine.track(this, Item.SIGNAL_childrenChanged)
        return this.$children ? this.$children.filter(child => child instanceof Item) : []
    }
    // a list assignment: QQuickItemPrivate::children_clear, then children_append for each item
    set children(list){
        for(let child of this.children) child.$setParent(null)
        for(let child of list || []){
            if(child instanceof Item) child.$setParent(this)
        }
    }
    get resources(){ return this.$children ? this.$children.filter(child => !(child instanceof Item)) : [] }
    get data(){ return this.$children ? this.$children.slice() : [] }

    get left(){ return this.$line(LEFT) }
    get right(){ return this.$line(RIGHT) }
    get horizontalCenter(){ return this.$line(HCENTER) }
    get top(){ return this.$line(TOP) }
    get bottom(){ return this.$line(BOTTOM) }
    get verticalCenter(){ return this.$line(VCENTER) }
    get baseline(){ return this.$line(BASELINE) }

    $line(line){
        if(!this.$lines) this.$lines = new Array(7)
        return this.$lines[line] || (this.$lines[line] = new AnchorLine(this, line))
    }

    // a hidden item's DOM is not shown: it writes its display now and the rest when it is shown
    $flush(bits){
        if(this.$v[Item.VISIBLE]){
            this.$paint(bits)
            return
        }
        this.$deferred |= bits & ~VISIBILITY
        if(bits & VISIBILITY) this.$paint(VISIBILITY)
    }

    $paint(bits){
        let dom = this.$dom
        if(!dom) return
        let v = this.$v
        let style = dom.style
        if(bits & GEOMETRY){
            // a default is not written: the browser keeps every inline declaration per element
            let transformed = v[Item.ROTATION] !== 0 || v[Item.SCALE] !== 1
            let transform = v[Item.X] !== 0 || v[Item.Y] !== 0 || transformed ? 'translate(' + v[Item.X] + 'px,' + v[Item.Y] + 'px)' : ''
            if(v[Item.ROTATION] !== 0) transform += ' rotate(' + v[Item.ROTATION] + 'deg)'
            if(v[Item.SCALE] !== 1) transform += ' scale(' + v[Item.SCALE] + ')'
            style.transform = transform
            style.transformOrigin = transformed ? ORIGIN[v[Item.TRANSFORMORIGIN]] || '50% 50%' : ''
            style.width = Math.max(0, v[Item.WIDTH]) + 'px'
            style.height = Math.max(0, v[Item.HEIGHT]) + 'px'
        }
        if(bits & VISIBILITY) style.display = this.$ownVisible && !this.$culled ? '' : 'none'
        if(bits & OPACITY) style.opacity = v[Item.OPACITY] === 1 ? '' : v[Item.OPACITY]
        if(bits & Z) style.zIndex = v[Item.Z] === 0 ? '' : v[Item.Z]
        if(bits & CLIP) style.overflow = v[Item.CLIP] ? 'hidden' : ''
    }

    // A position set by a view or a positioner: QQuickItem::geometryChange -> QQuickAnchorsPrivate::updateMe
    // puts an anchored axis back at once, so it stays where the anchors keep it
    $place(id, value){
        if(!(this.$anchored & (id === Item.X ? ANCHOR_H : ANCHOR_V))) this.$write(id, value)
    }

    $disposeNative(){
        if(this.$dom && this.$dom.parentNode) this.$dom.parentNode.removeChild(this.$dom)
        this.$dom = null
    }

    // Attaches a root item to a DOM element.
    $mount(element){
        element.appendChild(this.$dom)
    }
}

defineType(Item, {
    props: {
        x: { kind: 'real', effect: geometry(GEOMETRY) },
        y: { kind: 'real', effect: geometry(GEOMETRY) },
        z: {
            kind: 'real',
            effect(item){
                engine.markDirty(item, Z)
                if(item.$parent instanceof Item) item.$parent.$paintOrder = null
            },
        },
        width: { kind: 'real', effect: sizeChanged },
        height: { kind: 'real', effect: sizeChanged },
        implicitWidth: { kind: 'real', effect(item, value){ if(!(item.$explicit & EXPLICIT_WIDTH)) item.$write(Item.WIDTH, value) } },
        implicitHeight: { kind: 'real', effect(item, value){ if(!(item.$explicit & EXPLICIT_HEIGHT)) item.$write(Item.HEIGHT, value) } },
        opacity: { kind: 'real', initial: 1, effect: geometry(OPACITY) },
        scale: { kind: 'real', initial: 1, effect: geometry(GEOMETRY) },
        rotation: { kind: 'real', effect: geometry(GEOMETRY) },
        transformOrigin: { kind: 'int', initial: 4, effect: geometry(GEOMETRY) },
        visible: {
            kind: 'bool',
            initial: true,
            effect(item, value){
                engine.markDirty(item, value ? VISIBILITY | item.$deferred : VISIBILITY)
                if(value) item.$deferred = 0
                if(item.$children){
                    for(let child of item.$children){
                        if(child instanceof Item) child.$write(Item.VISIBLE, child.$ownVisible)
                    }
                }
                if(item.$parent && item.$parent.$childGeometryChanged) item.$parent.$childGeometryChanged(item)
            },
        },
        enabled: { kind: 'bool', initial: true, effect(item){ item.$enabledChanged() } },
        focus: 'bool',
        activeFocus: 'bool',
        activeFocusOnTab: 'bool',
        antialiasing: 'bool',
        smooth: { kind: 'bool', initial: true },
        baselineOffset: 'real',
        layer: 'var',
        clip: { kind: 'bool', effect: geometry(CLIP) },
        state: 'string',
        states: { kind: 'var', effect(item){ if(Item.$states) Item.$states.statesChanged(item) } },
        transitions: 'var',
        anchors: { group: null },   // set below, Anchors needs Item
        Layout: { group: null },    // set by Layouts.js
        Keys: { group: null },      // set by KeyDelivery.js
        KeyNavigation: { group: null },
        ListView: { group: null },  // set by ListView.js
    },
    signals: ['childrenChanged'],
})

// QQuickItemPrivate::culled: not painted, the visible property stays (a delegate in the reuse pool)
Item.prototype.$culled = false

Object.assign(Item, { TopLeft: 0, Top: 1, TopRight: 2, Left: 3, Center: 4, Right: 5, BottomLeft: 6, Bottom: 7, BottomRight: 8 })

// ---- anchors: internal bindings that write x/width and y/height directly ----

const EXPLICIT_LEFT = 1, EXPLICIT_RIGHT = 2, EXPLICIT_TOP = 4, EXPLICIT_BOTTOM = 8

// hcenter()/vcenter() of qquickanchors.cpp: qRound of the half, unless alignWhenCentered is off
function half(item, size){
    let anchors = item.$v[Item.ANCHORS]
    return anchors && !anchors.$v[Anchors.ALIGNWHENCENTERED] ? size / 2 : Math.round(size / 2)
}

// Position of an anchor line in the coordinates of the anchored item's parent
function linePos(item, anchorLine){
    if(!anchorLine) return 0
    let target = anchorLine.item
    let base = 0
    if(target !== item.$parent){
        base = anchorLine.line >= TOP ? target.y : target.x
    }
    switch(anchorLine.line){
        case LEFT: case TOP: return base
        case RIGHT: return base + target.width
        case BOTTOM: return base + target.height
        case HCENTER: return base + half(target, target.width)
        case VCENTER: return base + half(target, target.height)
        case BASELINE: return base + target.baselineOffset
    }
    return 0
}

// The fields of the group are read untracked: a write to one of them updates its axis itself, like the setters of QQuickAnchors
function margin(anchors, id){
    return anchors.$v[id]
}

const field = (anchors, id) => anchors.$v[id]

const H = {
    fillX: (item, a) => linePos(item, field(a, Anchors.FILL).left) + margin(a, Anchors.LEFTMARGIN),
    fillWidth: (item, a) => field(a, Anchors.FILL).width - margin(a, Anchors.LEFTMARGIN) - margin(a, Anchors.RIGHTMARGIN),
    centerX: (item, a) => linePos(item, field(a, Anchors.CENTERIN).horizontalCenter) - half(item, item.width) + field(a, Anchors.HORIZONTALCENTEROFFSET),
    leftX: (item, a) => linePos(item, field(a, Anchors.LEFT)) + margin(a, Anchors.LEFTMARGIN),
    leftRightWidth: (item, a) => linePos(item, field(a, Anchors.RIGHT)) - margin(a, Anchors.RIGHTMARGIN) - linePos(item, field(a, Anchors.LEFT)) - margin(a, Anchors.LEFTMARGIN),
    rightX: (item, a) => linePos(item, field(a, Anchors.RIGHT)) - margin(a, Anchors.RIGHTMARGIN) - item.width,
    hcenterX: (item, a) => linePos(item, field(a, Anchors.HORIZONTALCENTER)) - half(item, item.width) + field(a, Anchors.HORIZONTALCENTEROFFSET),
}

const V = {
    fillY: (item, a) => linePos(item, field(a, Anchors.FILL).top) + margin(a, Anchors.TOPMARGIN),
    fillHeight: (item, a) => field(a, Anchors.FILL).height - margin(a, Anchors.TOPMARGIN) - margin(a, Anchors.BOTTOMMARGIN),
    centerY: (item, a) => linePos(item, field(a, Anchors.CENTERIN).verticalCenter) - half(item, item.height) + field(a, Anchors.VERTICALCENTEROFFSET),
    topY: (item, a) => linePos(item, field(a, Anchors.TOP)) + margin(a, Anchors.TOPMARGIN),
    topBottomHeight: (item, a) => linePos(item, field(a, Anchors.BOTTOM)) - margin(a, Anchors.BOTTOMMARGIN) - linePos(item, field(a, Anchors.TOP)) - margin(a, Anchors.TOPMARGIN),
    bottomY: (item, a) => linePos(item, field(a, Anchors.BOTTOM)) - margin(a, Anchors.BOTTOMMARGIN) - item.height,
    vcenterY: (item, a) => linePos(item, field(a, Anchors.VERTICALCENTER)) - half(item, item.height) + field(a, Anchors.VERTICALCENTEROFFSET),
    baselineY: (item, a) => linePos(item, field(a, Anchors.BASELINE)) - item.baselineOffset + field(a, Anchors.BASELINEOFFSET),
}

function setupAnchors(anchors){
    let item = anchors.$owner
    let v = anchors.$v
    let fill = v[Anchors.FILL], centerIn = v[Anchors.CENTERIN]

    // Only bindings created by the anchors are replaced; a user binding of width/height stays
    // unless the anchors now drive that size.
    let bind = (id, fn) => {
        let previous = item.$findBinding(id)
        if(previous && (fn || previous.scope === anchors)){
            item.$takeBinding(id)
            engine.disposeBinding(previous)
        }
        if(fn){
            let binding = new engine.Binding(item, id, fn, anchors)
            binding.flags |= engine.SETTER
            binding.next = item.$bindings
            item.$bindings = binding
            engine.scheduleBinding(binding)
        }
    }

    let horizontalChanged = false, verticalChanged = false
    let left = v[Anchors.LEFT], right = v[Anchors.RIGHT], hcenter = v[Anchors.HORIZONTALCENTER]
    let horizontal = fill ? 'fill' : centerIn ? 'center' : left && right ? 'leftRight' : left ? 'left' : right ? 'right' : hcenter ? 'hcenter' : null
    if(horizontal !== anchors.$horizontal){
        anchors.$horizontal = horizontal
        item.$anchored = (item.$anchored & ~ANCHOR_H) | (horizontal ? ANCHOR_H : 0)
        item.$anchorsFillWidth = horizontal === 'fill' || horizontal === 'leftRight'
        if(item.$anchorsFillWidth) item.$explicit |= EXPLICIT_WIDTH
        horizontalChanged = true
    }

    let top = v[Anchors.TOP], bottom = v[Anchors.BOTTOM], vcenter = v[Anchors.VERTICALCENTER], baseline = v[Anchors.BASELINE]
    let vertical = fill ? 'fill' : centerIn ? 'center' : top && bottom ? 'topBottom' : top ? 'top' : bottom ? 'bottom' : vcenter ? 'vcenter' : baseline ? 'baseline' : null
    if(vertical !== anchors.$vertical){
        anchors.$vertical = vertical
        item.$anchored = (item.$anchored & ~ANCHOR_V) | (vertical ? ANCHOR_V : 0)
        item.$anchorsFillHeight = vertical === 'fill' || vertical === 'topBottom'
        if(item.$anchorsFillHeight) item.$explicit |= EXPLICIT_HEIGHT
        verticalChanged = true
    }

    // fillChanged(): the position first, then the size
    if(horizontalChanged) bind(Item.X, { fill: H.fillX, center: H.centerX, leftRight: H.leftX, left: H.leftX, right: H.rightX, hcenter: H.hcenterX }[horizontal])
    if(verticalChanged) bind(Item.Y, { fill: V.fillY, center: V.centerY, topBottom: V.topY, top: V.topY, bottom: V.bottomY, vcenter: V.vcenterY, baseline: V.baselineY }[vertical])
    if(horizontalChanged) bind(Item.WIDTH, item.$anchorsFillWidth ? (horizontal === 'fill' ? H.fillWidth : H.leftRightWidth) : null)
    if(verticalChanged) bind(Item.HEIGHT, item.$anchorsFillHeight ? (vertical === 'fill' ? V.fillHeight : V.topBottomHeight) : null)
}

const reanchor = (anchors) => { if(anchors.$owner.$anchorsReady) setupAnchors(anchors) }
const HORIZONTAL = 1, VERTICAL = 2
const AXIS_IDS = []

// QQuickAnchorsPrivate::updateHorizontalAnchors / updateVerticalAnchors
function updateAxes(anchors, axes){
    let item = anchors.$owner
    for(let id of AXIS_IDS[axes]){
        let binding = item.$findBinding(id)
        if(binding && binding.scope === anchors && !(binding.flags & engine.PENDING)) engine.runBinding(binding)
    }
}
const axisOf = (axes) => (anchors) => updateAxes(anchors, axes)

// QQuickAnchors::setMargins: the sides not set explicitly take the value and notify, then the geometry
function marginsWritten(anchors, value){
    let axes = 0
    for(let [name, bit, axis] of MARGIN_SIDES){
        let id = Anchors[name.toUpperCase()]
        let old = anchors.$v[id]
        if(anchors.$explicitMargins & bit || old === value) continue
        anchors.$v[id] = value
        axes |= axis
        engine.emit(anchors, Anchors.$type.props[id].signal, old, value)
    }
    if(axes) updateAxes(anchors, axes)
}
const LINE_AXES = { fill: HORIZONTAL | VERTICAL, centerIn: HORIZONTAL | VERTICAL, left: HORIZONTAL, right: HORIZONTAL, horizontalCenter: HORIZONTAL, top: VERTICAL, bottom: VERTICAL, verticalCenter: VERTICAL, baseline: VERTICAL }
const MARGIN_SIDES = [['rightMargin', EXPLICIT_RIGHT, HORIZONTAL], ['leftMargin', EXPLICIT_LEFT, HORIZONTAL], ['topMargin', EXPLICIT_TOP, VERTICAL], ['bottomMargin', EXPLICIT_BOTTOM, VERTICAL]]
const EXPLICIT_BITS = { leftMargin: EXPLICIT_LEFT, rightMargin: EXPLICIT_RIGHT, topMargin: EXPLICIT_TOP, bottomMargin: EXPLICIT_BOTTOM }

class Anchors extends GroupObject {
    constructor(owner){
        super(owner)
        this.$horizontal = null
        this.$vertical = null
        this.$explicitMargins = 0
    }

    // Any write of a side margin (QML, a binding, a Binding element) makes it explicit, like
    // QQuickAnchors::setLeftMargin.
    $write(id, value){
        let prop = Anchors.$type.props[id]
        let bit = EXPLICIT_BITS[prop.name]
        if(bit) this.$explicitMargins |= bit
        // QQuickAnchors::setLeft, setFill: the signal first, then the geometry
        let axes = LINE_AXES[prop.name]
        let old = axes ? this.$v[id] : undefined
        super.$write(id, value)
        if(axes && this.$v[id] !== old){
            reanchor(this)
            updateAxes(this, axes)
        }
    }

    $bind(id, fn, scope, self){
        let bit = EXPLICIT_BITS[Anchors.$type.props[id].name]
        if(bit) this.$explicitMargins |= bit
        return super.$bind(id, fn, scope, self)
    }
}

defineType(Anchors, {
    props: {
        fill: 'var',
        centerIn: 'var',
        left: 'var',
        right: 'var',
        horizontalCenter: 'var',
        top: 'var',
        bottom: 'var',
        verticalCenter: 'var',
        baseline: 'var',
        // QQuickAnchors::setLeftMargin, setHorizontalCenterOffset: the geometry first, then the signal
        alignWhenCentered: { kind: 'bool', initial: true, effect: axisOf(HORIZONTAL | VERTICAL) },
        margins: { kind: 'real', effect: marginsWritten },
        leftMargin: { kind: 'real', effect: axisOf(HORIZONTAL) },
        rightMargin: { kind: 'real', effect: axisOf(HORIZONTAL) },
        topMargin: { kind: 'real', effect: axisOf(VERTICAL) },
        bottomMargin: { kind: 'real', effect: axisOf(VERTICAL) },
        horizontalCenterOffset: { kind: 'real', effect: axisOf(HORIZONTAL) },
        verticalCenterOffset: { kind: 'real', effect: axisOf(VERTICAL) },
        baselineOffset: { kind: 'real', effect: axisOf(VERTICAL) },
    },
})

AXIS_IDS[HORIZONTAL] = [Item.X, Item.WIDTH]
AXIS_IDS[VERTICAL] = [Item.Y, Item.HEIGHT]
AXIS_IDS[HORIZONTAL | VERTICAL] = [Item.X, Item.Y, Item.WIDTH, Item.HEIGHT]

// The anchors group property needs the Anchors class, so it is wired after both exist.
{
    let prop = Item.$type.propByName.get('anchors')
    prop.group = Anchors
    let id = prop.id
    Object.defineProperty(Item.prototype, 'anchors', {
        get(){
            let anchors = this.$v[id]
            if(!anchors){
                anchors = this.$v[id] = new Anchors(this)
                engine.scheduleComponentComplete(this)
            }
            return anchors
        },
        configurable: true,
    })
}

// Q_OBJECT_COMPAT_PROPERTY of QQuickItemPrivate (Qt 6): bindings on them update before the notify signal
Item.prototype.$bindableSignals = new Set([Item.SIGNAL_xChanged, Item.SIGNAL_yChanged, Item.SIGNAL_widthChanged, Item.SIGNAL_heightChanged])

module.exports = { Item, Anchors, AnchorLine, bits: { GEOMETRY, VISIBILITY, OPACITY, Z, CLIP, STYLE } }
