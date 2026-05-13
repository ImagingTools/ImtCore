const { Popup } = require('./Popup')
const { QString, QBool, QReal, QVar, QInt } = require('../utils/properties')

class Menu extends Popup {
    static defaultProperties = {
        // Menu-specific properties (Qt Quick Controls Menu)
        title: { type: QString, value: '', changed: '$titleChanged' },
        cascade: { type: QBool, value: true },
        overlap: { type: QReal, value: 0 },
        count: { type: QInt, value: 0 },
        currentIndex: { type: QInt, value: -1, changed: '$currentIndexChanged' },
        delegate: { type: QVar },
        contentModel: { type: QVar },
        icon: { type: QVar },
    }

    static defaultSignals = {
        // Popup already provides: aboutToShow, aboutToHide, closed
        // Menu adds: (none extra beyond Popup, but we ensure opened is available)
        opened: { params: [] },
    }

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)

        // Override Popup's contentBox styling for menu appearance
        if (this.$contentBox){
            this.$contentBox.style.minWidth = '200px'
            this.$contentBox.style.padding = '4px 0'
            this.$contentBox.style.overflowX = 'visible'
            this.$contentBox.style.overflowY = 'auto'
            this.$contentBox.style.maxHeight = '100vh'
        }

        // Internal ordered list of content items (separate from QML children)
        this.$contentItems = []
    }

    // Override Popup's addDomChild: force children to position:relative
    // so they participate in the flex column layout (Item default is absolute)
    addDomChild(child){
        super.addDomChild(child)
        if (child.$dom){
            child.$dom.style.position = 'relative'
            child.$dom.style.left = ''
            child.$dom.style.top = ''
        }
    }

    // ── Child management ────────────────────────────────────────

    addChild(child){
        super.addChild(child)
        // Track in content items
        if (child.UID !== undefined) {
            this.$contentItems.push(child)
        }
        // Auto-set MenuItem.menu back-reference
        if (child.getProperty && child.getProperty('menu')){
            child.getProperty('menu').reset(this)
        }
        this.$updateCount()
        // Track currentIndex via highlighted
        this.$trackHighlight(child)
    }

    $trackHighlight(child){
        if (!child.getProperty) return
        let hlProp = child.getProperty('highlighted')
        if (!hlProp) return
        let notify = hlProp.getNotify()
        if (notify && notify.connect){
            notify.connect(() => {
                if (child.getPropertyValue('highlighted')){
                    let idx = this.$contentItems.indexOf(child)
                    if (idx >= 0) this.getProperty('currentIndex').reset(idx)
                }
            })
        }
    }

    $updateCount(){
        this.getProperty('count').reset(this.$contentItems.length)
    }

    // ── Qt Methods: item access ─────────────────────────────────

    itemAt(index){
        return (index >= 0 && index < this.$contentItems.length) ? this.$contentItems[index] : null
    }

    menuAt(index){
        let item = this.itemAt(index)
        return (item && item instanceof Menu) ? item : null
    }

    actionAt(index){
        let item = this.itemAt(index)
        if (!item) return null
        // If child is a MenuItem with an action, return the action
        if (item.getProperty && item.getPropertyValue) {
            let action = item.getPropertyValue('action')
            if (action) return action
        }
        // If child is an Action directly
        if (typeof global !== 'undefined' && global.Action && item instanceof global.Action) return item
        return null
    }

    // ── Qt Methods: add ─────────────────────────────────────────

    addItem(item){
        if (!item) return
        this.addChild(item)
    }

    addMenu(menu){
        if (!menu) return
        this.addChild(menu)
    }

    addAction(action){
        if (!action) return
        // Wrap action in a MenuItem (Qt behaviour: delegate creates MenuItem for Action)
        let mi = new (global.MenuItem)(this)
        mi.getProperty('action').reset(action)
        // addChild already called by MenuItem constructor via super → addChild chain
    }

    // ── Qt Methods: insert ──────────────────────────────────────

    insertItem(index, item){
        if (!item) return
        index = Math.max(0, Math.min(index, this.$contentItems.length))
        // Add as QML child (appends to children & DOM)
        super.addChild(item)
        if (item.getProperty && item.getProperty('menu')){
            item.getProperty('menu').reset(this)
        }
        // Reorder in content items list
        let curIdx = this.$contentItems.indexOf(item)
        if (curIdx >= 0) this.$contentItems.splice(curIdx, 1)
        this.$contentItems.splice(index, 0, item)
        // Reorder in DOM
        this.$reorderDom()
        this.$updateCount()
        this.$trackHighlight(item)
    }

    insertMenu(index, menu){
        this.insertItem(index, menu)
    }

    insertAction(index, action){
        if (!action) return
        let mi = new (global.MenuItem)(this)
        mi.getProperty('action').reset(action)
        // Move to correct position
        let curIdx = this.$contentItems.indexOf(mi)
        if (curIdx >= 0) this.$contentItems.splice(curIdx, 1)
        index = Math.max(0, Math.min(index, this.$contentItems.length))
        this.$contentItems.splice(index, 0, mi)
        this.$reorderDom()
        this.$updateCount()
    }

    // ── Qt Methods: remove ──────────────────────────────────────

    removeItem(item){
        if (!item) return
        let idx = this.$contentItems.indexOf(item)
        if (idx >= 0) {
            this.$contentItems.splice(idx, 1)
            // Remove from QML children
            let children = this.getProperty('children').get()
            let ci = children.indexOf(item)
            if (ci >= 0) children.splice(ci, 1)
            // Remove DOM
            if (item.$dom && item.$dom.parentNode){
                item.$dom.parentNode.removeChild(item.$dom)
            }
            if (item.destroy) item.destroy()
            this.$updateCount()
        }
    }

    removeMenu(menu){
        this.removeItem(menu)
    }

    removeAction(action){
        if (!action) return
        // Find MenuItem that wraps this action
        for (let i = 0; i < this.$contentItems.length; i++){
            let item = this.$contentItems[i]
            if (item.getPropertyValue && item.getPropertyValue('action') === action){
                this.removeItem(item)
                return
            }
        }
    }

    // ── Qt Methods: take (remove without destroy, transfer ownership) ──

    takeItem(index){
        if (index < 0 || index >= this.$contentItems.length) return null
        let item = this.$contentItems[index]
        this.$contentItems.splice(index, 1)
        let children = this.getProperty('children').get()
        let ci = children.indexOf(item)
        if (ci >= 0) children.splice(ci, 1)
        if (item.$dom && item.$dom.parentNode){
            item.$dom.parentNode.removeChild(item.$dom)
        }
        this.$updateCount()
        return item
    }

    takeMenu(index){
        let item = this.itemAt(index)
        if (item && item instanceof Menu){
            return this.takeItem(index)
        }
        return null
    }

    takeAction(index){
        let item = this.itemAt(index)
        if (!item) return null
        let action = null
        if (item.getPropertyValue) action = item.getPropertyValue('action')
        if (typeof global !== 'undefined' && global.Action && item instanceof global.Action) action = item
        if (action) {
            this.takeItem(index)
            return action
        }
        return null
    }

    // ── Qt Methods: move ────────────────────────────────────────

    moveItem(from, to){
        if (from < 0 || from >= this.$contentItems.length) return
        if (to < 0 || to >= this.$contentItems.length) return
        let item = this.$contentItems.splice(from, 1)[0]
        this.$contentItems.splice(to, 0, item)
        this.$reorderDom()
    }

    // ── Qt Methods: open / close / popup / dismiss ──────────────

    open(){
        if (this.getPropertyValue('opened')) return
        this.getSignal('aboutToShow')()
        this.getProperty('visible').reset(true)
        this.getProperty('opened').reset(true)
    }

    close(){
        if (!this.getPropertyValue('opened')) return
        this.getSignal('aboutToHide')()
        this.getProperty('visible').reset(false)
        this.getProperty('opened').reset(false)
        // Also close sub-menus
        for (let item of this.$contentItems){
            if (item instanceof Menu && item.getPropertyValue('opened')){
                item.close()
            }
        }
    }

    popup(...args){
        // Overloaded: popup(), popup(x,y), popup(x,y,menuItem),
        //             popup(parent, x, y), popup(parent, x, y, menuItem)
        let x, y, menuItem

        if (args.length === 0){
            // popup() — open at current position
        } else if (args.length === 2 && typeof args[0] === 'number' && typeof args[1] === 'number'){
            // popup(x, y)
            x = args[0]; y = args[1]
        } else if (args.length === 3 && typeof args[0] === 'number' && typeof args[1] === 'number'){
            // popup(x, y, menuItem)
            x = args[0]; y = args[1]; menuItem = args[2]
        } else if (args.length >= 3 && typeof args[0] === 'object' && typeof args[1] === 'number'){
            // popup(parent, x, y [, menuItem])
            x = args[1]; y = args[2]; menuItem = args[3]
        } else if (args.length === 1){
            // popup(menuItem)
            menuItem = args[0]
        }

        if (x !== undefined && y !== undefined){
            this.getProperty('x').reset(x)
            this.getProperty('y').reset(y)
        }

        if (menuItem){
            let idx = this.$contentItems.indexOf(menuItem)
            if (idx >= 0) this.getProperty('currentIndex').reset(idx)
        } else {
            this.getProperty('currentIndex').reset(-1)
        }

        this.open()
    }

    dismiss(){
        // dismiss() closes the whole hierarchy: this menu + parent menus
        this.close()
        // Walk up parent chain and close parent menus
        let p = this.getProperty('parent').get()
        while (p){
            if (p instanceof Menu){
                p.close()
            }
            if (p.getProperty) {
                p = p.getProperty('parent').get()
            } else {
                break
            }
        }
    }

    // ── Visibility handler (extends Popup) ──────────────────────

    $visibleChanged(){
        super.$visibleChanged()

        if (this.getPropertyValue('visible')){
            this.getSignal('opened')()            // Clamp to viewport after layout completes
            requestAnimationFrame(() => this.$clampToViewport())
        }
    }

    $clampToViewport(){
        if (!this.$contentBox) return
        let rect = this.$contentBox.getBoundingClientRect()
        let vw = window.innerWidth
        let vh = window.innerHeight
        let changed = false
        let x = rect.left
        let y = rect.top

        // If menu extends past right edge, shift left
        if (rect.right > vw){
            x = Math.max(0, vw - rect.width)
            changed = true
        }
        // If menu extends past bottom edge, shift up
        if (rect.bottom > vh){
            y = Math.max(0, vh - rect.height)
            changed = true
        }
        // If menu extends past left edge
        if (x < 0){
            x = 0
            changed = true
        }
        // If menu extends past top edge
        if (y < 0){
            y = 0
            changed = true
        }

        if (changed){
            this.$contentBox.style.left = `${x}px`
            this.$contentBox.style.top = `${y}px`        }
    }

    $titleChanged(){
        // title is used by MenuBar to display the menu trigger label
    }

    $currentIndexChanged(){
        // Update highlighted state on items
        let curIdx = this.getPropertyValue('currentIndex')
        for (let i = 0; i < this.$contentItems.length; i++){
            let item = this.$contentItems[i]
            if (item.getProperty && item.getProperty('highlighted')){
                item.getProperty('highlighted').reset(i === curIdx)
            }
        }
    }

    // ── DOM reordering helper ───────────────────────────────────

    $reorderDom(){
        if (!this.$contentBox) return
        for (let item of this.$contentItems){
            if (item.$dom && item.$dom.parentNode === this.$contentBox){
                this.$contentBox.appendChild(item.$dom)
            }
        }
    }

    destroy(){
        this.$contentItems = []
        super.destroy()
    }
}

module.exports.Menu = Menu
