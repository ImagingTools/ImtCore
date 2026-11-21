// QQuickControl (QtQuick.Templates Control, Qt 6.12): the base of the controls. An item with
//  - a font that passes down to the controls below it (QQuickControlPrivate::resolveFont / updateFontRecur): a field reads
//    the value the control sets itself, else the one the nearest control above it sets, else the theme's;
//  - the paddings (padding, horizontalPadding / verticalPadding, each side) and the insets of the background;
//  - a background sized to the control less the insets, and a content item placed at the paddings, availableWidth wide
//    (resizeBackground, resizeContent); their implicit sizes as implicitBackground* / implicitContent*;
//  - hover (hovered, hoverEnabled), the focus policy and the reason of the last focus (focusReason, visualFocus).
// A press on a control is taken by Pane, Frame, the buttons... not by Control itself or Label (checked in Qt 6.12: a
// MouseArea under them gets it). The palette of a control defaults to the Basic theme (Palette.js).
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Item } = require('../Item')
const { Font } = require('../Font')
const keys = require('../KeyDelivery')
const QtEnums = require('../../Qt/enums')

const NoFocus = 0, TabFocus = 1, ClickFocus = 2, StrongFocus = TabFocus | ClickFocus | 8, WheelFocus = StrongFocus | 4
const MouseFocusReason = 0, TabFocusReason = 1, BacktabFocusReason = 2, ShortcutFocusReason = 5, OtherFocusReason = 7

// ---- the font of a control -------------------------------------------------------------------------------------------

// the fields of a font; pixelSize and pointSize are one (the size)
const FONT_FIELDS = ['family', 'size', 'bold', 'weight', 'italic', 'underline', 'strikeout', 'capitalization', 'letterSpacing',
    'wordSpacing', 'hintingPreference', 'kerning', 'preferShaping', 'styleName']
const fieldOf = (name) => (name === 'pixelSize' || name === 'pointSize' ? 'size' : name)

// the font of a control (and of a Label, a TextField, a Popup): the fields set on it are its own, the others are inherited
class ControlFont extends Font {
    constructor(owner){
        super(owner)
        this.$explicit = new Set()
        this.$inheriting = false
        this.$followers = null
    }
    $write(id, value){
        let prop = this.constructor.$type.props[id]
        if(!this.$inheriting && prop) this.$explicit.add(fieldOf(prop.name))
        super.$write(id, value)
        if(!this.$inheriting && prop) propagateFont(this.$owner)
        if(this.$followers) for(let follower of this.$followers) follower.$copyFont(this)
    }
    // the fields of font (another one: its values) that this one does not set
    $inherit(values){
        this.$inheriting = true
        try {
            for(let field of FONT_FIELDS){
                if(this.$explicit.has(field)) continue
                if(field === 'size'){
                    if(values.pointSize !== this.$v[Font.POINTSIZE] || values.pixelSize !== this.$v[Font.PIXELSIZE]){
                        // the size of the font above as it is: its pixelSize and pointSize
                        this.$v[Font.POINTSIZE] = values.pointSize
                        super.$write(Font.PIXELSIZE, values.pixelSize)
                        engine.emit(this, Font.SIGNAL_pointSizeChanged)
                    }
                    continue
                }
                super.$write(Font[field.toUpperCase()], values[field])
            }
        } finally {
            this.$inheriting = false
        }
        if(this.$followers) for(let follower of this.$followers) follower.$copyFont(this)
    }
    // the values of the fields (what a control below inherits)
    $values(){
        let v = this.$v
        return {
            family: v[Font.FAMILY], pixelSize: v[Font.PIXELSIZE], pointSize: v[Font.POINTSIZE], bold: v[Font.BOLD], weight: v[Font.WEIGHT],
            italic: v[Font.ITALIC], underline: v[Font.UNDERLINE], strikeout: v[Font.STRIKEOUT], capitalization: v[Font.CAPITALIZATION],
            letterSpacing: v[Font.LETTERSPACING], wordSpacing: v[Font.WORDSPACING], hintingPreference: v[Font.HINTINGPREFERENCE],
            kerning: v[Font.KERNING], preferShaping: v[Font.PREFERSHAPING], styleName: v[Font.STYLENAME],
        }
    }
    // a Text that shows this font (the label of a control's IconLabel): it copies every change
    $follow(text){
        if(!this.$followers) this.$followers = new Set()
        this.$followers.add(text)
        text.$copyFont(this)
    }
    $unfollow(text){ if(this.$followers) this.$followers.delete(text) }
}
defineType(ControlFont, {})

