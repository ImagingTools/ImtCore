const Popup = require("./Popup")
const Real = require("../../QtQml/Real")
const Int = require("../../QtQml/Int")
const Var = require("../../QtQml/Var")
const StringProp = require("../../QtQml/String")
const Signal = require("../../QtQml/Signal")

class Menu extends Popup {
    static meta = Object.assign({}, Popup.meta, {
        cascade:       { type: Var,        value: true },
        count:         { type: Int,        value: 0 },
        currentIndex:  { type: Int,        value: -1 },
        delegate:      { type: Var },
        displayMargin: { type: Real,       value: 0 },
        focus:         { type: Var,        value: true },
        icon:          { type: Var },
        implicitHeight: { type: Real,      value: 0 },
        implicitWidth:  { type: Real,      value: 0 },
        overlap:       { type: Int,        value: 0 },
        title:         { type: StringProp, value: '' },

        cascadeChanged:       { type: Signal, args: [] },
        countChanged:         { type: Signal, args: [] },
        currentIndexChanged:  { type: Signal, args: [] },
        delegateChanged:      { type: Signal, args: [] },
        displayMarginChanged: { type: Signal, args: [] },
        iconChanged:          { type: Signal, args: [] },
        implicitHeightChanged: { type: Signal, args: [] },
        implicitWidthChanged: { type: Signal, args: [] },
        overlapChanged:       { type: Signal, args: [] },
        titleChanged:         { type: Signal, args: [] },

        triggered:   { type: Signal, args: ['action'] },
        aboutToHide: { type: Signal, args: [] },
        aboutToShow: { type: Signal, args: [] }
    })

    // Ordered list of content items (used for keyboard navigation & API)
    __menuItems = []
    // The currently active submenu (if any)
    __activeSubmenu = null
    // Parent menu (set when opened as a submenu)
    __parentMenu = null

    static create(parent = null, properties = {}) {
        let obj = super.create(parent, properties)
        let box = obj.__contentDOM
        if (box) {
            box.classList.add('Menu')
            // Menu items stack vertically; height is auto (driven by content)
            box.style.padding    = '4px 0'
            box.style.minWidth   = '200px'
            box.style.height     = 'auto'
        }
        return obj
    }

    // ── Child DOM hook ────────────────────────────────────────────────────────
    // Reset child positions so flex column layout works correctly

    __onChildDOM(child, dom) {
        dom.style.position = 'relative'
        dom.style.left     = ''
        dom.style.top      = ''
        dom.style.width    = '100%'
    }

    // ── Property change handlers ──────────────────────────────────────────────

    SLOT_visibleChanged(oldValue, newValue) {
        super.SLOT_visibleChanged(oldValue, newValue)
        if (!newValue) {
            // Close active submenu
            if (this.__activeSubmenu && this.__activeSubmenu.visible) {
                this.__activeSubmenu.close()
            }
            this.__activeSubmenu = null
            this.currentIndex = -1
            // Remove keyboard listener
            if (this.__contentDOM)
                this.__contentDOM.removeEventListener('keydown', this.__menuKeyHandler)
        } else {
            // Install keyboard navigation
            let self = this.__self || this
            this.__menuKeyHandler = (e) => { self.__handleMenuKey(e) }
            if (this.__contentDOM) {
                this.__contentDOM.addEventListener('keydown', this.__menuKeyHandler)
                requestAnimationFrame(() => {
                    if (self.visible && self.__contentDOM) self.__contentDOM.focus()
                })
            }
        }
    }

    SLOT_currentIndexChanged(oldValue, newValue) {
        for (let i = 0; i < this.__menuItems.length; i++) {
            let item = this.__menuItems[i]
            if (item && item.highlighted !== undefined) {
                item.highlighted = (i === newValue)
            } else if (item && item.__triggerDOM) {
                // Submenu trigger row
                item.__triggerDOM.style.backgroundColor = (i === newValue) ? '#e8e8e8' : 'transparent'
            }
        }
    }

    // ── Keyboard navigation ───────────────────────────────────────────────────

