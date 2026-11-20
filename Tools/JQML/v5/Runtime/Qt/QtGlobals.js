// The objects of the Qt global a QML document reads: Qt.application (QQuickApplication), Qt.styleHints (QStyleHints),
// Qt.inputMethod (QInputMethod), Qt.platform (QQmlPlatform) and the properties of the Qt object itself (objectName,
// uiLanguage). Each is a QtObject, so a binding on Qt.application.state or Qt.uiLanguage follows its changes; what the
// browser does not have reads as Qt reads it without the feature (an offscreen platform).
const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { QRectF } = require('../Core/ValueTypes')
const QtEnums = require('./enums')

const hasWindow = () => typeof window !== 'undefined' && typeof document !== 'undefined'

// Qt::ApplicationState of the page: hidden tab, a window without the focus, or the active one
function pageState(){
    if(!hasWindow()) return QtEnums.ApplicationInactive
    if(document.hidden) return QtEnums.ApplicationHidden
    return typeof document.hasFocus === 'function' && document.hasFocus() ? QtEnums.ApplicationActive : QtEnums.ApplicationInactive
}

// Qt::ColorScheme of prefers-color-scheme
function colorScheme(){
    if(!hasWindow() || typeof window.matchMedia !== 'function') return QtEnums.ColorScheme.Unknown
    if(window.matchMedia('(prefers-color-scheme: dark)').matches) return QtEnums.ColorScheme.Dark
    if(window.matchMedia('(prefers-color-scheme: light)').matches) return QtEnums.ColorScheme.Light
    return QtEnums.ColorScheme.Unknown
}

// ---- Qt.application: QQuickApplication (QQmlApplication) ------------------------------------------------------------
class Application extends QtObject {
    constructor(){
        super(null)
        if(hasWindow()){
            this.$v[Application.DISPLAYNAME] = document.title || ''
            this.$v[Application.ARGUMENTS] = [String(window.location && window.location.href || '')]
            this.$v[Application.STATE] = pageState()
            let update = () => engine.batch(() => this.$write(Application.STATE, pageState()))
            document.addEventListener('visibilitychange', update)
            window.addEventListener('focus', update)
            window.addEventListener('blur', update)
            window.addEventListener('pagehide', () => engine.emitArgs(this, Application.SIGNAL_aboutToQuit, []))
        }
    }
    get active(){ return this.state === QtEnums.ApplicationActive }
    get font(){ return require('../Core/ValueTypes').makeFont({}) }
    get styleHints(){ return styleHints() }
    get supportsMultipleWindows(){ return false }
    get screens(){ return [] }
    setName(name){ this.name = name }
    setOrganization(organization){ this.organization = organization }
    setDomain(domain){ this.domain = domain }
    setVersion(version){ this.version = version }
}
defineType(Application, {
    props: {
        arguments: { kind: 'var', readonly: true },
        name: 'string',
        version: 'string',
        organization: 'string',
        domain: 'string',
        displayName: 'string',
        state: { kind: 'int', readonly: true, initial: QtEnums.ApplicationInactive },
        layoutDirection: { kind: 'int', readonly: true, initial: QtEnums.LeftToRight },
    },
    signals: ['aboutToQuit', 'activeChanged', 'screensChanged'],
})

