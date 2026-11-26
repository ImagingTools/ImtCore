// The popups of QtQuick.Templates over Popup (Qt 6.12): ToolTip (and the ToolTip attached to items), Dialog,
// DialogButtonBox (a Container of buttons), Menu and MenuItem, Drawer. Checked against Qt 6.12
// (tests/diff/cases/popups.qml):
//  - ToolTip: delay, timeout (closes after it), show(text, timeout) / hide(); the attached one is one tool tip shared by
//    the items of a window;
//  - DialogButtonBox: the standard buttons with their roles, laid out as QPlatformDialogHelper::WinLayout (by role: Reset,
//    Yes, Accept, Destructive, No, Reject, Apply, Help; in a role by the value of the button); a click: clicked(button),
//    then accepted / rejected / applied / reset / discarded / helpRequested by the role;
//  - Dialog: its footer box accepts and rejects it (result Accepted 1 / Rejected 0, accepted / rejected, closed), Escape
//    rejects it; its title is the text of its header;
//  - Menu: popup(x, y) opens it there; Up / Down move currentIndex (the item highlighted), Return triggers; a click on an
//    item triggers it and closes the menu;
//  - Drawer: open() / close() move position from 0 to 1 and back (its enter / exit transitions), it is placed at its edge.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { GroupObject } = require('../../Core/QtObject')
const { Item } = require('../Item')
const { Control, withSetters } = require('./Control')
const { Popup, Overlay } = require('./Popup')
const { Container } = require('./Containers')
const { AbstractButton } = require('./AbstractButton')
const QtEnums = require('../../Qt/enums')

// ---- ToolTip (QQuickToolTip) ----
class ToolTip extends Popup {
    constructor(parent = null){
        super(parent)
        this.$plain(ToolTip.TIMEOUT, -1)
        this.$delayTimer = null
        this.$timeoutTimer = null
    }
    $setVisible(value){
        // QQuickToolTip::setVisible: shown after delay, hidden after timeout
        if(value && this.$v[ToolTip.DELAY] > 0 && !this.$v[Popup.VISIBLE] && !this.$delaying){
            this.$stopTimers()
            this.$delaying = true
            this.$delayTimer = setTimeout(() => {
                this.$delaying = false
                if(!(this.$f & engine.DESTROYED)) engine.batch(() => super.$setVisible(true))
            }, this.$v[ToolTip.DELAY])
            return
        }
        if(!value) this.$stopTimers()
        super.$setVisible(value)
        if(value) this.$startTimeout()
    }
    $startTimeout(){
        if(this.$timeoutTimer) clearTimeout(this.$timeoutTimer)
        this.$timeoutTimer = null
        let timeout = this.$v[ToolTip.TIMEOUT]
        if(timeout > 0) this.$timeoutTimer = setTimeout(() => { if(!(this.$f & engine.DESTROYED)) this.close() }, timeout)
    }
    $stopTimers(){
        this.$delaying = false
        if(this.$delayTimer) clearTimeout(this.$delayTimer)
        if(this.$timeoutTimer) clearTimeout(this.$timeoutTimer)
        this.$delayTimer = this.$timeoutTimer = null
    }
    show(text, ms = -1){
        engine.batch(() => {
            if(text !== undefined) this.text = text
            if(ms !== undefined) this.$plain(ToolTip.TIMEOUT, ms)
            this.$setVisible(true)
        })
    }
    hide(){ this.close() }
}
defineType(ToolTip, { props: { delay: 'int', timeout: 'int', text: 'string' } })
ToolTip.prototype.$popupName = 'ToolTip'