// the theme's font (QQuickTheme::font): the defaults of Font
const THEME_FONT = (() => {
    let initial = Font.$type.initial
    let at = (name) => initial[Font[name.toUpperCase()]]
    return { family: at('family'), pixelSize: at('pixelSize'), pointSize: at('pointSize'), bold: at('bold'), weight: at('weight'),
        italic: at('italic'), underline: at('underline'), strikeout: at('strikeout'), capitalization: at('capitalization'),
        letterSpacing: at('letterSpacing'), wordSpacing: at('wordSpacing'), hintingPreference: at('hintingPreference'),
        kerning: at('kerning'), preferShaping: at('preferShaping'), styleName: at('styleName') }
})()

// the item a font passes to (a Popup: the item it is shown in)
const fontParentOf = (item) => (item.$paletteParent ? item.$paletteParent() : item.$parent && item.$parent.$isItem ? item.$parent : null)

// the font of the nearest item above that has one (a control, a Window), or null
function inheritedFont(item){
    for(let it = fontParentOf(item); it; it = fontParentOf(it)){
        if(it.$controlFont) return it.$controlFont
    }
    return null
}

// QQuickControlPrivate::resolveFont: the fields of the font above; then the controls below
function resolveFont(item){
    let font = item.$controlFont
    if(!font) return
    let above = inheritedFont(item)
    font.$inherit(above ? above.$values() : THEME_FONT)
    propagateFont(item)
}

// QQuickControlPrivate::updateFontRecur: the controls below an item take its font, through items that have none
function propagateFont(item){
    let visit = (parent) => {
        for(let child of parent.$children || []){
            if(!child.$isItem) continue
            if(child.$controlFont) resolveFont(child)
            else visit(child)
        }
        for(let popup of parent.$popups || []) resolveFont(popup)
    }
    visit(item)
}

// installs the font of a control-like item (Control, Label, TextField, TextArea): slot is the id of its font property
function installFont(item, slot){
    let font = new ControlFont(item)
    item.$v[slot] = font
    item.$controlFont = font
    resolveFont(item)
    return font
}

// QQuickItem::setWidth by the control: the size is set (QQuickItemPrivate::widthValid), the implicit one no longer applies
function setSize(item, id, bit, value){
    item.$explicit |= bit
    item.$write(id, value)
}

// ---- Control ----------------------------------------------------------------------------------------------------------

const PADDINGS = ['padding', 'horizontalPadding', 'verticalPadding', 'topPadding', 'leftPadding', 'rightPadding', 'bottomPadding']
const INSETS = ['topInset', 'leftInset', 'rightInset', 'bottomInset']

class Control extends Item {
    constructor(parent = null){
        super(parent)
        // the paddings and insets set on the control (the others follow padding, horizontalPadding...)
        this.$pads = { padding: 0 }
        this.$insets = {}
        this.$defaults = null       // the items the style made (background, contentItem...), deleted when replaced
        installFont(this, Control.FONT)
        this.$v[Item.ACTIVEFOCUSONTAB] = false
        this.$hoverSet = false
        this.$v[Control.HOVERENABLED] = this.$calcHoverEnabled()
        engine.scheduleComponentComplete(this)
    }

    // QQuickItem::itemChange(ItemParentHasChanged): the font of the new place
    $parentChanged(old, parent){
        super.$parentChanged(old, parent)
        if(this.$controlFont) resolveFont(this)
        if(this.$hoverSet === false) this.$plain(Control.HOVERENABLED, this.$calcHoverEnabled())
    }

    // QQuickControlPrivate::calcHoverEnabled: the hoverEnabled a control above sets, else QStyleHints::useHoverEffects
    $calcHoverEnabled(){
        for(let it = fontParentOf(this); it; it = fontParentOf(it)){
            if(it.$hoverSet) return it.$v[Control.HOVERENABLED]
        }
        return !!require('../../Qt/Qt').Qt.styleHints.useHoverEffects
    }
    $setHoverEnabled(value){
        this.$hoverSet = value !== undefined
        let enabled = value === undefined ? this.$calcHoverEnabled() : !!value
        this.$plain(Control.HOVERENABLED, enabled)
        if(!enabled) this.$plain(Control.HOVERED, false)
        // QQuickControlPrivate::updateHoverEnabledRecur: the controls below that do not set it
        let visit = (parent) => {
            for(let child of parent.$children || []){
                if(!child.$isItem) continue
                if(child instanceof Control && !child.$hoverSet) child.$setHoverEnabled(undefined)
                else if(!(child instanceof Control)) visit(child)
            }
        }
        visit(this)
    }

