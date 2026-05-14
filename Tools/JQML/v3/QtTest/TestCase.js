const Item = require("../QtQuick/Item")
const Bool = require("../QtQml/Bool")
const Int = require("../QtQml/Int")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")
const QtEnums = require("../Qt/enums")

class TestFailError extends Error {}
class TestSkipError extends Error {}

class TestCase extends Item {
    static Continue = 0
    static Abort = 1

    static SkipSingle = 0
    static SkipAll = 1

    static meta = Object.assign({}, Item.meta, {
        name: { type: String, value: '' },
        when: { type: Bool, value: true },
        optional: { type: Bool, value: false },
        running: { type: Bool, value: false },
        passed: { type: Int, value: 0 },
        failed: { type: Int, value: 0 },
        skipped: { type: Int, value: 0 },

        nameChanged: { type: Signal, args:[] },
        whenChanged: { type: Signal, args:[] },
        optionalChanged: { type: Signal, args:[] },
        runningChanged: { type: Signal, args:[] },
        passedChanged: { type: Signal, args:[] },
        failedChanged: { type: Signal, args:[] },
        skippedChanged: { type: Signal, args:[] },

        started: { type: Signal, args:[] },
        finished: { type: Signal, args:[] },
        testStarted: { type: Signal, args:['name'] },
        testFinished: { type: Signal, args:['name', 'ok'] },
        testFailed: { type: Signal, args:['name', 'message'] },
        testSkipped: { type: Signal, args:['name', 'message'] },
    })

    __started = false
    __skipAll = false
    __expectFailState = null

    static create(parent=null, properties = {}, context = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('TestCase')

        return obj
    }

    'SLOT_Component.completed'(){
        this.__tryStart()
    }

    SLOT_whenChanged(oldValue, newValue){
        this.__tryStart()
    }

    __tryStart(){
        if(this.__started || !this.when) return
        this.__started = true
        this.__runAllTests()
    }

    async __runAllTests(){
        this.running = true
        this.passed = 0
        this.failed = 0
        this.skipped = 0
        this.__skipAll = false
        this.__expectFailState = null

        this.started()

        try {
            await this.__callIfExists('initTestCase')

            let testNames = this.__collectTestNames()
            for(let i = 0; i < testNames.length; i++){
                if(this.__skipAll) break
                await this.__runSingleTest(testNames[i])
            }
        } finally {
            try {
                await this.__callIfExists('cleanupTestCase')
            } catch(err) {
                this.failed += 1
                console.error(`[TestCase] cleanupTestCase failed: ${this.__errorMessage(err)}`)
            }

            this.running = false
            this.finished()
        }
    }

    __collectTestNames(){
        let names = new Set()
        let proto = this

        while(proto && proto !== Object.prototype){
            let keys = Object.getOwnPropertyNames(proto)
            for(let i = 0; i < keys.length; i++){
                let key = keys[i]
                if(key.startsWith('test_') && typeof this[key] === 'function'){
                    names.add(key)
                }
            }
            proto = Object.getPrototypeOf(proto)
        }

        return Array.from(names).sort()
    }

    async __runSingleTest(testName){
        let ok = true
        this.__expectFailState = null
        this.testStarted(testName)

        try {
            await this.__callIfExists('init')
            await this.__callIfExists(testName)

            if(this.__expectFailState){
                throw new TestFailError(this.__expectFailState.message || 'expectFail() was set, but no failure happened')
            }

            this.passed += 1
        } catch(err) {
            if(err instanceof TestSkipError){
                this.skipped += 1
                this.testSkipped(testName, this.__errorMessage(err))
            } else {
                ok = false
                this.failed += 1
                let message = this.__errorMessage(err)
                this.testFailed(testName, message)
                console.error(`[TestCase] FAIL ${testName}: ${message}`)
            }
        } finally {
            this.__expectFailState = null

            try {
                await this.__callIfExists('cleanup')
            } catch(err) {
                ok = false
                this.failed += 1
                let message = `cleanup failed: ${this.__errorMessage(err)}`
                this.testFailed(testName, message)
                console.error(`[TestCase] FAIL ${testName}: ${message}`)
            }

            this.testFinished(testName, ok)
        }
    }

    async __callIfExists(methodName){
        let fn = this[methodName]
        if(typeof fn !== 'function') return

        let result = fn.call(this)
        if(result && typeof result.then === 'function'){
            await result
        }
    }

    __errorMessage(err){
        if(err && err.message !== undefined) return err.message
        return String(err)
    }

