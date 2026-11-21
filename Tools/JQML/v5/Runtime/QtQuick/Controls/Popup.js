// Popup and Overlay of QtQuick.Templates (qquickpopup.cpp, qquickoverlay.cpp, Qt 6.12). A Popup is no item: it is shown by
// its popup item (QQuickPopupItem, a Page) in the Overlay of the window, the topmost item of the window (z 1000001);
// its properties are those of the popup item. Checked against Qt 6.12 (tests/diff/cases/popup.qml):
//  - x and y are in its parent item; the popup item is at the parent's place in the overlay plus them; with margins
//    (>= 0) it is kept inside the overlay and x, y read where it is;
//  - open(): aboutToShow, visible, opened (after the enter transition); close(): aboutToHide, opened false, visible false
//    (after the exit transition), closed;
//  - a press outside closes it (CloseOnPressOutside); outside a modal one the press goes no further, outside another
//    one it goes on to the items below; Escape closes the popup that has the focus (CloseOnEscape);
//  - a modal popup dims the window (dim: the Overlay.modal / Overlay.modeless item of the style below the popup).
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { QtObject, GroupObject } = require('../../Core/QtObject')
const { Item } = require('../Item')
const { Control, withSetters } = require('./Control')
const { Page } = require('./Containers')
const QtEnums = require('../../Qt/enums')

const NoAutoClose = 0, CloseOnPressOutside = 1, CloseOnPressOutsideParent = 2, CloseOnReleaseOutside = 4,
    CloseOnReleaseOutsideParent = 8, CloseOnEscape = 16
const OVERLAY_Z = 1000001
const PopupFocusReason = 4

// ---- Overlay (QQuickOverlay) ----
class Overlay extends Item {
    constructor(parent = null){
        super(parent)
        this.$popups = []           // open (visible) popups, in stacking order
        ;(Overlay.$all || (Overlay.$all = [])).push(this)
    }
    // the overlay of the window an item is in: a child of its topmost item, made on the first use
    // (QQuickOverlay::overlay(window): only an item in a window has one; the content of a popup is in the window of the
    // popup's parent item)
    static of(item){
        let top = item
        for(let guard = 0; top && guard < 1000; guard++){
            if(top.$popup && top.$popup.$parentItem && top.$popup.$parentItem() && top.$popup.$parentItem() !== top){
                top = top.$popup.$parentItem()
                continue
            }
            if(top.$parent && top.$parent.$isItem){ top = top.$parent; continue }
            break
        }
        if(!top || !top.$isItem) return null
        if(!top.$overlay && !Overlay.$isRoot(top)) return null
        if(top.$overlay && !(top.$overlay.$f & engine.DESTROYED)) return top.$overlay
        let overlay = new Overlay(top)
        overlay.objectName = 'overlay'
        overlay.$write(Item.Z, OVERLAY_Z)
        overlay.$bind(Item.WIDTH, () => top.width, null, overlay)
        overlay.$bind(Item.HEIGHT, () => top.height, null, overlay)
        top.$overlay = overlay
        return overlay
    }
    // a root of the scene: the page's content item, a window, a root the pointer delivery shows (a test)
    static $isRoot(top){
        let { Window } = require('../Window/Window')
        if(top === Window.$pageContent || top instanceof Window) return true
        let scene = require('../PointerDelivery').scene
        if(scene.roots.includes(top)) return true
        if(top.$dom && top.$dom.isConnected) return true
        // a document of no page (a test of node, a bench): its root has no parent
        return !Window.$pageContent && !scene.roots.length && !(top.$delegateOf)
    }
    $stacked(){ return this.$popups.slice().sort((a, b) => a.$item.$v[Item.Z] - b.$item.$v[Item.Z]) }
    $add(popup){ if(!this.$popups.includes(popup)) this.$popups.push(popup) }
    $remove(popup){
        let i = this.$popups.indexOf(popup)
        if(i >= 0) this.$popups.splice(i, 1)
    }
    // QQuickOverlayPrivate::handlePress: the popups from the top try to close; a modal one takes the press
    $mousePress(x, y, button){
        if(!this.$popups.length) return false
        engine.emitArgs(this, Overlay.SIGNAL_pressed, [])
        for(let popup of this.$stacked().reverse()){
            if(popup.$pressedOutside(x, y)) popup.$tryClose(CloseOnPressOutside | CloseOnPressOutsideParent)
            if(popup.$v[Popup.MODAL]) return true
        }
        return false
    }
    $mouseRelease(x, y){
        engine.emitArgs(this, Overlay.SIGNAL_released, [])
        for(let popup of this.$stacked().reverse()){
            if(popup.$pressedOutside(x, y)) popup.$tryClose(CloseOnReleaseOutside | CloseOnReleaseOutsideParent)
        }
    }
    $mouseMove(){}
    // Escape: the topmost open popup that closes on it (QQuickPopupPrivate::handleEscape)
    $escape(){
        for(let popup of this.$stacked().reverse()){
            if(popup.$v[Popup.CLOSEPOLICY] & CloseOnEscape){
                if(popup.$closeByEscape) popup.$closeByEscape()
                else popup.close()
                return true
            }
            if(popup.$v[Popup.MODAL]) return false
        }
        return false
    }
    // a modal popup blocks the hover of the items below it
    $acceptsHover(){ return this.$popups.some(p => p.$v[Popup.MODAL]) }
    $hoverEnter(){}
    $hoverMove(){}
    $hoverLeave(){}
    $wheel(){ return this.$popups.some(p => p.$v[Popup.MODAL]) }
}
defineType(Overlay, { props: { modal: { kind: 'var', initial: null }, modeless: { kind: 'var', initial: null } }, signals: ['pressed', 'released'] })