// ---- Qt.styleHints: QStyleHints with the defaults of QPlatformIntegration ---------------------------------------------
class StyleHints extends QtObject {
    constructor(){
        super(null)
        this.$v[StyleHints.COLORSCHEME] = colorScheme()
        if(hasWindow() && typeof window.matchMedia === 'function'){
            let query = window.matchMedia('(prefers-color-scheme: dark)')
            let update = () => engine.batch(() => this.$write(StyleHints.COLORSCHEME, colorScheme()))
            if(query.addEventListener) query.addEventListener('change', update)
        }
    }
    get keyboardAutoRepeatRate(){ return 30 }
    get keyboardAutoRepeatRateF(){ return 30 }
    get mouseDoubleClickDistance(){ return 5 }
    get touchDoubleTapDistance(){ return 10 }
    get passwordMaskDelay(){ return 0 }
    get passwordMaskCharacter(){ return '●' }
    get fontSmoothingGamma(){ return 1.7 }
    get startDragVelocity(){ return 0 }
    get useRtlExtensions(){ return false }
    get setFocusOnTouchRelease(){ return false }
    get showIsFullScreen(){ return false }
    get showIsMaximized(){ return false }
    get singleClickActivation(){ return false }
    get menuSelectionWraps(){ return true }
    get accessibility(){ return null }
}
defineType(StyleHints, {
    props: {
        mouseDoubleClickInterval: { kind: 'int', initial: 400 },
        mousePressAndHoldInterval: { kind: 'int', initial: 800 },
        startDragDistance: { kind: 'int', initial: 10 },
        startDragTime: { kind: 'int', initial: 500 },
        keyboardInputInterval: { kind: 'int', initial: 400 },
        cursorFlashTime: { kind: 'int', initial: 1000 },
        tabFocusBehavior: { kind: 'int', initial: QtEnums.TabFocusAllControls },
        useHoverEffects: { kind: 'bool', initial: true },
        wheelScrollLines: { kind: 'int', initial: 3 },
        mouseQuickSelectionThreshold: { kind: 'int', initial: 10 },
        showShortcutsInContextMenus: { kind: 'bool', initial: true },
        toolTipWakeUpDelay: { kind: 'int', initial: 700 },
        toolTipWakeUpDelayAsMSec: { kind: 'int', initial: 700 },
        contextMenuTrigger: { kind: 'int', initial: QtEnums.ContextMenuTrigger.Press },
        colorScheme: { kind: 'int', readonly: true },
    },
})

// ---- Qt.inputMethod: QInputMethod without a virtual keyboard ---------------------------------------------------------
class InputMethod extends QtObject {
    get locale(){ return require('./Locale').locale() }
    show(){ this.$write(InputMethod.VISIBLE, true) }
    hide(){ this.$write(InputMethod.VISIBLE, false) }
    commit(){}
    reset(){}
    update(){}
    invokeAction(){}
}
defineType(InputMethod, {
    props: {
        visible: { kind: 'bool', readonly: true },
        animating: { kind: 'bool', readonly: true },
        keyboardRectangle: { kind: 'var', readonly: true, initial: new QRectF() },
        cursorRectangle: { kind: 'var', readonly: true, initial: new QRectF() },
        anchorRectangle: { kind: 'var', readonly: true, initial: new QRectF() },
        inputItemClipRectangle: { kind: 'var', readonly: true, initial: new QRectF() },
        inputDirection: { kind: 'int', readonly: true, initial: QtEnums.LeftToRight },
    },
    signals: ['localeChanged'],
})

// ---- Qt.platform: QQmlPlatform. os stays "web" (v5 code tells the browser by it); the plugin is the browser one ----------
class Platform extends QtObject {
    get os(){ return 'web' }
    get pluginName(){ return 'web' }
}
defineType(Platform, { props: {} })

// ---- the properties of the Qt object (QtObject of qqmlbuiltinfunctions: objectName, uiLanguage) -----------------------
class QtState extends QtObject {
    constructor(){
        super(null)
        // QJSEngine::uiLanguage starts as the first UI language of the system locale
        let language = typeof navigator !== 'undefined' && navigator && navigator.language ? String(navigator.language) : ''
        this.$v[QtState.UILANGUAGE] = language.split('-')[0]
    }
}
defineType(QtState, { props: { uiLanguage: 'string' } })

let made = {}
const once = (key, Class) => made[key] || (made[key] = new Class())
const styleHints = () => once('styleHints', StyleHints)

// Defines the members of the Qt object on it: the objects made on first use, objectName and uiLanguage forwarded
function install(Qt){
    Object.defineProperty(Qt, 'application', { get: () => once('application', Application), enumerable: true, configurable: true })
    Object.defineProperty(Qt, 'styleHints', { get: styleHints, enumerable: true, configurable: true })
    Object.defineProperty(Qt, 'inputMethod', { get: () => once('inputMethod', InputMethod), enumerable: true, configurable: true })
    Object.defineProperty(Qt, 'platform', { get: () => once('platform', Platform), enumerable: true, configurable: true })
    const state = () => once('state', QtState)
    for(let name of ['objectName', 'uiLanguage']){
        Object.defineProperty(Qt, name, { get: () => state()[name], set: (value) => { state()[name] = value }, enumerable: true, configurable: true })
        Object.defineProperty(Qt, name + 'Changed', { get: () => state()[name + 'Changed'], enumerable: true, configurable: true })
    }
}

module.exports = { install, Application, StyleHints, InputMethod, Platform, QtState }
