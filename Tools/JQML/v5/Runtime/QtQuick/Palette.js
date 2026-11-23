// The palette of an item (QQuickPalette, QQuickColorGroup; Qt 6: every Item has one). A role reads, in its color group (the
// current one: disabled while the item is not enabled, else active), the color the item sets itself, else the one the
// nearest item above it sets, else the default palette: the Basic theme's for a control (QQuickTheme), the system's for
// any other item (QGuiApplication::palette). Only the roles an item sets pass to the items below it (QPalette::resolve),
// as Qt 6.12 shows: a Rectangle in a Control that sets palette.button reads that button and the system window.
// palette.button = c sets the role in every group, palette.disabled.button = c in that group only.
const engine = require('../Core/engine')
const { GroupObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { coerceColor } = require('../Core/color')

const ROLES = ['accent', 'alternateBase', 'base', 'brightText', 'button', 'buttonText', 'dark', 'highlight', 'highlightedText',
    'light', 'link', 'linkVisited', 'mid', 'midlight', 'placeholderText', 'shadow', 'text', 'toolTipBase', 'toolTipText',
    'window', 'windowText']
const GROUPS = ['active', 'inactive', 'disabled']

const palette = (values) => Object.fromEntries(ROLES.map((role, i) => [role, values[i]]))
// Qt 6.12, QT_QUICK_CONTROLS_STYLE=Basic: the palette of a Control (active and inactive are the same)
const BASIC_ACTIVE = palette(['#308cc6', '#f7f7f7', '#ffffff', '#ffffff', '#e0e0e0', '#26282a', '#353637', '#0066ff', '#090909',
    '#f6f6f6', '#45a7d7', '#ff00ff', '#bdbdbd', '#e4e4e4', '#88353637', '#28282a', '#353637', '#ffffff', '#000000', '#ffffff', '#26282a'])
const BASIC_DISABLED = palette(['#919191', '#f7f7f7', '#d6d6d6', '#4dffffff', '#e0e0e0', '#4d26282a', '#353637', '#f0f6ff', '#090909',
    '#f6f6f6', '#45a7d7', '#ff00ff', '#bdbdbd', '#e4e4e4', '#88353637', '#28282a', '#7f353637', '#ffffff', '#000000', '#ffffff', '#bdbebf'])
// the palette of the application (any other item)
const SYSTEM_ACTIVE = palette(['#308cc6', '#f7f7f7', '#ffffff', '#ffffff', '#efefef', '#000000', '#9f9f9f', '#308cc6', '#ffffff',
    '#ffffff', '#0000ff', '#ff00ff', '#b8b8b8', '#cacaca', '#80000000', '#767676', '#000000', '#ffffdc', '#000000', '#efefef', '#000000'])
const SYSTEM_DISABLED = palette(['#919191', '#f7f7f7', '#efefef', '#ffffff', '#efefef', '#bebebe', '#bebebe', '#919191', '#ffffff',
    '#ffffff', '#0000ff', '#ff00ff', '#b8b8b8', '#cacaca', '#80000000', '#b1b1b1', '#bebebe', '#ffffdc', '#000000', '#efefef', '#bebebe'])
const THEMES = {
    basic: { active: BASIC_ACTIVE, inactive: BASIC_ACTIVE, disabled: BASIC_DISABLED },
    system: { active: SYSTEM_ACTIVE, inactive: SYSTEM_ACTIVE, disabled: SYSTEM_DISABLED },
}

// a role an item set anywhere since the last one: an item that sets its first role is not one the items below follow
// yet, so they follow this signal too (one subscription each, emitted only when an item starts setting roles)
class PaletteEpoch extends GroupObject {}
defineType(PaletteEpoch, { signals: ['changed'] })
const epoch = new PaletteEpoch(null)

// the item a palette (and a font) passes to: the parent item, or what $paletteParent() of a Popup gives
const parentOf = (item) => (item.$paletteParent ? item.$paletteParent() : item.$parent && item.$parent.$isItem ? item.$parent : null)

// the color of role in group for item: its own, the one of an item above it, the default
function resolve(item, group, role){
    engine.track(epoch, PaletteEpoch.SIGNAL_changed)
    for(let it = item; it; it = parentOf(it)){
        let own = it.$palette
        if(own){
            let colors = own.$groups[group]
            engine.track(colors, ColorGroup.SIGNAL_changed)
            if(colors.$set.has(role)) return colors.$colors[role]
        }
    }
    return THEMES[item.$theme || 'system'][group][role]
}

// a write of a binding (palette.button: x, palette.disabled.button: x): the role of the id
function writeRole(id, value){
    let prop = this.constructor.$type.props[id]
    if(prop) this[prop.name] = value
}

// QQuickColorGroup: the roles of one group of the palette of an item
class ColorGroup extends GroupObject {
    constructor(owner, group = 'active', palette = null){
        super(owner)
        this.$group = group
        this.$pal = palette
        this.$set = new Set()
        this.$colors = Object.create(null)
    }
    $setRole(role, value){
        let color = coerceColor(value)
        if(this.$set.has(role) && this.$colors[role] === color) return
        this.$set.add(role)
        this.$colors[role] = color
        if(this.$pal) this.$pal.$started()
        engine.emit(this, ColorGroup.SIGNAL_changed)
    }
    $resetRole(role){
        if(!this.$set.delete(role)) return
        delete this.$colors[role]
        engine.emit(this, ColorGroup.SIGNAL_changed)
    }
    // the item the palette is of
    $item(){ return this.$pal ? this.$pal.$owner : this.$owner }
}
ColorGroup.prototype.$write = writeRole
defineType(ColorGroup, {
    props: Object.fromEntries(ROLES.map((role) => [role, {
        kind: 'var',
        get(){ return resolve(this.$item(), this.$group, role) },
        set(value){ if(value === undefined) this.$resetRole(role); else this.$setRole(role, value) },
    }])),
    signals: ['changed'],
})

// QQuickPalette: the roles of the current group, and the groups
class Palette extends GroupObject {
    constructor(owner){
        super(owner)
        this.$groups = {}
        for(let group of GROUPS) this.$v[Palette[group.toUpperCase()]] = this.$groups[group] = new ColorGroup(owner, group, this)
        owner.$palette = this
        this.$hasRoles = false
    }
    $started(){
        if(this.$hasRoles) return
        this.$hasRoles = true
        engine.emit(epoch, PaletteEpoch.SIGNAL_changed)
    }
    // the group the item reads: disabled while it is not enabled (QQuickPalette::currentColorGroup)
    $current(){
        let item = this.$owner
        let Class = item.constructor
        if(Class.SIGNAL_enabledChanged !== undefined) engine.track(item, Class.SIGNAL_enabledChanged)
        return item.$v[Class.ENABLED] === false ? 'disabled' : 'active'
    }
    // palette: other (QQuickPalette::fromQPalette): the roles other sets
    $assignFrom(other){
        if(!other || !other.$groups) return
        for(let group of GROUPS){
            for(let role of other.$groups[group].$set) this.$groups[group].$setRole(role, other.$groups[group].$colors[role])
        }
    }
    toString(){ return 'QQuickPalette(0x0)' }
}
Palette.prototype.$write = writeRole
defineType(Palette, {
    props: Object.assign(Object.fromEntries(ROLES.map((role) => [role, {
        kind: 'var',
        get(){ return resolve(this.$owner, this.$current(), role) },
        set(value){
            engine.batch(() => {
                for(let group of GROUPS){
                    if(value === undefined) this.$groups[group].$resetRole(role)
                    else this.$groups[group].$setRole(role, value)
                }
            })
        },
    }])), {
        active: { group: ColorGroup },
        inactive: { group: ColorGroup },
        disabled: { group: ColorGroup },
    }),
})

module.exports = { Palette, ColorGroup, ROLES, GROUPS, THEMES, resolvePaletteRole: resolve }
