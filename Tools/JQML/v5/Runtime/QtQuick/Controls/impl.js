// QtQuick.Controls.impl of Qt 6.12, what the Basic style is made of: Color (blend, transparent), IconLabel (an icon and a
// text laid out by display and alignment), CheckLabel and MnemonicLabel (the texts of the buttons), PaddedRectangle,
// ColorImage (an image in one color) and IconImage. And make(): an item with its properties and bindings, how the style
// of each control writes the QML of the Basic style.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { coerceColor } = require('../../Core/color')
const { Item } = require('../Item')
const { Text } = require('../Text')
const { Rectangle, Border } = require('../Rectangle')
const { Image } = require('../Image')
const { Font } = require('../Font')
const { GroupObject } = require('../../Core/QtObject')
const QtEnums = require('../../Qt/enums')

// ---- make: an item of Class with props; a function is a binding, 'group.name' a property of a group, 'onX' a handler ----
function propId(obj, name){
    let prop = obj.constructor.$type.propByName.get(name)
    if(!prop) throw new Error(obj.constructor.name + ' has no property ' + name)
    return prop.id
}
function apply(obj, props){
    for(let key of Object.keys(props)){
        let value = props[key]
        if(/^on[A-Z]/.test(key)){
            let signal = obj.constructor['SIGNAL_' + key[2].toLowerCase() + key.slice(3)]
            engine.connect(obj, signal, value, null, engine.PRIORITY_NORMAL, obj)
            continue
        }
        let dot = key.indexOf('.')
        let target = dot > 0 ? obj[key.slice(0, dot)] : obj
        let name = dot > 0 ? key.slice(dot + 1) : key
        if(typeof value === 'function') target.$bind(propId(target, name), value, null, obj)
        else target[name] = value
    }
    return obj
}
function make(Class, parent, props = {}){
    let obj = new Class(parent)
    return apply(obj, props)
}

// ---- Color (QQuickColor) ----
const channels = (color) => {
    let hex = coerceColor(color)
    let at = hex.length === 9 ? 3 : 1
    return [parseInt(hex.slice(at, at + 2), 16), parseInt(hex.slice(at + 2, at + 4), 16), parseInt(hex.slice(at + 4, at + 6), 16), hex.length === 9 ? parseInt(hex.slice(1, 3), 16) : 255]
}
const hex2 = (v) => Math.max(0, Math.min(255, v)).toString(16).padStart(2, '0')
// QQuickColor::blend: a to b by factor in RGB, opaque (Qt 6.12: blend("#e0e0e0", "#bdbdbd", 0.5) is #cfcfcf)
function blend(a, b, factor){
    if(factor <= 0) return coerceColor(a)
    if(factor >= 1) return coerceColor(b)
    let x = channels(a), y = channels(b)
    let mix = (i) => Math.floor(x[i] * (1 - factor) + y[i] * factor + 0.5)
    return '#' + hex2(mix(0)) + hex2(mix(1)) + hex2(mix(2))
}
// QQuickColor::transparent: the color with an opacity
function transparent(color, opacity){
    let c = channels(color)
    return '#' + hex2(Math.round(Math.max(0, Math.min(1, opacity)) * 255)) + hex2(c[0]) + hex2(c[1]) + hex2(c[2])
}
const Color = { blend, transparent }

// ---- the icon of a button (QQuickIcon): a value of name, source, width, height, color, cache ----
class Icon extends GroupObject {}
defineType(Icon, {
    props: { name: 'string', source: 'string', width: 'int', height: 'int', color: { kind: 'color', initial: '#00000000' }, cache: { kind: 'bool', initial: true } },
})

