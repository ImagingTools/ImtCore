// Run: node --test tests/unit
// The modifiers of keys and the mouse as Qt has them: on macOS the Command key is Qt::ControlModifier and the Control
// key Qt::MetaModifier (Qt::AA_MacDontSwapCtrlAndMeta is off); elsewhere Control is Control and the Windows key Meta.
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

const ControlModifier = 0x04000000, MetaModifier = 0x10000000, ShiftModifier = 0x02000000

function onPlatform(platform, fn){
    let previous = Object.getOwnPropertyDescriptor(globalThis, 'navigator')
    Object.defineProperty(globalThis, 'navigator', { value: { platform }, configurable: true, writable: true })
    try {
        fn()
    } finally {
        if(previous) Object.defineProperty(globalThis, 'navigator', previous)
        else delete globalThis.navigator
    }
}

function cases(name, runtime){
    const { keyEvent, delivery, shortcut } = runtime

    test(name + ': on macOS Command+C is Ctrl+C, Control is Meta', () => {
        onPlatform('MacIntel', () => {
            let event = new keyEvent.KeyEvent({ key: 'c', code: 'KeyC', metaKey: true })
            assert.equal(event.modifiers, ControlModifier)
            assert.equal(event.key, 0x43)
            assert.equal(new keyEvent.KeyEvent({ key: 'c', code: 'KeyC', ctrlKey: true, shiftKey: true }).modifiers, MetaModifier | ShiftModifier)
            assert.equal(delivery.modifiersOf({ metaKey: true }), ControlModifier)
            assert.equal(delivery.modifiersOf({ ctrlKey: true }), MetaModifier)
        })
    })

    test(name + ': on other platforms Control is Ctrl, the Windows key Meta', () => {
        onPlatform('Win32', () => {
            assert.equal(new keyEvent.KeyEvent({ key: 'c', code: 'KeyC', ctrlKey: true }).modifiers, ControlModifier)
            assert.equal(new keyEvent.KeyEvent({ key: 'c', code: 'KeyC', metaKey: true }).modifiers, MetaModifier)
            assert.equal(delivery.modifiersOf({ ctrlKey: true }), ControlModifier)
        })
    })

    test(name + ': Command+S on macOS and Ctrl+S elsewhere give the same Shortcut modifiers', () => {
        let mac, windows
        onPlatform('MacIntel', () => { mac = shortcut.modsOf({ metaKey: true }) })
        onPlatform('Win32', () => { windows = shortcut.modsOf({ ctrlKey: true }) })
        assert.equal(mac, windows)
    })
}

for(let [name, load] of runtimes) cases(name, load())
