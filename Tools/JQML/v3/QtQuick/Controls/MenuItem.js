const AbstractButton = require("./AbstractButton")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const StringProp = require("../../QtQml/String")
const Signal = require("../../QtQml/Signal")

class MenuItem extends AbstractButton {
    static meta = Object.assign({}, AbstractButton.meta, {
        action:               { type: Var },
        arrow:                { type: Var },
        highlighted:          { type: Var,        value: false },
        implicitTextPadding:  { type: Real,        value: 0 },
        implicitHeight:       { type: Real,        value: 0 },
        implicitWidth:        { type: Real,        value: 0 },
        menu:                 { type: Var },
        shortcut:             { type: StringProp,  value: '' },
        spacing:              { type: Real,        value: 6 },
        subMenu:              { type: Var },
        text:                 { type: StringProp,  value: '' },
        textPadding:          { type: Real,        value: 0 },

        actionChanged:              { type: Signal, args: [] },
        arrowChanged:               { type: Signal, args: [] },
        highlightedChanged:         { type: Signal, args: [] },
        implicitTextPaddingChanged: { type: Signal, args: [] },
        implicitHeightChanged:      { type: Signal, args: [] },
        implicitWidthChanged:       { type: Signal, args: [] },
        menuChanged:                { type: Signal, args: [] },
        shortcutChanged:            { type: Signal, args: [] },
        spacingChanged:             { type: Signal, args: [] },
        subMenuChanged:             { type: Signal, args: [] },
        textChanged:                { type: Signal, args: [] },
        textPaddingChanged:         { type: Signal, args: [] },

        triggered: { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}) {
        let obj = super.create(parent, properties)
        let dom = obj.__getDOM()
        dom.classList.add('MenuItem')
        obj.__setDOMStyle({
            display:         'flex',
            flexDirection:   'row',
            alignItems:      'center',
            padding:         '4px 12px',
            cursor:          'pointer',
            userSelect:      'none',
            whiteSpace:      'nowrap',
            minWidth:        '200px',
            minHeight:       '30px',
            boxSizing:       'border-box',
            backgroundColor: 'transparent',
            transition:      'background-color 0.08s ease',
            outline:         'none',
        })

        // ── Sub-elements ──────────────────────────────────────────────────────

        // Checkmark indicator (reserved space for alignment even when hidden)
        let checkEl = document.createElement('span')
        checkEl.style.cssText = 'width:16px;flex-shrink:0;text-align:center;font-size:14px;margin-right:4px;visibility:hidden;'
        checkEl.textContent = '\u2713'
        dom.appendChild(checkEl)
        obj.__checkEl = checkEl

        // Icon image
        let iconEl = document.createElement('img')
        iconEl.style.cssText = 'width:16px;height:16px;flex-shrink:0;object-fit:contain;display:none;'
        dom.appendChild(iconEl)
        obj.__iconEl = iconEl

        // Spacer between icon and text
        let iconSpacer = document.createElement('span')
        iconSpacer.style.cssText = 'width:6px;flex-shrink:0;display:none;'
        dom.appendChild(iconSpacer)
        obj.__iconSpacer = iconSpacer

        // Text span
        let textEl = document.createElement('span')
        textEl.style.flex = '1'
        dom.appendChild(textEl)
        obj.__textEl = textEl

        // Shortcut hint (right-aligned, dim)
        let scEl = document.createElement('span')
        scEl.style.cssText = 'flex-shrink:0;margin-left:24px;font-size:12px;color:#888;display:none;'
        dom.appendChild(scEl)
        obj.__scEl = scEl

        // Initial text render
        obj.__renderText(obj.text || '')

        // ── DOM events ────────────────────────────────────────────────────────

        dom.addEventListener('mouseenter', () => {
            if (obj.enabled !== false) obj.highlighted = true
        })
        dom.addEventListener('mouseleave', () => {
            obj.highlighted = false
        })
        dom.addEventListener('mousedown', () => {
            if (obj.enabled !== false) obj.__setDOMStyle({ backgroundColor: '#d0d0d0' })
        })
        dom.addEventListener('mouseup', () => {
            obj.__setDOMStyle({ backgroundColor: obj.highlighted ? '#e8e8e8' : 'transparent' })
        })
        dom.addEventListener('click', (e) => {
            e.stopPropagation()
            if (obj.enabled === false) return

            if (obj.checkable) {
                obj.checked = !obj.checked
                if (obj.toggled) obj.toggled()
            }

            if (obj.triggered) obj.triggered()

            // Fire action if bound
            let action = obj.action
            if (action && action.trigger) action.trigger()

            // Dismiss the entire menu hierarchy
            let menu = obj.menu
            if (menu) menu.dismiss ? menu.dismiss() : menu.close()
        })

        return obj
    }

    // ── Property change handlers ──────────────────────────────────────────────

    SLOT_textChanged(oldValue, newValue) {
        this.__renderText(newValue || '')
    }

    SLOT_highlightedChanged(oldValue, newValue) {
        this.__setDOMStyle({ backgroundColor: newValue ? '#e8e8e8' : 'transparent' })
    }

    SLOT_checkableChanged(oldValue, newValue) {
        this.__updateCheckIndicator()
    }

    SLOT_checkedChanged(oldValue, newValue) {
        this.__updateCheckIndicator()
    }

    SLOT_enabledChanged(oldValue, newValue) {
        this.__setDOMStyle({ opacity: newValue ? '1' : '0.4', cursor: newValue ? 'pointer' : 'default' })
    }

    SLOT_shortcutChanged(oldValue, newValue) {
        let scEl = this.__scEl
        if (!scEl) return
        if (newValue) {
            scEl.textContent    = newValue
            scEl.style.display  = ''
        } else {
            scEl.textContent    = ''
            scEl.style.display  = 'none'
        }
    }

    SLOT_parentChanged(oldValue, newValue) {
        super.SLOT_parentChanged && super.SLOT_parentChanged(oldValue, newValue)
        // Auto-set menu reference when parented to a Menu/Popup
        if (newValue && typeof newValue.__contentDOM !== 'undefined' && this.menu === undefined) {
            this.menu = newValue
        }
    }

    // ── Internal helpers ──────────────────────────────────────────────────────

    // Render text, handling & mnemonic (e.g. "&File" → underlined F)
    __renderText(text) {
        let el = this.__textEl
        if (!el) return
        el.innerHTML = ''
        if (!text) return

        let ampIdx = text.indexOf('&')
        if (ampIdx >= 0 && ampIdx < text.length - 1) {
            if (ampIdx > 0) el.appendChild(document.createTextNode(text.substring(0, ampIdx)))
            let u = document.createElement('span')
            u.style.textDecoration = 'underline'
            u.textContent = text[ampIdx + 1]
            el.appendChild(u)
            if (ampIdx + 2 < text.length) el.appendChild(document.createTextNode(text.substring(ampIdx + 2)))
            this.__mnemonic = text[ampIdx + 1].toLowerCase()
        } else {
            el.textContent = text
            this.__mnemonic = null
        }
    }

    __updateCheckIndicator() {
        let el = this.__checkEl
        if (!el) return
        if (this.checkable) {
            el.style.display    = ''
            el.style.visibility = this.checked ? 'visible' : 'hidden'
        } else {
            // Keep space reserved if siblings are checkable (handled by menu)
            if (!this.__checkColumnReserved) el.style.display = 'none'
        }
    }

    // Called by parent Menu when any sibling is checkable (reserve alignment space)
    __setCheckColumnReserved(reserved) {
        this.__checkColumnReserved = reserved
        let el = this.__checkEl
        if (!el) return
        if (reserved) {
            el.style.display    = ''
            if (!this.checkable) el.style.visibility = 'hidden'
        } else {
            this.__updateCheckIndicator()
        }
    }
}



module.exports = MenuItem