// ToolTip.text / .visible / .delay / .timeout on an item: one tool tip of the window shows the text of the item
class ToolTipAttached extends GroupObject {
    // the shared tool tip (QQuickToolTipAttachedPrivate::instance), made by the style
    get toolTip(){
        let overlay = Overlay.of(this.$owner)
        if(!overlay) return null
        if(!overlay.$toolTip || overlay.$toolTip.$f & engine.DESTROYED){
            let make = ToolTipAttached.$make || ((parent) => new ToolTip(parent))
            overlay.$toolTip = make(this.$owner)
        }
        return overlay.$toolTip
    }
    $write(id, value){
        super.$write(id, id === ToolTipAttached.VISIBLE ? !!value : value)
        if(id === ToolTipAttached.VISIBLE) this.$setShown(!!value)
        else if(id === ToolTipAttached.TEXT && this.$shown()) this.toolTip.text = value
    }
    $shown(){
        let tip = this.$v[ToolTipAttached.VISIBLE] ? this.toolTip : null
        return tip && tip.$attachedBy === this
    }
    $setShown(value){
        let tip = this.toolTip
        if(!tip) return
        if(value){
            engine.batch(() => {
                tip.$attachedBy = this
                if(tip.$parent !== this.$owner) tip.$setParent(this.$owner)
                tip.text = this.$v[ToolTipAttached.TEXT]
                tip.delay = this.$v[ToolTipAttached.DELAY]
                tip.$plain(ToolTip.TIMEOUT, this.$v[ToolTipAttached.TIMEOUT])
                tip.$setVisible(true)
            })
        } else if(tip.$attachedBy === this){
            tip.close()
        }
    }
    show(text, ms = -1){
        engine.batch(() => {
            super.$write(ToolTipAttached.TEXT, text)
            if(ms !== undefined) super.$write(ToolTipAttached.TIMEOUT, ms)
            super.$write(ToolTipAttached.VISIBLE, true)
            this.$setShown(true)
        })
    }
    hide(){ engine.batch(() => { super.$write(ToolTipAttached.VISIBLE, false); this.$setShown(false) }) }
}
defineType(ToolTipAttached, { props: { text: 'string', delay: { kind: 'int', initial: 700 }, timeout: { kind: 'int', initial: 10000 }, visible: 'bool' } })

// ---- DialogButtonBox (QQuickDialogButtonBox) ----
const AcceptRole = 0, RejectRole = 1, DestructiveRole = 2, ActionRole = 3, HelpRole = 4, YesRole = 5, NoRole = 6, ResetRole = 7, ApplyRole = 8, InvalidRole = -1
const STANDARD = [
    // [button, text, role]
    [0x400, 'OK', AcceptRole], [0x800, 'Save', AcceptRole], [0x1000, 'Save All', AcceptRole], [0x2000, 'Open', AcceptRole],
    [0x4000, 'Yes', YesRole], [0x8000, 'Yes to All', YesRole], [0x10000, 'No', NoRole], [0x20000, 'No to All', NoRole],
    [0x40000, 'Abort', RejectRole], [0x80000, 'Retry', AcceptRole], [0x100000, 'Ignore', AcceptRole], [0x200000, 'Close', RejectRole],
    [0x400000, 'Cancel', RejectRole], [0x800000, 'Discard', DestructiveRole], [0x1000000, 'Help', HelpRole], [0x2000000, 'Apply', ApplyRole],
    [0x4000000, 'Reset', ResetRole], [0x8000000, 'Restore Defaults', ResetRole],
]
// QPlatformDialogHelper::WinLayout: the order of the roles
const WIN_ORDER = [ResetRole, YesRole, AcceptRole, ActionRole, DestructiveRole, NoRole, RejectRole, ApplyRole, HelpRole]

class DialogButtonBoxAttached extends GroupObject {}
defineType(DialogButtonBoxAttached, { props: { buttonBox: { kind: 'var', readonly: true, initial: null }, buttonRole: { kind: 'int', initial: InvalidRole } } })

