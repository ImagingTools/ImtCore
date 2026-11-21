const engine = require('../Core/engine')
const { Item } = require('./Item')

const NoButton = 0, LeftButton = 1, RightButton = 2, MiddleButton = 4

// ---- scene geometry -------------------------------------------------------------------------

function scenePos(item){
    let x = 0, y = 0
    for(let it = item; it instanceof Item; it = it.$parent){
        x += it.$v[Item.X]
        y += it.$v[Item.Y]
    }
    return { x, y }
}

function contains(item, x, y){
    return x >= 0 && y >= 0 && x <= item.$v[Item.WIDTH] && y <= item.$v[Item.HEIGHT]
}

// children in paint order: z, then declaration order. Hit testing runs on every mouse move over the whole tree,
// so the order is kept on the item (Item drops it when its children or their z change)
function paintOrder(item){
    if(item.$paintOrder) return item.$paintOrder
    let items = []
    let stacked = false
    for(let child of item.$children || []){
        if(!(child instanceof Item)) continue
        if(child.$v[Item.Z] !== 0) stacked = true
        items.push(child)
    }
    if(stacked){
        let index = new Map(items.map((child, i) => [child, i]))
        items.sort((a, b) => a.$v[Item.Z] - b.$v[Item.Z] || index.get(a) - index.get(b))
    }
    item.$paintOrder = items
    return items
}

// an item that takes mouse events: presses, hover or the wheel. $acceptsMouse: an item that takes the press without
// handling it here (TextInput, TextEdit: setAcceptedMouseButtons(Qt::LeftButton), their native control does the rest)
const handlesMouse = (item) => item.$mousePress !== undefined || item.$acceptsMouse !== undefined || item.$acceptsHover !== undefined || item.$wheel !== undefined

// Items under a scene point that take mouse events (all items with `all`), topmost first, each with the point in its
// own coordinates. Invisible or disabled items hide their subtree; a clipping item hides children outside it.
function itemsAt(item, x, y, out, all){
    if(!item.$v[Item.VISIBLE]) return out
    if(!item.$v[Item.ENABLED]) return out
    let lx = x - item.$v[Item.X], ly = y - item.$v[Item.Y]
    let inside = contains(item, lx, ly)
    if(item.$v[Item.CLIP] && !inside) return out
    let children = paintOrder(item)
    if(children){
        for(let i = children.length - 1; i >= 0; i--) itemsAt(children[i], lx, ly, out, all)
    }
    if(inside && (all || handlesMouse(item))) out.push({ item, x: lx, y: ly })
    return out
}

// ---- delivery -------------------------------------------------------------------------------

// QQuickDeliveryAgentPrivate::sendFilteredPointerEvent: the ancestors of the receiver that filter the events of their
// children (Flickable: childMouseEventFilter) see the event first, the outermost one first. The result: the item
// that takes the mouse instead of the receiver, or null; filtered: the receiver does not get the event
function filterChildEvent(kind, receiver, x, y, buttons, seen){
    let filters = []
    for(let it = receiver && receiver.$parent; it instanceof Item; it = it.$parent){
        if(it.$filterPointer) filters.unshift(it)
    }
    let result = { grabber: null, filtered: false }
    for(let filter of filters){
        if(seen){
            if(seen.has(filter)) continue
            seen.add(filter)
        }
        let p = local(filter, x, y)
        let r = filter.$filterPointer(kind, receiver, p.x, p.y, buttons)
        if(r.grab && !result.grabber) result.grabber = filter
        if(r.filter){
            result.filtered = true
            if(!result.grabber) result.grabber = filter
            break
        }
    }
    return result
}

// the grab goes to another item: the one that had it is told (QQuickWindow::mouseUngrabEvent, MouseArea.canceled)
function transferGrab(item){
    let old = scene.grabber
    scene.grabber = item
    if(old && old !== item && !(old.$f & engine.DESTROYED) && old.$ungrabMouse) old.$ungrabMouse()
    if(scene.grabber !== item) scene.grabber = item
}