// ---- MnemonicLabel, CheckLabel ----
// QQuickMnemonicLabel: "&File" shows File (the mnemonic underlined while Alt is down; "&&" is an &)
const stripMnemonic = (text) => String(text === undefined || text === null ? '' : text).replace(/&(&?)/g, (m, amp) => amp)
class MnemonicLabel extends Text {
    $copyFont(font){ copyFont(this, font) }
}
defineType(MnemonicLabel, {})
// QQuickCheckLabel: the text of a check box, radio button or switch: left aligned, vertically centered, elided right
class CheckLabel extends Text {
    constructor(parent = null){
        super(parent)
        this.horizontalAlignment = Text.AlignLeft
        this.verticalAlignment = Text.AlignVCenter
        this.elide = Text.ElideRight
    }
    $copyFont(font){ copyFont(this, font) }
}
defineType(CheckLabel, {})

// the font of a control as the font of a Text: each field copied
const COPIED = ['family', 'bold', 'weight', 'italic', 'underline', 'strikeout', 'capitalization', 'letterSpacing', 'wordSpacing',
    'hintingPreference', 'kerning', 'preferShaping', 'styleName']
function copyFont(text, font){
    let own = text.font
    engine.batch(() => {
        for(let name of COPIED) own.$write(Font[name.toUpperCase()], font.$v[Font[name.toUpperCase()]])
        own.$v[Font.POINTSIZE] = font.$v[Font.POINTSIZE]
        own.$write(Font.PIXELSIZE, font.$v[Font.PIXELSIZE])
    })
}
// a text, a text input or a text edit follows the font of a control (font: control.font)
for(let Class of [Text, require('../TextInput').TextInput, require('../TextEdit').TextEdit]) Class.prototype.$copyFont = function(font){ copyFont(this, font) }

// ---- PaddedRectangle (QQuickPaddedRectangle): a rectangle drawn inside its paddings ----
class PaddedRectangle extends Rectangle {
    $paint(dirty){
        super.$paint(dirty)
        if(!this.$dom) return
        let v = this.$v
        let pad = (side) => (Number.isNaN(v[side]) ? v[PaddedRectangle.PADDING] : v[side])
        let top = pad(PaddedRectangle.TOPPADDING), left = pad(PaddedRectangle.LEFTPADDING)
        let right = pad(PaddedRectangle.RIGHTPADDING), bottom = pad(PaddedRectangle.BOTTOMPADDING)
        // the rectangle less the paddings: a clip of the box
        this.$dom.style.clipPath = top || left || right || bottom ? 'inset(' + top + 'px ' + right + 'px ' + bottom + 'px ' + left + 'px round ' + v[Rectangle.RADIUS] + 'px)' : ''
    }
}
const restyle = (rect) => engine.markDirty(rect, require('../Item').bits.STYLE)
defineType(PaddedRectangle, {
    props: {
        padding: { kind: 'real', effect: restyle },
        topPadding: { kind: 'real', initial: NaN, effect: restyle },
        leftPadding: { kind: 'real', initial: NaN, effect: restyle },
        rightPadding: { kind: 'real', initial: NaN, effect: restyle },
        bottomPadding: { kind: 'real', initial: NaN, effect: restyle },
    },
})

// ---- ColorImage (QQuickColorImage): the picture as a mask, painted in color (defaultColor: the color it is drawn in) ----
class ColorImage extends Image {
    $paint(dirty){
        super.$paint(dirty)
        if(!this.$dom) return
        let s = this.$dom.style
        let color = this.$v[ColorImage.COLOR]
        // a picture of the style is drawn in its color already (a vector made for it)
        let tinted = !String(this.$v[Image.SOURCE] || '').startsWith('data:image/svg') && color && coerceColor(color) !== '#00000000' && coerceColor(color) !== coerceColor(this.$v[ColorImage.DEFAULTCOLOR])
        if(!tinted || !s.backgroundImage || s.backgroundImage === 'none'){
            s.maskImage = s.webkitMaskImage = ''
            if(tinted === false && s.backgroundColor) s.backgroundColor = ''
            return
        }
        // the alpha of the picture with the color: a mask over a plain fill
        let image = s.backgroundImage
        s.maskImage = s.webkitMaskImage = image
        s.maskSize = s.webkitMaskSize = s.backgroundSize
        s.maskRepeat = s.webkitMaskRepeat = s.backgroundRepeat
        s.maskPosition = s.webkitMaskPosition = s.backgroundPosition
        s.backgroundImage = 'none'
        s.backgroundColor = require('../../Core/color').toCss(color)
    }
}
const repaint = (image) => engine.markDirty(image, require('../Item').bits.STYLE)
defineType(ColorImage, {
    props: {
        color: { kind: 'color', initial: '#00000000', effect: repaint },
        defaultColor: { kind: 'color', initial: '#00000000', effect: repaint },
    },
})