class DialogButtonBox extends Container {
    constructor(parent = null){
        super(parent)
        this.$standard = new Map()      // standard button -> button
    }
    // QQuickDialogButtonBoxPrivate::updateLayout: the buttons without a width share the available width; the content
    // height is the tallest button (getContentHeight)
    $updateLayout(){
        let items = this.$v[Container.CONTENTMODEL].$items
        let spacing = this.$v[Control.SPACING]
        let maxHeight = 0
        for(let item of items) maxHeight = Math.max(maxHeight, item.$v[Item.IMPLICITHEIGHT])
        this.$plain(Control.IMPLICITCONTENTHEIGHT, maxHeight)
        if(this.$v[DialogButtonBox.ALIGNMENT]) return
        let reserved = 0, resizable = 0
        for(let item of items){
            if(item.$explicit & 1 && !item.$boxWidth) reserved += item.$v[Item.WIDTH]
            else resizable++
        }
        let width = (this.$v[Control.AVAILABLEWIDTH] - reserved - Math.max(0, items.length - 1) * spacing) / Math.max(1, resizable)
        for(let item of items){
            if(item.$explicit & 1 && !item.$boxWidth) continue
            item.$boxWidth = true
            item.$explicit |= 1
            item.$write(Item.WIDTH, width)
            if(!(item.$explicit & 2)) item.$write(Item.HEIGHT, maxHeight)
        }
    }
    $write(id, value){
        super.$write(id, value)
        if(id === Item.WIDTH || id === Control.SPACING || id === DialogButtonBox.ALIGNMENT || id === Control.LEFTPADDING || id === Control.RIGHTPADDING) this.$updateLayout()
    }
    // the implicit content width is the content item's (its buttons in a row), the height the tallest button's
    $updateImplicitContent(){
        let content = this.$v[Control.CONTENTITEM]
        this.$plain(Control.IMPLICITCONTENTWIDTH, content ? content.$v[Item.IMPLICITWIDTH] : 0)
        if(this.$v[Container.CONTENTMODEL]) this.$updateLayout()
    }
    $itemAdded(item){
        engine.connect(item, Item.SIGNAL_implicitHeightChanged, () => this.$updateLayout(), null, engine.PRIORITY_NORMAL, this)
        this.$updateLayout()
        let attached = item.DialogButtonBox
        attached.$write(DialogButtonBoxAttached.BUTTONBOX, this)
        let clicked = item.constructor.SIGNAL_clicked
        if(clicked !== undefined) engine.connect(item, clicked, () => this.$buttonClicked(item), null, engine.PRIORITY_NORMAL, this)
        if(!this.$sorting) this.$sort()
    }
    // QQuickDialogButtonBoxPrivate::updateLayout: the buttons by role (WinLayout), then by standard button
    $sort(){
        let model = this.$v[Container.CONTENTMODEL]
        let items = model.$items.slice()
        this.$updateLayout()
        let key = (item) => {
            let role = item.DialogButtonBox.$v[DialogButtonBoxAttached.BUTTONROLE]
            let at = WIN_ORDER.indexOf(role)
            return [at < 0 ? WIN_ORDER.length : at, item.$standardButton || 0x10000000]
        }
        let sorted = items.slice().sort((a, b) => {
            let ka = key(a), kb = key(b)
            return ka[0] - kb[0] || ka[1] - kb[1] || items.indexOf(a) - items.indexOf(b)
        })
        this.$sorting = true
        try {
            sorted.forEach((item, to) => {
                let from = model.$items.indexOf(item)
                if(from !== to) model.move(from, to, 1)
            })
        } finally {
            this.$sorting = false
        }
    }
    $setStandardButtons(value){
        value = Number(value) || 0
        this.$plain(DialogButtonBox.STANDARDBUTTONS, value)
        if(!(this.$f & engine.COMPLETED) && !this.$madeOnce){
            this.$pendingButtons = true
            return
        }
        this.$makeStandardButtons()
    }
    $makeStandardButtons(){
        let value = this.$v[DialogButtonBox.STANDARDBUTTONS]
        engine.batch(() => {
            for(let [button, item] of Array.from(this.$standard)){
                if(!(value & button)){
                    this.$standard.delete(button)
                    this.removeItem(item)
                }
            }
            this.$sorting = true
            try {
                for(let [button, text, role] of STANDARD){
                    if(!(value & button) || this.$standard.has(button)) continue
                    let item = this.$makeButton()
                    if(!item) continue
                    item.$standardButton = button
                    item.text = text
                    item.DialogButtonBox.buttonRole = role
                    this.$standard.set(button, item)
                    this.addItem(item)
                }
            } finally {
                this.$sorting = false
            }
            this.$sort()
        })
    }
    // a standard button: of the delegate, else a Button
    $makeButton(){
        let delegate = this.$v[DialogButtonBox.DELEGATE]
        if(delegate && delegate.createObject) return delegate.createObject(this)
        return new (require('./AbstractButton').Button)(this)
    }
    standardButton(button){ return this.$standard.get(button) || null }
    $buttonClicked(item){
        engine.emitArgs(this, DialogButtonBox.SIGNAL_clicked, [item])
        let role = item.DialogButtonBox.$v[DialogButtonBoxAttached.BUTTONROLE]
        let signal = role === AcceptRole || role === YesRole ? 'accepted' : role === RejectRole || role === NoRole ? 'rejected'
            : role === ApplyRole ? 'applied' : role === ResetRole ? 'reset' : role === DestructiveRole ? 'discarded' : role === HelpRole ? 'helpRequested' : null
        if(signal) engine.emit(this, DialogButtonBox['SIGNAL_' + signal])
    }
    $componentComplete(){
        super.$componentComplete()
        if(this.$pendingButtons){
            this.$pendingButtons = false
            this.$madeOnce = true
            this.$makeStandardButtons()
        }
    }
}
defineType(DialogButtonBox, {
    props: { position: { kind: 'int', initial: 1 }, alignment: 'int', standardButtons: 'int', delegate: { kind: 'component', initial: null }, buttonLayout: 'int',
        defaultButton: { kind: 'var', initial: null }, defaultStandardButton: 'int' },
    signals: ['accepted', 'rejected', 'helpRequested', 'clicked(button)', 'applied', 'reset', 'discarded'],
})
DialogButtonBox.$setters = withSetters(Container, [
    [DialogButtonBox.STANDARDBUTTONS, (b, v) => b.$setStandardButtons(v)],
    [DialogButtonBox.ALIGNMENT, (b, v) => b.$plain(DialogButtonBox.ALIGNMENT, v === undefined ? 0 : v)],
])
const STANDARD_ENUMS = { NoButton: 0, Ok: 0x400, Save: 0x800, SaveAll: 0x1000, Open: 0x2000, Yes: 0x4000, YesToAll: 0x8000, No: 0x10000,
    NoToAll: 0x20000, Abort: 0x40000, Retry: 0x80000, Ignore: 0x100000, Close: 0x200000, Cancel: 0x400000, Discard: 0x800000, Help: 0x1000000,
    Apply: 0x2000000, Reset: 0x4000000, RestoreDefaults: 0x8000000 }