const scene = {
    roots: [],
    element: null,
    grabber: null,
    pressedButtons: NoButton,
    hovered: [],
    x: -1,
    y: -1,
    modifiers: 0,
    contextMenuGrabbed: false,
}

function targetsAt(x, y, all = false){
    let out = []
    for(let i = scene.roots.length - 1; i >= 0; i--){
        let root = scene.roots[i]
        if(root.$f & engine.DESTROYED) continue
        itemsAt(root, x, y, out, all)
    }
    return out
}

function local(item, x, y){
    let pos = scenePos(item)
    return { x: x - pos.x, y: y - pos.y }
}

// native: the TextInput or TextEdit whose native control is under the point. True when the press reaches it before
// an item above takes it (QQuickDeliveryAgent, topmost first): the native control handles the press then
function press(x, y, button, buttons, modifiers, doubleClick, native = null){
    let reached = false
    engine.batch(() => {
        scene.x = x
        scene.y = y
        scene.pressedButtons = buttons
        scene.modifiers = modifiers
        let grabber = scene.grabber
        if(grabber && !(grabber.$f & engine.DESTROYED)){
            // another button while grabbed goes to the grabber
            let p = local(grabber, x, y)
            if(grabber.$mousePress) grabber.$mousePress(p.x, p.y, button, buttons, modifiers)
            return
        }
        scene.grabber = null
        let filtered = new Set()
        for(let target of targetsAt(x, y)){
            let item = target.item
            let filter = filterChildEvent('press', item, x, y, buttons, filtered)
            if(filter.filtered){
                scene.grabber = filter.grabber
                break
            }
            if(native && item === native){
                reached = true
                break
            }
            if(item.$mousePress){
                if(item.$mousePress(target.x, target.y, button, buttons, modifiers)){
                    scene.grabber = item
                    break
                }
            } else if(item.$acceptsMouse){
                scene.grabber = item
                // QQuickTextInput::mousePressEvent: activeFocusOnPress gives the field the focus (Qt::MouseFocusReason)
                if(item.$pressFocus && button === LeftButton) item.$pressFocus()
                break
            }
        }
        if(doubleClick && scene.grabber && scene.grabber.$mouseDoubleClick){
            let p = local(scene.grabber, x, y)
            scene.grabber.$mouseDoubleClick(p.x, p.y, button, buttons, modifiers)
        }
        scene.contextMenuGrabbed = !!(scene.grabber && button === RightButton && scene.grabber.$mousePress)
        // QWindowPrivate::maybeSynthesizeContextMenuEvent: a right press no item takes is a context menu event
        // (QQuickDeliveryAgentPrivate::deliverContextMenuEvent: ContextMenu of QtQuick.Controls)
        if(button === RightButton && !scene.grabber && !reached && contextMenuHandler) contextMenuHandler(x, y, targetsAt(x, y, true).map(t => t.item))
    })
    return reached
}
let contextMenuHandler = null
function setContextMenuHandler(fn){ contextMenuHandler = fn }

function move(x, y, buttons, modifiers){
    engine.batch(() => {
        scene.x = x
        scene.y = y
        scene.modifiers = modifiers
        let grabber = scene.grabber
        if(grabber && !(grabber.$f & engine.DESTROYED)){
            let filter = filterChildEvent('move', grabber, x, y, buttons, null)
            if(filter.grabber && filter.grabber !== grabber){
                transferGrab(filter.grabber)
                return
            }
            if(filter.filtered) return
            let p = local(grabber, x, y)
            if(grabber.$mouseMove) grabber.$mouseMove(p.x, p.y, buttons, modifiers)
        } else {
            scene.grabber = null
            hover(x, y, modifiers)
        }
    })
}