    // ---- paddings and insets: a side set on the control, else horizontal / vertical, else padding ----
    $setPadding(name, value){
        if(value === undefined) delete this.$pads[name]
        else this.$pads[name] = Number(value)
        if(name === 'padding' && value === undefined) this.$pads.padding = 0
        this.$updatePaddings()
    }
    $updatePaddings(){
        let p = this.$pads
        let horizontal = p.horizontalPadding !== undefined ? p.horizontalPadding : p.padding
        let vertical = p.verticalPadding !== undefined ? p.verticalPadding : p.padding
        let values = {
            padding: p.padding, horizontalPadding: horizontal, verticalPadding: vertical,
            topPadding: p.topPadding !== undefined ? p.topPadding : vertical,
            bottomPadding: p.bottomPadding !== undefined ? p.bottomPadding : vertical,
            leftPadding: p.leftPadding !== undefined ? p.leftPadding : horizontal,
            rightPadding: p.rightPadding !== undefined ? p.rightPadding : horizontal,
        }
        engine.batch(() => {
            for(let name of PADDINGS) this.$plain(Control[name.toUpperCase()], values[name])
            this.$updateBaseline()
            this.$updateAvailable()
            this.$resizeContent()
        })
    }
    $setInset(name, value){
        if(value === undefined) delete this.$insets[name]
        else this.$insets[name] = Number(value)
        this.$plain(Control[name.toUpperCase()], value === undefined ? 0 : Number(value))
        this.$resizeBackground()
    }
    $updateAvailable(){
        let v = this.$v
        this.$write(Control.AVAILABLEWIDTH, Math.max(0, v[Item.WIDTH] - v[Control.LEFTPADDING] - v[Control.RIGHTPADDING]))
        this.$write(Control.AVAILABLEHEIGHT, Math.max(0, v[Item.HEIGHT] - v[Control.TOPPADDING] - v[Control.BOTTOMPADDING]))
    }

    $write(id, value){
        super.$write(id, value)
        if(id === Item.WIDTH || id === Item.HEIGHT){
            this.$updateAvailable()
            this.$resizeBackground()
            this.$resizeContent()
        }
    }

    // QQuickControlPrivate::resizeBackground: the background fills the control less the insets, unless it has a size and
    // a place of its own (an explicit width and an x not 0) and no insets are set
    $resizeBackground(){
        let background = this.$v[Control.BACKGROUND]
        if(!background || background.$f & engine.DESTROYED) return
        let v = this.$v
        let insets = this.$insets
        // a size of its own: one its QML gave it before it became the background (QQuickControlPrivate::extra.hasBackgroundWidth)
        let own = background.$ownSize || 0
        if((!(own & 1) && background.$v[Item.X] === 0) || insets.leftInset !== undefined || insets.rightInset !== undefined){
            background.$write(Item.X, v[Control.LEFTINSET])
            setSize(background, Item.WIDTH, 1, v[Item.WIDTH] - v[Control.LEFTINSET] - v[Control.RIGHTINSET])
        }
        if((!(own & 2) && background.$v[Item.Y] === 0) || insets.topInset !== undefined || insets.bottomInset !== undefined){
            background.$write(Item.Y, v[Control.TOPINSET])
            setSize(background, Item.HEIGHT, 2, v[Item.HEIGHT] - v[Control.TOPINSET] - v[Control.BOTTOMINSET])
        }
    }
    // QQuickControl::resizeContent: the content item at the paddings, the available size
    $resizeContent(){
        let content = this.$v[Control.CONTENTITEM]
        if(!content || content.$f & engine.DESTROYED) return
        let v = this.$v
        content.$write(Item.X, v[Control.LEFTPADDING])
        content.$write(Item.Y, v[Control.TOPPADDING])
        setSize(content, Item.WIDTH, 1, v[Control.AVAILABLEWIDTH])
        setSize(content, Item.HEIGHT, 2, v[Control.AVAILABLEHEIGHT])
    }