// Overlay.overlay (of an item), Overlay.modal / Overlay.modeless (of a popup): the attached object
class OverlayAttached extends GroupObject {
    get overlay(){
        let owner = this.$owner
        let item = owner instanceof Popup ? owner.$parentItem() : owner
        return item ? Overlay.of(item) : null
    }
}
defineType(OverlayAttached, { props: { modal: { kind: 'var', initial: null }, modeless: { kind: 'var', initial: null } }, signals: ['pressed', 'released'] })

// ---- the popup item (QQuickPopupItem) ----
class PopupItem extends Page {
    constructor(popup){
        super(null)
        this.$popup = popup
        this.$isFocusScope = true
        this.$v[Item.VISIBLE] = false
        this.$ownVisible = false
    }
    // the font and the palette of the popup pass from the item it is in (QQuickPopupItemPrivate::resolveFont)
    $paletteParent(){ return this.$popup ? this.$popup.$parentItem() : null }
    $keyEvent(event, kind){
        let popup = this.$popup
        // (a popup that is closed or closing takes no key)
        if(!popup.$v[Popup.VISIBLE] || popup.$transitionTo === false) return 0
        if(event.key === QtEnums.Key_Escape && kind === 'pressed' && (popup.$v[Popup.CLOSEPOLICY] & CloseOnEscape)){
            if(popup.$closeByEscape) popup.$closeByEscape()
            else popup.close()
            return 1
        }
        return 0
    }
    $setDelegate(id, item, z){
        super.$setDelegate(id, item, z)
        if(this.$popup) this.$popup.$itemDelegateChanged(id)
    }
}
defineType(PopupItem, {})
PopupItem.prototype.$controlName = 'Popup'

// the properties a popup has as its item has them: written to the item, read back from it
const FORWARDED = ['z', 'width', 'height', 'implicitWidth', 'implicitHeight', 'contentWidth', 'contentHeight', 'availableWidth',
    'availableHeight', 'padding', 'topPadding', 'leftPadding', 'rightPadding', 'bottomPadding', 'horizontalPadding',
    'verticalPadding', 'spacing', 'background', 'contentItem', 'clip', 'opacity', 'scale', 'enabled', 'transformOrigin',
    'implicitContentWidth', 'implicitContentHeight', 'implicitBackgroundWidth', 'implicitBackgroundHeight', 'topInset',
    'leftInset', 'rightInset', 'bottomInset', 'locale', 'mirrored']