    __assertFailed(message){
        if(this.__expectFailState){
            let mode = this.__expectFailState.mode
            this.__expectFailState = null

            if(mode === TestCase.Abort){
                throw new TestSkipError(message || 'Expected failure (aborted)')
            }

            return false
        }

        throw new TestFailError(message || 'Assertion failed')
    }

    fail(message = ''){
        this.__assertFailed(message || 'fail() called')
        return false
    }

    verify(statement, message = ''){
        if(!statement){
            this.__assertFailed(message || 'verify() failed')
            return false
        }
        return true
    }

    compare(actual, expected, message = ''){
        if(actual !== expected){
            let details = `compare() failed: actual=${actual}, expected=${expected}`
            this.__assertFailed(message || details)
            return false
        }
        return true
    }

    fuzzyCompare(actual, expected, epsilon = 0.00001, message = ''){
        if(typeof actual !== 'number' || typeof expected !== 'number'){
            this.__assertFailed(message || 'fuzzyCompare() accepts only numbers')
            return false
        }

        if(Math.abs(actual - expected) > Math.abs(epsilon)){
            let details = `fuzzyCompare() failed: actual=${actual}, expected=${expected}, epsilon=${epsilon}`
            this.__assertFailed(message || details)
            return false
        }

        return true
    }

    skip(message = '', mode = TestCase.SkipSingle){
        if(mode === TestCase.SkipAll){
            this.__skipAll = true
        }

        throw new TestSkipError(message || 'Skipped')
    }

    expectFail(dataTag = '', message = '', mode = TestCase.Continue){
        this.__expectFailState = {
            dataTag,
            message,
            mode,
        }
    }

    wait(timeout = 0){
        let ms = Math.max(0, Number(timeout) || 0)
        return new Promise((resolve)=>{
            setTimeout(resolve, ms)
        })
    }

    async tryVerify(callback, timeout = 5000, message = ''){
        if(typeof callback !== 'function'){
            this.__assertFailed(message || 'tryVerify() expects function callback')
            return false
        }

        let deadline = Date.now() + Math.max(0, Number(timeout) || 0)
        while(Date.now() <= deadline){
            let result = false
            try {
                result = !!callback()
            } catch(err) {}

            if(result) return true
            await this.wait(10)
        }

        this.__assertFailed(message || 'tryVerify() timeout')
        return false
    }

    async tryCompare(obj, propertyName, expected, timeout = 5000, message = ''){
        let deadline = Date.now() + Math.max(0, Number(timeout) || 0)
        while(Date.now() <= deadline){
            let current = undefined
            try {
                current = obj ? obj[propertyName] : undefined
            } catch(err) {}

            if(current === expected) return true
            await this.wait(10)
        }

        let details = `tryCompare() timeout: ${propertyName}`
        this.__assertFailed(message || details)
        return false
    }

    __mouseDomButton(button = QtEnums.LeftButton){
        if(button === QtEnums.RightButton) return 2
        if(button === QtEnums.MiddleButton) return 1
        return 0
    }

    __mouseDomButtons(buttons = QtEnums.NoButton){
        let value = Number(buttons)
        return Number.isFinite(value) ? value : QtEnums.NoButton
    }

    __mousePoint(item, x, y){
        if(!item || typeof item.__getDOM !== 'function'){
            this.__assertFailed('mouse event target should be an Item-like object')
            return null
        }

        let dom = item.__getDOM()
        if(!dom || typeof dom.getBoundingClientRect !== 'function'){
            this.__assertFailed('mouse event target has no DOM representation')
            return null
        }

        let rect = dom.getBoundingClientRect()
        let localX = x === undefined || x === null ? rect.width / 2 : Number(x)
        let localY = y === undefined || y === null ? rect.height / 2 : Number(y)

        if(!Number.isFinite(localX) || !Number.isFinite(localY)){
            this.__assertFailed('mouse coordinates should be finite numbers')
            return null
        }

        let pageX = rect.left + localX + (window.scrollX || 0)
        let pageY = rect.top + localY + (window.scrollY || 0)

        return {
            pageX,
            pageY,
        }
    }

    __dispatchMouseEvent(type, point, button = QtEnums.LeftButton, buttons = QtEnums.NoButton, modifiers = QtEnums.NoModifier){
        if(typeof window === 'undefined' || typeof window.dispatchEvent !== 'function') return false

        let shiftKey = !!(modifiers & QtEnums.ShiftModifier)
        let ctrlKey = !!(modifiers & QtEnums.ControlModifier)
        let altKey = !!(modifiers & QtEnums.AltModifier)
        let metaKey = !!(modifiers & QtEnums.MetaModifier)

        let event = new MouseEvent(type, {
            bubbles: true,
            cancelable: true,
            clientX: point.pageX - (window.scrollX || 0),
            clientY: point.pageY - (window.scrollY || 0),
            screenX: point.pageX,
            screenY: point.pageY,
            button: this.__mouseDomButton(button),
            buttons: this.__mouseDomButtons(buttons),
            shiftKey,
            ctrlKey,
            altKey,
            metaKey,
        })

        window.dispatchEvent(event)
        return true
    }

