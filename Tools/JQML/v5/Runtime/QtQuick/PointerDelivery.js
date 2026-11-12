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

// an item that takes mouse events: presses, hover or the wheel
const handlesMouse = (item) => item.$mousePress !== undefined || item.$acceptsHover !== undefined || item.$wheel !== undefined

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

const scene = {
    roots: [],
    element: null,
    grabber: null,
    pressedButtons: NoButton,
    hovered: [],
    x: -1,
    y: -1,
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

function press(x, y, button, buttons, modifiers, doubleClick){
    engine.batch(() => {
        scene.x = x
        scene.y = y
        scene.pressedButtons = buttons
        let grabber = scene.grabber
        if(grabber && !(grabber.$f & engine.DESTROYED)){
            // another button while grabbed goes to the grabber
            let p = local(grabber, x, y)
            if(grabber.$mousePress) grabber.$mousePress(p.x, p.y, button, buttons, modifiers)
            return
        }
        scene.grabber = null
        for(let target of targetsAt(x, y)){
            let item = target.item
            if(item.$mousePress){
                if(item.$mousePress(target.x, target.y, button, buttons, modifiers)){
                    scene.grabber = item
                    break
                }
            } else if(item.$acceptsMouse){
                scene.grabber = item
                break
            }
        }
        if(doubleClick && scene.grabber && scene.grabber.$mouseDoubleClick){
            let p = local(scene.grabber, x, y)
            scene.grabber.$mouseDoubleClick(p.x, p.y, button, buttons, modifiers)
        }
        scene.contextMenuGrabbed = !!(scene.grabber && button === RightButton && scene.grabber.$mousePress)
    })
}

function move(x, y, buttons, modifiers){
    engine.batch(() => {
        scene.x = x
        scene.y = y
        let grabber = scene.grabber
        if(grabber && !(grabber.$f & engine.DESTROYED)){
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
        let grabber = scene.grabber
        if(grabber && !(grabber.$f & engine.DESTROYED)){
            let p = local(grabber, x, y)
            if(grabber.$mouseRelease) grabber.$mouseRelease(p.x, p.y, button, buttons, modifiers)
        }
        if(!buttons){
            scene.grabber = null
            hover(x, y, modifiers)
        }
    })
}

// QQuickDeliveryAgent: the topmost hover-accepting item under the cursor and those of its ancestors; what lies below it is not hovered
function hover(x, y, modifiers){
    let now = []
    let cursor = null
    let leaf = null
    for(let target of targetsAt(x, y)){
        let item = target.item
        if(cursor === null && item.$cursor) cursor = item.$cursor()
        if(!item.$acceptsHover || !item.$acceptsHover()) continue
        if(!leaf) leaf = item
        else {
            let ancestor = leaf.$parent
            while(ancestor && ancestor !== item) ancestor = ancestor.$parent
            if(!ancestor) continue
        }
        now.push(target)
    }
    for(let item of scene.hovered){
        if(!now.some(target => target.item === item) && !(item.$f & engine.DESTROYED)) item.$hoverLeave()
    }
    let previous = scene.hovered
    scene.hovered = now.map(target => target.item)
    for(let target of now){
        if(previous.indexOf(target.item) < 0) target.item.$hoverEnter(target.x, target.y, modifiers)
        else target.item.$hoverMove(target.x, target.y, modifiers)
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

// the grab ends, like QQuickWindow::ungrabMouse when the grabber hides or gets disabled
function ungrab(item){
    if(scene.grabber === item) scene.grabber = null
    let index = scene.hovered.indexOf(item)
    if(index >= 0) scene.hovered.splice(index, 1)
}

function setCursor(cursor){
    if(scene.element) scene.element.style.cursor = cursor || ''
}

// ---- DOM ------------------------------------------------------------------------------------

const DOM_BUTTON = [LeftButton, MiddleButton, RightButton, 8, 16]

function modifiersOf(e){
    return (e.shiftKey ? 0x02000000 : 0) | (e.ctrlKey ? 0x04000000 : 0) | (e.altKey ? 0x08000000 : 0) | (e.metaKey ? 0x10000000 : 0)
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
        if(isNative(e)) return
        // the browser would blur the focused input and start a text selection; in Qt a press moves focus only
        // when QML asks for it (forceActiveFocus, activeFocusOnPress)
        e.preventDefault()
        let p = pointOf(e)
        // the second press of a double click also delivers MouseButtonDblClick to the grabber
        press(p.x, p.y, DOM_BUTTON[e.button] || LeftButton, e.buttons, modifiersOf(e), e.detail === 2)
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

module.exports = { NoButton, LeftButton, scenePos, contains, scene, targetsAt, press, move, release, leaveWindow, wheel, ungrab }