// ---- Popup (QQuickPopup) ----
class Popup extends QtObject {
    constructor(parent = null){
        super(null)
        let item = this.$item = new PopupItem(this)
        // QQuickPopupItem: its objectName is the QML name of the popup's type
        item.objectName = this.$popupName || 'Popup'
        this.$syncing = false
        this.$marginsSet = {}
        this.$requested = { x: 0, y: 0 }
        this.$dimSet = false
        this.$transition = null
        this.$focusBefore = null
        this.$dimmer = null
        // the item's values are the popup's
        for(let name of FORWARDED){
            let itemId = item.constructor.$type.propByName.get(name).id
            let id = Popup[name.toUpperCase()]
            this.$v[id] = item.$v[itemId]
            engine.connect(item, item.constructor.$type.props[itemId].signal, () => this.$pull(id, item.$v[itemId]), null, engine.PRIORITY_NORMAL, this)
        }
        this.$v[Popup.FONT] = item.font
        this.$v[Popup.PALETTE] = item.palette
        this.$v[Popup.CLOSEPOLICY] = CloseOnEscape | CloseOnPressOutside
        engine.connect(item, Item.SIGNAL_activeFocusChanged, () => this.$pull(Popup.ACTIVEFOCUS, item.$v[Item.ACTIVEFOCUS]), null, engine.PRIORITY_NORMAL, this)
        engine.connect(item, Item.SIGNAL_widthChanged, () => this.$reposition(), null, engine.PRIORITY_NORMAL, this)
        engine.connect(item, Item.SIGNAL_heightChanged, () => this.$reposition(), null, engine.PRIORITY_NORMAL, this)
        if(parent) this.$setParent(parent)
        engine.scheduleComponentComplete(this)
    }
    // the value of the item: kept, then its notification (bindings it runs write the popup as any write does)
    $pull(id, value){
        let old = this.$v[id]
        if(old === value || (old !== old && value !== value)) return
        this.$v[id] = value
        engine.emit(this, this.constructor.$type.props[id].signal, old, value)
    }
    // a write that keeps the value only (as a control's $plain)
    $plain(id, value){ this.$pull(id, value) }
    // a write to a forwarded property: the item's (its setter: padding, background...)
    $push(id, value){
        let name = this.constructor.$type.props[id].name
        if(name === 'width' || name === 'height'){
            this.$item.$explicit |= name === 'width' ? 1 : 2
        }
        this.$item[name] = value
    }
    $itemDelegateChanged(id){}

    // the item a popup is placed in: its parent when it is an item
    $parentItem(){ return this.$parent && this.$parent.$isItem ? this.$parent : null }
    $parentChanged(old, parent){
        super.$parentChanged(old, parent)
        if(this.$item){
            // the font and palette of the new parent
            require('./Control').resolveFont(this.$item)
            if(this.$v[Popup.VISIBLE]) this.$reposition()
        }
    }
    // an item declared in the popup: in its content item (contentData)
    $childAdded(child){
        if(child.$isItem && child !== this.$item){
            child.$setParent(this.$item)
            engine.emit(this, Popup.SIGNAL_contentChildrenChanged)
        }
    }
    get contentChildren(){ return this.$item.contentChildren }
    get contentData(){ return this.$item.contentChildren }

    $overlay(){
        let item = this.$parentItem()
        return item ? Overlay.of(item) : null
    }