// IconImage (QQuickIconImage): an image of an icon, by its source (a theme icon by name the page has none of)
class IconImage extends ColorImage {}
defineType(IconImage, { props: { name: 'string' } })

// ---- IconLabel (QQuickIconLabel) ----
const IconOnly = 0, TextOnly = 1, TextBesideIcon = 2, TextUnderIcon = 3
const AlignLeft = 1, AlignRight = 2, AlignHCenter = 4, AlignTop = 32, AlignBottom = 64, AlignVCenter = 128

// QStyle::alignedRect: a box of size in the rect by the alignment
function alignedX(alignment, width, left, available){
    if(alignment & AlignRight) return left + available - width
    if(alignment & AlignHCenter) return left + (available - width) / 2
    return left
}
function alignedY(alignment, height, top, available){
    if(alignment & AlignBottom) return top + available - height
    if(alignment & AlignVCenter) return top + (available - height) / 2
    return top
}

class IconLabel extends Item {
    constructor(parent = null){
        super(parent)
        this.$image = null
        this.$label = null
        this.$font = null
        this.$layoutQueued = false
    }
    $hasIcon(){
        let icon = this.$v[IconLabel.ICON]
        return this.$v[IconLabel.DISPLAY] !== TextOnly && !!icon && !!(icon.$v[Icon.SOURCE] || icon.$v[Icon.NAME])
    }
    $hasText(){ return this.$v[IconLabel.DISPLAY] !== IconOnly && !!this.$v[IconLabel.TEXT] }

    // QQuickIconLabelPrivate::updateOrSyncImage / updateOrSyncLabel: the children it needs
    $sync(){
        if(this.$hasText()){
            if(!this.$label){
                this.$label = new MnemonicLabel(this)
                this.$label.objectName = 'label'
                this.$label.elide = Text.ElideRight
                this.$label.verticalAlignment = Text.AlignVCenter
                engine.connect(this.$label, Item.SIGNAL_implicitWidthChanged, () => this.$relayout(), null, engine.PRIORITY_NORMAL, this)
                engine.connect(this.$label, Item.SIGNAL_implicitHeightChanged, () => this.$relayout(), null, engine.PRIORITY_NORMAL, this)
                engine.connect(this.$label, Item.SIGNAL_baselineOffsetChanged, () => this.$layout(), null, engine.PRIORITY_NORMAL, this)
                if(this.$font) this.$font.$follow(this.$label)
            }
            let label = this.$label
            label.text = stripMnemonic(this.$v[IconLabel.TEXT])
            label.color = this.$v[IconLabel.COLOR]
            label.horizontalAlignment = this.$v[IconLabel.ALIGNMENT] & (AlignLeft | AlignRight | AlignHCenter) || AlignHCenter
        } else if(this.$label){
            if(this.$font) this.$font.$unfollow(this.$label)
            this.$label.destroy()
            this.$label = null
        }
        if(this.$hasIcon()){
            if(!this.$image){
                this.$image = new IconImage(this)
                this.$image.objectName = 'image'
                this.$image.fillMode = Image.PreserveAspectFit
                engine.connect(this.$image, Item.SIGNAL_implicitWidthChanged, () => this.$relayout(), null, engine.PRIORITY_NORMAL, this)
                engine.connect(this.$image, Item.SIGNAL_implicitHeightChanged, () => this.$relayout(), null, engine.PRIORITY_NORMAL, this)
            }
            let icon = this.$v[IconLabel.ICON], image = this.$image
            image.source = icon.$v[Icon.SOURCE] || ''
            image.name = icon.$v[Icon.NAME] || ''
            image.sourceSize.width = icon.$v[Icon.WIDTH]
            image.sourceSize.height = icon.$v[Icon.HEIGHT]
            image.defaultColor = this.$v[IconLabel.DEFAULTICONCOLOR]
            let color = icon.$v[Icon.COLOR]
            image.color = color && color !== '#00000000' ? color : this.$v[IconLabel.DEFAULTICONCOLOR]
            image.cache = icon.$v[Icon.CACHE]
        } else if(this.$image){
            this.$image.destroy()
            this.$image = null
        }
        this.$relayout()
    }