    // ---- the delegates: background, contentItem (and the indicator of a button) ----
    // QQuickControlPrivate::setBackground / setContentItem: the old item leaves (one the style made is deleted), the new one
    // is a child of the control; the background below the others (z -1 unless it has a z), the content above it
    $setDelegate(id, item, z){
        let old = this.$v[id]
        if(old === item) return
        if(old && !(old.$f & engine.DESTROYED)){
            this.$unwatchImplicit(old)
            if(this.$defaults && this.$defaults.has(old)){
                this.$defaults.delete(old)
                old.destroy()
            } else if(old.$parent === this){
                old.$setParent(null)
            }
        }
        this.$plain(id, item || null)
        if(item){
            if(item.$ownSize === undefined) item.$ownSize = item.$explicit & 3
            if(item.$parent !== this) item.$setParent(this)
            if(z !== undefined && item.$v[Item.Z] === 0) item.$write(Item.Z, z)
            this.$watchImplicit(item, id)
        }
        this.$delegateChanged(id)
    }
    $delegateChanged(id){
        if(id === Control.BACKGROUND){
            this.$updateImplicitBackground()
            this.$resizeBackground()
        } else if(id === Control.CONTENTITEM){
            this.$updateImplicitContent()
            this.$resizeContent()
            let content = this.$v[Control.CONTENTITEM]
            if(content) content.$implicitWatch.push(engine.connect(content, Item.SIGNAL_baselineOffsetChanged, () => this.$updateBaseline(), null, engine.PRIORITY_NORMAL, this))
            this.$updateBaseline()
        }
    }
    // the implicit size of a delegate: implicitBackgroundWidth, implicitContentWidth...
    $watchImplicit(item, id){
        let update = () => this.$delegateImplicitChanged(id)
        item.$implicitWatch = [
            engine.connect(item, Item.SIGNAL_implicitWidthChanged, update, null, engine.PRIORITY_NORMAL, this),
            engine.connect(item, Item.SIGNAL_implicitHeightChanged, update, null, engine.PRIORITY_NORMAL, this),
        ]
    }
    $unwatchImplicit(item){
        if(!item.$implicitWatch) return
        for(let link of item.$implicitWatch) engine.unsubscribe(link)
        item.$implicitWatch = null
    }
    $delegateImplicitChanged(id){
        if(id === Control.BACKGROUND) this.$updateImplicitBackground()
        else if(id === Control.CONTENTITEM) this.$updateImplicitContent()
    }
    $updateImplicitBackground(){
        let background = this.$v[Control.BACKGROUND]
        this.$write(Control.IMPLICITBACKGROUNDWIDTH, background ? background.$v[Item.IMPLICITWIDTH] : 0)
        this.$write(Control.IMPLICITBACKGROUNDHEIGHT, background ? background.$v[Item.IMPLICITHEIGHT] : 0)
    }
    // QQuickControl::implicitContentWidth: the implicit size of the content item
    $updateImplicitContent(){
        let content = this.$v[Control.CONTENTITEM]
        this.$write(Control.IMPLICITCONTENTWIDTH, content ? content.$v[Item.IMPLICITWIDTH] : 0)
        this.$write(Control.IMPLICITCONTENTHEIGHT, content ? content.$v[Item.IMPLICITHEIGHT] : 0)
    }

    // a delegate the style makes: kept unless the QML of the control sets its own (deferred, as Qt's quickBeginDeferred)
    $styleDelegate(id, make, z){
        if(this.$v[id]) return null
        let item = make()
        if(!this.$defaults) this.$defaults = new Set()
        this.$defaults.add(item)
        this.$setDelegate(id, item, z)
        return item
    }
    // the style's delegates (Basic): made once the control is complete
    $makeStyle(){}

    $componentComplete(){
        super.$componentComplete()
        engine.batch(() => this.$makeStyle())
    }

    // ---- hover ----
    $acceptsHover(){ return this.$v[Control.HOVERENABLED] && this.$v[Item.ENABLED] }
    $hoverEnter(){ this.$setHovered(true) }
    $hoverMove(){ this.$setHovered(true) }
    $hoverLeave(){ this.$setHovered(false) }
    $setHovered(value){ this.$write(Control.HOVERED, !!value && this.$v[Control.HOVERENABLED]) }

    // ---- focus ----
    $focusEvent(active, reason){
        if(active) this.$write(Control.FOCUSREASON, reason)
        this.$write(Control.VISUALFOCUS, active && (this.$v[Control.FOCUSREASON] === TabFocusReason
            || this.$v[Control.FOCUSREASON] === BacktabFocusReason || this.$v[Control.FOCUSREASON] === ShortcutFocusReason))
    }
    // QQuickControlPrivate::handlePress: the focus of a click (ClickFocus)
    $clickFocus(){
        if((this.$v[Control.FOCUSPOLICY] & ClickFocus) === ClickFocus && !this.$v[Item.ACTIVEFOCUS]) this.forceActiveFocus(MouseFocusReason)
    }
    // QQuickControl::mousePressEvent: a plain Control does not take the press (Pane, Frame, buttons do)
    $takesPress(){ return false }
    $wheel(x, y, angleDelta){ return !!this.$v[Control.WHEELENABLED] }