const ROLE_ENUMS = { InvalidRole, AcceptRole, RejectRole, DestructiveRole, ActionRole, HelpRole, YesRole, NoRole, ResetRole, ApplyRole }
Object.assign(DialogButtonBox, STANDARD_ENUMS, ROLE_ENUMS, { Header: 0, Footer: 1, WinLayout: 0, MacLayout: 1, KdeLayout: 2, GnomeLayout: 3, AndroidLayout: 4 })
// the role of a button written on the attached object moves it
DialogButtonBoxAttached.prototype.$write = function(id, value){
    GroupObject.prototype.$write.call(this, id, value)
    let box = this.$v[DialogButtonBoxAttached.BUTTONBOX]
    if(id === DialogButtonBoxAttached.BUTTONROLE && box && !box.$sorting) box.$sort()
}
for(let [name, value] of Object.entries(Object.assign({}, STANDARD_ENUMS, ROLE_ENUMS))) Object.defineProperty(DialogButtonBoxAttached.prototype, name, { value, configurable: true })

// ---- Dialog (QQuickDialog) ----
const Rejected = 0, Accepted = 1
class Dialog extends Popup {
    constructor(parent = null){
        super(parent)
        this.$write(Popup.FOCUS, true)
        // the header and footer of a dialog are those of its popup item (a Page)
        for(let name of ['title', 'header', 'footer']){
            let itemId = this.$item.constructor.$type.propByName.get(name).id
            engine.connect(this.$item, this.$item.constructor.$type.props[itemId].signal, () => this.$pull(Dialog[name.toUpperCase()], this.$item.$v[itemId]), null, engine.PRIORITY_NORMAL, this)
        }
        for(let name of ['implicitHeaderWidth', 'implicitHeaderHeight', 'implicitFooterWidth', 'implicitFooterHeight']){
            let itemId = this.$item.constructor.$type.propByName.get(name).id
            engine.connect(this.$item, this.$item.constructor.$type.props[itemId].signal, () => this.$pull(Dialog[name.toUpperCase()], this.$item.$v[itemId]), null, engine.PRIORITY_NORMAL, this)
        }
    }
    $write(id, value){
        if(this.$syncing) return super.$write(id, value)
        if(id === Dialog.TITLE) return (this.$item.title = value)
        if(id === Dialog.HEADER) return (this.$item.header = value)
        if(id === Dialog.FOOTER){
            this.$item.footer = value
            this.$connectBox(value)
            return
        }
        if(id === Dialog.STANDARDBUTTONS){
            super.$write(id, value)
            let box = this.$box()
            if(box) box.standardButtons = value
            return
        }
        super.$write(id, value)
    }
    $box(){
        let footer = this.$item.$v[this.$item.constructor.FOOTER]
        return footer instanceof DialogButtonBox ? footer : null
    }
    // QQuickDialogPrivate::handleAccept / handleReject: the box of the footer accepts and rejects the dialog
    $connectBox(box){
        if(!(box instanceof DialogButtonBox) || box.$dialog === this) return
        box.$dialog = this
        engine.connect(box, DialogButtonBox.SIGNAL_accepted, () => this.accept(), null, engine.PRIORITY_NORMAL, this)
        engine.connect(box, DialogButtonBox.SIGNAL_rejected, () => this.reject(), null, engine.PRIORITY_NORMAL, this)
        for(let name of ['applied', 'reset', 'discarded', 'helpRequested'])
            engine.connect(box, DialogButtonBox['SIGNAL_' + name], () => engine.emit(this, Dialog['SIGNAL_' + name]), null, engine.PRIORITY_NORMAL, this)
        if(this.$v[Dialog.STANDARDBUTTONS]) box.standardButtons = this.$v[Dialog.STANDARDBUTTONS]
    }
    standardButton(button){ let box = this.$box(); return box ? box.standardButton(button) : null }
    // QQuickDialog::done: the result, then accepted / rejected, then closed
    done(result){
        engine.batch(() => {
            this.$plain(Dialog.RESULT, result)
            if(result === Accepted) engine.emit(this, Dialog.SIGNAL_accepted)
            else if(result === Rejected) engine.emit(this, Dialog.SIGNAL_rejected)
            this.close()
        })
    }
    accept(){ this.done(Accepted) }
    reject(){ this.done(Rejected) }
    // Escape rejects (QQuickDialog::keyReleaseEvent -> reject)
    $closeByEscape(){ this.reject() }
}
defineType(Dialog, {
    props: {
        title: 'string', header: { kind: 'var', initial: null }, footer: { kind: 'var', initial: null }, standardButtons: 'int', result: 'int',
        implicitHeaderWidth: { kind: 'real', readonly: true }, implicitHeaderHeight: { kind: 'real', readonly: true },
        implicitFooterWidth: { kind: 'real', readonly: true }, implicitFooterHeight: { kind: 'real', readonly: true },
    },
    signals: ['accepted', 'rejected', 'applied', 'reset', 'discarded', 'helpRequested'],
})
Object.assign(Dialog, STANDARD_ENUMS, { Rejected, Accepted })
Dialog.prototype.$popupName = 'Dialog'

