// ApplicationWindow, MenuBar and MenuBarItem of QtQuick.Templates (Qt 6.12). Checked against Qt 6.12
// (tests/diff/cases/applicationwindow.qml):
//  - ApplicationWindow: a Window (the page) with a menu bar, a header and a footer, and its items in a content item
//    between them (QQuickApplicationWindowPrivate::relayout); its font and palette pass to the controls in it; the
//    attached ApplicationWindow.window / header / footer / menuBar / contentItem / activeFocusControl;
//  - MenuBar: a MenuBarItem (its delegate) for each menu; a click on an item opens its menu below it, highlighted.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { GroupObject } = require('../../Core/QtObject')
const { Item } = require('../Item')
const { Window } = require('../Window/Window')
const { Control, withSetters, installFont, resolveFont } = require('./Control')
const { Container, ContentItem } = require('./Containers')
const { AbstractButton } = require('./AbstractButton')

// ---- ApplicationWindow (QQuickApplicationWindow) ----
class ApplicationWindow extends Window {
    constructor(parent = null){
        super(parent)
        installFont(this, ApplicationWindow.FONT)
        let content = new ContentItem(null)
        content.objectName = 'ApplicationWindow'
        content.$delegateOf = this
        content.$setParent(this)
        this.$plain(ApplicationWindow.CONTENTITEM, content)
        this.$relayout()
    }
    $plain(id, value){ Item.prototype.$write.call(this, id, value) }
    $parentChanged(old, parent){
        super.$parentChanged(old, parent)
        if(this.$controlFont) resolveFont(this)
    }
    // the items declared in the window: in its content item
    $childAdded(child){
        if(child.$delegateOf === this) return
        let content = this.$v[ApplicationWindow.CONTENTITEM]
        if(content && child !== content) child.$setParent(content)
    }
    $setPart(id, item){
        let old = this.$v[id]
        if(old === item) return
        if(old && old.$parent === this && !(old.$f & engine.DESTROYED)) old.$setParent(null)
        this.$plain(id, item || null)
        if(item){
            item.$delegateOf = this
            if(item.$parent !== this) item.$setParent(this)
            // a ToolBar, TabBar or DialogButtonBox in the header or footer takes that position
            if(id !== ApplicationWindow.BACKGROUND && id !== ApplicationWindow.MENUBAR && item.constructor.$type.propByName.has('position') && item.constructor.Header !== undefined)
                item.position = id === ApplicationWindow.HEADER ? 0 : 1
            if(id === ApplicationWindow.BACKGROUND && item.$v[Item.Z] === 0) item.$write(Item.Z, -1)
            let relayout = () => this.$relayout()
            engine.connect(item, Item.SIGNAL_heightChanged, relayout, null, engine.PRIORITY_NORMAL, this)
            engine.connect(item, Item.SIGNAL_implicitHeightChanged, relayout, null, engine.PRIORITY_NORMAL, this)
            engine.connect(item, Item.SIGNAL_visibleChanged, relayout, null, engine.PRIORITY_NORMAL, this)
        }
        this.$relayout()
    }
    // QQuickApplicationWindowPrivate::relayout
    $relayout(){
        let v = this.$v
        let w = v[Item.WIDTH], h = v[Item.HEIGHT]
        let menuBar = v[ApplicationWindow.MENUBAR], header = v[ApplicationWindow.HEADER], footer = v[ApplicationWindow.FOOTER]
        let content = v[ApplicationWindow.CONTENTITEM], background = v[ApplicationWindow.BACKGROUND]
        let size = (item, implicit) => (item.$explicit & 2 ? item.$v[Item.HEIGHT] : item.$v[Item.IMPLICITHEIGHT])
        let mbh = menuBar && menuBar.$v[Item.VISIBLE] ? size(menuBar) : 0
        let hh = header && header.$v[Item.VISIBLE] ? size(header) : 0
        let fh = footer && footer.$v[Item.VISIBLE] ? size(footer) : 0
        let place = (item, y, height) => {
            item.$write(Item.X, 0)
            item.$write(Item.Y, y)
            item.$explicit |= 1
            item.$write(Item.WIDTH, w)
            if(height !== undefined) item.$write(Item.HEIGHT, height)
        }
        if(menuBar) place(menuBar, 0, mbh)
        if(header) place(header, mbh, hh)
        if(footer) place(footer, h - fh, fh)
        if(content){
            content.$explicit |= 3
            place(content, mbh + hh, Math.max(0, h - mbh - hh - fh))
        }
        if(background){
            background.$explicit |= 3
            place(background, 0, h)
        }
    }
    $write(id, value){
        let parts = [ApplicationWindow.HEADER, ApplicationWindow.FOOTER, ApplicationWindow.MENUBAR, ApplicationWindow.BACKGROUND]
        if(parts.includes(id)) return this.$setPart(id, value)
        super.$write(id, value)
        if(id === Item.WIDTH || id === Item.HEIGHT) this.$relayout()
    }
    get contentData(){ let c = this.$v[ApplicationWindow.CONTENTITEM]; return c && c.$children ? c.$children.slice() : [] }
    // the control that has the active focus (QQuickApplicationWindow::activeFocusControl)
    get activeFocusControl(){
        let keys = require('../KeyDelivery')
        for(let it = keys.agent.activeFocusItem; it; it = it.$parent){
            if(it instanceof Control || (it.$controlFont && it !== this)) return it
        }
        return null
    }
}
defineType(ApplicationWindow, {
    props: {
        background: { kind: 'var', initial: null }, contentItem: { kind: 'var', readonly: true, initial: null },
        font: { group: require('./Control').ControlFont }, header: { kind: 'var', initial: null }, footer: { kind: 'var', initial: null },
        menuBar: { kind: 'var', initial: null }, locale: { kind: 'var', effect: (owner) => require('./Control').propagateLocale(owner) },
        // the safe area margins of the window (none on a page)
        topPadding: 'real', leftPadding: 'real', rightPadding: 'real', bottomPadding: 'real',
    },
    signals: ['activeFocusControlChanged'],
})
ApplicationWindow.prototype.$theme = 'basic'
Object.defineProperty(ApplicationWindow.prototype, 'contentItem', {
    get(){ return this.$v[ApplicationWindow.CONTENTITEM] },
    configurable: true,
})