    // ---- open / close ----
    open(){ engine.batch(() => this.$setVisible(true)) }
    close(){ engine.batch(() => this.$setVisible(false)) }
    forceActiveFocus(reason){ this.$item.forceActiveFocus(reason) }
    $setVisible(value){
        value = !!value
        if(this.$transitionTo === value) return
        if(!value && !this.$v[Popup.VISIBLE] && !this.$transition) return
        if(value && this.$v[Popup.VISIBLE] && !this.$transition) return
        if(!(this.$f & engine.COMPLETED)){
            // a popup declared visible opens when it is complete
            this.$openOnComplete = value
            return
        }
        if(value) this.$show()
        else this.$hide()
    }
    // QQuickPopupPrivate::prepareEnterTransition, transitionEnter, finalizeEnterTransition
    $show(){
        let overlay = this.$overlay()
        if(!overlay) return
        this.$transitionTo = true
        engine.emit(this, Popup.SIGNAL_aboutToShow)
        let item = this.$item
        if(item.$parent !== overlay) item.$setParent(overlay)
        overlay.$add(this)
        this.$updateDimmer(overlay)
        item.$ownVisible = true
        item.$write(Item.VISIBLE, true)
        this.$pull(Popup.VISIBLE, true)
        this.$reposition()
        if(this.$v[Popup.FOCUS]){
            let keys = require('../KeyDelivery')
            this.$focusBefore = keys.agent.activeFocusItem
            item.forceActiveFocus(PopupFocusReason)
        }
        this.$runTransition(this.$v[Popup.ENTER], () => {
            this.$transitionTo = undefined
            this.$pull(Popup.OPENED, true)
            engine.emit(this, Popup.SIGNAL_opened)
        })
    }
    // QQuickPopupPrivate::prepareExitTransition, transitionExit, finalizeExitTransition
    $hide(){
        this.$transitionTo = false
        engine.emit(this, Popup.SIGNAL_aboutToHide)
        this.$pull(Popup.OPENED, false)
        let item = this.$item
        // the active focus in the popup: on its item or on an item in it
        let keys = require('../KeyDelivery')
        let inside = (it) => { for(; it; it = it.$parent) if(it === item) return true; return false }
        let hadFocus = item.$v[Item.ACTIVEFOCUS] || inside(keys.agent.activeFocusItem)
        this.$runTransition(this.$v[Popup.EXIT], () => {
            this.$transitionTo = undefined
            item.$ownVisible = false
            item.$write(Item.VISIBLE, false)
            let overlay = item.$parent
            if(overlay && overlay.$remove) overlay.$remove(this)
            if(this.$dimmer) this.$dimmer.$write(Item.VISIBLE, false)
            this.$pull(Popup.VISIBLE, false)
            // the focus goes back to the item that had it before the popup
            if(hadFocus){
                let before = this.$focusBefore
                this.$focusBefore = null
                if(before && !(before.$f & engine.DESTROYED) && before.$v[Item.VISIBLE] && !inside(before)) before.forceActiveFocus()
                else {
                    // nothing had it: the window (QQuickWindow: its content item)
                    item.$setFocus(false)
                    let root = overlay
                    while(root && root.$parent instanceof Item) root = root.$parent
                    if(root && inside(keys.agent.activeFocusItem)) root.forceActiveFocus()
                }
            }
            engine.emit(this, Popup.SIGNAL_closed)
        })
    }
    $runTransition(transition, done){
        let { TransitionManager } = require('../TransitionManager')
        if(this.$transition){
            let running = this.$transition
            this.$transition = null
            running.cancel()
        }
        if(!transition){
            done()
            return
        }
        let manager = new TransitionManager(null)
        let finished = false
        manager.complete = function(){
            TransitionManager.prototype.complete.call(this)
            if(finished) return
            finished = true
            done()
        }
        this.$transition = manager
        manager.transition([], transition, this.$item)
    }
    // QQuickPopupPrivate::tryClose
    $tryClose(flags){
        let policy = this.$v[Popup.CLOSEPOLICY]
        if(!(policy & flags)) return false
        this.close()
        return true
    }
    // the point (in the overlay) outside the popup item (and, for the ...Parent policies, outside its parent)
    $pressedOutside(x, y){
        let item = this.$item
        return !(x >= item.$v[Item.X] && y >= item.$v[Item.Y] && x <= item.$v[Item.X] + item.$v[Item.WIDTH] && y <= item.$v[Item.Y] + item.$v[Item.HEIGHT])
    }