// ---- MenuItem (QQuickMenuItem) ----
class MenuItem extends AbstractButton {
    $trigger(){
        super.$trigger()
        engine.emit(this, MenuItem.SIGNAL_triggered)
    }
    $delegateChanged(id){
        if(id === MenuItem.ARROW) return
        super.$delegateChanged(id)
    }
    $hoverEnter(){
        super.$hoverEnter()
        let menu = this.$v[MenuItem.MENU]
        if(menu && menu.$itemHovered) menu.$itemHovered(this)
    }
}
defineType(MenuItem, {
    props: {
        highlighted: 'bool', arrow: { kind: 'var', initial: null }, menu: { kind: 'var', readonly: true, initial: null },
        subMenu: { kind: 'var', readonly: true, initial: null }, implicitTextPadding: 'real', textPadding: { kind: 'real', readonly: true },
    },
    signals: ['triggered'],
})
MenuItem.$setters = withSetters(AbstractButton, [[MenuItem.ARROW, (m, v) => m.$setDelegate(MenuItem.ARROW, v)]])

// ---- Menu (QQuickMenu) ----
class Menu extends Popup {
    constructor(parent = null){
        super(parent)
        let { ObjectModel } = require('../../QtQml/Models/ObjectModel')
        this.$plain(Menu.CONTENTMODEL, new ObjectModel(null))
        this.$plain(Menu.CASCADE, true)
        this.$v[Menu.ICON] = new (require('./impl').Icon)(this)
        this.$plain(Menu.CURRENTINDEX, -1)
        this.$write(Popup.FOCUS, true)
        this.$write(Popup.MARGINS, 0)
    }
    // the items declared in the menu: its content model (QQuickMenuPrivate::insertItem)
    $childObjectAdded(child){ if(child instanceof require('./Actions').Action) this.$childAdded(child) }
    $childAdded(child){
        // an Action declared in the menu: an item for it (QQuickMenuPrivate::contentData_append: insertAction)
        if(child instanceof require('./Actions').Action){
            if(!this.$declaredActions) this.$declaredActions = new Set()
            if(this.$declaredActions.has(child)) return
            this.$declaredActions.add(child)
            this.$insert(this.$items().length, this.$actionItem(child))
            return
        }
        if(!child.$isItem || child === this.$item) return
        this.$insert(this.$items().length, child)
    }
    $items(){ return this.$v[Menu.CONTENTMODEL].$items }
    get count(){
        engine.track(this.$v[Menu.CONTENTMODEL], this.$v[Menu.CONTENTMODEL].constructor.SIGNAL_countChanged)
        return this.$items().length
    }
    $insert(index, item){
        let model = this.$v[Menu.CONTENTMODEL]
        if(model.$items.includes(item)) return
        model.insert(index, item)
        if(item instanceof MenuItem){
            item.$plain(MenuItem.MENU, this)
            engine.connect(item, MenuItem.SIGNAL_triggered, () => this.dismiss(), null, engine.PRIORITY_NORMAL, this)
        }
        engine.emit(this, Menu.SIGNAL_countChanged)
    }
    itemAt(index){ return this.$items()[index] || null }
    addItem(item){ engine.batch(() => this.$insert(this.$items().length, item)) }
    insertItem(index, item){ engine.batch(() => this.$insert(index, item)) }
    moveItem(from, to){ engine.batch(() => this.$v[Menu.CONTENTMODEL].move(from, to, 1)) }
    takeItem(index){
        let item = this.itemAt(index)
        if(!item) return null
        engine.batch(() => {
            this.$v[Menu.CONTENTMODEL].remove(index, 1)
            item.$setParent(null)
            engine.emit(this, Menu.SIGNAL_countChanged)
        })
        return item
    }
    removeItem(item){
        let taken = this.takeItem(typeof item === 'number' ? item : this.$items().indexOf(item))
        if(taken) taken.destroy()
    }
    // sub menus: an item of the delegate (a MenuItem) opens the menu (QQuickMenuPrivate::createItem(menu))
    $menuItem(menu){
        let delegate = this.$v[Menu.DELEGATE]
        let item = delegate && delegate.createObject ? delegate.createObject(null) : new MenuItem(null)
        item.$plain(MenuItem.SUBMENU, menu)
        item.text = menu.title
        menu.$parentMenu = this
        engine.connect(item, AbstractButton.SIGNAL_clicked, () => { menu.$setParent(item); menu.x = item.width; menu.y = 0; menu.open() }, null, engine.PRIORITY_NORMAL, this)
        return item
    }
    menuAt(index){ let item = this.itemAt(index); return item && item.subMenu || null }
    addMenu(menu){ this.insertMenu(this.$items().length, menu) }
    insertMenu(index, menu){ if(menu) this.insertItem(index, this.$menuItem(menu)) }
    removeMenu(menu){ let i = this.$items().findIndex(it => it.subMenu === menu); if(i >= 0) this.removeItem(i) }
    takeMenu(index){ let menu = this.menuAt(index); if(menu) this.takeItem(index).destroy(); return menu }
    // actions: an item of the delegate shows the action
    $actionItem(action){
        let delegate = this.$v[Menu.DELEGATE]
        // (no delegate yet: the MenuItem of the style, the delegate of Menu.qml)
        let item = delegate && delegate.createObject ? delegate.createObject(null) : new (require('./Basic').MenuItem || MenuItem)(null)
        item.action = action
        return item
    }
    actionAt(index){ let item = this.itemAt(index); return item && item.action || null }
    addAction(action){ this.insertAction(this.$items().length, action) }
    insertAction(index, action){ if(action) this.insertItem(index, this.$actionItem(action)) }
    removeAction(action){ let i = this.$items().findIndex(it => it.action === action); if(i >= 0) this.removeItem(i) }
    takeAction(index){ let action = this.actionAt(index); if(action) this.takeItem(index).destroy(); return action }
    dismiss(){
        this.close()
        if(this.$parentMenu) this.$parentMenu.dismiss()
    }
    // QQuickMenu::popup([parent,] [x, y,] [item]): at a point of the parent (else of the menu's parent)
    popup(...args){
        engine.batch(() => {
            let parent = null
            if(args[0] && args[0].$isItem && typeof args[1] === 'number'){ parent = args.shift() }
            let at = typeof args[0] === 'number' ? { x: args[0], y: args[1] } : args[0] && typeof args[0] === 'object' && 'x' in args[0] && !args[0].$isItem ? args[0] : null
            if(parent && parent !== this.$parentItem()) this.$setParent(parent)
            if(at){
                this.x = at.x
                this.y = at.y
            }
            this.open()
        })
    }
    $setCurrentIndex(index){
        let items = this.$items()
        this.$plain(Menu.CURRENTINDEX, index)
        items.forEach((item, i) => { if(item instanceof MenuItem) item.highlighted = i === index })
    }
    $itemHovered(item){ this.$setCurrentIndex(this.$items().indexOf(item)) }
    // the items a key goes to (separators and disabled ones are skipped)
    $step(sign){
        let items = this.$items()
        let i = this.$v[Menu.CURRENTINDEX]
        for(let n = 0; n < items.length; n++){
            i = i + sign
            if(i < 0) i = items.length - 1
            if(i >= items.length) i = 0
            let item = items[i]
            if(item instanceof MenuItem && item.enabled) return this.$setCurrentIndex(i)
        }
    }
    $hide(){
        super.$hide()
        this.$setCurrentIndex(-1)
    }
    // the menus declared in the menu: items of it (QQuickMenuPrivate::contentData_append; the actions are items as they
    // come, $childAdded)
    $componentComplete(){
        let declared = (this.$children || []).filter(c => c instanceof Menu && c !== this)
        for(let child of declared) this.insertMenu(this.$items().length, child)
        super.$componentComplete()
    }
}
defineType(Menu, {
    props: {
        contentModel: { kind: 'var', readonly: true }, title: 'string', cascade: 'bool', overlap: 'real',
        delegate: { kind: 'component', initial: null }, currentIndex: 'int', separatorsCollapsible: { kind: 'bool', initial: true },
        icon: { group: require('./impl').Icon },
    },
    signals: ['countChanged'],
})
Menu.prototype.$popupName = 'Menu'
// the keys of an open menu: its popup item has the focus (QQuickMenu::keyPressEvent)
{
    let { PopupItem } = require('./Popup')
    let base = PopupItem.prototype.$keyEvent
    PopupItem.prototype.$keyEvent = function(event, kind){
        let menu = this.$popup
        if(menu instanceof Menu && kind === 'pressed'){
            let K = QtEnums
            if(event.key === K.Key_Up){ menu.$step(-1); return 1 }
            if(event.key === K.Key_Down){ menu.$step(1); return 1 }
            if(event.key === K.Key_Return || event.key === K.Key_Enter || event.key === K.Key_Space){
                let item = menu.itemAt(menu.$v[Menu.CURRENTINDEX])
                if(item instanceof MenuItem) item.click()
                return 1
            }
        }
        if(menu && menu.$closeByEscape && kind === 'pressed' && event.key === QtEnums.Key_Escape && (menu.$v[Popup.CLOSEPOLICY] & 16)){
            menu.$closeByEscape()
            return 1
        }
        return base.call(this, event, kind)
    }
}
// the Popup setter map does not exist: a Menu keeps currentIndex itself
{
    let base = Popup.prototype.$write
    Menu.prototype.$write = function(id, value){
        if(id === Menu.CURRENTINDEX && !this.$syncing) return this.$setCurrentIndex(value)
        return base.call(this, id, value)
    }
}