    $iconSize(){
        let image = this.$image
        if(!image) return { w: 0, h: 0 }
        let icon = this.$v[IconLabel.ICON]
        // the icon is shown at icon.width x icon.height at most, at the size of its picture
        let w = image.$v[Item.IMPLICITWIDTH], h = image.$v[Item.IMPLICITHEIGHT]
        let iw = icon.$v[Icon.WIDTH], ih = icon.$v[Icon.HEIGHT]
        if(iw > 0 && w > iw){ h = h * iw / w; w = iw }
        if(ih > 0 && h > ih){ w = w * ih / h; h = ih }
        return { w, h }
    }

    // QQuickIconLabelPrivate::updateImplicitSize
    $relayout(){
        let v = this.$v
        let pad = (id) => v[id]
        let hp = pad(IconLabel.LEFTPADDING) + pad(IconLabel.RIGHTPADDING), vp = pad(IconLabel.TOPPADDING) + pad(IconLabel.BOTTOMPADDING)
        let icon = this.$iconSize()
        let label = this.$label
        let lw = label ? label.$v[Item.IMPLICITWIDTH] : 0, lh = label ? label.$v[Item.IMPLICITHEIGHT] : 0
        let both = !!this.$image && !!label
        let spacing = both ? v[IconLabel.SPACING] : 0
        let display = v[IconLabel.DISPLAY]
        let w, h
        if(display === TextUnderIcon){
            w = Math.max(icon.w, lw)
            h = icon.h + spacing + lh
        } else {
            w = icon.w + spacing + lw
            h = Math.max(icon.h, lh)
        }
        this.$write(Item.IMPLICITWIDTH, w + hp)
        this.$write(Item.IMPLICITHEIGHT, h + vp)
        this.$layout()
    }

