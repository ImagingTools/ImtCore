const engine = require('../Core/engine')
const mouse = require('../QtQuick/PointerDelivery')
const QtEnums = require('../Qt/enums')
const { defineType } = require('../Core/type')
const { Item } = require('../QtQuick/Item')

const NoButton = 0, LeftButton = 1

class TestFailError extends Error {}
class TestSkipError extends Error {}

// qtest_compareInternal: deep comparison of values, arrays and objects
function equal(actual, expected){
    if(actual === expected) return true
    if(typeof actual === 'number' && typeof expected === 'number') return actual !== actual && expected !== expected
    if(!actual || !expected || typeof actual !== 'object' || typeof expected !== 'object') return false
    if(actual instanceof Date && expected instanceof Date) return actual.getTime() === expected.getTime()
    if(Array.isArray(actual) !== Array.isArray(expected)) return false
    let keys = Object.keys(actual)
    if(keys.length !== Object.keys(expected).length) return false
    for(let key of keys){
        if(!equal(actual[key], expected[key])) return false
    }
    return true
}

class TestCase extends Item {
    constructor(parent = null){
        super(parent)
        this.$pressedButtons = NoButton
        this.$expectFail = null
        this.$skipAll = false
        this.$started = false
        this.$temporary = []
        engine.scheduleCompleted(this)
    }

    $complete(){
        super.$complete()
        engine.queueCall(() => this.$tryStart())
    }

    $tryStart(){
        if(this.$started || !this.$v[TestCase.WHEN] || this.$f & engine.DESTROYED) return
        this.$started = true
        this.$run()
    }

    $testNames(){
        let names = new Set()
        for(let proto = Object.getPrototypeOf(this); proto && proto !== TestCase.prototype; proto = Object.getPrototypeOf(proto)){
            for(let key of Object.getOwnPropertyNames(proto)){
                if(key.startsWith('test_') && !key.endsWith('_data') && typeof this[key] === 'function') names.add(key)
            }
        }
        return Array.from(names).sort()
    }

    async $call(name){
        if(typeof this[name] !== 'function') return
        let result = this[name]()
        if(result && typeof result.then === 'function') await result
    }

    async $run(){
        this.running = true
        try {
            await this.$call('initTestCase')
            for(let name of this.$testNames()){
                if(this.$skipAll || engine.isStopped()) break
                await this.$runTest(name)
            }
        } catch(error) {
            this.$report(this.$v[TestCase.NAME] + '::initTestCase', error)
        } finally {
            try {
                await this.$call('cleanupTestCase')
            } catch(error) {
                this.$report(this.$v[TestCase.NAME] + '::cleanupTestCase', error)
            }
            this.running = false
            this.completed = true
        }
    }

    async $runTest(name){
        this.$expectFail = null
        try {
            await this.$call('init')
            await this.$call(name)
            if(this.$expectFail) throw new TestFailError(this.$expectFail.message || 'expectFail() was set, but no failure happened')
        } catch(error) {
            this.$report(name, error)
        } finally {
            this.$expectFail = null
            this.$destroyTemporary()
            try {
                await this.$call('cleanup')
            } catch(error) {
                this.$report(name + ' cleanup', error)
            }
        }
    }

    $report(name, error){
        if(error instanceof TestSkipError) console.log('SKIP   : ' + name + ' ' + error.message)
        else console.error('FAIL!  : ' + name + ' ' + (error && error.message !== undefined ? error.message : error))
    }

    $failed(message){
        if(this.$expectFail){
            let state = this.$expectFail
            this.$expectFail = null
            if(state.abort) throw new TestSkipError(message)
            return false
        }
        throw new TestFailError(message)
    }

    $destroyTemporary(){
        for(let obj of this.$temporary){
            if(obj && !(obj.$f & engine.DESTROYED)) obj.destroy()
        }
        this.$temporary = []
    }

    // ---- assertions ----

    fail(message){
        return this.$failed(message === undefined ? '' : String(message))
    }

    verify(condition, message){
        if(!condition) return this.$failed(message === undefined ? '' : String(message))
        return true
    }

    compare(actual, expected, message){
        if(!equal(actual, expected)){
            return this.$failed((message === undefined ? 'Compared values are not the same' : String(message)) +
                '\n   Actual   (): ' + actual + '\n   Expected (): ' + expected)
        }
        return true
    }

    fuzzyCompare(actual, expected, delta, message){
        if(delta === undefined) return this.$failed('A delta value is required for fuzzyCompare')
        if(Math.abs(actual - expected) > delta) return this.$failed(message === undefined ? 'Compared values are not the same with delta(' + delta + ')' : String(message))
        return true
    }

    skip(message){
        throw new TestSkipError(message === undefined ? '' : String(message))
    }

    expectFail(tag, message){
        this.$expectFail = { tag, message, abort: true }
    }

    expectFailContinue(tag, message){
        this.$expectFail = { tag, message, abort: false }
    }

    warn(message){
        console.warn('WARNING: ' + message)
    }

    ignoreWarning(){}
    failOnWarning(){}