function release(x, y, button, buttons, modifiers){
    engine.batch(() => {
        scene.x = x
        scene.y = y
        scene.pressedButtons = buttons
        scene.modifiers = modifiers
        let grabber = scene.grabber
        if(grabber && !(grabber.$f & engine.DESTROYED)){
            let filter = filterChildEvent('release', grabber, x, y, buttons, null)
            let p = local(grabber, x, y)
            if(!filter.filtered && grabber.$mouseRelease) grabber.$mouseRelease(p.x, p.y, button, buttons, modifiers)
            else if(filter.filtered && grabber.$ungrabMouse) grabber.$ungrabMouse()
        }
        if(!buttons){
            scene.grabber = null
            hover(x, y, modifiers)
        }
    })
}

// QQuickDeliveryAgent: the topmost hover-accepting item under the cursor and those of its ancestors; what lies below it is not hovered
// QQuickDeliveryAgentPrivate::deliverHoverEventToItem: an item under the point that takes hover is entered or moved
// over, one that was hovered and is not is left now. A MouseArea does not block hover events (it ignores them).
function hoverItem(item, x, y, pass){
    let index = scene.hovered.indexOf(item)
    if(item.$acceptsHover && item.$acceptsHover() && contains(item, x, y)){
        pass.hovered.add(item)
        pass.leafFound = true
        if(index < 0){
            scene.hovered.push(item)
            item.$hoverEnter(x, y, pass.modifiers)
        } else item.$hoverMove(x, y, pass.modifiers)
    } else if(index >= 0){
        scene.hovered.splice(index, 1)
        if(!(item.$f & engine.DESTROYED) && item.$hoverLeave) item.$hoverLeave()
    }
    return false
}

// QQuickDeliveryAgentPrivate::deliverHoverEventRecursive: children topmost first, then the item. Once the hovered
// leaf is found the pass goes on to its ancestors only, not to the siblings below it.
function hoverRecursive(item, x, y, pass){
    let lx = x - item.$v[Item.X], ly = y - item.$v[Item.Y]
    let children = paintOrder(item)
    for(let i = children.length - 1; i >= 0; i--){
        let child = children[i]
        if(!child.$v[Item.VISIBLE] || !child.$v[Item.ENABLED]) continue
        if(child.$v[Item.CLIP] && !contains(child, lx - child.$v[Item.X], ly - child.$v[Item.Y])) continue
        if(hoverRecursive(child, lx, ly, pass)) return true
        if(pass.leafFound) break
    }
    return hoverItem(item, lx, ly, pass)
}

// QQuickDeliveryAgentPrivate::deliverHoverEvent: enter and move go out during the pass, so the item the pointer
// enters gets it before an item below that the pass does not visit is left (after the pass)
let hoverPass = null

function hover(x, y, modifiers){
    // an item shown from a hover handler: the next frame delivers the hover again
    if(hoverPass){
        engine.queueFrame(frameHover)
        return
    }
    let cursor = null
    for(let target of targetsAt(x, y)){
        let item = target.item
        if(cursor === null && item.$cursor) cursor = item.$cursor()
    }
    let pass = hoverPass = { hovered: new Set(), leafFound: false, modifiers }
    try {
        for(let i = scene.roots.length - 1; i >= 0 && !pass.leafFound; i--){
            let root = scene.roots[i]
            if(root.$f & engine.DESTROYED || !root.$v[Item.VISIBLE] || !root.$v[Item.ENABLED]) continue
            if(root.$v[Item.CLIP] && !contains(root, x - root.$v[Item.X], y - root.$v[Item.Y])) continue
            if(hoverRecursive(root, x, y, pass)) break
        }
        for(let item of scene.hovered.slice()){
            if(pass.hovered.has(item)) continue
            scene.hovered.splice(scene.hovered.indexOf(item), 1)
            if(!(item.$f & engine.DESTROYED)) item.$hoverLeave()
        }
    } finally {
        hoverPass = null
    }
    setCursor(cursor)
}