    // ---- dimming (QQuickOverlayPrivate::createOverlay) ----
    $dimmed(){ return this.$dimSet ? !!this.$v[Popup.DIM] : !!this.$v[Popup.MODAL] }
    $updateDimmer(overlay){
        let dim = this.$dimmed()
        if(!dim){
            if(this.$dimmer) this.$dimmer.$write(Item.VISIBLE, false)
            return
        }
        let modal = !!this.$v[Popup.MODAL]
        let component = (this.$overlayAttached && this.$overlayAttached.$v[modal ? OverlayAttached.MODAL : OverlayAttached.MODELESS])
            || overlay.$v[modal ? Overlay.MODAL : Overlay.MODELESS]
            || (this.$styleDimmer && this.$styleDimmer(modal))
        if(this.$dimmer && this.$dimmer.$dimModal !== modal){
            this.$dimmer.destroy()
            this.$dimmer = null
        }
        if(!this.$dimmer && component){
            let dimmer = component.createObject ? component.createObject(overlay) : component
            if(dimmer){
                dimmer.$dimModal = modal
                if(dimmer.$parent !== overlay) dimmer.$setParent(overlay)
                dimmer.$bind(Item.WIDTH, () => overlay.width, null, dimmer)
                dimmer.$bind(Item.HEIGHT, () => overlay.height, null, dimmer)
                this.$dimmer = dimmer
            }
        }
        if(this.$dimmer){
            this.$dimmer.$write(Item.Z, this.$item.$v[Item.Z])
            this.$dimmer.$write(Item.VISIBLE, true)
            // below the popup item among the overlay's children
            let children = overlay.$children
            let at = children.indexOf(this.$dimmer)
            if(at >= 0){
                children.splice(at, 1)
                children.splice(Math.max(0, children.indexOf(this.$item)), 0, this.$dimmer)
                overlay.$paintOrder = null
                if(this.$dimmer.$dom && this.$item.$dom && this.$item.$dom.parentNode === overlay.$dom) overlay.$dom.insertBefore(this.$dimmer.$dom, this.$item.$dom)
            }
        }
    }

    // ---- the place of the popup item (QQuickPopupPrivate::reposition) ----
    $reposition(){
        if(!this.$v[Popup.VISIBLE] && !this.$transitionTo) return
        let item = this.$item
        let overlay = item.$parent
        if(!overlay || !(overlay instanceof Overlay)) return
        let w = item.$v[Item.WIDTH], h = item.$v[Item.HEIGHT]
        let x, y
        let center = this.$anchors && this.$anchors.$v[PopupAnchors.CENTERIN]
        let parent = this.$parentItem()
        let origin = parent ? mapToOverlay(parent, overlay) : { x: 0, y: 0 }
        if(center){
            let c = mapToOverlay(center, overlay)
            x = c.x + (center.$v[Item.WIDTH] - w) / 2
            y = c.y + (center.$v[Item.HEIGHT] - h) / 2
        } else {
            x = origin.x + this.$requested.x
            y = origin.y + this.$requested.y
        }
        // margins >= 0: inside the overlay less them
        let m = this.$margins()
        let ow = overlay.$v[Item.WIDTH], oh = overlay.$v[Item.HEIGHT]
        if(m.right >= 0 && x + w > ow - m.right) x = ow - m.right - w
        if(m.left >= 0 && x < m.left) x = m.left
        if(m.bottom >= 0 && y + h > oh - m.bottom) y = oh - m.bottom - h
        if(m.top >= 0 && y < m.top) y = m.top
        item.$write(Item.X, x)
        item.$write(Item.Y, y)
        this.$setEffective(x - origin.x, y - origin.y)
    }
    // effectiveX / effectiveY: what x and y read, with their notifications
    $setEffective(x, y){
        let e = this.$effective || (this.$effective = { x: 0, y: 0 })
        if(e.x !== x){ e.x = x; engine.emit(this, Popup.SIGNAL_xChanged) }
        if(e.y !== y){ e.y = y; engine.emit(this, Popup.SIGNAL_yChanged) }
    }
    $margins(){
        let v = this.$v, set = this.$marginsSet
        let all = v[Popup.MARGINS]
        let side = (id, name) => (set[name] ? v[id] : all)
        return { top: side(Popup.TOPMARGIN, 'top'), left: side(Popup.LEFTMARGIN, 'left'), right: side(Popup.RIGHTMARGIN, 'right'), bottom: side(Popup.BOTTOMMARGIN, 'bottom') }
    }

    $componentComplete(){
        super.$componentComplete()
        if(this.$makeStyle) engine.batch(() => this.$makeStyle())
        if(this.$openOnComplete){
            this.$openOnComplete = undefined
            this.$setVisible(true)
        }
    }

