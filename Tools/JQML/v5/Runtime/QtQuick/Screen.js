// The attached Screen of QtQuick (QQuickScreenAttached, Qt 6.12): the screen the page is on. A page has one screen, so
// every object reads the same values: Screen.width of an item and of a QtObject (which Qt gives the primary screen).
// The sizes are those of window.screen in CSS pixels; the densities take the 96 dpi of a CSS pixel; the orientation is
// the one a touch device reports (QPlatformScreen::orientation of a desktop is PrimaryOrientation, 0).
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { GroupObject } = require('../Core/QtObject')
const { Item } = require('./Item')

const PrimaryOrientation = 0, PortraitOrientation = 1, LandscapeOrientation = 2, InvertedPortraitOrientation = 4, InvertedLandscapeOrientation = 8
const ORIENTATIONS = {
    'portrait-primary': PortraitOrientation, 'landscape-primary': LandscapeOrientation,
    'portrait-secondary': InvertedPortraitOrientation, 'landscape-secondary': InvertedLandscapeOrientation,
}
const DOTS_PER_MM = 96 / 25.4

function read(){
    let g = typeof globalThis !== 'undefined' ? globalThis : {}
    let s = g.screen || {}
    let width = s.width || 0, height = s.height || 0
    let touch = g.navigator && g.navigator.maxTouchPoints > 0
    return {
        width, height,
        desktopAvailableWidth: s.availWidth || width,
        desktopAvailableHeight: s.availHeight || height,
        devicePixelRatio: g.devicePixelRatio || 1,
        virtualX: s.availLeft || 0,
        virtualY: s.availTop || 0,
        // QScreen::primaryOrientation: landscape when the screen is at least as wide as it is high
        primaryOrientation: width >= height ? LandscapeOrientation : PortraitOrientation,
        orientation: touch && s.orientation ? ORIENTATIONS[s.orientation.type] || PrimaryOrientation : PrimaryOrientation,
    }
}

class ScreenAttached extends GroupObject {
    constructor(){
        super(null)
        this.$update()
        let g = typeof globalThis !== 'undefined' ? globalThis : null
        if(g && typeof g.addEventListener === 'function') g.addEventListener('resize', () => engine.batch(() => this.$update()))
    }

    $update(){
        let values = read()
        for(let name in values) this.$write(ScreenAttached[name.toUpperCase()], values[name])
    }

    // QScreen::angleBetween: the rotation from orientation a to b, in degrees
    angleBetween(a, b){
        let primary = this.$v[ScreenAttached.PRIMARYORIENTATION]
        if(a === PrimaryOrientation) a = primary
        if(b === PrimaryOrientation) b = primary
        if(a === b || !a || !b) return 0
        let delta = Math.log2(a) - Math.log2(b)
        if(delta < 0) delta += 4
        return [0, 90, 180, 270][delta] || 0
    }
}
defineType(ScreenAttached, {
    props: {
        name: { kind: 'string', readonly: true },
        manufacturer: { kind: 'string', readonly: true },
        model: { kind: 'string', readonly: true },
        serialNumber: { kind: 'string', readonly: true },
        width: { kind: 'int', readonly: true },
        height: { kind: 'int', readonly: true },
        desktopAvailableWidth: { kind: 'int', readonly: true },
        desktopAvailableHeight: { kind: 'int', readonly: true },
        pixelDensity: { kind: 'real', readonly: true, initial: DOTS_PER_MM },
        logicalPixelDensity: { kind: 'real', readonly: true, initial: DOTS_PER_MM },
        devicePixelRatio: { kind: 'real', readonly: true, initial: 1 },
        orientation: { kind: 'int', readonly: true },
        primaryOrientation: { kind: 'int', readonly: true },
        virtualX: { kind: 'int', readonly: true },
        virtualY: { kind: 'int', readonly: true },
    },
})

let shared = null
const screen = () => shared || (shared = new ScreenAttached())

// the type Screen: Screen.width of an object that is not an item reads the same screen
class Screen extends GroupObject {}
defineType(Screen, {})
for(let name of ['name', 'manufacturer', 'model', 'serialNumber', 'width', 'height', 'desktopAvailableWidth', 'desktopAvailableHeight',
    'pixelDensity', 'logicalPixelDensity', 'devicePixelRatio', 'orientation', 'primaryOrientation', 'virtualX', 'virtualY']){
    Object.defineProperty(Screen, name, { get: () => screen()[name], configurable: true })
}
Screen.angleBetween = (a, b) => screen().angleBetween(a, b)

const prop = Item.$type.propByName.get('Screen')
prop.group = ScreenAttached
Object.defineProperty(Item.prototype, 'Screen', { get: screen, configurable: true })

module.exports = { Screen, ScreenAttached }
