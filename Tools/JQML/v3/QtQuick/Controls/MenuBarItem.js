const Control = require("./Control")
const StringProp = require("../../QtQml/String")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class MenuBarItem extends Control {
    static meta = Object.assign({}, Control.meta, {
        highlighted: { type: Var,        value: false },
        menu:        { type: Var },
        text:        { type: StringProp, value: '' },

        highlightedChanged: { type: Signal, args: [] },
        menuChanged:        { type: Signal, args: [] },
        textChanged:        { type: Signal, args: [] },

        triggered: { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}) {
        let obj = super.create(parent, properties)
        let dom = obj.__getDOM()
        dom.classList.add('MenuBarItem')
        obj.__setDOMStyle({
            display:         'flex',
            alignItems:      'center',
            padding:         '4px 12px',
            cursor:          'pointer',
            userSelect:      'none',
            whiteSpace:      'nowrap',
            boxSizing:       'border-box',
            position:        'relative',
            backgroundColor: 'transparent',
        })

        // Text node for label rendering
        obj.__textNode = document.createTextNode(obj.text || '')
        dom.appendChild(obj.__textNode)

        // ── DOM event listeners ───────────────────────────────────────────────

        dom.addEventListener('click', (e) => {
            e.stopPropagation()
            if (!obj.enabled) return
            let menu = obj.menu
            if (menu) {
                let rect = dom.getBoundingClientRect()
                if (menu.visible) {
                    menu.close ? menu.close() : (menu.visible = false)
                } else {
                    obj.__closeAllSiblingMenus()
                    menu.popup
                        ? menu.popup(rect.left, rect.bottom)
                        : (menu.x = rect.left, menu.y = rect.bottom, menu.visible = true)
                }
            }
            if (obj.triggered) obj.triggered()
        })

        dom.addEventListener('mouseenter', () => {
            if (!obj.enabled) return
            obj.__setDOMStyle({ backgroundColor: '#e0e0e0' })
            obj.highlighted = true
            // Hover-to-switch: if any sibling menu is open — open ours instead
            if (obj.__menuBarHasOpenMenu()) {
                let menu = obj.menu
                if (menu) {
                    obj.__closeAllSiblingMenus()
                    let rect = dom.getBoundingClientRect()
                    menu.popup
                        ? menu.popup(rect.left, rect.bottom)
                        : (menu.x = rect.left, menu.y = rect.bottom, menu.visible = true)
                }
            }
        })

        dom.addEventListener('mouseleave', () => {
            obj.__setDOMStyle({ backgroundColor: 'transparent' })
            obj.highlighted = false
        })

        return obj
    }

    // ── Property change handlers ─────────────────────────────────────────────

    SLOT_textChanged(oldValue, newValue) {
        if (this.__textNode) this.__textNode.nodeValue = newValue || ''
    }

    SLOT_highlightedChanged(oldValue, newValue) {
        this.__setDOMStyle({
            backgroundColor: newValue ? '#e0e0e0' : 'transparent'
        })
    }

    // ── Internal helpers ────────────────────────────────────────────────────

    __menuBarHasOpenMenu() {
        let parent = this.parent
        if (!parent || !parent.children) return false
        for (let sibling of parent.children) {
            if (sibling === this) continue
            let m = sibling.menu
            if (m && m.visible) return true
        }
        return false
    }

    __closeAllSiblingMenus() {
        let parent = this.parent
        if (!parent || !parent.children) return
        for (let sibling of parent.children) {
            if (sibling === this) continue
            let m = sibling.menu
            if (m && m.visible) {
                m.close ? m.close() : (m.visible = false)
            }
        }
    }
}



module.exports = MenuBarItem