    // "Waits for ms milliseconds while processing Qt events": the web cannot block, the caller awaits
    wait(ms){
        return new Promise(resolve => setTimeout(resolve, Math.max(0, Number(ms) || 0)))
    }

    sleep(ms){
        let end = Date.now() + (Number(ms) || 0)
        while(Date.now() < end){}
    }

    async tryVerify(fn, timeout = 5000, message){
        let deadline = Date.now() + timeout
        for(;;){
            if(fn()) return true
            if(Date.now() >= deadline) return this.$failed(message === undefined ? 'function returned false' : String(message))
            await this.wait(50)
        }
    }

    async tryCompare(obj, prop, expected, timeout = 5000, message){
        let deadline = Date.now() + timeout
        for(;;){
            if(equal(obj[prop], expected)) return true
            if(Date.now() >= deadline){
                return this.$failed((message === undefined ? 'property ' + prop : String(message)) +
                    '\n   Actual   (): ' + obj[prop] + '\n   Expected (): ' + expected)
            }
            await this.wait(50)
        }
    }

    findChild(parent, objectName){
        if(!parent || !parent.$children) return null
        for(let child of parent.$children){
            if(child.$v[0] === objectName) return child
        }
        for(let child of parent.$children){
            let found = this.findChild(child, objectName)
            if(found) return found
        }
        return null
    }

    createTemporaryObject(component, parent, properties){
        if(!component || typeof component.createObject !== 'function') return null
        let obj = component.createObject(parent === undefined ? null : parent, properties || null)
        if(obj) this.$temporary.push(obj)
        return obj
    }

    isPolishScheduled(item){
        return !!(item && item.$polishQueued)
    }

    waitForPolish(){
        engine.flushFrame()
        return true
    }

    waitForItemPolished(){
        return this.waitForPolish()
    }

    // ---- mouse (quicktestevent.cpp) ----

    $scene(item, x, y){
        if(!(item instanceof Item)) return this.$failed('mouse event target should be an Item')
        if(x === undefined) x = item.width / 2
        if(y === undefined) y = item.height / 2
        let pos = mouse.scenePos(item)
        return { x: pos.x + x, y: pos.y + y }
    }

    mousePress(item, x, y, button = LeftButton, modifiers = 0){
        let p = this.$scene(item, x, y)
        this.$pressedButtons |= button
        mouse.press(p.x, p.y, button, this.$pressedButtons, modifiers, false)
    }

    mouseRelease(item, x, y, button = LeftButton, modifiers = 0){
        let p = this.$scene(item, x, y)
        this.$pressedButtons &= ~button
        mouse.release(p.x, p.y, button, this.$pressedButtons, modifiers)
    }

    mouseMove(item, x, y, delay, buttons = NoButton, modifiers = 0){
        let p = this.$scene(item, x, y)
        mouse.move(p.x, p.y, buttons || this.$pressedButtons, modifiers)
    }

    mouseClick(item, x, y, button = LeftButton, modifiers = 0){
        let p = this.$scene(item, x, y)
        // a native control is focused by the browser on a real press
        if(item.$input) item.$input.focus()
        mouse.press(p.x, p.y, button, button | this.$pressedButtons, modifiers, false)
        mouse.release(p.x, p.y, button, this.$pressedButtons, modifiers)
    }

    mouseDoubleClickSequence(item, x, y, button = LeftButton, modifiers = 0){
        let p = this.$scene(item, x, y)
        mouse.press(p.x, p.y, button, button, modifiers, false)
        mouse.release(p.x, p.y, button, NoButton, modifiers)
        mouse.press(p.x, p.y, button, button, modifiers, true)
        mouse.release(p.x, p.y, button, NoButton, modifiers)
    }

    // deprecated in Qt 5: a lone MouseButtonDblClick event
    mouseDoubleClick(item, x, y, button = LeftButton, modifiers = 0){
        let p = this.$scene(item, x, y)
        let grabber = mouse.scene.grabber
        if(grabber && grabber.$mouseDoubleClick){
            let pos = mouse.scenePos(grabber)
            engine.batch(() => grabber.$mouseDoubleClick(p.x - pos.x, p.y - pos.y, button, button, modifiers))
        }
    }

    mouseDrag(item, x, y, dx, dy, button = LeftButton, modifiers = 0){
        let threshold = 10
        let stepX = Math.abs(Math.round(dx / 3)) < threshold + 1 ? 0 : Math.round(dx / 3)
        let stepY = Math.abs(Math.round(dy / 3)) < threshold + 1 ? 0 : Math.round(dy / 3)
        this.mousePress(item, x, y, button, modifiers)
        this.mouseMove(item, x + (dx > 0 ? threshold + 1 : 0), y + (dy > 0 ? threshold + 1 : 0), -1, button, modifiers)
        if(stepX || stepY){
            this.mouseMove(item, x + stepX, y + stepY, -1, button, modifiers)
            this.mouseMove(item, x + 2 * stepX, y + 2 * stepY, -1, button, modifiers)
        }
        this.mouseMove(item, x + dx, y + dy, -1, button, modifiers)
        this.mouseRelease(item, x + dx, y + dy, button, modifiers)
    }