// ApplicationWindow.window / header / footer / menuBar / contentItem / activeFocusControl on an item
class ApplicationWindowAttached extends GroupObject {
    $window(){
        for(let it = this.$owner; it; it = it.$parent) if(it instanceof ApplicationWindow) return it
        return null
    }
    get window(){ return this.$window() }
    get header(){ let w = this.$window(); return w ? w.header : null }
    get footer(){ let w = this.$window(); return w ? w.footer : null }
    get menuBar(){ let w = this.$window(); return w ? w.menuBar : null }
    get contentItem(){ let w = this.$window(); return w ? w.contentItem : null }
    get activeFocusControl(){ let w = this.$window(); return w ? w.activeFocusControl : null }
}
defineType(ApplicationWindowAttached, {})

// ---- MenuBarItem (QQuickMenuBarItem) ----
class MenuBarItem extends AbstractButton {
    constructor(parent = null){
        super(parent)
        this.$write(Control.FOCUSPOLICY, 0)
    }
    $trigger(){
        super.$trigger()
        engine.emit(this, MenuBarItem.SIGNAL_triggered)
    }
    $hoverEnter(){
        super.$hoverEnter()
        let bar = this.$v[MenuBarItem.MENUBAR]
        if(bar && bar.$itemHovered) bar.$itemHovered(this)
    }
}
defineType(MenuBarItem, {
    props: { menuBar: { kind: 'var', readonly: true, initial: null }, menu: { kind: 'var', initial: null }, highlighted: 'bool' },
    signals: ['triggered'],
})