    __handleMenuKey(e) {
        if (!this.visible) return
        let handled = false

        switch (e.key) {
            case 'ArrowDown':
                this.__navigateNext()
                handled = true
                break
            case 'ArrowUp':
                this.__navigatePrev()
                handled = true
                break
            case 'ArrowRight':
                this.__openCurrentSubmenu()
                handled = true
                break
            case 'ArrowLeft':
                if (this.__parentMenu) { this.close(); handled = true }
                break
            case 'Enter':
            case 'Return':
                this.__triggerCurrent()
                handled = true
                break
            case 'Escape':
                this.close()
                handled = true
                break
        }

        if (handled) { e.preventDefault(); e.stopPropagation() }
    }

    __navigableItems() {
        return this.__menuItems.filter(item => {
            if (!item) return false
            if (item.__isSeparator) return false
            if (item.__triggerDOM) return true // submenu trigger
            return item.enabled !== false && item.visible !== false
        })
    }

    __navigateNext() {
        let navigable = this.__navigableItems()
        if (!navigable.length) return
        let cur     = this.__menuItems[this.currentIndex]
        let navIdx  = navigable.indexOf(cur)
        let next    = navigable[(navIdx + 1) % navigable.length]
        this.currentIndex = this.__menuItems.indexOf(next)
    }

    __navigatePrev() {
        let navigable = this.__navigableItems()
        if (!navigable.length) return
        let cur    = this.__menuItems[this.currentIndex]
        let navIdx = navigable.indexOf(cur)
        let prev   = navigable[navIdx <= 0 ? navigable.length - 1 : navIdx - 1]
        this.currentIndex = this.__menuItems.indexOf(prev)
    }

    __openCurrentSubmenu() {
        let idx  = this.currentIndex
        let item = this.__menuItems[idx]
        if (item && item.__triggerDOM && item.__submenu) {
            this.__openSubmenu(item.__submenu, item.__triggerDOM)
        }
    }

    __triggerCurrent() {
        let idx  = this.currentIndex
        let item = this.__menuItems[idx]
        if (!item) return
        if (item.__triggerDOM) {
            this.__openCurrentSubmenu()
        } else if (item.__getDOM) {
            item.__getDOM().click()
        }
    }

    // ── Submenu handling ──────────────────────────────────────────────────────

    __openSubmenu(submenu, triggerEl) {
        if (submenu.visible) return

        if (this.__activeSubmenu && this.__activeSubmenu !== submenu && this.__activeSubmenu.visible) {
            this.__activeSubmenu.close()
        }
        this.__activeSubmenu = submenu
        submenu.__parentMenu  = this

        submenu.open()

        // Position submenu to the right of the trigger row
        requestAnimationFrame(() => {
            if (!submenu.visible || !submenu.__contentDOM) return
            let trigRect = triggerEl.getBoundingClientRect()
            let subBox   = submenu.__contentDOM
            let subRect  = subBox.getBoundingClientRect()
            let vw       = window.innerWidth
            let vh       = window.innerHeight
            let x = trigRect.right
            let y = trigRect.top
            if (x + subRect.width > vw) x = trigRect.left - subRect.width
            if (y + subRect.height > vh) y = Math.max(0, vh - subRect.height)
            subBox.style.left = x + 'px'
            subBox.style.top  = y + 'px'
        })
    }

    // ── QML child tracking ────────────────────────────────────────────────────
    // When a child Menu (submenu) is added, create a trigger row instead of
    // letting its __contentDOM appear inside this menu.

    SLOT_childrenChanged(oldValue, newValue) {
        // Re-sync menuItems from children list
        this.__syncMenuItems()
    }

    __syncMenuItems() {
        // Rebuild __menuItems from children
        // Children were already connected to contentDOM via __onChildDOM / __connectDOM
        let children = this.__children
        this.__menuItems = []
        for (let child of children) {
            let proxy = child.__proxy || child
            if (proxy.__contentDOM) {
                // It's a submenu (Popup/Menu) – create/reuse a trigger row
                if (!proxy.__triggerDOM) {
                    this.__buildSubmenuTrigger(proxy)
                }
                this.__menuItems.push(proxy)
            } else {
                this.__menuItems.push(proxy)
                // Set back-reference so MenuItem knows its parent menu
                if (proxy.menu !== undefined && proxy.menu !== this) {
                    proxy.menu = this
                }
            }
        }
        this.count = this.__menuItems.length
    }