    $write(id, value){
        if(this.$syncing) return QtObject.prototype.$write.call(this, id, value)
        let name = this.constructor.$type.props[id] && this.constructor.$type.props[id].name
        if(FORWARDED.includes(name)) return this.$push(id, value)
        if(id === Popup.VISIBLE) return this.$setVisible(value)
        if(id === Popup.X || id === Popup.Y){
            // QQuickPopup::setX: the position asked for; x reads where the popup is (effectiveX, 0 before it is shown)
            this.$requested[id === Popup.X ? 'x' : 'y'] = Number(value) || 0
            return this.$reposition()
        }
        if(id === Popup.FOCUS){
            QtObject.prototype.$write.call(this, id, !!value)
            if(value && this.$v[Popup.VISIBLE]) this.$item.forceActiveFocus(PopupFocusReason)
            return
        }
        if(id === Popup.DIM){
            this.$dimSet = value !== undefined
            QtObject.prototype.$write.call(this, id, value === undefined ? !!this.$v[Popup.MODAL] : !!value)
            return
        }
        if(id === Popup.MODAL){
            QtObject.prototype.$write.call(this, id, !!value)
            if(!this.$dimSet) QtObject.prototype.$write.call(this, Popup.DIM, !!value)
            return
        }
        if(id === Popup.MARGINS || id === Popup.TOPMARGIN || id === Popup.LEFTMARGIN || id === Popup.RIGHTMARGIN || id === Popup.BOTTOMMARGIN){
            if(id !== Popup.MARGINS){
                let side = name.replace('Margin', '')
                this.$marginsSet[side] = value !== undefined
                QtObject.prototype.$write.call(this, id, value === undefined ? this.$v[Popup.MARGINS] : Number(value))
            } else {
                QtObject.prototype.$write.call(this, id, value === undefined ? -1 : Number(value))
                for(let [side, sid] of [['top', Popup.TOPMARGIN], ['left', Popup.LEFTMARGIN], ['right', Popup.RIGHTMARGIN], ['bottom', Popup.BOTTOMMARGIN]]){
                    if(!this.$marginsSet[side]) QtObject.prototype.$write.call(this, sid, this.$v[Popup.MARGINS])
                }
            }
            return this.$reposition()
        }
        if(id === 1){
            // parent: the item it is placed in
            return this.$setParent(value === undefined ? null : value)
        }
        QtObject.prototype.$write.call(this, id, value)
    }
}

// a point of an item in the overlay
function mapToOverlay(item, overlay){
    let x = 0, y = 0
    for(let it = item; it && it.$isItem && it !== overlay.$parent; it = it.$parent){
        x += it.$v[Item.X]
        y += it.$v[Item.Y]
    }
    return { x, y }
}

// QQuickPopupAnchors: anchors.centerIn
class PopupAnchors extends GroupObject {
    $write(id, value){
        super.$write(id, value)
        let popup = this.$owner
        if(popup && popup.$reposition) popup.$reposition()
    }
}
defineType(PopupAnchors, { props: { centerIn: { kind: 'var', initial: null } } })