    // QQuickIconLabelPrivate::layout: the icon and the label in the available box by the alignment
    $layout(){
        let v = this.$v
        let left = v[IconLabel.LEFTPADDING], top = v[IconLabel.TOPPADDING]
        let availableWidth = v[Item.WIDTH] - left - v[IconLabel.RIGHTPADDING]
        let availableHeight = v[Item.HEIGHT] - top - v[IconLabel.BOTTOMPADDING]
        let alignment = v[IconLabel.ALIGNMENT]
        let mirrored = v[IconLabel.MIRRORED]
        let image = this.$image, label = this.$label
        let icon = this.$iconSize()
        let lw = label ? Math.min(label.$v[Item.IMPLICITWIDTH], availableWidth) : 0
        let lh = label ? Math.min(label.$v[Item.IMPLICITHEIGHT], availableHeight) : 0
        let place = (item, x, y, w, h) => {
            item.$write(Item.X, x)
            item.$write(Item.Y, y)
            item.$write(Item.WIDTH, w)
            item.$write(Item.HEIGHT, h)
        }
        if(image && label){
            let spacing = v[IconLabel.SPACING]
            if(v[IconLabel.DISPLAY] === TextUnderIcon){
                let h = icon.h + spacing + lh
                let y = alignedY(alignment, h, top, availableHeight)
                place(image, alignedX(alignment, icon.w, left, availableWidth), y, icon.w, icon.h)
                place(label, alignedX(alignment, lw, left, availableWidth), y + icon.h + spacing, lw, lh)
            } else {
                lw = Math.min(label.$v[Item.IMPLICITWIDTH], Math.max(0, availableWidth - icon.w - spacing))
                let w = icon.w + spacing + lw
                let x = alignedX(alignment, w, left, availableWidth)
                let first = mirrored ? x + lw + spacing : x, second = mirrored ? x : x + icon.w + spacing
                place(image, first, alignedY(alignment, icon.h, top, availableHeight), icon.w, icon.h)
                place(label, second, alignedY(alignment, lh, top, availableHeight), lw, lh)
            }
        } else if(label){
            place(label, alignedX(alignment, lw, left, availableWidth), alignedY(alignment, lh, top, availableHeight), lw, lh)
        } else if(image){
            place(image, alignedX(alignment, icon.w, left, availableWidth), alignedY(alignment, icon.h, top, availableHeight), icon.w, icon.h)
        }
        // the baseline of the label (QQuickIconLabel::baselineOffset)
        this.$write(Item.BASELINEOFFSET, label ? label.$v[Item.Y] + label.$v[Item.BASELINEOFFSET] : 0)
    }

    $write(id, value){
        super.$write(id, value)
        if(id === Item.WIDTH || id === Item.HEIGHT) this.$layout()
    }

    // font: control.font (the font of the control, followed); a plain font is copied once
    get font(){
        if(!this.$font){
            this.$ownFont = this.$ownFont || new Font(this)
            return this.$ownFont
        }
        return this.$font
    }
    set font(value){ this.$setFont(value) }
    $setFont(font){
        if(this.$font === font) return
        if(this.$font && this.$label) this.$font.$unfollow(this.$label)
        this.$font = font && font.$follow ? font : null
        if(this.$label){
            if(this.$font) this.$font.$follow(this.$label)
            else if(font) copyFont(this.$label, font)
        }
    }
    $iconChanged(){ this.$sync() }
}
const sync = (label) => label.$sync()
const relayout = (label) => label.$relayout()
defineType(IconLabel, {
    props: {
        icon: { kind: 'var', effect(label, value, old){
            if(old && old.$iconUsers) old.$iconUsers.delete(label)
            if(value && value.$owner !== undefined) (value.$iconUsers || (value.$iconUsers = new Set())).add(label)
            label.$sync()
        } },
        text: { kind: 'string', effect: sync },
        color: { kind: 'color', effect: sync },
        display: { kind: 'int', initial: TextBesideIcon, effect: sync },
        spacing: { kind: 'real', effect: relayout },
        mirrored: { kind: 'bool', effect: relayout },
        alignment: { kind: 'int', initial: AlignHCenter | AlignVCenter, effect: sync },
        topPadding: { kind: 'real', effect: relayout },
        leftPadding: { kind: 'real', effect: relayout },
        rightPadding: { kind: 'real', effect: relayout },
        bottomPadding: { kind: 'real', effect: relayout },
        defaultIconColor: { kind: 'color', effect: sync },
    },
})
Object.assign(IconLabel, { IconOnly, TextOnly, TextBesideIcon, TextUnderIcon })

// an icon of a button that changed: the IconLabels that show it
Icon.prototype.$write = function(id, value){
    GroupObject.prototype.$write.call(this, id, value)
    if(this.$iconUsers) for(let label of this.$iconUsers) label.$sync()
}

module.exports = { make, apply, Color, blend, transparent, Icon, IconLabel, MnemonicLabel, CheckLabel, PaddedRectangle, ColorImage, IconImage,
    stripMnemonic, copyFont, IconOnly, TextOnly, TextBesideIcon, TextUnderIcon }
