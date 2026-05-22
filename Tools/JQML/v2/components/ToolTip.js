const { Popup } = require('./Popup')
const { QString, QBool, QInt, QReal, QVar } = require('../utils/properties')

/**
 * ToolTip — Qt Quick Controls ToolTip implementation for JQML v2
 * https://doc.qt.io/qt-6/qml-qtquick-controls-tooltip.html
 *
 * A tooltip popup that displays a short text hint. Supports delay, timeout,
 * and attached ToolTip.text / ToolTip.visible properties.
 */

class ToolTip extends Popup {
    static defaultProperties = {
        text: { type: QString, value: '', changed: '$textChanged' },
        delay: { type: QInt, value: 0 },
        timeout: { type: QInt, value: -1, changed: '$timeoutChanged' },
    }

    static defaultSignals = {}

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)

        // Override default close policy for tooltips
        this.getProperty('closePolicy').reset(Popup.CloseOnEscape | Popup.CloseOnPressOutside)

        // Style the content box as a tooltip
        this.$contentBox.style.cssText = 'position:absolute;display:flex;flex-direction:column;pointer-events:auto;background:#616161;color:#ffffff;border:none;border-radius:4px;box-shadow:0 2px 4px rgba(0,0,0,0.2);padding:4px 8px;font-size:12px;max-width:300px;word-wrap:break-word;'

        // Text node
        this.$textNode = document.createTextNode(this.getPropertyValue('text'))
        this.$contentBox.appendChild(this.$textNode)

        this.$timeoutTimer = null
        this.$delayTimer = null
    }

    open(){
        let delay = this.getPropertyValue('delay')
        if (delay > 0){
            if (this.$delayTimer) clearTimeout(this.$delayTimer)
            this.$delayTimer = setTimeout(() => {
                this.$delayTimer = null
                this.$doOpen()
            }, delay)
        } else {
            this.$doOpen()
        }
    }

    $doOpen(){
        super.open()
        let timeout = this.getPropertyValue('timeout')
        if (timeout > 0){
            if (this.$timeoutTimer) clearTimeout(this.$timeoutTimer)
            this.$timeoutTimer = setTimeout(() => {
                this.$timeoutTimer = null
                this.close()
            }, timeout)
        }
    }

    close(){
        if (this.$delayTimer){
            clearTimeout(this.$delayTimer)
            this.$delayTimer = null
        }
        if (this.$timeoutTimer){
            clearTimeout(this.$timeoutTimer)
            this.$timeoutTimer = null
        }
        super.close()
    }

    show(text, timeout){
        if (text !== undefined){
            this.getProperty('text').reset(text)
        }
        if (timeout !== undefined){
            this.getProperty('timeout').reset(timeout)
        }
        this.open()
    }

    hide(){
        this.close()
    }

    $textChanged(){
        if (this.$textNode){
            this.$textNode.nodeValue = this.getPropertyValue('text')
        }
    }

    $timeoutChanged(){
        // If visible and timeout changed, restart timer
        if (this.getPropertyValue('visible')){
            if (this.$timeoutTimer) clearTimeout(this.$timeoutTimer)
            let timeout = this.getPropertyValue('timeout')
            if (timeout > 0){
                this.$timeoutTimer = setTimeout(() => {
                    this.$timeoutTimer = null
                    this.close()
                }, timeout)
            }
        }
    }

    destroy(){
        if (this.$delayTimer) clearTimeout(this.$delayTimer)
        if (this.$timeoutTimer) clearTimeout(this.$timeoutTimer)
        delete this.$textNode
        delete this.$delayTimer
        delete this.$timeoutTimer
        super.destroy()
    }
}

module.exports.ToolTip = ToolTip