function leaveWindow(){
    engine.batch(() => {
        for(let item of scene.hovered){
            if(!(item.$f & engine.DESTROYED)) item.$hoverLeave()
        }
        scene.hovered = []
        scene.x = scene.y = -1
    })
}

function wheel(x, y, angleDelta, pixelDelta, buttons, modifiers){
    let accepted = false
    engine.batch(() => {
        for(let target of targetsAt(x, y)){
            let item = target.item
            if(item.$wheel && item.$wheel(target.x, target.y, angleDelta, pixelDelta, buttons, modifiers)){
                accepted = true
                break
            }
        }
    })
    return accepted
}

// the grab ends, like QQuickWindow::ungrabMouse when the grabber hides or gets disabled; the item is no longer hovered
function ungrab(item){
    if(scene.grabber === item) scene.grabber = null
    let index = scene.hovered.indexOf(item)
    if(index >= 0) scene.hovered.splice(index, 1)
}

// QQuickItem::isUnderMouse: the last pointer position inside the item (its shape, not what covers it), in its
// coordinates, or null
function pointerOver(item){
    if(scene.x < 0 || scene.y < 0) return false
    let p = local(item, scene.x, scene.y)
    return contains(item, p.x, p.y) ? p : null
}

// QQuickDeliveryAgentPrivate::deliverHoverEvent at the last pointer position (QQuickMouseArea::itemChange)
function deliverHover(){
    if(scene.x < 0 || scene.y < 0) return
    hover(scene.x, scene.y, scene.modifiers)
}

// the item is among the hovered items (QQuickDeliveryAgentPrivate::hoverItems)
function isHovered(item){
    return scene.hovered.indexOf(item) >= 0
}

// QQuickDeliveryAgentPrivate::flushFrameSynchronousEvents: when the scene changed, a still pointer gets a hover
// event at its last position with the next frame (items appear, hide or move under it), unless an item grabs it
const frameHover = {
    $f: 0,
    $frameQueued: false,
    $frame(){
        if(scene.grabber || scene.x < 0 || scene.y < 0) return
        hover(scene.x, scene.y, scene.modifiers)
    },
}

engine.onSceneChanged(() => {
    if(!scene.grabber && scene.x >= 0 && scene.y >= 0 && scene.roots.length) engine.queueFrame(frameHover)
})

// Qt::CursorShape -> the CSS cursor that looks like it (QCursor shapes by number; BitmapCursor / CustomCursor have none)
const CSS_CURSORS = ['default', 'default', 'crosshair', 'wait', 'text', 'ns-resize', 'ew-resize', 'nesw-resize', 'nwse-resize',
    'move', 'none', 'row-resize', 'col-resize', 'pointer', 'not-allowed', 'help', 'progress', 'grab', 'grabbing', 'copy', 'move',
    'alias']

function cssCursor(shape){
    if(typeof shape === 'number') return CSS_CURSORS[shape] || 'default'
    return shape || ''
}

// QGuiApplication::setOverrideCursor / restoreOverrideCursor: a cursor over every item (TableView resizing a column);
// the last one set wins, an owner takes its own off
const overrides = []
function setOverrideCursor(owner, shape){
    let at = overrides.findIndex(o => o.owner === owner)
    if(at >= 0) overrides.splice(at, 1)
    if(shape !== null && shape !== undefined) overrides.push({ owner, shape })
    setCursor(scene.lastCursor === undefined ? null : scene.lastCursor)
}

function setCursor(cursor){
    scene.lastCursor = cursor
    if(overrides.length) cursor = overrides[overrides.length - 1].shape
    if(scene.element) scene.element.style.cursor = cursor === null || cursor === undefined ? '' : cssCursor(cursor)
}

// ---- DOM ------------------------------------------------------------------------------------

const DOM_BUTTON = [LeftButton, MiddleButton, RightButton, 8, 16]