// ---- Drawer (QQuickDrawer) ----
const TopEdge = 1, LeftEdge = 2, RightEdge = 4, BottomEdge = 8
class Drawer extends Popup {
    constructor(parent = null){
        super(parent)
        this.$plain(Drawer.EDGE, LeftEdge)
        this.$plain(Drawer.INTERACTIVE, true)
        this.$plain(Drawer.DRAGMARGIN, 10)
        this.$write(Popup.MODAL, true)
        this.$write(Popup.FOCUS, true)
        this.$write(Popup.CLOSEPOLICY, 16 | 4)
    }
    // the place of the drawer by its edge and position (QQuickDrawerPrivate::reposition)
    $reposition(){
        if(!this.$v[Popup.VISIBLE] && !this.$transitionTo) return
        let item = this.$item
        let overlay = item.$parent
        if(!overlay || !(overlay instanceof Overlay)) return
        let ow = overlay.$v[Item.WIDTH], oh = overlay.$v[Item.HEIGHT]
        let w = item.$v[Item.WIDTH], h = item.$v[Item.HEIGHT]
        let p = this.$v[Drawer.POSITION]
        let x = 0, y = 0
        switch(this.$v[Drawer.EDGE]){
        case LeftEdge: x = -w + p * w; break
        case RightEdge: x = ow - p * w; break
        case TopEdge: y = -h + p * h; break
        case BottomEdge: y = oh - p * h; break
        }
        item.$write(Item.X, x)
        item.$write(Item.Y, y)
        this.$setEffective(x, y)
    }
    $setPosition(value){
        this.$plain(Drawer.POSITION, Math.max(0, Math.min(1, Number(value))))
        this.$reposition()
    }
    // enter / exit: the position to 1 and back (the transitions of the style animate it)
    $runTransition(transition, done){
        let to = this.$transitionTo ? 1 : 0
        if(!transition){
            this.$setPosition(to)
            return super.$runTransition(transition, done)
        }
        // a transition without a property of its own animates the position
        this.$animatePosition(to, done)
    }
    $animatePosition(to, done){
        if(this.$positionTimer) clearInterval(this.$positionTimer)
        let from = this.$v[Drawer.POSITION]
        // SmoothedAnimation { velocity: 5 }: 5 units a second
        let duration = Math.abs(to - from) / 5 * 1000
        let start = Date.now()
        if(duration <= 0){ this.$setPosition(to); return done() }
        this.$positionTimer = setInterval(() => {
            if(this.$f & engine.DESTROYED) return clearInterval(this.$positionTimer)
            let t = Math.min(1, (Date.now() - start) / duration)
            engine.batch(() => {
                this.$setPosition(from + (to - from) * t)
                if(t >= 1){
                    clearInterval(this.$positionTimer)
                    this.$positionTimer = null
                    done()
                }
            })
        }, 16)
    }
}
defineType(Drawer, { props: { edge: 'int', position: 'real', dragMargin: 'real', interactive: 'bool' } })
Drawer.prototype.$popupName = 'Drawer'
{
    let base = Popup.prototype.$write
    Drawer.prototype.$write = function(id, value){
        if(id === Drawer.POSITION && !this.$syncing) return this.$setPosition(value)
        if(id === Drawer.EDGE && !this.$syncing){
            base.call(this, id, value)
            return this.$reposition()
        }
        return base.call(this, id, value)
    }
}

// ToolTip.* and DialogButtonBox.* on items
for(let [name, Group] of [['ToolTip', ToolTipAttached], ['DialogButtonBox', DialogButtonBoxAttached]]){
    let prop = Item.$type.propByName.get(name)
    if(!prop) continue
    prop.group = Group
    Object.defineProperty(Item.prototype, name, {
        get(){ return this.$v[prop.id] || (this.$v[prop.id] = new Group(this)) },
        configurable: true,
    })
}

module.exports = { ToolTip, ToolTipAttached, DialogButtonBox, DialogButtonBoxAttached, Dialog, Menu, MenuItem, Drawer, STANDARD }