    __buildSubmenuTrigger(submenu) {
        // Hide submenu's own overlay DOM – it will be opened on hover/click
        // The content box should not be visible until submenu.open() is called

        // Create a trigger row (looks like a MenuItem)
        let row = document.createElement('div')
        row.style.cssText = 'display:flex;flex-direction:row;align-items:center;padding:4px 12px;cursor:pointer;user-select:none;white-space:nowrap;color:#000;background:transparent;min-width:200px;min-height:30px;box-sizing:border-box;position:relative;transition:background-color 0.08s ease;width:100%;'

        // Text label (from submenu.title)
        let textSpan = document.createElement('span')
        textSpan.style.flex = '1'
        textSpan.textContent = submenu.title || ''
        row.appendChild(textSpan)

        // Arrow indicator
        let arrow = document.createElement('span')
        arrow.style.cssText = 'margin-left:12px;font-size:10px;color:#666;'
        arrow.textContent = '\u25B6'
        row.appendChild(arrow)

        // Store references
        submenu.__triggerDOM = row
        row.__submenu        = submenu

        // Append trigger row to this menu's content box
        if (this.__contentDOM) this.__contentDOM.appendChild(row)

        // Hover events with delay (like Qt)
        let showTimer = null
        let hideTimer = null
        let self = this.__self || this

        const scheduleShow = () => {
            clearTimeout(hideTimer)
            clearTimeout(showTimer)
            showTimer = setTimeout(() => {
                self.__openSubmenu(submenu, row)
            }, 200)
        }
        const scheduleHide = () => {
            clearTimeout(showTimer)
            hideTimer = setTimeout(() => {
                if (submenu.visible) {
                    let onRow    = row.matches(':hover')
                    let onSubBox = submenu.__contentDOM && submenu.__contentDOM.matches(':hover')
                    if (!onRow && !onSubBox) submenu.close()
                    else scheduleHide()
                }
            }, 300)
        }

        row.addEventListener('mouseenter', () => {
            row.style.backgroundColor = '#e8e8e8'
            scheduleShow()
        })
        row.addEventListener('mouseleave', () => {
            row.style.backgroundColor = 'transparent'
            scheduleHide()
        })
        row.addEventListener('click', (e) => {
            e.stopPropagation()
            self.__openSubmenu(submenu, row)
        })

        // Track title changes
        submenu.titleChanged && submenu.titleChanged.connect
            && submenu.titleChanged.connect(() => { textSpan.textContent = submenu.title || '' })
    }

    // ── Qt public API ─────────────────────────────────────────────────────────

    popup(x, y, menuItem) {
        if (x !== undefined) this.x = x
        if (y !== undefined) this.y = y
        if (menuItem) {
            let idx = this.__menuItems.indexOf(menuItem)
            if (idx >= 0) this.currentIndex = idx
        }
        this.open()
    }

    dismiss() {
        this.close()
        // Close parent menus too
        let m = this.__parentMenu
        while (m) {
            m.close()
            m = m.__parentMenu
        }
    }

    itemAt(index) {
        return (index >= 0 && index < this.__menuItems.length)
            ? this.__menuItems[index] : null
    }

    addItem(item) {
        if (item) this.__children.push(item.__self || item)
        this.__syncMenuItems()
    }

    removeItem(item) {
        let idx = this.__children.indexOf(item.__self || item)
        if (idx >= 0) {
            this.__children.splice(idx, 1)
            let dom = item.__getDOM ? item.__getDOM() : item.__triggerDOM
            if (dom && dom.parentNode) dom.parentNode.removeChild(dom)
        }
        this.__syncMenuItems()
    }

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    __destroy() {
        if (this.__contentDOM)
            this.__contentDOM.removeEventListener('keydown', this.__menuKeyHandler)
        super.__destroy()
    }
}



module.exports = Menu
