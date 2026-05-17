const { Item } = require('./Item')
const { QVar, QBool, QReal } = require('../utils/properties')

/**
 * Overlay — Qt Quick Controls Overlay implementation for JQML v2
 * https://doc.qt.io/qt-6/qml-qtquick-controls-overlay.html
 *
 * Provides an overlay layer for popups. In Qt Quick Controls, Overlay is
 * a singleton attached property that manages the overlay stack for
 * the application window.
 *
 * In this web-based JQML implementation, the overlay is a DOM layer
 * that popups attach to. This class manages the popup stack.
 */

class Overlay extends Item {
    static defaultProperties = {
        modal: { type: QVar },
        modeless: { type: QVar },
        pressed: { type: QBool, value: false },
    }

    static defaultSignals = {
        released: { params: [] },
    }

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)
        this.setStyle({ display: 'none' })
        this.$popups = []
    }

    /**
     * Register a popup in the overlay stack
     */
    addPopup(popup){
        if (!this.$popups.includes(popup)){
            this.$popups.push(popup)
        }
    }

    /**
     * Unregister a popup from the overlay stack
     */
    removePopup(popup){
        let idx = this.$popups.indexOf(popup)
        if (idx >= 0) this.$popups.splice(idx, 1)
    }

    /**
     * Get the list of currently visible popups
     */
    get popups(){
        return this.$popups.filter(p => p.getPropertyValue && p.getPropertyValue('visible'))
    }

    destroy(){
        this.$popups = []
        super.destroy()
    }
}

module.exports.Overlay = Overlay
