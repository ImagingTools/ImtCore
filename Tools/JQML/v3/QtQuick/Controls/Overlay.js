const Item = require("../Item")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

/**
 * Overlay — a singleton-like item that manages the z-layering of popups.
 * In QML, Overlay is an attached type. Here it acts as a container you can
 * query for the list of currently-open popups.
 */
class Overlay extends Item {
    static meta = Object.assign({}, Item.meta, {
        popups: { type: Var, value: undefined },
        popupsChanged: { type: Signal, args: [] },
    })

    static create(parent, ...args) {
        let obj = super.create(parent, ...args)
        obj.__DOM.style.display = 'none'
        obj.__popupList = []
        return obj
    }

    addPopup(popup) {
        if (!this.__popupList.includes(popup)) {
            this.__popupList.push(popup)
            this.popups = this.__popupList.slice()
            this.popupsChanged()
        }
    }

    removePopup(popup) {
        let idx = this.__popupList.indexOf(popup)
        if (idx >= 0) {
            this.__popupList.splice(idx, 1)
            this.popups = this.__popupList.slice()
            this.popupsChanged()
        }
    }

    getPopups() {
        return this.__popupList.filter(p => p.visible)
    }
}

module.exports = Overlay