// Qt on macOS (Qt::AA_MacDontSwapCtrlAndMeta off): the Command key is Qt::ControlModifier, the Control key Qt::MetaModifier
const isMac = () => typeof navigator !== 'undefined' && !!navigator && /Mac|iPhone|iPad|iPod/.test(String(navigator.platform || ''))

function modifiersOf(e){
    let mac = isMac()
    return (e.shiftKey ? 0x02000000 : 0) | (e.altKey ? 0x08000000 : 0)
        | (e.ctrlKey ? (mac ? 0x10000000 : 0x04000000) : 0) | (e.metaKey ? (mac ? 0x04000000 : 0x10000000) : 0)
}

function pointOf(e){
    let rect = scene.element.getBoundingClientRect()
    return { x: e.clientX - rect.left, y: e.clientY - rect.top }
}

// a native control (input, textarea) handles its own pointer events
function isNative(e){
    let target = e.target
    return target && (target.tagName === 'INPUT' || target.tagName === 'TEXTAREA' || target.isContentEditable)
}

let listening = false
function listen(element){
    scene.element = element
    if(listening || typeof window === 'undefined') return
    listening = true
    window.addEventListener('mousedown', (e) => {
        let p = pointOf(e)
        // a native control (input, textarea) not made by a TextInput or TextEdit handles its own pointer events
        if(isNative(e) && !e.target.$item) return
        // the second press of a double click also delivers MouseButtonDblClick to the grabber. The press goes down
        // the scene as in Qt: when it reaches the TextInput of the native control under it, that control takes it
        let native = isNative(e) ? e.target.$item : null
        if(press(p.x, p.y, DOM_BUTTON[e.button] || LeftButton, e.buttons, modifiersOf(e), e.detail === 2, native)) return
        // the browser would blur the focused input and start a text selection; in Qt a press moves focus only
        // when QML asks for it (forceActiveFocus, activeFocusOnPress)
        e.preventDefault()
    })
    window.addEventListener('mousemove', (e) => {
        let p = pointOf(e)
        move(p.x, p.y, scene.grabber ? e.buttons : NoButton, modifiersOf(e))
    })
    window.addEventListener('mouseup', (e) => {
        let p = pointOf(e)
        release(p.x, p.y, DOM_BUTTON[e.button] || LeftButton, e.buttons, modifiersOf(e))
    })
    window.addEventListener('contextmenu', (e) => {
        if(scene.contextMenuGrabbed) e.preventDefault()
    })
    window.addEventListener('wheel', (e) => {
        let p = pointOf(e)
        // DOM delta points down for "scroll down", Qt's angleDelta points away from the user; 120 per notch
        let unit = e.deltaMode === 1 ? 40 : e.deltaMode === 2 ? 120 * 3 : 1.2
        let angleDelta = { x: Math.round(-e.deltaX * unit), y: Math.round(-e.deltaY * unit) }
        let pixelDelta = e.deltaMode === 0 ? { x: -e.deltaX, y: -e.deltaY } : { x: 0, y: 0 }
        if(wheel(p.x, p.y, angleDelta, pixelDelta, e.buttons, modifiersOf(e))) e.preventDefault()
    }, { passive: false })
    document.documentElement.addEventListener('mouseleave', leaveWindow)
    // QQuickDeliveryAgent: a single touch point is delivered as the left button (mouse synthesized from touch); the
    // browser does not pan or zoom the page, the views flick
    if(element && element.style) element.style.touchAction = 'none'
    let touchId = null
    const touchPoint = (e) => {
        for(let touch of e.changedTouches) if(touch.identifier === touchId) return touch
        return null
    }
    window.addEventListener('touchstart', (e) => {
        if(touchId !== null || e.touches.length !== 1) return
        let touch = e.changedTouches[0]
        if(isNative(e) && !e.target.$item) return
        touchId = touch.identifier
        let p = pointOf(touch)
        let native = isNative(e) ? e.target.$item : null
        if(press(p.x, p.y, LeftButton, LeftButton, modifiersOf(e), false, native)) return
        if(e.cancelable) e.preventDefault()
    }, { passive: false })
    window.addEventListener('touchmove', (e) => {
        let touch = touchPoint(e)
        if(!touch) return
        let p = pointOf(touch)
        move(p.x, p.y, LeftButton, modifiersOf(e))
        if(scene.grabber && e.cancelable) e.preventDefault()
    }, { passive: false })
    const touchEnd = (e) => {
        let touch = touchPoint(e)
        if(!touch) return
        touchId = null
        let p = pointOf(touch)
        release(p.x, p.y, LeftButton, NoButton, modifiersOf(e))
    }
    window.addEventListener('touchend', touchEnd)
    window.addEventListener('touchcancel', touchEnd)
}

