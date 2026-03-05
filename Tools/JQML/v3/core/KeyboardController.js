const QtEnums = require("../Qt/enums")

module.exports = {
    shortcuts: [],

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

            for (let obj of elements) {
                if (e.key === 'ArrowLeft') {
                    if (obj.KeyNavigation.left) {
                        e.preventDefault()
                        obj.KeyNavigation.left.focus = true
                        return
                    }
                }
                if (e.key === 'ArrowRight') {
                    if (obj.KeyNavigation.right) {
                        e.preventDefault()
                        obj.KeyNavigation.right.focus = true
                        return
                    }
                }
                if (e.key === 'ArrowUp') {
                    if (obj.KeyNavigation.up) {
                        e.preventDefault()
                        obj.KeyNavigation.up.focus = true
                        return
                    }
                }
                if (e.key === 'ArrowDown') {
                    if (obj.KeyNavigation.down) {
                        e.preventDefault()
                        obj.KeyNavigation.down.focus = true
                        return
                    }
                }
                if (e.key === 'Tab' && e.shiftKey) {
                    if (obj.KeyNavigation.backtab) {
                        e.preventDefault()
                        obj.KeyNavigation.backtab.focus = true
                        return
                    }
                } else if (e.key === 'Tab') {
                    if (obj.KeyNavigation.tab) {
                        e.preventDefault()
                        obj.KeyNavigation.tab.focus = true
                        return
                    }
                }

                e.accepted = false
                obj['Keys.pressed'](e)
                if (e.accepted) return

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
                if (e.key === 'Space') {
                    e.accepted = false
                    obj['Keys.spacePressed'](e)
                    if (e.accepted) return
                }
            }

            if (e.key === 'Shift' || e.key === 'Control' || e.key === 'Alt') return
            let key = e.key
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
