// Run: node --test tests/unit
// The Qt object of QML (qqmlbuiltinfunctions.cpp, qnamespace.h): the enum values, the keys and cursors as Qt numbers, the
// colors and value types, and the objects of the Qt global. What only Qt can tell is compared with Qt itself in
// tests/diff/cases/qt_namespace_*.qml and qt_color_math.qml; these cover what depends on the browser.
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

function onPlatform(platform, fn){
    let previous = Object.getOwnPropertyDescriptor(globalThis, 'navigator')
    Object.defineProperty(globalThis, 'navigator', { value: { platform, language: 'de-AT' }, configurable: true, writable: true })
    try {
        fn()
    } finally {
        if(previous) Object.defineProperty(globalThis, 'navigator', previous)
        else delete globalThis.navigator
    }
}

function cases(name, runtime){
    const { Qt, keyEvent, engine, delivery, MouseArea, Item } = runtime

    // ---- enums ----

    test(name + ': the enums have the numbers of Qt (orientation, focus policy, shortcut context, roles)', () => {
        assert.equal(Qt.Horizontal, 1)
        assert.equal(Qt.Vertical, 2)
        assert.equal(Qt.StrongFocus, 11)
        assert.equal(Qt.WheelFocus, 15)
        assert.equal(Qt.WindowShortcut, 1)
        assert.equal(Qt.ApplicationShortcut, 2)
        assert.equal(Qt.DisplayRole, 0)
        assert.equal(Qt.EditRole, 2)
        assert.equal(Qt.UserRole, 0x100)
        assert.equal(Qt.AscendingOrder, 0)
        assert.equal(Qt.ElideRight, 1)
        assert.equal(Qt.ColorScheme.Dark, 2)
        assert.equal(Qt.Key_Escape, 0x01000000)
        assert.equal(Qt.Key_Return, 0x01000004)
        assert.equal(Qt.Key_Enter, 0x01000005)
        assert.equal(Qt.ArrowCursor, 0)
        assert.equal(Qt.PointingHandCursor, 13)
        // a ListView takes Qt.Horizontal: ListView.Horizontal is Qt::Horizontal
        assert.equal(runtime.types.ListView.Horizontal, Qt.Horizontal)
        assert.equal(runtime.types.ListView.Vertical, Qt.Vertical)
    })

    // ---- keys ----

    test(name + ': a DOM key event gives the Qt key: named keys, characters, keypad Enter, an unknown key', () => {
        let key = (init) => new keyEvent.KeyEvent(init).key
        assert.equal(key({ key: 'Escape' }), Qt.Key_Escape)
        assert.equal(key({ key: 'Enter' }), Qt.Key_Return)
        assert.equal(key({ key: 'Enter', location: 3 }), Qt.Key_Enter)
        assert.equal(key({ key: 'ArrowLeft' }), Qt.Key_Left)
        assert.equal(key({ key: 'PageDown' }), Qt.Key_PageDown)
        assert.equal(key({ key: 'F12' }), Qt.Key_F12)
        assert.equal(key({ key: 'Tab', shiftKey: true }), Qt.Key_Backtab)
        assert.equal(key({ key: ' ' }), Qt.Key_Space)
        assert.equal(key({ key: 'a', code: 'KeyA' }), Qt.Key_A)
        // the physical key in another layout: Ctrl+A is Key_A
        assert.equal(key({ key: 'ф', code: 'KeyA' }), Qt.Key_A)
        assert.equal(key({ key: '7', code: 'Digit7' }), Qt.Key_7)
        assert.equal(key({ key: ',', code: 'Comma' }), Qt.Key_Comma)
        assert.equal(key({ key: 'ä', code: 'Quote' }), 0xc4)
        assert.equal(key({ key: 'AudioVolumeUp' }), Qt.Key_VolumeUp)
        assert.equal(key({ key: 'SomethingNew' }), Qt.Key_unknown)
        // Qt numbers compare as Qt code compares them
        assert.ok(key({ key: 'F5' }) >= Qt.Key_F1 && key({ key: 'F5' }) <= Qt.Key_F35)
        assert.equal(typeof key({ key: 'Escape' }), 'number')
    })

    test(name + ': on macOS the Control key is Key_Meta and Command is Key_Control, as the modifiers', () => {
        onPlatform('MacIntel', () => {
            assert.equal(new keyEvent.KeyEvent({ key: 'Control' }).key, Qt.Key_Meta)
            assert.equal(new keyEvent.KeyEvent({ key: 'Meta' }).key, Qt.Key_Control)
        })
        onPlatform('Win32', () => {
            assert.equal(new keyEvent.KeyEvent({ key: 'Control' }).key, Qt.Key_Control)
            assert.equal(new keyEvent.KeyEvent({ key: 'Meta' }).key, Qt.Key_Meta)
        })
    })

    test(name + ': a Qt key gives the DOM key a synthetic event carries, and back', () => {
        for(let k of ['Key_Escape', 'Key_Tab', 'Key_Backspace', 'Key_Return', 'Key_Delete', 'Key_Home', 'Key_End', 'Key_Left',
            'Key_Up', 'Key_Right', 'Key_Down', 'Key_PageUp', 'Key_PageDown', 'Key_F1', 'Key_F35', 'Key_Shift', 'Key_Alt']){
            let dom = keyEvent.domKey(Qt[k])
            assert.ok(dom, k)
            onPlatform('Win32', () => assert.equal(new keyEvent.KeyEvent({ key: dom }).key, Qt[k], k))
        }
        assert.equal(keyEvent.domKey(Qt.Key_Enter), 'Enter')
    })

    test(name + ': KeyEvent.matches compares the event with the bindings of a StandardKey', () => {
        let undo = new keyEvent.KeyEvent({ key: 'z', code: 'KeyZ', ctrlKey: true })
        onPlatform('Win32', () => {
            let event = new keyEvent.KeyEvent({ key: 'z', code: 'KeyZ', ctrlKey: true })
            assert.equal(event.matches(runtime.types.StandardKey.Undo), true)
            assert.equal(event.matches(runtime.types.StandardKey.Copy), false)
            let escape = new keyEvent.KeyEvent({ key: 'Escape' })
            assert.equal(escape.matches(runtime.types.StandardKey.Cancel), true)
        })
        assert.ok(undo)
    })

    // ---- cursors ----

    test(name + ': a cursor shape is a Qt number, shown as the CSS cursor that looks like it', () => {
        let element = { style: { cursor: '' } }
        let root = new Item(null)
        engine.batch(() => { root.width = 400; root.height = 300 })
        let area
        engine.batch(() => {
            area = new MouseArea(root)
            area.width = 400
            area.height = 300
            area.hoverEnabled = true
        })
        assert.equal(area.cursorShape, Qt.ArrowCursor)
        let shown = (shape) => {
            engine.batch(() => { area.cursorShape = shape })
            delivery.scene.element = element
            delivery.scene.roots = [root]
            delivery.move(10, 10, 0, 0)
            delivery.deliverHover()
            return element.style.cursor
        }
        assert.equal(shown(Qt.PointingHandCursor), 'pointer')
        assert.equal(shown(Qt.SizeHorCursor), 'ew-resize')
        assert.equal(shown(Qt.SizeVerCursor), 'ns-resize')
        assert.equal(shown(Qt.IBeamCursor), 'text')
        assert.equal(shown(Qt.ForbiddenCursor), 'not-allowed')
        assert.equal(shown(Qt.BlankCursor), 'none')
        assert.equal(shown(Qt.ArrowCursor), 'default')
        delivery.scene.roots = []
        delivery.scene.element = null
    })

    // ---- the objects of the Qt global ----

    test(name + ': Qt.application, Qt.styleHints and Qt.inputMethod read as Qt reads them without the platform features', () => {
        assert.equal(Qt.application.layoutDirection, Qt.LeftToRight)
        assert.equal(Qt.application.supportsMultipleWindows, false)
        assert.ok(Array.isArray(Qt.application.arguments) || Qt.application.arguments === undefined)
        engine.batch(() => Qt.application.setName('ProLife'))
        assert.equal(Qt.application.name, 'ProLife')
        assert.equal(Qt.styleHints.mouseDoubleClickInterval, 400)
        assert.equal(Qt.styleHints.startDragDistance, 10)
        assert.equal(Qt.styleHints.mousePressAndHoldInterval, 800)
        assert.equal(Qt.styleHints.wheelScrollLines, 3)
        assert.equal(Qt.styleHints.passwordMaskCharacter, '●')
        assert.equal(Qt.inputMethod.visible, false)
        assert.equal(String(Qt.inputMethod.keyboardRectangle), 'QRectF(0, 0, 0, 0)')
        engine.batch(() => Qt.inputMethod.show())
        assert.equal(Qt.inputMethod.visible, true)
        engine.batch(() => Qt.inputMethod.hide())
        assert.equal(Qt.inputMethod.visible, false)
        assert.equal(Qt.platform.os, 'web')
        assert.equal(Qt.platform.pluginName, 'web')
        assert.equal(Qt.isQtObject(Qt.application), true)
        assert.equal(Qt.isQtObject(Qt), true)
        assert.equal(Qt.isQtObject({}), false)
    })

    test(name + ': a binding on Qt.uiLanguage follows it; uiLanguageChanged is a signal', () => {
        let item = new Item(null)
        let changes = 0
        Qt.uiLanguageChanged.connect(() => changes++)
        engine.batch(() => item.$bind(Item.OBJECTNAME, () => 'lang:' + Qt.uiLanguage))
        engine.batch(() => { Qt.uiLanguage = 'de' })
        assert.equal(item.objectName, 'lang:de')
        engine.batch(() => { Qt.uiLanguage = 'fr' })
        assert.equal(item.objectName, 'lang:fr')
        assert.equal(changes, 2)
    })

    // ---- functions ----

    test(name + ': Qt.include tells it is not supported; Qt.exit ends the page with its code', () => {
        let warnings = []
        let warn = console.warn
        console.warn = (...a) => warnings.push(a.join(' '))
        try {
            let heard = null
            let result = Qt.include('lib.js', (r) => { heard = r })
            assert.equal(result.status, result.NETWORK_ERROR)
            assert.equal(heard, result)
            assert.equal(warnings.length, 1)
        } finally {
            console.warn = warn
        }
        let w = typeof window !== 'undefined' ? window : null
        if(w){
            Qt.exit(3)
            assert.equal(w.__jqmlExitCode, 3)
            assert.equal(w.__jqmlQuit, true)
            delete w.__jqmlExitCode
            delete w.__jqmlQuit
        }
    })

    test(name + ': the enum helpers read an enum given as its object of names', () => {
        assert.equal(Qt.enumValueToString(Qt.ColorScheme, 2), 'Dark')
        assert.equal(Qt.enumStringToValue(Qt.ColorScheme, 'Light'), 1)
        assert.deepEqual(Array.from(Qt.enumValueToStrings(Qt.ColorScheme, 0)), ['Unknown'])
        assert.throws(() => Qt.enumStringToValue(Qt.ColorScheme, 'Nope'))
        assert.throws(() => Qt.enumValueToString(5, 1))
    })

    test(name + ': value types keep float components and print as Qt prints them', () => {
        assert.equal(String(Qt.vector3d(0.1, 2, 3)), 'QVector3D(0.1, 2, 3)')
        assert.equal(Qt.vector3d(0.1, 0, 0).x, Math.fround(0.1))
        assert.equal(String(Qt.vector2d(1e7, 0.00001)), 'QVector2D(1e+07, 1e-05)')
        assert.equal(String(Qt.quaternion(1, 0, 0, 0).times(Qt.vector3d(1, 2, 3))), 'QVector3D(1, 2, 3)')
        assert.equal(String(Qt.matrix4x4().times(Qt.vector4d(1, 2, 3, 4))), 'QVector4D(1, 2, 3, 4)')
        assert.equal(Qt.matrix4x4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16).m34, 12)
        assert.throws(() => Qt.matrix4x4(1, 2, 3))
        let font = Qt.font({ family: 'Arial', pixelSize: 13, bold: true, nonsense: 1 })
        assert.equal(font.weight, 700)
        assert.equal(font.italic, false)
        assert.equal('nonsense' in font, false)
    })
}

for(let [name, load] of runtimes) cases(name, load())