function addRoot(item, element){
    scene.roots.push(item)
    listen(element)
}

// ---- Item API: coordinate mapping (QQuickItem::mapToItem, mapFromItem, ...) ----------------

function pointArgs(args){
    let a = args[0]
    if(a && typeof a === 'object'){
        return 'width' in a ? { x: a.x, y: a.y, width: a.width, height: a.height } : { x: a.x, y: a.y }
    }
    return args.length >= 4 ? { x: args[0], y: args[1], width: args[2], height: args[3] } : { x: args[0], y: args[1] }
}

function translated(p, dx, dy){
    let out = { x: p.x + dx, y: p.y + dy }
    if('width' in p){
        out.width = p.width
        out.height = p.height
    }
    return out
}

function windowOrigin(){
    if(!scene.element) return { x: 0, y: 0 }
    let rect = scene.element.getBoundingClientRect()
    return { x: rect.left + (window.screenX || 0), y: rect.top + (window.screenY || 0) }
}

Object.assign(Item.prototype, {
    mapToItem(item, ...args){
        let p = pointArgs(args)
        let from = scenePos(this)
        let to = item instanceof Item ? scenePos(item) : { x: 0, y: 0 }
        return translated(p, from.x - to.x, from.y - to.y)
    },
    mapFromItem(item, ...args){
        let p = pointArgs(args)
        let from = item instanceof Item ? scenePos(item) : { x: 0, y: 0 }
        let to = scenePos(this)
        return translated(p, from.x - to.x, from.y - to.y)
    },
    mapToGlobal(...args){
        let p = pointArgs(args)
        let from = scenePos(this), origin = windowOrigin()
        return translated(p, from.x + origin.x, from.y + origin.y)
    },
    mapFromGlobal(...args){
        let p = pointArgs(args)
        let to = scenePos(this), origin = windowOrigin()
        return translated(p, -to.x - origin.x, -to.y - origin.y)
    },
    contains(point){
        return !!point && contains(this, point.x, point.y)
    },
    // "the first visible child item found at point (x, y) within the item's coordinate system"
    childAt(x, y){
        let children = paintOrder(this)
        if(!children) return null
        for(let i = children.length - 1; i >= 0; i--){
            let child = children[i]
            if(!child.$v[Item.VISIBLE]) continue
            let lx = x - child.$v[Item.X], ly = y - child.$v[Item.Y]
            if(lx >= 0 && ly >= 0 && lx < child.$v[Item.WIDTH] && ly < child.$v[Item.HEIGHT]) return child
        }
        return null
    },
})

{
    let mount = Item.prototype.$mount
    Item.prototype.$mount = function(element){
        mount.call(this, element)
        addRoot(this, element)
    }
}

module.exports = { setContextMenuHandler, setOverrideCursor, modifiersOf, NoButton, LeftButton, scenePos, contains, scene, targetsAt, press, move, release, leaveWindow, wheel, ungrab, pointerOver, deliverHover, isHovered }
