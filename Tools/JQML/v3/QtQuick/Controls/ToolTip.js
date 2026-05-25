const Popup = require("./Popup")
const Real = require("../../QtQml/Real")
const Int = require("../../QtQml/Int")
const Var = require("../../QtQml/Var")
const StringProp = require("../../QtQml/String")
const Signal = require("../../QtQml/Signal")

class ToolTip extends Popup {
    static meta = Object.assign({}, Popup.meta, {
        delay:   { type: Int,        value: 0  },
        text:    { type: StringProp, value: '' },
        timeout: { type: Int,        value: -1 },

        delayChanged:   { type: Signal, args: [] },
        textChanged:    { type: Signal, args: [] },
        timeoutChanged: { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}) {
        let obj = super.create(parent, properties)
        let box = obj.__contentDOM
        if (box) {
            box.classList.add('ToolTip')
            // Override default popup styling for tooltip look
            box.style.cssText = 'position:absolute;display:flex;flex-direction:column;pointer-events:none;background:#616161;color:#ffffff;border:none;border-radius:4px;box-shadow:0 2px 4px rgba(0,0,0,0.2);padding:4px 8px;font-size:12px;max-width:300px;word-wrap:break-word;box-sizing:border-box;outline:none;'
        }

        // Text node
        obj.__tooltipTextNode = document.createTextNode(obj.text || '')
        if (box) box.appendChild(obj.__tooltipTextNode)

        return obj
    }

    // ── Property change handlers ──────────────────────────────────────────────

    SLOT_textChanged(oldValue, newValue) {
        if (this.__tooltipTextNode) this.__tooltipTextNode.nodeValue = newValue || ''
    }

    SLOT_timeoutChanged(oldValue, newValue) {
        // Restart auto-close if already visible
        if (this.visible && newValue > 0) {
            this.__startTimeoutTimer()
        } else {
            this.__clearTimeoutTimer()
        }
    }

    // ── Internal helpers ──────────────────────────────────────────────────────

    __startDelayTimer() {
        this.__clearDelayTimer()
        let self = this.__self || this
        let delay = this.delay
        if (delay > 0) {
            this.__delayTimer = setTimeout(() => {
                this.__delayTimer = null
                self.__doOpen()
            }, delay)
        } else {
            this.__doOpen()
        }
    }

    __clearDelayTimer() {
        if (this.__delayTimer) { clearTimeout(this.__delayTimer); this.__delayTimer = null }
    }

    __startTimeoutTimer() {
        this.__clearTimeoutTimer()
        let timeout = this.timeout
        if (timeout > 0) {
            let self = this.__self || this
            this.__timeoutTimer = setTimeout(() => {
                this.__timeoutTimer = null
                self.close()
            }, timeout)
        }
    }

    __clearTimeoutTimer() {
        if (this.__timeoutTimer) { clearTimeout(this.__timeoutTimer); this.__timeoutTimer = null }
    }

    __doOpen() {
        super.open()
        this.__startTimeoutTimer()
    }

    // ── Public API ────────────────────────────────────────────────────────────

    open() {
        this.__startDelayTimer()
    }

    close() {
        this.__clearDelayTimer()
        this.__clearTimeoutTimer()
        super.close()
    }

    // show(text?, timeout?) – convenience method matching Qt API
    show(text, timeout) {
        if (text    !== undefined) this.text    = text
        if (timeout !== undefined) this.timeout = timeout
        this.open()
    }

    hide() {
        this.close()
    }

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    __destroy() {
        this.__clearDelayTimer()
        this.__clearTimeoutTimer()
        super.__destroy()
    }
}



module.exports = ToolTip