    mouseWheel(item, x, y, xDelta = 0, yDelta = 0, buttons = NoButton, modifiers = 0){
        let p = this.$scene(item, x, y)
        mouse.wheel(p.x, p.y, { x: xDelta, y: yDelta }, { x: 0, y: 0 }, buttons, modifiers)
    }

    // ---- keyboard ----

    $key(type, key, modifiers = 0){
        if(typeof window === 'undefined') return
        // Qt key enums are DOM key names; letter and digit codes become characters, Backtab is Shift+Tab
        let domKey = String(key)
        if(typeof key === 'number' && ((key >= 0x30 && key <= 0x39) || (key >= 0x41 && key <= 0x5a))){
            domKey = String.fromCharCode(key)
            if(!(modifiers & QtEnums.ShiftModifier)) domKey = domKey.toLowerCase()
        }
        if(key === QtEnums.Key_Backtab){
            domKey = 'Tab'
            modifiers |= QtEnums.ShiftModifier
        }
        let event = new KeyboardEvent(type, {
            bubbles: true,
            cancelable: true,
            key: domKey,
            code: domKey,
            shiftKey: !!(modifiers & QtEnums.ShiftModifier),
            ctrlKey: !!(modifiers & QtEnums.ControlModifier),
            altKey: !!(modifiers & QtEnums.AltModifier),
            metaKey: !!(modifiers & QtEnums.MetaModifier),
        })
        let target = document.activeElement && document.activeElement !== document.body ? document.activeElement : window
        target.dispatchEvent(event)
    }

    keyPress(key, modifiers){ this.$key('keydown', key, modifiers) }
    keyRelease(key, modifiers){ this.$key('keyup', key, modifiers) }

    keyClick(key, modifiers){
        let input = document.activeElement
        if(!modifiers && input && (input.tagName === 'TEXTAREA' || input.tagName === 'INPUT')){
            let ch = typeof key === 'string' && key.length === 1 ? key : key === QtEnums.Key_Return && input.tagName === 'TEXTAREA' ? '\n' : null
            if(ch){
                this.$typeChar(input, ch)
                return
            }
        }
        this.$key('keydown', key, modifiers)
        this.$moveCaret(input, key, modifiers)
        this.$key('keyup', key, modifiers)
    }

    // what the browser does itself on a real key press
    $moveCaret(input, key, modifiers){
        if(modifiers || !input || (input.tagName !== 'TEXTAREA' && input.tagName !== 'INPUT')) return
        let at = input.selectionEnd
        let to = key === QtEnums.Key_Left ? at - 1 : key === QtEnums.Key_Right ? at + 1 : key === QtEnums.Key_Home ? 0 : key === QtEnums.Key_End ? input.value.length : null
        if(to === null) return
        to = Math.max(0, Math.min(input.value.length, to))
        input.setSelectionRange(to, to)
        input.dispatchEvent(new Event('selectionchange'))
    }

    $typeChar(input, ch){
        // the text of the item reaches the native control with the frame: what the program just set comes first
        let current = String(input.$item.text)
        if(input.value !== current){
            input.value = current
            input.setSelectionRange(current.length, current.length)
        }
        this.$key('keydown', ch === '\n' ? QtEnums.Key_Enter : ch)
        let start = input.selectionStart === null ? input.value.length : input.selectionStart
        let end = input.selectionEnd === null ? start : input.selectionEnd
        input.value = input.value.slice(0, start) + ch + input.value.slice(end)
        input.setSelectionRange(start + 1, start + 1)
        input.dispatchEvent(new Event('input', { bubbles: true }))
        this.$key('keyup', ch === '\n' ? QtEnums.Key_Enter : ch)
    }

    keySequence(sequence){
        for(let part of String(sequence).split(',')){
            let keys = part.trim().split('+')
            let modifiers = 0
            for(let i = 0; i < keys.length - 1; i++){
                let name = keys[i].toLowerCase()
                if(name === 'ctrl') modifiers |= QtEnums.ControlModifier
                else if(name === 'shift') modifiers |= QtEnums.ShiftModifier
                else if(name === 'alt') modifiers |= QtEnums.AltModifier
                else if(name === 'meta') modifiers |= QtEnums.MetaModifier
            }
            this.keyClick(keys[keys.length - 1], modifiers)
        }
    }

    // JQML extensions: typing into the native control of a TextInput / TextEdit
    typeText(item, text){
        let input = item && item.$input
        if(!input) return false
        input.focus()
        for(let ch of String(text)){
            this.$typeChar(input, ch)
        }
        return true
    }

    clearText(item){
        let input = item && item.$input
        if(!input) return false
        input.focus()
        input.value = ''
        input.dispatchEvent(new Event('input', { bubbles: true }))
        return true
    }
}

defineType(TestCase, {
    props: {
        name: 'string',
        when: { kind: 'bool', initial: true, effect(test, value){ if(value && test.$f & engine.COMPLETED) test.$tryStart() } },
        optional: 'bool',
        completed: 'bool',
        running: 'bool',
        windowShown: { kind: 'bool', initial: true },
    },
})

module.exports = { TestFailError, TestCase }