// ---- MenuBar (QQuickMenuBar) ----
class MenuBar extends Container {
    constructor(parent = null){
        super(parent)
        this.$menus = []
        this.$write(Control.FOCUSPOLICY, 2)     // Qt.ClickFocus
    }
    // the menus declared in it: an item of the delegate for each (QQuickMenuBarPrivate::contentData_append)
    $childAdded(child){
        if(child.$delegateOf === this || child === this.$v[Control.CONTENTITEM] || child === this.$v[Control.BACKGROUND]) return
        if(child.$isItem) return super.$childAdded(child)
    }
    // the declared menus get their items once the style has given the delegate
    $componentComplete(){
        super.$componentComplete()
        let { Menu } = require('./Popups')
        for(let child of (this.$children || []).slice()) if(child instanceof Menu && !this.$menus.includes(child)) this.addMenu(child)
    }
    get menus(){ engine.track(this, MenuBar.SIGNAL_menusChanged); return this.$menus.slice() }
    menuAt(index){ return this.$menus[index] || null }
    addMenu(menu){ this.insertMenu(this.$menus.length, menu) }
    insertMenu(index, menu){
        if(!menu || this.$menus.includes(menu)) return
        engine.batch(() => {
            let delegate = this.$v[MenuBar.DELEGATE]
            let item = delegate && delegate.createObject ? delegate.createObject(null) : new MenuBarItem(null)
            if(!item) return
            item.$plain(MenuBarItem.MENUBAR, this)
            item.menu = menu
            if(!item.text) item.$bind(AbstractButton.TEXT, () => menu.title, null, item)
            this.$menus.splice(index, 0, menu)
            this.insertItem(index, item)
            engine.connect(item, AbstractButton.SIGNAL_clicked, () => this.$toggleMenu(item), null, engine.PRIORITY_NORMAL, this)
            engine.connect(menu, menu.constructor.SIGNAL_closed, () => {
                if(this.$openItem === item){
                    this.$openItem = null
                    item.highlighted = false
                }
            }, null, engine.PRIORITY_NORMAL, this)
            engine.emit(this, MenuBar.SIGNAL_menusChanged)
        })
    }
    removeMenu(menu){
        let i = this.$menus.indexOf(menu)
        if(i < 0) return
        this.$menus.splice(i, 1)
        this.removeItem(i)
        engine.emit(this, MenuBar.SIGNAL_menusChanged)
    }
    takeMenu(index){
        let menu = this.$menus[index]
        if(menu) this.removeMenu(menu)
        return menu || null
    }
    // QQuickMenuBarPrivate::toggleCurrentMenu: the menu of an item opens below it
    $toggleMenu(item){
        let menu = item.menu
        if(!menu) return
        if(menu.visible){
            menu.close()
            return
        }
        if(this.$openItem && this.$openItem !== item && this.$openItem.menu) this.$openItem.menu.close()
        this.$openItem = item
        item.highlighted = true
        this.setCurrentIndex(this.$v[Container.CONTENTMODEL].$items.indexOf(item))
        menu.$setParent(item)
        menu.x = 0
        menu.y = item.height
        menu.open()
    }
    // a hovered item opens its menu while another one is open
    $itemHovered(item){
        if(this.$openItem && this.$openItem !== item) this.$toggleMenu(item)
    }
}
defineType(MenuBar, { props: { delegate: { kind: 'component', initial: null } }, signals: ['menusChanged'] })

for(let [name, Group] of [['ApplicationWindow', ApplicationWindowAttached]]){
    let prop = Item.$type.propByName.get(name)
    if(!prop) continue
    prop.group = Group
    Object.defineProperty(Item.prototype, name, {
        get(){ return this.$v[prop.id] || (this.$v[prop.id] = new Group(this)) },
        configurable: true,
    })
}

module.exports = { ApplicationWindow, ApplicationWindowAttached, MenuBar, MenuBarItem }