    // QQuickControlPrivate::updateBaselineOffset: topPadding and the baseline of the content item, unless it is set
    $updateBaseline(){
        if(this.$baselineSet) return
        let content = this.$v[Control.CONTENTITEM]
        this.$plain(Item.BASELINEOFFSET, content ? this.$v[Control.TOPPADDING] + content.$v[Item.BASELINEOFFSET] : 0)
    }
}

// written through Control.$setters (an assignment, a binding)
const padding = () => 'real'
const inset = () => 'real'

defineType(Control, {
    props: {
        font: { group: ControlFont },
        availableWidth: { kind: 'real', readonly: true },
        availableHeight: { kind: 'real', readonly: true },
        padding: padding('padding'),
        topPadding: padding('topPadding'),
        leftPadding: padding('leftPadding'),
        rightPadding: padding('rightPadding'),
        bottomPadding: padding('bottomPadding'),
        horizontalPadding: padding('horizontalPadding'),
        verticalPadding: padding('verticalPadding'),
        topInset: inset('topInset'),
        leftInset: inset('leftInset'),
        rightInset: inset('rightInset'),
        bottomInset: inset('bottomInset'),
        spacing: 'real',
        locale: 'var',
        focusPolicy: {
            kind: 'int',
            effect(control, value){ control.$write(Item.ACTIVEFOCUSONTAB, (value & TabFocus) === TabFocus) },
        },
        focusReason: { kind: 'int', initial: OtherFocusReason },
        visualFocus: { kind: 'bool', readonly: true },
        hovered: { kind: 'bool', readonly: true },
        // QStyleHints::useHoverEffects: a page with a mouse hovers
        hoverEnabled: 'bool',
        wheelEnabled: 'bool',
        background: { kind: 'var', initial: null },
        contentItem: { kind: 'var', initial: null },
        mirrored: { kind: 'bool', readonly: true },
        implicitContentWidth: { kind: 'real', readonly: true },
        implicitContentHeight: { kind: 'real', readonly: true },
        implicitBackgroundWidth: { kind: 'real', readonly: true },
        implicitBackgroundHeight: { kind: 'real', readonly: true },
    },
})

// Properties whose writes do more than keep the value: QML assignments, JS assignments and the writes of bindings run their
// setter (Control.$setters: id -> (control, value)); the setter keeps the value with plain writes ($inSetter)
const SETTERS = new Map()
for(let name of PADDINGS) SETTERS.set(Control[name.toUpperCase()], (control, value) => control.$setPadding(name, value))
for(let name of INSETS) SETTERS.set(Control[name.toUpperCase()], (control, value) => control.$setInset(name, value))
SETTERS.set(Item.BASELINEOFFSET, (control, value) => {
    control.$baselineSet = value !== undefined
    if(value === undefined) control.$updateBaseline()
    else control.$plain(Item.BASELINEOFFSET, value)
})
SETTERS.set(Control.HOVERENABLED, (control, value) => control.$setHoverEnabled(value))
SETTERS.set(Control.BACKGROUND, (control, value) => control.$setDelegate(Control.BACKGROUND, value, -1))
SETTERS.set(Control.CONTENTITEM, (control, value) => control.$setDelegate(Control.CONTENTITEM, value))
Control.$setters = SETTERS

// a subclass adds setters of its own: Class.$setters = withSetters(Base, [[id, fn]...])
function withSetters(Base, entries){
    let map = new Map(Base.$setters)
    for(let [id, fn] of entries) map.set(id, fn)
    return map
}

const baseWrite = Control.prototype.$write
// a write that keeps the value only (what a setter does)
Control.prototype.$plain = function(id, value){ return baseWrite.call(this, id, value) }
Control.prototype.$write = function(id, value){
    // the setter of a property keeps its value with plain writes; a write of another property while it runs (a binding
    // that follows) goes through that one's setter
    let setter = this.constructor.$setters.get(id)
    let busy = this.$settingIds
    if(!setter || (busy && busy.has(id))) return baseWrite.call(this, id, value)
    if(!busy) busy = this.$settingIds = new Set()
    busy.add(id)
    try {
        setter(this, value)
    } finally {
        busy.delete(id)
    }
}

Object.assign(Control, { NoFocus, TabFocus, ClickFocus, StrongFocus, WheelFocus })
// the palette of a control defaults to the theme's (QQuickTheme::palette): the Basic one
Control.prototype.$theme = 'basic'

module.exports = { Control, ControlFont, withSetters, installFont, resolveFont, propagateFont, FONT_FIELDS, THEME_FONT,
    NoFocus, TabFocus, ClickFocus, StrongFocus, WheelFocus, MouseFocusReason, TabFocusReason, BacktabFocusReason, OtherFocusReason }
