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
        // Qt Basic Menu: background=palette.window(#fff), border=palette.dark(#c0c0c0)
        //                implicitWidth=200, implicitHeight=40
        if (this.$contentBox){
            this.$contentBox.style.minWidth = '200px'
            this.$contentBox.style.padding = '4px 0'
            this.$contentBox.style.overflowX = 'visible'
            this.$contentBox.style.overflowY = 'auto'
            this.$contentBox.style.maxHeight = '100vh'
            // Menu height must auto-size to content (items are dynamic).
            // Prevent Popup's $applyGeometry from locking a pixel height.
            this.$contentBox.style.height = 'auto'
        }

        // Menu contentBox should never have a locked pixel height
        this.$autoContentHeight = true

        // Internal ordered list of content items (separate from QML children)
        this.$contentItems = []

        // Override Popup's mouse handlers: also check if click is inside a child submenu
        this.$onDocumentMouseDown = (e) => {
            if (!this.getPropertyValue('visible')) return
            if (this.$contentBox && this.$contentBox.contains(e.target)) return
            if (this.$isInsideSubmenu(e.target)) return
            if (this.$isInsideParentMenu(e.target)) return
            let policy = this.getPropertyValue('closePolicy')
            if (policy & 3){ // CloseOnPressOutside | CloseOnPressOutsideParent
                this.close()
            }
        }

        this.$onDocumentMouseUp = (e) => {
            if (!this.getPropertyValue('visible')) return
            if (this.$contentBox && this.$contentBox.contains(e.target)) return
            if (this.$isInsideSubmenu(e.target)) return
            if (this.$isInsideParentMenu(e.target)) return
            let policy = this.getPropertyValue('closePolicy')
            if (policy & 12){ // CloseOnReleaseOutside | CloseOnReleaseOutsideParent
                this.close()
            }
        }

        // Override Popup's resize handler: recalc geometry then clamp
        this.$onResize = () => {
            if (this.getPropertyValue('visible')){
                this.$applyGeometry()
                // Update original position from freshly computed geometry
                if (this.$contentBox){
                    let rect = this.$contentBox.getBoundingClientRect()
                    this.$originalX = rect.left
                    this.$originalY = rect.top
                }
                this.$clampToViewport()
            }
        }

        // Keyboard navigation handler
        this.$onKeyDown = (e) => {
            if (!this.getPropertyValue('visible')) return

            switch (e.key){
                case 'ArrowDown':
                    e.preventDefault()
                    e.stopPropagation()
                    this.$navigateNext()
                    break
                case 'ArrowUp':
                    e.preventDefault()
                    e.stopPropagation()
                    this.$navigatePrev()
                    break
                case 'ArrowRight':
                    e.preventDefault()
                    e.stopPropagation()
                    this.$openCurrentSubmenu()
                    break
                case 'ArrowLeft':
                    e.preventDefault()
                    e.stopPropagation()
                    this.$closeToParent()
                    break
                case 'Enter':
                case 'Return':
                    e.preventDefault()
                    e.stopPropagation()
                    this.$triggerCurrent()
                    break
                case 'Escape':
                    e.preventDefault()
                    e.stopPropagation()
                    this.close()
                    break
                default:
                    // Mnemonic: check if pressed key matches an & mnemonic in items
                    this.$handleMnemonic(e)
                    break
            }
        }

        // Reference to the active submenu (only one at a time)
        this.$activeSubmenu = null

        // Parent menu reference (set by parent when submenu is added)
        this.$parentMenu = null
    }

    // Check if a DOM target is inside any child submenu's content box
    $isInsideSubmenu(target){
        let children = this.getProperty('children').get()
        for (let child of children){
            if (child instanceof Menu){
                if (child.$contentBox && child.$contentBox.contains(target)) return true
                // Recurse into nested submenus
                if (child.$isInsideSubmenu && child.$isInsideSubmenu(target)) return true
            }
        }
        return false
    }

    // Check if a DOM target is inside a parent menu's content box (for submenus)
    $isInsideParentMenu(target){
        let p = this.getProperty('parent').get()
        while (p){
            if (p instanceof Menu){
                if (p.$contentBox && p.$contentBox.contains(target)) return true
                if (p.$isInsideSubmenu && p.$isInsideSubmenu(target)) return true
            }
            if (p.getProperty) p = p.getProperty('parent').get()
            else break
        }
        return false
    }

    // Override Popup's addDomChild: force children to position:relative
    // so they participate in the flex column layout (Item default is absolute)
    addDomChild(child){
        // If child is a submenu, don't append its DOM (it lives on document.body as a Popup).
        // The trigger row was already added in addChild.
        if (child instanceof Menu) return

        super.addDomChild(child)
        if (child.$dom){
            child.$dom.style.position = 'relative'
            child.$dom.style.left = ''
            child.$dom.style.top = ''
        }
    }

    // ── Child management ────────────────────────────────────────

    addChild(child){
        // If child is a Menu, treat it as a submenu
        if (child instanceof Menu){
            // Re-parent
            if (child.getProperty && child.getProperty('parent') && child.getProperty('parent').get() !== this){
                child.getProperty('parent').reset(this)
            } else {
                super.addChild(child)
            }

            // Create a trigger row (looks like a MenuItem)
            let trigger = this.$createSubmenuTrigger(child)

            // Track the trigger in contentItems (not the Menu itself)
            if (this.$contentItems.indexOf(trigger) < 0){
                this.$contentItems.push(trigger)
            }

            // Store references
            child.$submenuTrigger = trigger
            trigger.$submenu = child

            this.$updateCount()
            return
        }

        // Non-submenu child: normal flow
        if (child.getProperty && child.getProperty('parent') && child.getProperty('parent').get() !== this){
            child.getProperty('parent').reset(this)
        } else {
            super.addChild(child)
        }
        // Track in content items
        if (child.UID !== undefined && this.$contentItems.indexOf(child) < 0) {
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

    $createSubmenuTrigger(submenu){
        let trigger = document.createElement('div')
        trigger.style.cssText = 'display:flex;flex-direction:row;align-items:center;padding:4px 12px;cursor:pointer;user-select:none;white-space:nowrap;color:#000;background:transparent;min-width:200px;min-height:30px;box-sizing:border-box;position:relative;transition:background-color 0.08s ease;pointer-events:auto;'

        let textSpan = document.createElement('span')
        textSpan.style.flex = '1'
        textSpan.style.textAlign = 'left'
        textSpan.textContent = submenu.getPropertyValue('title') || ''
        trigger.appendChild(textSpan)

        // Arrow indicator
        let arrow = document.createElement('span')
        arrow.style.cssText = 'margin-left:12px;font-size:10px;color:#666;flex-shrink:0;'
        arrow.textContent = '\u25B6' // ▶
        trigger.appendChild(arrow)

        // Track title changes
        submenu.getProperty('title').getNotify().connect(() => {
            textSpan.textContent = submenu.getPropertyValue('title') || ''
        })

        // Hover: highlight + open submenu with delay (Qt ~225ms)
        let hideTimer = null
        let showTimer = null

        let scheduleHide = () => {
            if (hideTimer) clearTimeout(hideTimer)
            hideTimer = setTimeout(() => {
                hideTimer = null
                if (!submenu.getPropertyValue('opened')) return
                let onTrigger = trigger.matches(':hover')
                let onSubmenu = submenu.$contentBox && submenu.$contentBox.matches(':hover')
                if (!onTrigger && !onSubmenu){
                    submenu.close()
                    if (this.$activeSubmenu === submenu) this.$activeSubmenu = null
                } else {
                    scheduleHide()
                }
            }, 300)
        }

        let cancelHide = () => {
            if (hideTimer){ clearTimeout(hideTimer); hideTimer = null }
        }

        let cancelShow = () => {
            if (showTimer){ clearTimeout(showTimer); showTimer = null }
        }

        trigger.addEventListener('mouseenter', () => {
            cancelHide()
            trigger.style.backgroundColor = '#e8e8e8'
            // Delay submenu opening (~225ms) to avoid accidental opening
            cancelShow()
            showTimer = setTimeout(() => {
                showTimer = null
                this.$openSubmenu(submenu, trigger)
            }, 225)
        })
        trigger.addEventListener('mouseleave', () => {
            trigger.style.backgroundColor = 'transparent'
            cancelShow()
            scheduleHide()
        })

        // Lazily attach contentBox listeners on first open (contentBox may not
        // exist yet during construction — Popup constructor runs after Item's
        // addChild fires, so $contentBox is still undefined at trigger creation)
        trigger.$contentBoxListenersAttached = false
        trigger.$attachContentBoxListeners = () => {
            if (trigger.$contentBoxListenersAttached) return
            if (!submenu.$contentBox) return
            trigger.$contentBoxListenersAttached = true
            submenu.$contentBox.addEventListener('mouseenter', () => cancelHide())
            submenu.$contentBox.addEventListener('mouseleave', () => scheduleHide())
        }

        // Append trigger to parent menu's content box
        if (this.$contentBox){
            this.$contentBox.appendChild(trigger)
        }

        // Store DOM reference for reorder/removal
        trigger.UID = submenu.UID
        trigger.$dom = trigger
        trigger.getPropertyValue = (name) => {
            if (name === 'visible') return true
            return undefined
        }

        return trigger
    }

    $openSubmenu(submenu, triggerEl){
        if (submenu.getPropertyValue('opened')) return

        // Close any previously active submenu first
        if (this.$activeSubmenu && this.$activeSubmenu !== submenu && this.$activeSubmenu.getPropertyValue('opened')){
            this.$activeSubmenu.close()
        }
        this.$activeSubmenu = submenu

        // Set parent reference for keyboard navigation (Left arrow closes submenu)
        submenu.$parentMenu = this

        // Lazily attach contentBox mouse listeners (contentBox didn't exist at trigger creation)
        if (triggerEl.$attachContentBoxListeners) triggerEl.$attachContentBoxListeners()

        // Position the submenu relative to the trigger
        let triggerRect = triggerEl.getBoundingClientRect()

        // Override the Popup geometry computation: position content box directly
        submenu.open()

        // After open, compute position: prefer right, fall back to left
        requestAnimationFrame(() => {
            if (!submenu.$contentBox || !submenu.getPropertyValue('opened')) return
            let subRect = submenu.$contentBox.getBoundingClientRect()
            let vw = window.innerWidth
            let vh = window.innerHeight

            // Find parent menu's content box rect for left-side fallback
            let parentRect = this.$contentBox ? this.$contentBox.getBoundingClientRect() : triggerRect

            let x, y
            // Try right side first
            if (triggerRect.right + subRect.width <= vw){
                x = triggerRect.right
            } else {
                // Not enough room on right — show to the left of parent menu
                x = parentRect.left - subRect.width
                if (x < 0) x = 0 // ultimate fallback
            }

            y = triggerRect.top
            // Clamp bottom
            if (y + subRect.height > vh) y = Math.max(0, vh - subRect.height)
            // Clamp top
            if (y < 0) y = 0

            submenu.$contentBox.style.left = x + 'px'
            submenu.$contentBox.style.top = y + 'px'
            submenu.$originalX = x
            submenu.$originalY = y
        })
    }

    // Menu auto-sizes to content: never lock contentBox to a pixel height
    $applyGeometry(){
        super.$applyGeometry()
        if (this.$autoContentHeight && this.$contentBox){
            this.$contentBox.style.height = 'auto'
        }
    }

    // ── Keyboard navigation helpers ─────────────────────────────

    // Get navigable items (skip separators, disabled items, and hidden items)
    $getNavigableItems(){
        let result = []
        for (let item of this.$contentItems){
            // Submenu triggers are plain DOM objects with $submenu reference — navigable
            if (item.$submenu){
                result.push(item)
                continue
            }
            // Skip non-QML objects (shouldn't happen, but guard)
            if (!item.getProperty) continue
            // Skip MenuSeparator
            if (item.$isSeparator) continue
            let enabledProp = item.getProperty('enabled')
            if (enabledProp && !enabledProp.get()) continue
            let visProp = item.getProperty('visible')
            if (visProp && !visProp.get()) continue
            result.push(item)
        }
        return result
    }

    $navigateNext(){
        let navigable = this.$getNavigableItems()
        if (navigable.length === 0) return
        let curIdx = this.getPropertyValue('currentIndex')
        let curItem = curIdx >= 0 ? this.$contentItems[curIdx] : null
        let navIdx = navigable.indexOf(curItem)
        let nextIdx = (navIdx + 1) % navigable.length
        let newItem = navigable[nextIdx]
        let newContentIdx = this.$contentItems.indexOf(newItem)
        this.getProperty('currentIndex').reset(newContentIdx)
    }

    $navigatePrev(){
        let navigable = this.$getNavigableItems()
        if (navigable.length === 0) return
        let curIdx = this.getPropertyValue('currentIndex')
        let curItem = curIdx >= 0 ? this.$contentItems[curIdx] : null
        let navIdx = navigable.indexOf(curItem)
        let nextIdx = navIdx <= 0 ? navigable.length - 1 : navIdx - 1
        let newItem = navigable[nextIdx]
        let newContentIdx = this.$contentItems.indexOf(newItem)
        this.getProperty('currentIndex').reset(newContentIdx)
    }

    $openCurrentSubmenu(){
        let curIdx = this.getPropertyValue('currentIndex')
        if (curIdx < 0 || curIdx >= this.$contentItems.length) return
        let item = this.$contentItems[curIdx]
        // Check if item is a submenu trigger
        if (item.$submenu){
            this.$openSubmenu(item.$submenu, item.$dom || item)
            // Focus the submenu and highlight first item
            item.$submenu.getProperty('currentIndex').reset(-1)
            item.$submenu.$navigateNext()
        }
    }

    $closeToParent(){
        // If this is a submenu, close it and return focus to parent
        if (this.$parentMenu){
            this.close()
            // Parent menu regains keyboard focus
            if (this.$parentMenu.$contentBox){
                this.$parentMenu.$contentBox.focus()
            }
        }
    }

    $triggerCurrent(){
        let curIdx = this.getPropertyValue('currentIndex')
        if (curIdx < 0 || curIdx >= this.$contentItems.length) return
        let item = this.$contentItems[curIdx]
        // If it's a submenu trigger, open it
        if (item.$submenu){
            this.$openCurrentSubmenu()
            return
        }
        // Simulate click on the MenuItem
        if (item.getDom){
            item.getDom().click()
        }
    }

    $handleMnemonic(e){
        // Mnemonic activation: single character key press (with or without Alt)
        let key = e.key.length === 1 ? e.key.toLowerCase() : null
        if (!key) return
        for (let item of this.$contentItems){
            if (!item.getPropertyValue) continue
            let text = item.getPropertyValue('text') || ''
            let ampIdx = text.indexOf('&')
            if (ampIdx >= 0 && ampIdx < text.length - 1){
                let mnemonic = text[ampIdx + 1].toLowerCase()
                if (mnemonic === key){
                    let idx = this.$contentItems.indexOf(item)
                    this.getProperty('currentIndex').reset(idx)
                    // If submenu, open; otherwise trigger
                    if (item.$submenu){
                        this.$openCurrentSubmenu()
                    } else if (item.getDom){
                        item.getDom().click()
                    }
                    e.preventDefault()
                    e.stopPropagation()
                    return
                }
            }
        }
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
        // Update checkmark column reservation: if any item is checkable, all reserve space
        this.$updateCheckColumnReservation()
    }

    // If any MenuItem in the menu has checkable=true, all items reserve the checkmark column
    $updateCheckColumnReservation(){
        let hasCheckable = false
        for (let item of this.$contentItems){
            if (item.getPropertyValue && item.getPropertyValue('checkable')){
                hasCheckable = true
                break
            }
        }
        for (let item of this.$contentItems){
            if (item.$setCheckColumnReserved){
                item.$setCheckColumnReserved(hasCheckable)
            }
        }
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
        // Add as QML child (re-parents & moves DOM)
        this.addChild(item)
        // For submenus, reorder the trigger, not the menu itself
        let trackItem = (item instanceof Menu && item.$submenuTrigger) ? item.$submenuTrigger : item
        // Reorder in content items list
        let curIdx = this.$contentItems.indexOf(trackItem)
        if (curIdx >= 0) this.$contentItems.splice(curIdx, 1)
        this.$contentItems.splice(index, 0, trackItem)
        // Reorder in DOM
        this.$reorderDom()
        this.$updateCount()
        if (!(item instanceof Menu)) this.$trackHighlight(item)
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
        // For submenus, find and remove the trigger
        if (item instanceof Menu && item.$submenuTrigger){
            let trigger = item.$submenuTrigger
            let idx = this.$contentItems.indexOf(trigger)
            if (idx >= 0) this.$contentItems.splice(idx, 1)
            if (trigger.parentNode) trigger.parentNode.removeChild(trigger)
            if (item.getPropertyValue('opened')) item.close()
            delete item.$submenuTrigger
            this.$updateCount()
            return
        }
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
        // Save focused element for restoration on close
        this.$previousFocus = document.activeElement
        this.getSignal('aboutToShow')()
        this.getProperty('visible').reset(true)
        this.getProperty('opened').reset(true)
        // Attach keyboard listener and focus the content box
        if (this.$contentBox){
            this.$contentBox.setAttribute('tabindex', '-1')
            this.$contentBox.style.outline = 'none'
            this.$contentBox.addEventListener('keydown', this.$onKeyDown)
            requestAnimationFrame(() => {
                if (this.$contentBox && this.getPropertyValue('opened')){
                    this.$contentBox.focus()
                }
            })
        }
    }

    close(){
        if (!this.getPropertyValue('opened')) return
        this.getSignal('aboutToHide')()
        // Remove keyboard listener
        if (this.$contentBox){
            this.$contentBox.removeEventListener('keydown', this.$onKeyDown)
        }
        this.getProperty('visible').reset(false)
        this.getProperty('opened').reset(false)
        // Reset currentIndex
        this.getProperty('currentIndex').reset(-1)
        // Close active submenu
        if (this.$activeSubmenu && this.$activeSubmenu.getPropertyValue('opened')){
            this.$activeSubmenu.close()
        }
        this.$activeSubmenu = null
        // Close sub-menus (triggers have $submenu reference)
        for (let item of this.$contentItems){
            if (item.$submenu && item.$submenu.getPropertyValue('opened')){
                item.$submenu.close()
            }
        }
        // Also close child Menus directly
        let children = this.getProperty('children').get()
        for (let child of children){
            if (child instanceof Menu && child.getPropertyValue('opened')){
                child.close()
            }
        }
        // Restore focus
        if (this.$previousFocus && this.$previousFocus.focus){
            this.$previousFocus.focus()
            this.$previousFocus = null
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
            requestAnimationFrame(() => {
                if (!this.$contentBox) return
                // Save the original (unclamped) position
                let rect = this.$contentBox.getBoundingClientRect()
                this.$originalX = rect.left
                this.$originalY = rect.top
                this.$clampToViewport()
            })
        }
    }

    $clampToViewport(){
        if (!this.$contentBox) return
        let rect = this.$contentBox.getBoundingClientRect()
        let vw = window.innerWidth
        let vh = window.innerHeight

        // Start from the original (unclamped) position
        let x = this.$originalX !== undefined ? this.$originalX : rect.left
        let y = this.$originalY !== undefined ? this.$originalY : rect.top
        let w = rect.width
        let h = rect.height

        // Clamp right edge
        if (x + w > vw) x = Math.max(0, vw - w)
        // Clamp bottom edge
        if (y + h > vh) y = Math.max(0, vh - h)
        // Clamp left edge
        if (x < 0) x = 0
        // Clamp top edge
        if (y < 0) y = 0

        this.$contentBox.style.left = `${x}px`
        this.$contentBox.style.top = `${y}px`
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
            } else if (item.$dom){
                // Submenu trigger (plain DOM element)
                item.$dom.style.backgroundColor = (i === curIdx) ? '#e8e8e8' : 'transparent'
            }
        }
    }

    // ── DOM reordering helper ───────────────────────────────────

    $reorderDom(){
        if (!this.$contentBox) return
        for (let item of this.$contentItems){
            // For submenu triggers (plain DOM objects), use $dom or the object itself
            let dom = item.$dom || (item.getDom ? item.getDom() : null)
            if (dom && dom.parentNode === this.$contentBox){
                this.$contentBox.appendChild(dom)
            }
        }
    }

    destroy(){
        if (this.$contentBox){
            this.$contentBox.removeEventListener('keydown', this.$onKeyDown)
        }
        window.removeEventListener('resize', this.$onResize)
        this.$contentItems = []
        super.destroy()
    }
}

module.exports.Menu = Menu