const real = (initial = 0) => ({ kind: 'real', initial })
defineType(Popup, {
    props: {
        x: 'real', y: 'real', z: 'real', width: 'real', height: 'real', implicitWidth: 'real', implicitHeight: 'real',
        contentWidth: 'real', contentHeight: 'real', availableWidth: { kind: 'real', readonly: true }, availableHeight: { kind: 'real', readonly: true },
        margins: real(-1), topMargin: real(-1), leftMargin: real(-1), rightMargin: real(-1), bottomMargin: real(-1),
        padding: 'real', topPadding: 'real', leftPadding: 'real', rightPadding: 'real', bottomPadding: 'real',
        locale: 'var', font: { group: require('./Control').ControlFont },
        background: { kind: 'var', initial: null }, contentItem: { kind: 'var', initial: null },
        clip: 'bool', focus: 'bool', activeFocus: { kind: 'bool', readonly: true }, modal: 'bool', dim: 'bool', visible: 'bool',
        opacity: real(1), scale: real(1), closePolicy: 'int', transformOrigin: { kind: 'int', initial: 4 },
        enter: { kind: 'var', initial: null }, exit: { kind: 'var', initial: null }, spacing: 'real',
        opened: { kind: 'bool', readonly: true }, mirrored: { kind: 'bool', readonly: true }, enabled: { kind: 'bool', initial: true },
        palette: { group: require('../Palette').Palette }, horizontalPadding: 'real', verticalPadding: 'real',
        implicitContentWidth: { kind: 'real', readonly: true }, implicitContentHeight: { kind: 'real', readonly: true },
        implicitBackgroundWidth: { kind: 'real', readonly: true }, implicitBackgroundHeight: { kind: 'real', readonly: true },
        topInset: 'real', leftInset: 'real', rightInset: 'real', bottomInset: 'real', popupType: 'int',
        Overlay: { group: OverlayAttached }, anchors: { group: PopupAnchors },
    },
    signals: ['opened', 'closed', 'aboutToShow', 'aboutToHide', 'paletteCreated', 'contentChildrenChanged', 'windowChanged(window)'],
})
// Overlay and anchors: made on the first use (the getters above)
for(let name of ['Overlay', 'anchors']){
    let Group = name === 'Overlay' ? OverlayAttached : PopupAnchors
    let id = Popup[name.toUpperCase()]
    Object.defineProperty(Popup.prototype, name, {
        get(){
            if(!this.$v[id]){
                this.$v[id] = new Group(this)
                if(name === 'Overlay') this.$overlayAttached = this.$v[id]
                else this.$anchors = this.$v[id]
            }
            return this.$v[id]
        },
        configurable: true,
    })
}
// Popup.transformOrigin (Item.TransformOrigin)
Object.assign(Popup, { TopLeft: 0, Top: 1, TopRight: 2, Left: 3, Center: 4, Right: 5, BottomLeft: 6, Bottom: 7, BottomRight: 8 })
Object.assign(Popup, { NoAutoClose, CloseOnPressOutside, CloseOnPressOutsideParent, CloseOnReleaseOutside, CloseOnReleaseOutsideParent,
    CloseOnEscape, Item: 0, Window: 1, Native: 2 })
// x and y read where the popup is (QQuickPopup::x: effectiveX)
for(let name of ['x', 'y']){
    let signal = Popup['SIGNAL_' + name + 'Changed']
    let set = Object.getOwnPropertyDescriptor(Popup.prototype, name).set
    Object.defineProperty(Popup.prototype, name, {
        get(){
            engine.track(this, signal)
            return this.$effective ? this.$effective[name] : 0
        },
        set,
        configurable: true,
    })
}
// read-only for QML, written by the popup (the item's values)
for(let name of ['availableWidth', 'availableHeight', 'implicitContentWidth', 'implicitContentHeight', 'implicitBackgroundWidth', 'implicitBackgroundHeight', 'mirrored']){
    let id = Popup[name.toUpperCase()]
    let get = Object.getOwnPropertyDescriptor(Popup.prototype, name).get
    Object.defineProperty(Popup.prototype, name, { get, set(){ throw new TypeError('Cannot assign to read-only property "' + name + '"') }, configurable: true })
}

// Overlay.overlay on any item
{
    let prop = Item.$type.propByName.get('Overlay')
    if(prop){
        prop.group = OverlayAttached
        Object.defineProperty(Item.prototype, 'Overlay', {
            get(){ return this.$v[prop.id] || (this.$v[prop.id] = new OverlayAttached(this)) },
            configurable: true,
        })
    }
}

// the shortcuts and the open popups (QQuickShortcutContext, QQuickPopupItem::event ShortcutOverride)
{
    const { Shortcut } = require('../Shortcut')
    const keys = () => require('../KeyDelivery')
    const openPopups = () => (Overlay.$all || []).filter(o => !(o.$f & engine.DESTROYED)).flatMap(o => o.$popups || []).filter(p => p.$v[Popup.VISIBLE])
    const inItem = (it, item) => { for(; it; it = it.$parent) if(it === item) return true; return false }
    Shortcut.$override = (key, mods) => {
        if(key !== 'escape') return false
        let focus = keys().agent.activeFocusItem
        return openPopups().some(p => (p.$v[Popup.CLOSEPOLICY] & CloseOnEscape) && inItem(focus, p.$item))
    }
    Shortcut.$blocked = (shortcut) => {
        let modal = openPopups().filter(p => p.$v[Popup.MODAL])
        if(!modal.length) return false
        let owner = shortcut.$parent
        return !modal.some(p => inItem(owner, p.$item) || owner === p)
    }
}

module.exports = { Popup, PopupItem, Overlay, OverlayAttached, PopupAnchors, NoAutoClose, CloseOnPressOutside, CloseOnEscape }
