const QtEnums = require("../Qt/enums")

module.exports = {
    shortcuts: [],

    __activateKeyNavigationTarget(target){
        if(!target) return false
        if(typeof target.forceActiveFocus === 'function'){
            target.forceActiveFocus()
        } else {
            target.focus = true
        }
        return true
    },

    __activateTabFocusTarget(target, forward){
        if(!target || typeof target.nextItemInFocusChain !== 'function') return false

        let current = target
        let steps = 0

        while(steps < 1000){
            current = current.nextItemInFocusChain(forward)
            if(!current || current === target) return false

            if(current.activeFocusOnTab && current.enabled && current.visible){
                return this.__activateKeyNavigationTarget(current)
            }

            steps++
        }

        return false
    },

    // Makes the DOM keyboard event look like a Qt KeyEvent for QML handlers:
    // adds 'modifiers' and converts letter/digit keys to Qt key codes (Qt.Key_A etc.)
    __prepareKeyEvent(e){
        let modifiers = QtEnums.NoModifier
        if (e.shiftKey) modifiers |= QtEnums.ShiftModifier
        if (e.ctrlKey) modifiers |= QtEnums.ControlModifier
        if (e.altKey) modifiers |= QtEnums.AltModifier
        if (e.metaKey) modifiers |= QtEnums.MetaModifier
        e.modifiers = modifiers

        if (typeof e.key !== 'string' || e.key.length !== 1) return

        let qtKey = 0
        let upperKey = e.key.toUpperCase()
        if ((upperKey >= 'A' && upperKey <= 'Z') || (upperKey >= '0' && upperKey <= '9')) {
            qtKey = upperKey.charCodeAt(0)
        } else if ((e.ctrlKey || e.metaKey || e.altKey) && /^Key[A-Z]$/.test(e.code)) {
            // Non-latin keyboard layouts: shortcuts like Ctrl+A use the physical key
            qtKey = e.code.charCodeAt(3)
        }

        if (qtKey) {
            Object.defineProperty(e, 'key', { value: qtKey, configurable: true })
        }
    },

    add: function (obj) {
        this.shortcuts.push(obj)
    },
    remove: function (obj) {
        let index = this.shortcuts.indexOf(obj)
        if (index >= 0) this.shortcuts.splice(index, 1)
    },

    init: function () {
        window.onkeydown = (e) => {
            let elements = JQApplication.focusTree
            let domKey = e.key
            this.__prepareKeyEvent(e)

            for (let obj of elements) {
                if (e.key === 'ArrowLeft') {
                    if (obj.KeyNavigation.left) {
                        e.preventDefault()
                        this.__activateKeyNavigationTarget(obj.KeyNavigation.left)
                        return
                    }
                }
                if (e.key === 'ArrowRight') {
                    if (obj.KeyNavigation.right) {
                        e.preventDefault()
                        this.__activateKeyNavigationTarget(obj.KeyNavigation.right)
                        return
                    }
                }
                if (e.key === 'ArrowUp') {
                    if (obj.KeyNavigation.up) {
                        e.preventDefault()
                        this.__activateKeyNavigationTarget(obj.KeyNavigation.up)
                        return
                    }
                }
                if (e.key === 'ArrowDown') {
                    if (obj.KeyNavigation.down) {
                        e.preventDefault()
                        this.__activateKeyNavigationTarget(obj.KeyNavigation.down)
                        return
                    }
                }
                if (e.key === 'Tab' && e.shiftKey) {
                    if (obj.KeyNavigation.backtab) {
                        e.preventDefault()
                        this.__activateKeyNavigationTarget(obj.KeyNavigation.backtab)
                        return
                    }
                } else if (e.key === 'Tab') {
                    if (obj.KeyNavigation.tab) {
                        e.preventDefault()
                        this.__activateKeyNavigationTarget(obj.KeyNavigation.tab)
                        return
                    }
                }

                e.accepted = false
                obj['Keys.pressed'](e)
                if (e.accepted) return

                if (e.key === 'Tab' && e.shiftKey) {
                    e.accepted = false
                    obj['Keys.backtabPressed'](e)
                    if (e.accepted) return
                } else if (e.key === 'Tab') {
                    e.accepted = false
                    obj['Keys.tabPressed'](e)
                    if (e.accepted) return
                }

                if (e.key === 'ArrowLeft') {
                    e.accepted = false
                    obj['Keys.leftPressed'](e)
                    if (e.accepted) return
                }
                if (e.key === 'ArrowRight') {
                    e.accepted = false
                    obj['Keys.rightPressed'](e)
                    if (e.accepted) return
                }
                if (e.key === 'ArrowUp') {
                    e.accepted = false
                    obj['Keys.upPressed'](e)
                    if (e.accepted) return
                }
                if (e.key === 'ArrowDown') {
                    e.accepted = false
                    obj['Keys.downPressed'](e)
                    if (e.accepted) return
                }
                if (e.key === 'Return') {
                    e.accepted = false
                    obj['Keys.returnPressed'](e)
                    if (e.accepted) return
                }
                if (e.key === 'Enter') {
                    e.accepted = false
                    obj['Keys.enterPressed'](e)
                    if (e.accepted) return
                }
                if (e.code === 'Space') {
                    e.accepted = false
                    obj['Keys.spacePressed'](e)
                    if (e.accepted) return
                }
                if (e.key === 'Escape') {
                    e.accepted = false
                    obj['Keys.escapePressed'](e)
                    if (e.accepted) return
                }
            }

            if (e.key === 'Tab') {
                let tabTarget = elements.length > 0 ? elements[elements.length - 1] : null
                if (this.__activateTabFocusTarget(tabTarget, !e.shiftKey)){
                    e.preventDefault()
                    return
                }
            }

            if (domKey === 'Shift' || domKey === 'Control' || domKey === 'Alt') return
            let key = domKey
            if (e.key === 'ArrowLeft') key = 'Left'
            if (e.key === 'ArrowRight') key = 'Right'
            if (e.key === 'ArrowUp') key = 'Up'
            if (e.key === 'ArrowDown') key = 'Down'
            if (e.key === 'Return') key = 'Enter'
            if (e.code === 'Space') key = 'Space'


            let currentShortcuts = []
            for (let shortcut of this.shortcuts) {
                if (shortcut.enabled) {
                    if (shortcut.sequence) {
                        let stateKeys = {
                            altKey: false,
                            ctrlKey: false,
                            shiftKey: false,
                            key: ''
                        }
                        let skeys = shortcut.sequence.split('+')
                        for (let skey of skeys) {
                            if (skey === 'Shift') {
                                stateKeys.shiftKey = true
                            } else if (skey === 'Alt') {
                                stateKeys.altKey = true
                            } else if (skey === 'Ctrl') {
                                stateKeys.ctrlKey = true
                            } else {
                                stateKeys.key = skey === 'Return' ? 'Enter' : skey
                            }
                        }
                        if (stateKeys.key === (key.length === 1 ? key.toUpperCase() : key) && stateKeys.shiftKey === e.shiftKey && stateKeys.ctrlKey === e.ctrlKey && stateKeys.altKey === e.altKey) {
                            currentShortcuts.push(shortcut)
                        }
                    } else if (shortcut.sequences.length) {
                        let i = 0
                        let find = false
                        while (i < shortcut.sequences.length && !find) {
                            let stateKeys = {
                                altKey: false,
                                ctrlKey: false,
                                shiftKey: false,
                                key: ''
                            }
                            let skeys = shortcut.sequences[i].split('+')
                            for (let skey of skeys) {
                                if (skey === 'Shift') {
                                    stateKeys.shiftKey = true
                                } else if (skey === 'Alt') {
                                    stateKeys.altKey = true
                                } else if (skey === 'Ctrl') {
                                    stateKeys.ctrlKey = true
                                } else {
                                    stateKeys.key = skey === 'Return' ? 'Enter' : skey
                                }
                            }
                            if (stateKeys.key === (key.length === 1 ? key.toUpperCase() : key) && stateKeys.shiftKey === e.shiftKey && stateKeys.ctrlKey === e.ctrlKey && stateKeys.altKey === e.altKey) {
                                currentShortcuts.push(shortcut)
                                find = true
                            }
                            i++
                        }
                    }
                }

            }
            if (currentShortcuts.length >= 1) {
                e.preventDefault()
                if (currentShortcuts.length === 1) {
                    currentShortcuts[0].activated()
                } else {
                    currentShortcuts[currentShortcuts.length - 1].activatedAmbiguously()
                    currentShortcuts.pop()
                }
                return
            }
        }
    }
}