    mouseMove(item, x, y, delay = 0, modifiers = QtEnums.NoModifier){
        let point = this.__mousePoint(item, x, y)
        if(!point) return false

        if(delay > 0) {
            console.warn('[TestCase] mouseMove delay is ignored in web runtime')
        }

        return this.__dispatchMouseEvent('mousemove', point, QtEnums.NoButton, QtEnums.NoButton, modifiers)
    }

    mousePress(item, x, y, button = QtEnums.LeftButton, modifiers = QtEnums.NoModifier, delay = 0){
        let point = this.__mousePoint(item, x, y)
        if(!point) return false

        if(delay > 0) {
            console.warn('[TestCase] mousePress delay is ignored in web runtime')
        }

        this.__dispatchMouseEvent('mousemove', point, button, button, modifiers)
        return this.__dispatchMouseEvent('mousedown', point, button, button, modifiers)
    }

    mouseRelease(item, x, y, button = QtEnums.LeftButton, modifiers = QtEnums.NoModifier, delay = 0){
        let point = this.__mousePoint(item, x, y)
        if(!point) return false

        if(delay > 0) {
            console.warn('[TestCase] mouseRelease delay is ignored in web runtime')
        }

        this.__dispatchMouseEvent('mousemove', point, button, button, modifiers)
        return this.__dispatchMouseEvent('mouseup', point, button, QtEnums.NoButton, modifiers)
    }

    mouseClick(item, x, y, button = QtEnums.LeftButton, modifiers = QtEnums.NoModifier, delay = 0){
        let point = this.__mousePoint(item, x, y)
        if(!point) return false

        if(delay > 0) {
            console.warn('[TestCase] mouseClick delay is ignored in web runtime')
        }

        this.__dispatchMouseEvent('mousemove', point, button, button, modifiers)
        this.__dispatchMouseEvent('mousedown', point, button, button, modifiers)
        this.__dispatchMouseEvent('mouseup', point, button, QtEnums.NoButton, modifiers)
        return this.__dispatchMouseEvent('click', point, button, QtEnums.NoButton, modifiers)
    }

    mouseWheel(item, x, y, dx = 0, dy = 0, modifiers = QtEnums.NoModifier){
        let point = this.__mousePoint(item, x, y)
        if(!point) return false

        if(typeof window === 'undefined' || typeof window.dispatchEvent !== 'function') return false

        let shiftKey = !!(modifiers & QtEnums.ShiftModifier)
        let ctrlKey = !!(modifiers & QtEnums.ControlModifier)
        let altKey = !!(modifiers & QtEnums.AltModifier)
        let metaKey = !!(modifiers & QtEnums.MetaModifier)

        let event = new WheelEvent('wheel', {
            bubbles: true,
            cancelable: true,
            clientX: point.pageX - (window.scrollX || 0),
            clientY: point.pageY - (window.scrollY || 0),
            screenX: point.pageX,
            screenY: point.pageY,
            deltaX: (dx || 0) * 8,
            deltaY: (dy || 0) * 8,
            deltaMode: 0,
            shiftKey,
            ctrlKey,
            altKey,
            metaKey,
        })

        window.dispatchEvent(event)
        return true
    }

    mouseDoubleClick(item, x, y, button = QtEnums.LeftButton, modifiers = QtEnums.NoModifier, delay = 0){
        let point = this.__mousePoint(item, x, y)
        if(!point) return false

        if(delay > 0) {
            console.warn('[TestCase] mouseDoubleClick delay is ignored in web runtime')
        }

        this.__dispatchMouseEvent('mousemove', point, button, button, modifiers)

        this.__dispatchMouseEvent('mousedown', point, button, button, modifiers)
        this.__dispatchMouseEvent('mouseup', point, button, QtEnums.NoButton, modifiers)
        this.__dispatchMouseEvent('click', point, button, QtEnums.NoButton, modifiers)

        this.__dispatchMouseEvent('mousedown', point, button, button, modifiers)
        this.__dispatchMouseEvent('mouseup', point, button, QtEnums.NoButton, modifiers)
        this.__dispatchMouseEvent('click', point, button, QtEnums.NoButton, modifiers)

        return this.__dispatchMouseEvent('dblclick', point, button, QtEnums.NoButton, modifiers)
    }

}



module.exports = TestCase