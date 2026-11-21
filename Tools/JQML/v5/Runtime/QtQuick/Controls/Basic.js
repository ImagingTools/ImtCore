// The Basic style of QtQuick.Controls (Qt 6.12, Controls/Basic/*.qml) over the templates: the paddings, the implicit size
// and the delegates of each control, written as the QML of the style is (make(): a function is a binding). The delegates
// are made when the control is complete and only when its QML does not give its own (deferred, as in Qt).
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Item } = require('../Item')
const { Rectangle, Border } = require('../Rectangle')
const { Image } = require('../Image')
const { Text } = require('../Text')
const T = require('./Templates')
const { make, apply, blend, IconLabel, CheckLabel, PaddedRectangle, ColorImage, IconOnly, TextUnderIcon } = require('./impl')
const { Behavior } = require('../Behavior')
const { SmoothedAnimation } = require('../SmoothedAnimation')

const AlignLeft = 1, AlignCenter = 4 | 128

// ---- the pictures of the style (Basic/images/*.png) as vectors of the same outline, drawn in a color ----
const svg = (w, h, body) => 'data:image/svg+xml,' + encodeURIComponent('<svg xmlns="http://www.w3.org/2000/svg" width="' + w + '" height="' + h + '" viewBox="0 0 ' + w + ' ' + h + '">' + body + '</svg>')
const IMAGES = {
    // check.png, 28 x 28
    check: (color) => svg(28, 28, '<path d="M6.8 15.2L11.2 19.6L20.8 6.6" fill="none" stroke="' + color + '" stroke-width="2.6"/>'),
    // double-arrow.png, 40 x 28 (ComboBox)
    'double-arrow': (color) => svg(40, 28, '<path d="M14.6 11.6L20 6.2L25.4 11.6M14.6 16.4L20 21.8L25.4 16.4" fill="none" stroke="' + color + '" stroke-width="2.2"/>'),
    // drop-indicator.png, 18 x 11
    'drop-indicator': (color) => svg(18, 11, '<path d="M1.8 1.8L9 9L16.2 1.8" fill="none" stroke="' + color + '" stroke-width="2.4"/>'),
}
const SIZES = { check: [28, 28], 'double-arrow': [40, 28], 'drop-indicator': [18, 11] }
// ColorImage { source: ".../images/<name>.png"; defaultColor: "#353637"; color: <binding> }: the vector drawn in the color
function styleImage(name, parent, color, props = {}){
    let image = make(ColorImage, parent, Object.assign({ width: SIZES[name][0], height: SIZES[name][1], defaultColor: '#353637', color }, props))
    image.$bind(Image.SOURCE, () => IMAGES[name](image.color), null, image)
    return image
}

// implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
// and the height, with the indicator for check boxes and the like
function implicitSize(c, indicator = false){
    c.$bind(Item.IMPLICITWIDTH, () => Math.max(c.implicitBackgroundWidth + c.leftInset + c.rightInset, c.implicitContentWidth + c.leftPadding + c.rightPadding), null, c)
    c.$bind(Item.IMPLICITHEIGHT, () => Math.max(c.implicitBackgroundHeight + c.topInset + c.bottomInset, c.implicitContentHeight + c.topPadding + c.bottomPadding,
        indicator ? c.implicitIndicatorHeight + c.topPadding + c.bottomPadding : 0), null, c)
}

// contentItem: IconLabel { spacing, mirrored, display, icon, text, font: control.font, ... }
function iconLabel(c, props){
    let label = make(IconLabel, null, Object.assign({
        spacing: () => c.spacing,
        mirrored: () => c.mirrored,
        display: () => c.display,
        icon: () => c.icon,
        text: () => c.text,
    }, props))
    label.$setFont(c.font)
    return label
}

// ---- Control, Label ----
class Control extends T.Control {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
    }
}
defineType(Control, {})

class Label extends T.Label {
    constructor(parent = null){
        super(parent)
        this.$bind(Text.COLOR, () => this.palette.windowText, null, this)
        this.$bind(Text.LINKCOLOR, () => this.palette.link, null, this)
    }
}
defineType(Label, {})

class AbstractButton extends T.AbstractButton {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
    }
}
defineType(AbstractButton, {})

// ---- Button, ToolButton, RoundButton ----
class Button extends T.Button {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
        this.padding = 6
        this.$bind(T.Control.HORIZONTALPADDING, () => this.padding + 2, null, this)
        this.spacing = 6
        this.icon.width = 24
        this.icon.height = 24
    }
    $makeStyle(){
        let c = this
        this.$styleDelegate(T.Control.CONTENTITEM, () => iconLabel(c, {
            defaultIconColor: () => (c.checked || c.highlighted ? c.palette.brightText
                : c.flat && !c.down ? (c.visualFocus ? c.palette.highlight : c.palette.windowText) : c.palette.buttonText),
            color: () => (c.checked || c.highlighted ? c.palette.brightText
                : c.flat && !c.down ? (c.visualFocus ? c.palette.highlight : c.palette.windowText) : c.palette.buttonText),
        }))
        this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, {
            implicitWidth: 100,
            implicitHeight: 40,
            visible: () => !c.flat || c.down || c.checked || c.highlighted,
            color: () => blend(c.checked || c.highlighted ? c.palette.dark : c.palette.button, c.palette.mid, c.down ? 0.5 : 0.0),
            'border.color': () => (c.visualFocus ? c.palette.highlight : c.palette.windowText),
            'border.width': () => (c.visualFocus ? 2 : 0),
        }), -1)
    }
}
defineType(Button, {})

class ToolButton extends T.ToolButton {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
        this.padding = 6
        this.spacing = 6
        this.icon.width = 24
        this.icon.height = 24
    }
    $makeStyle(){
        let c = this
        let iconColor = () => (c.visualFocus ? c.palette.highlight : c.palette.buttonText)
        this.$styleDelegate(T.Control.CONTENTITEM, () => iconLabel(c, { defaultIconColor: iconColor, color: iconColor }))
        this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, {
            implicitWidth: 40,
            implicitHeight: 40,
            opacity: () => (c.down ? 1.0 : 0.5),
            color: () => (c.down || c.checked || c.highlighted ? c.palette.mid : c.palette.button),
            'border.color': () => (c.visualFocus ? c.palette.highlight : c.palette.windowText),
            'border.width': () => (c.visualFocus ? 2 : 0),
        }), -1)
    }
}
defineType(ToolButton, {})

class RoundButton extends T.RoundButton {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
        this.padding = 6
        this.spacing = 6
        this.icon.width = 24
        this.icon.height = 24
    }
    $makeStyle(){
        let c = this
        let iconColor = () => (c.checked || c.highlighted ? c.palette.brightText
            : c.flat && !c.down ? (c.visualFocus ? c.palette.highlight : c.palette.windowText) : c.palette.buttonText)
        this.$styleDelegate(T.Control.CONTENTITEM, () => iconLabel(c, { defaultIconColor: iconColor, color: iconColor }))
        let background = this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, {
            implicitWidth: 40,
            implicitHeight: 40,
            radius: () => c.radius,
            visible: () => !c.flat || c.down || c.checked || c.highlighted,
            color: () => blend(c.checked || c.highlighted ? c.palette.dark : c.palette.button, c.palette.mid, c.down ? 0.5 : 0.0),
            'border.color': () => (c.visualFocus ? c.palette.highlight : c.palette.windowText),
            'border.width': () => (c.visualFocus ? 2 : 0),
        }), -1)
        if(background) background.$bind(Item.OPACITY, () => (background.enabled ? 1 : 0.3), null, background)
    }
}
defineType(RoundButton, {})

// ---- the indicators of CheckBox, RadioButton, Switch and their delegates ----
// x of the indicator: at the left padding with a text, centered without one (a delegate: at the right)
const indicatorX = (c, indicator, delegate) => () => (delegate
    ? (c.mirrored ? c.leftPadding : c.width - indicator.width - c.rightPadding)
    : (c.text ? (c.mirrored ? c.width - indicator.width - c.rightPadding : c.leftPadding) : c.leftPadding + (c.availableWidth - indicator.width) / 2))
const indicatorY = (c, indicator) => () => c.topPadding + (c.availableHeight - indicator.height) / 2

function checkIndicator(c, delegate){
    let indicator = make(Rectangle, null, {
        implicitWidth: 28,
        implicitHeight: 28,
        color: () => (c.down ? c.palette.light : c.palette.base),
        'border.width': () => (c.visualFocus ? 2 : 1),
        'border.color': () => (c.visualFocus ? c.palette.highlight : c.palette.mid),
    })
    indicator.$bind(Item.X, indicatorX(c, indicator, delegate), null, indicator)
    indicator.$bind(Item.Y, indicatorY(c, indicator), null, indicator)
    let check = styleImage('check', indicator, () => c.palette.text, { visible: () => c.checkState === T.Checked })
    check.$bind(Item.X, () => (indicator.width - check.width) / 2, null, check)
    check.$bind(Item.Y, () => (indicator.height - check.height) / 2, null, check)
    let partial = make(Rectangle, indicator, { width: 16, height: 3, color: () => c.palette.text, visible: () => c.checkState === T.PartiallyChecked })
    partial.$bind(Item.X, () => (indicator.width - partial.width) / 2, null, partial)
    partial.$bind(Item.Y, () => (indicator.height - partial.height) / 2, null, partial)
    return indicator
}

function radioIndicator(c, delegate){
    let indicator = make(Rectangle, null, {
        implicitWidth: 28,
        implicitHeight: 28,
        color: () => (c.down ? c.palette.light : c.palette.base),
        'border.width': () => (c.visualFocus ? 2 : 1),
        'border.color': () => (c.visualFocus ? c.palette.highlight : c.palette.mid),
    })
    indicator.$bind(Rectangle.RADIUS, () => indicator.width / 2, null, indicator)
    indicator.$bind(Item.X, indicatorX(c, indicator, delegate), null, indicator)
    indicator.$bind(Item.Y, indicatorY(c, indicator), null, indicator)
    let dot = make(Rectangle, indicator, { width: 20, height: 20, radius: 10, color: () => c.palette.text, visible: () => c.checked })
    dot.$bind(Item.X, () => (indicator.width - dot.width) / 2, null, dot)
    dot.$bind(Item.Y, () => (indicator.height - dot.height) / 2, null, dot)
    return indicator
}

function switchIndicator(c, delegate){
    let indicator = make(PaddedRectangle, null, {
        implicitWidth: 56,
        implicitHeight: 28,
        radius: 8,
        leftPadding: 0,
        rightPadding: 0,
        color: () => (c.checked ? c.palette.dark : c.palette.midlight),
        'border.width': 0,
        'border.color': () => blend(c.palette.dark, c.palette.base, c.enabled ? 0.0 : 0.5),
    })
    indicator.$bind(PaddedRectangle.PADDING, () => (indicator.height - 16) / 2, null, indicator)
    // a SwitchDelegate's switch is at the right, centered without a text
    indicator.$bind(Item.X, () => (c.text
        ? (delegate ? (c.mirrored ? c.leftPadding : c.width - indicator.width - c.rightPadding) : (c.mirrored ? c.width - indicator.width - c.rightPadding : c.leftPadding))
        : c.leftPadding + (c.availableWidth - indicator.width) / 2), null, indicator)
    indicator.$bind(Item.Y, indicatorY(c, indicator), null, indicator)
    let handle = make(Rectangle, indicator, {
        width: 28,
        height: 28,
        radius: 16,
        color: () => (c.down ? c.palette.light : c.palette.window),
        'border.width': () => (c.visualFocus ? 2 : 1),
        'border.color': () => (c.visualFocus ? c.palette.highlight : c.enabled ? c.palette.mid : c.palette.midlight),
    })
    // Behavior on x { enabled: !control.down; SmoothedAnimation { velocity: 200 } }
    let behavior = new Behavior(handle)
    make(SmoothedAnimation, behavior, { velocity: 200 })
    behavior.$bind(Behavior.ENABLED, () => !c.down, null, behavior)
    behavior.$setTargetProperty(handle, 'x')
    handle.$bind(Item.X, () => Math.max(0, Math.min(indicator.width - handle.width, c.visualPosition * indicator.width - handle.width / 2)), null, handle)
    handle.$bind(Item.Y, () => (indicator.height - handle.height) / 2, null, handle)
    return indicator
}

// contentItem: CheckLabel { leftPadding: indicator.width + spacing; text; font; color: windowText }
function checkLabel(c){
    let label = make(CheckLabel, null, {
        leftPadding: () => (c.indicator && !c.mirrored ? c.indicator.width + c.spacing : 0),
        rightPadding: () => (c.indicator && c.mirrored ? c.indicator.width + c.spacing : 0),
        text: () => c.text,
        color: () => c.palette.windowText,
    })
    c.font.$follow(label)
    return label
}

class CheckBox extends T.CheckBox {
    constructor(parent = null){
        super(parent)
        implicitSize(this, true)
        this.padding = 6
        this.spacing = 6
    }
    $makeStyle(){
        this.$styleDelegate(T.AbstractButton.INDICATOR, () => checkIndicator(this, false))
        this.$styleDelegate(T.Control.CONTENTITEM, () => checkLabel(this))
    }
}
defineType(CheckBox, {})

class RadioButton extends T.RadioButton {
    constructor(parent = null){
        super(parent)
        implicitSize(this, true)
        this.padding = 6
        this.spacing = 6
    }
    $makeStyle(){
        this.$styleDelegate(T.AbstractButton.INDICATOR, () => radioIndicator(this, false))
        this.$styleDelegate(T.Control.CONTENTITEM, () => checkLabel(this))
    }
}
defineType(RadioButton, {})

class Switch extends T.Switch {
    constructor(parent = null){
        super(parent)
        implicitSize(this, true)
        this.padding = 6
        this.spacing = 6
    }
    $makeStyle(){
        this.$styleDelegate(T.AbstractButton.INDICATOR, () => switchIndicator(this, false))
        this.$styleDelegate(T.Control.CONTENTITEM, () => checkLabel(this))
    }
}
defineType(Switch, {})

class DelayButton extends T.DelayButton {
    constructor(parent = null){
        super(parent)
        // transition: Transition { NumberAnimation { duration: ... } } (the progress is animated by the template)
        this.$plain(T.DelayButton.TRANSITION, new (require('../Transition').Transition)(this))
        implicitSize(this)
        this.padding = 6
        this.$bind(T.Control.HORIZONTALPADDING, () => this.padding + 2, null, this)
    }
    $makeStyle(){
        let c = this
        // two texts: the button's color, and the bright one over the progress (clipped at the progress)
        this.$styleDelegate(T.Control.CONTENTITEM, () => {
            let group = new Item(null)
            let text = (color, from) => {
                let clip = make(Item, group, { clip: () => c.progress > 0 })
                clip.$bind(Item.HEIGHT, () => group.height, null, clip)
                let label = make(Text, clip, {
                    text: () => c.text,
                    color,
                    horizontalAlignment: Text.AlignHCenter,
                    verticalAlignment: Text.AlignVCenter,
                    elide: Text.ElideRight,
                    opacity: () => (c.enabled ? 1 : 0.3),
                })
                c.font.$follow(label)
                label.$bind(Item.WIDTH, () => group.width, null, label)
                label.$bind(Item.HEIGHT, () => group.height, null, label)
                if(from){
                    clip.$bind(Item.X, () => -c.leftPadding + c.progress * c.width, null, clip)
                    clip.$bind(Item.WIDTH, () => (1 - c.progress) * c.width, null, clip)
                    clip.$bind(Item.VISIBLE, () => c.progress < 1, null, clip)
                } else {
                    clip.$bind(Item.X, () => -c.leftPadding, null, clip)
                    clip.$bind(Item.WIDTH, () => c.progress * c.width, null, clip)
                    clip.$bind(Item.VISIBLE, () => c.progress > 0, null, clip)
                }
                label.$bind(Item.X, () => -clip.x, null, label)
                return label
            }
            let first = text(() => c.palette.buttonText, true)
            text(() => c.palette.brightText, false)
            group.$bind(Item.IMPLICITWIDTH, () => first.implicitWidth, null, group)
            group.$bind(Item.IMPLICITHEIGHT, () => first.implicitHeight, null, group)
            return group
        })
        this.$styleDelegate(T.Control.BACKGROUND, () => {
            let background = make(Rectangle, null, {
                implicitWidth: 100,
                implicitHeight: 40,
                color: () => blend(c.palette.button, c.palette.mid, c.down ? 0.5 : 0.0),
                'border.color': () => (c.visualFocus ? c.palette.highlight : c.palette.windowText),
                'border.width': () => (c.visualFocus ? 2 : 0),
            })
            let bar = make(PaddedRectangle, background, {
                padding: () => (c.visualFocus ? 2 : 0),
                color: () => blend(c.palette.dark, c.palette.mid, c.down ? 0.5 : 0.0),
            })
            bar.$bind(Item.WIDTH, () => c.progress * background.width, null, bar)
            bar.$bind(Item.HEIGHT, () => background.height, null, bar)
            return background
        }, -1)
    }
}
defineType(DelayButton, {})

class TabButton extends T.TabButton {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
        this.padding = 6
        this.spacing = 6
        this.icon.width = 24
        this.icon.height = 24
    }
    $makeStyle(){
        let c = this
        let iconColor = () => (c.checked ? c.palette.windowText : c.palette.brightText)
        this.$styleDelegate(T.Control.CONTENTITEM, () => iconLabel(c, { defaultIconColor: iconColor, color: iconColor }))
        this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, {
            implicitHeight: 40,
            color: () => blend(c.checked ? c.palette.window : c.palette.dark, c.palette.mid, c.down ? 0.5 : 0.0),
            'border.width': 0,
            'border.color': () => c.palette.windowText,
        }), -1)
    }
}
defineType(TabButton, {})

// ---- ItemDelegate and the delegates with an indicator ----
const delegateAlignment = (c) => () => (c.display === IconOnly || c.display === TextUnderIcon ? AlignCenter : AlignLeft)

class ItemDelegate extends T.ItemDelegate {
    constructor(parent = null){
        super(parent)
        implicitSize(this, true)
        this.padding = 12
        this.spacing = 8
        this.icon.width = 24
        this.icon.height = 24
    }
    $makeStyle(){
        let c = this
        this.$styleDelegate(T.Control.CONTENTITEM, () => iconLabel(c, {
            alignment: delegateAlignment(c),
            defaultIconColor: () => c.palette.text,
            color: () => (c.highlighted ? c.palette.highlightedText : c.palette.text),
        }))
        this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, {
            implicitWidth: 100,
            implicitHeight: 40,
            visible: () => c.down || c.highlighted || c.visualFocus,
            color: () => blend(c.down ? c.palette.midlight : c.palette.light, c.palette.highlight, c.visualFocus ? 0.15 : 0.0),
            'border.width': 0,
            'border.color': () => (c.highlighted ? c.palette.highlight : c.palette.text),
        }), -1)
    }
}
defineType(ItemDelegate, {})

// CheckDelegate, RadioDelegate, SwitchDelegate: the text at the left, the indicator at the right
function indicatorDelegateStyle(c, indicator){
    c.$styleDelegate(T.AbstractButton.INDICATOR, () => indicator(c, true))
    c.$styleDelegate(T.Control.CONTENTITEM, () => iconLabel(c, {
        leftPadding: () => (c.mirrored && c.indicator ? c.indicator.width + c.spacing : 0),
        rightPadding: () => (!c.mirrored && c.indicator ? c.indicator.width + c.spacing : 0),
        alignment: delegateAlignment(c),
        defaultIconColor: () => c.palette.text,
        color: () => c.palette.text,
    }))
    c.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, {
        implicitWidth: 100,
        implicitHeight: 40,
        visible: () => c.down || c.highlighted,
        color: () => (c.down ? c.palette.midlight : c.palette.light),
    }), -1)
}
function delegateDefaults(c){
    implicitSize(c, true)
    c.padding = 12
    c.spacing = 12
    c.icon.width = 24
    c.icon.height = 24
}

class CheckDelegate extends T.CheckDelegate {
    constructor(parent = null){
        super(parent)
        delegateDefaults(this)
    }
    $makeStyle(){ indicatorDelegateStyle(this, checkIndicator) }
}
defineType(CheckDelegate, {})
class RadioDelegate extends T.RadioDelegate {
    constructor(parent = null){
        super(parent)
        delegateDefaults(this)
    }
    $makeStyle(){ indicatorDelegateStyle(this, radioIndicator) }
}
defineType(RadioDelegate, {})
class SwitchDelegate extends T.SwitchDelegate {
    constructor(parent = null){
        super(parent)
        delegateDefaults(this)
    }
    $makeStyle(){ indicatorDelegateStyle(this, switchIndicator) }
}
defineType(SwitchDelegate, {})

// ---- ScrollBar, ScrollIndicator ----
// a value of the QML that changes: fn read in a binding, onChange(value) when it changes (states: `when: ...`)
const { QtObject } = require('../../Core/QtObject')
class Watch extends QtObject {}
defineType(Watch, { props: { value: 'var' } })
function watch(owner, fn, onChange){
    let w = new Watch(owner)
    w.$bind(Watch.VALUE, fn, null, w)
    engine.connect(w, Watch.SIGNAL_valueChanged, () => onChange(w.$v[Watch.VALUE]), null, engine.PRIORITY_NORMAL, w)
    return w
}
// State { name: "active"; when: ...; PropertyChanges { opacity: 0.75 } } and Transition { from: "active"; SequentialAnimation {
// PauseAnimation { duration: 450 } NumberAnimation { property: "opacity"; duration: 200; to: 0.0 } } }
function activeOpacity(owner, item, when){
    let timer = null
    let stop = () => {
        if(timer){
            clearTimeout(timer)
            clearInterval(timer)
            timer = null
        }
    }
    let set = (value) => engine.batch(() => item.$write(Item.OPACITY, value))
    watch(owner, when, (active) => {
        stop()
        if(active){
            set(0.75)
            return
        }
        let from = item.$v[Item.OPACITY]
        timer = setTimeout(() => {
            let start = Date.now()
            timer = setInterval(() => {
                if(item.$f & engine.DESTROYED) return stop()
                let t = Math.min(1, (Date.now() - start) / 200)
                set(from + (0 - from) * t)
                if(t >= 1) stop()
            }, 16)
        }, 450)
    })
    item.$write(Item.OPACITY, when() ? 0.75 : 0.0)
}

class ScrollBar extends T.ScrollBar {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
        this.padding = 2
        this.$bind(Item.VISIBLE, () => this.policy !== T.ScrollBar.AlwaysOff, null, this)
        this.$bind(T.ScrollBar.MINIMUMSIZE, () => (this.orientation === 1 ? this.height / this.width : this.width / this.height), null, this)
    }
    $makeStyle(){
        let c = this
        let handle = this.$styleDelegate(T.Control.CONTENTITEM, () => make(Rectangle, null, {
            implicitWidth: () => (c.interactive ? 6 : 2),
            implicitHeight: () => (c.interactive ? 6 : 2),
            color: () => (c.pressed ? c.palette.dark : c.palette.mid),
        }))
        if(handle){
            handle.$bind(Rectangle.RADIUS, () => handle.width / 2, null, handle)
            activeOpacity(handle, handle, () => c.policy === T.ScrollBar.AlwaysOn || (c.active && c.size < 1.0))
        }
        this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, {
            visible: false,
            opacity: () => (c.contentItem ? c.contentItem.opacity : 0),
            color: () => c.palette.mid,
        }), -1)
    }
}
defineType(ScrollBar, {})

class ScrollIndicator extends T.ScrollIndicator {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
        this.padding = 2
    }
    $makeStyle(){
        let c = this
        let handle = this.$styleDelegate(T.Control.CONTENTITEM, () => make(Rectangle, null, {
            implicitWidth: 2,
            implicitHeight: 2,
            color: () => c.palette.mid,
            visible: () => c.size < 1.0,
        }))
        if(handle) activeOpacity(handle, handle, () => c.active)
        this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, {
            visible: false,
            opacity: () => (c.contentItem ? c.contentItem.opacity : 0),
            color: () => c.palette.mid,
        }), -1)
    }
}
defineType(ScrollIndicator, {})

// ---- Slider, RangeSlider ----
// the groove of a slider and the part of it up to the value (Slider.qml / RangeSlider.qml)
function sliderGroove(c, from, to){
    let groove = make(Rectangle, null, {
        implicitWidth: () => (c.horizontal ? 200 : 6),
        implicitHeight: () => (c.horizontal ? 6 : 200),
        radius: 3,
        color: () => c.palette.midlight,
        scale: () => (c.horizontal && c.mirrored ? -1 : 1),
        'border.width': 0,
        'border.color': () => (c.enabled ? c.palette.dark : c.palette.mid),
    })
    groove.$bind(Item.X, () => c.leftPadding + (c.horizontal ? 0 : (c.availableWidth - groove.width) / 2), null, groove)
    groove.$bind(Item.Y, () => c.topPadding + (c.horizontal ? (c.availableHeight - groove.height) / 2 : 0), null, groove)
    groove.$bind(Item.WIDTH, () => (c.horizontal ? c.availableWidth : groove.implicitWidth), null, groove)
    groove.$bind(Item.HEIGHT, () => (c.horizontal ? groove.implicitHeight : c.availableHeight), null, groove)
    let fill = make(Rectangle, groove, { radius: () => (from ? 0 : 3), color: () => c.palette.dark })
    if(from){
        // RangeSlider: between the handles
        fill.$bind(Item.X, () => (c.horizontal ? c.first.position * groove.width + 3 : 0), null, fill)
        fill.$bind(Item.Y, () => (c.horizontal ? 0 : c.second.visualPosition * groove.height + 3), null, fill)
        fill.$bind(Item.WIDTH, () => (c.horizontal ? c.second.position * groove.width - c.first.position * groove.width - 6 : 6), null, fill)
        fill.$bind(Item.HEIGHT, () => (c.horizontal ? 6 : c.second.position * groove.height - c.first.position * groove.height - 6), null, fill)
    } else {
        fill.$bind(Item.Y, () => (c.horizontal ? 0 : c.visualPosition * groove.height), null, fill)
        fill.$bind(Item.WIDTH, () => (c.horizontal ? c.position * groove.width : 6), null, fill)
        fill.$bind(Item.HEIGHT, () => (c.horizontal ? 6 : c.position * groove.height), null, fill)
    }
    return groove
}
// the round handle at a visual position
function sliderHandle(c, node){
    let visual = node ? () => node.visualPosition : () => c.visualPosition
    let pressed = node ? () => node.pressed : () => c.pressed
    let handle = make(Rectangle, null, {
        implicitWidth: 28,
        implicitHeight: 28,
        color: () => (pressed() ? c.palette.light : c.palette.window),
    })
    handle.$bind(Rectangle.RADIUS, () => handle.width / 2, null, handle)
    if(node){
        handle.border.$bind(Border.WIDTH, () => (c.activeFocus ? 2 : 1), null, handle)
        handle.border.$bind(Border.COLOR, () => c.handleBorderColor, null, handle)
    } else {
        handle.border.$bind(Border.WIDTH, () => (c.visualFocus ? 2 : 1), null, handle)
        handle.border.$bind(Border.COLOR, () => (handle.activeFocus ? c.palette.highlight : c.enabled ? c.palette.mid : c.palette.midlight), null, handle)
    }
    handle.$bind(Item.X, () => c.leftPadding + (c.horizontal ? visual() * (c.availableWidth - handle.width) : (c.availableWidth - handle.width) / 2), null, handle)
    handle.$bind(Item.Y, () => c.topPadding + (c.horizontal ? (c.availableHeight - handle.height) / 2 : visual() * (c.availableHeight - handle.height)), null, handle)
    return handle
}

class Slider extends T.Slider {
    constructor(parent = null){
        super(parent)
        this.$bind(Item.IMPLICITWIDTH, () => Math.max(this.implicitBackgroundWidth + this.leftInset + this.rightInset, this.implicitHandleWidth + this.leftPadding + this.rightPadding), null, this)
        this.$bind(Item.IMPLICITHEIGHT, () => Math.max(this.implicitBackgroundHeight + this.topInset + this.bottomInset, this.implicitHandleHeight + this.topPadding + this.bottomPadding), null, this)
        this.padding = 6
    }
    $makeStyle(){
        this.$styleDelegate(T.Slider.HANDLE, () => sliderHandle(this, null))
        this.$styleDelegate(T.Control.BACKGROUND, () => sliderGroove(this, false), -1)
    }
}
defineType(Slider, {})

class RangeSlider extends T.RangeSlider {
    constructor(parent = null){
        super(parent)
        this.$bind(Item.IMPLICITWIDTH, () => Math.max(this.implicitBackgroundWidth + this.leftInset + this.rightInset,
            this.first.implicitHandleWidth + this.leftPadding + this.rightPadding, this.second.implicitHandleWidth + this.leftPadding + this.rightPadding), null, this)
        this.$bind(Item.IMPLICITHEIGHT, () => Math.max(this.implicitBackgroundHeight + this.topInset + this.bottomInset,
            this.first.implicitHandleHeight + this.topPadding + this.bottomPadding, this.second.implicitHandleHeight + this.topPadding + this.bottomPadding), null, this)
        this.padding = 6
        // readonly property color handleBorderColor
        this.$bind(RangeSlider.HANDLEBORDERCOLOR, () => (this.activeFocus ? this.palette.highlight : this.enabled ? this.palette.mid : this.palette.midlight), null, this)
    }
    $makeStyle(){
        for(let node of [this.first, this.second]){
            if(node.handle) continue
            let handle = sliderHandle(this, node)
            if(!this.$defaults) this.$defaults = new Set()
            this.$defaults.add(handle)
            node.$setHandle(handle)
        }
        this.$styleDelegate(T.Control.BACKGROUND, () => sliderGroove(this, true), -1)
    }
}
defineType(RangeSlider, { props: { handleBorderColor: { kind: 'color', readonly: true } } })

// ---- ProgressBar, BusyIndicator ----
// ProgressBarImpl (QQuickBasicProgressBar): the part up to the progress; indeterminate: a block that runs along it
class ProgressBarImpl extends Item {
    constructor(parent = null){
        super(parent)
        this.$fill = make(Rectangle, this, {})
        this.$fill.$bind(Rectangle.COLOR, () => this.color, null, this.$fill)
        this.$fill.$bind(Item.HEIGHT, () => this.height, null, this.$fill)
        this.$fill.$bind(Item.WIDTH, () => (this.indeterminate ? Math.min(this.width, 12) : this.progress * this.width), null, this.$fill)
        this.$timer = null
        watch(this, () => this.indeterminate && this.visible, (on) => this.$run(on))
    }
    $run(on){
        if(this.$timer){ clearInterval(this.$timer); this.$timer = null }
        if(!on){ this.$fill.$write(Item.X, 0); return }
        let start = Date.now()
        this.$timer = setInterval(() => {
            if(this.$f & engine.DESTROYED) return clearInterval(this.$timer)
            let t = ((Date.now() - start) % 4000) / 4000
            engine.batch(() => this.$fill.$write(Item.X, t * Math.max(0, this.width - this.$fill.width)))
        }, 32)
    }
}
defineType(ProgressBarImpl, { props: { progress: 'real', indeterminate: 'bool', color: 'color' } })

class ProgressBar extends T.ProgressBar {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
    }
    $makeStyle(){
        let c = this
        this.$styleDelegate(T.Control.CONTENTITEM, () => make(ProgressBarImpl, null, {
            implicitHeight: 6,
            implicitWidth: 116,
            scale: () => (c.mirrored ? -1 : 1),
            progress: () => c.position,
            indeterminate: () => c.visible && c.indeterminate,
            color: () => c.palette.dark,
        }))
        let background = this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, {
            implicitWidth: 200,
            implicitHeight: 6,
            height: 6,
            color: () => c.palette.midlight,
        }), -1)
        if(background) background.$bind(Item.Y, () => (c.height - background.height) / 2, null, background)
    }
}
defineType(ProgressBar, {})

// BusyIndicatorImpl (QQuickBasicBusyIndicator): ten circles round a ring, filled one after another and emptied again
class BusyIndicatorImpl extends Item {
    constructor(parent = null){
        super(parent)
        this.$circles = []
        for(let i = 0; i < 10; i++){
            let circle = make(Rectangle, this, { 'border.width': 1 })
            circle.border.$bind(Border.COLOR, () => this.pen, null, circle)
            this.$circles.push(circle)
        }
        this.$phase = 0
        this.$timer = null
        engine.connect(this, Item.SIGNAL_widthChanged, () => this.$layoutCircles(), null, engine.PRIORITY_NORMAL, this)
        engine.connect(this, Item.SIGNAL_heightChanged, () => this.$layoutCircles(), null, engine.PRIORITY_NORMAL, this)
        watch(this, () => this.running && this.visible, (on) => this.$run(on))
        watch(this, () => [this.fill, this.pen], () => this.$paintCircles())
    }
    $layoutCircles(){
        let size = Math.min(this.$v[Item.WIDTH], this.$v[Item.HEIGHT])
        let d = size / 5.6
        let r = (size - d) / 2
        let cx = this.$v[Item.WIDTH] / 2, cy = this.$v[Item.HEIGHT] / 2
        this.$circles.forEach((circle, i) => {
            let a = (i / 10) * 2 * Math.PI - Math.PI / 2
            circle.$write(Item.X, cx + r * Math.cos(a) - d / 2)
            circle.$write(Item.Y, cy + r * Math.sin(a) - d / 2)
            circle.$write(Item.WIDTH, d)
            circle.$write(Item.HEIGHT, d)
            circle.$write(Rectangle.RADIUS, d / 2)
        })
    }
    // phase 0..19: the first ten fill the circles one by one, the next ten empty them
    $paintCircles(){
        let phase = this.$phase
        this.$circles.forEach((circle, i) => {
            let filled = phase < 10 ? i <= phase : i > phase - 10
            circle.$write(Rectangle.COLOR, filled ? this.fill : 'transparent')
        })
    }
    $run(on){
        if(this.$timer){ clearInterval(this.$timer); this.$timer = null }
        if(!on) return
        this.$timer = setInterval(() => {
            if(this.$f & engine.DESTROYED) return clearInterval(this.$timer)
            this.$phase = (this.$phase + 1) % 20
            engine.batch(() => this.$paintCircles())
        }, 100)
    }
}
defineType(BusyIndicatorImpl, { props: { pen: 'color', fill: 'color', running: 'bool' } })

class BusyIndicator extends T.BusyIndicator {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
        this.padding = 6
    }
    $makeStyle(){
        let c = this
        let busy = this.$styleDelegate(T.Control.CONTENTITEM, () => make(BusyIndicatorImpl, null, {
            implicitWidth: 48,
            implicitHeight: 48,
            pen: () => c.palette.dark,
            fill: () => c.palette.dark,
            running: () => c.running,
        }))
        if(busy){
            // opacity: running ? 1 : 0, Behavior on opacity { OpacityAnimator { duration: 250 } }
            let behavior = new Behavior(busy)
            make(require('../NumberAnimation').NumberAnimation, behavior, { duration: 250 })
            behavior.$setTargetProperty(busy, 'opacity')
            busy.$bind(Item.OPACITY, () => (c.running ? 1 : 0), null, busy)
        }
    }
}
defineType(BusyIndicator, {})

// ---- Dial: DialImpl (QQuickBasicDial: an 8 px arc up to the progress in a 1 px circle) and the dial indicator ----
class DialImpl extends Item {
    constructor(parent = null){
        super(parent)
        // the painting: a vector of the arc and the circle, the size of the item
        let picture = this.$picture = new Image(this)
        picture.$bind(Item.WIDTH, () => this.width, null, picture)
        picture.$bind(Item.HEIGHT, () => this.height, null, picture)
        picture.$bind(Image.SOURCE, () => dialSvg(this.width, this.height, this.progress, this.color, this.startAngle, this.endAngle), null, picture)
    }
}
defineType(DialImpl, { props: { progress: 'real', color: 'color', startAngle: 'real', endAngle: 'real' } })
function dialSvg(w, h, progress, color, startAngle, endAngle){
    let size = Math.min(w, h)
    if(size <= 0) return ''
    let pen = 8
    let r = (size - pen - 2) / 2
    let cx = w / 2, cy = h / 2
    let point = (deg, radius) => [cx + radius * Math.sin(deg * Math.PI / 180), cy - radius * Math.cos(deg * Math.PI / 180)]
    let end = startAngle + progress * (endAngle - startAngle)
    let [x0, y0] = point(startAngle, r), [x1, y1] = point(end, r)
    let large = Math.abs(end - startAngle) > 180 ? 1 : 0
    let arc = progress > 0 ? '<path d="M' + x0 + ' ' + y0 + 'A' + r + ' ' + r + ' 0 ' + large + ' 1 ' + x1 + ' ' + y1 + '" fill="none" stroke="' + color + '" stroke-width="' + pen + '"/>' : ''
    return 'data:image/svg+xml,' + encodeURIComponent('<svg xmlns="http://www.w3.org/2000/svg" width="' + w + '" height="' + h + '" viewBox="0 0 ' + w + ' ' + h + '">'
        + arc + '<circle cx="' + cx + '" cy="' + cy + '" r="' + (r + pen / 2 - 0.5) + '" fill="none" stroke="' + color + '" stroke-width="1"/></svg>')
}
class Dial extends T.Dial {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
    }
    $makeStyle(){
        let c = this
        let background = this.$styleDelegate(T.Control.BACKGROUND, () => {
            let dial = make(DialImpl, null, {
                implicitWidth: 184,
                implicitHeight: 184,
                color: () => (c.visualFocus ? c.palette.highlight : c.palette.dark),
                progress: () => c.position,
                opacity: () => (c.enabled ? 1 : 0.3),
                startAngle: () => c.startAngle,
                endAngle: () => c.endAngle,
            })
            return dial
        }, -1)
        let handle = this.$styleDelegate(T.Dial.HANDLE, () => make(ColorImage, null, {
            width: 14,
            height: 10,
            defaultColor: '#353637',
            color: () => (c.visualFocus ? c.palette.highlight : c.palette.dark),
            opacity: () => (c.enabled ? 1 : 0.3),
        }))
        if(handle){
            handle.$bind(Image.SOURCE, () => svg(14, 10, '<path d="M0.5 0.5H13.5L7 9.5Z" fill="' + handle.color + '"/>'), null, handle)
            handle.$bind(Item.X, () => (c.background ? c.background.x + c.background.width / 2 - handle.width / 2 : 0), null, handle)
            handle.$bind(Item.Y, () => (c.background ? c.background.y + c.background.height / 2 - handle.height / 2 : 0), null, handle)
            // Translate { y: -min(background size) * 0.4 + height / 2 } then Rotation { angle } about the middle
            handle.$bind(Item.ROTATION, () => c.angle, null, handle)
            handle.$dialOffset = () => (c.background ? -Math.min(c.background.width, c.background.height) * 0.4 + handle.height / 2 : 0)
            watch(handle, () => [handle.$dialOffset(), c.angle], () => this.$placeHandle(handle))
            this.$placeHandle(handle)
        }
    }
    // the rotation of the handle about its middle after its translation (CSS: translate then rotate about the middle)
    $placeHandle(handle){
        if(!handle.$dom) return
        let offset = handle.$dialOffset()
        handle.$dom.style.transformOrigin = '50% ' + (50 - offset / handle.height * 100) + '%'
        handle.$dom.style.translate = '0px ' + offset + 'px'
    }
}
defineType(Dial, {})

// ---- PageIndicator: a Row of the delegate (8 px dots, the current one darker) ----
// the dot of the style: Rectangle { required property int index; opacity: current 0.95, pressed 0.7, else 0.45 }
class PageDot extends Rectangle {}
defineType(PageDot, { props: { index: 'int', pressed: 'bool' } })
function pageDot(c){
    return new (require('../../QtQml/Component').Component)(null, (parent, ctx) => {
        let dot = make(PageDot, parent, { implicitWidth: 8, implicitHeight: 8, color: () => c.palette.dark })
        if(ctx && ctx.index !== undefined) dot.index = ctx.index
        dot.$bind(Rectangle.RADIUS, () => dot.width / 2, null, dot)
        dot.$bind(Item.OPACITY, () => (dot.index === c.currentIndex ? 0.95 : dot.pressed ? 0.7 : 0.45), null, dot)
        return dot
    })
}
class PageIndicator extends T.PageIndicator {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
        this.padding = 6
        this.spacing = 6
    }
    $makeStyle(){
        let c = this
        if(!this.$v[T.PageIndicator.DELEGATE]) this.$plain(T.PageIndicator.DELEGATE, pageDot(this))
        // contentItem: Row { spacing; Repeater { model: control.count; delegate: control.delegate } }
        this.$styleDelegate(T.Control.CONTENTITEM, () => {
            let row = make(require('../Row').Row, null, { spacing: () => c.spacing })
            make(require('../Repeater').Repeater, row, { model: () => c.count, delegate: () => c.delegate })
            // QQuickBasePositioner::componentComplete: the row is laid out when it is made
            row.forceLayout()
            return row
        })
    }
}
defineType(PageIndicator, {})

// ---- SpinBox ----
class SpinBox extends T.SpinBox {
    constructor(parent = null){
        super(parent)
        let c = this
        this.$bind(Item.IMPLICITWIDTH, () => Math.max(c.implicitBackgroundWidth + c.leftInset + c.rightInset, (c.contentItem ? c.contentItem.implicitWidth : 0) + c.leftPadding + c.rightPadding), null, this)
        this.$bind(Item.IMPLICITHEIGHT, () => Math.max(c.implicitBackgroundHeight + c.topInset + c.bottomInset, c.implicitContentHeight + c.topPadding + c.bottomPadding,
            c.up.implicitIndicatorHeight, c.down.implicitIndicatorHeight), null, this)
        this.$bind(T.Control.LEFTPADDING, () => c.padding + (c.mirrored ? (c.up.indicator ? c.up.indicator.width : 0) : (c.down.indicator ? c.down.indicator.width : 0)), null, this)
        this.$bind(T.Control.RIGHTPADDING, () => c.padding + (c.mirrored ? (c.down.indicator ? c.down.indicator.width : 0) : (c.up.indicator ? c.up.indicator.width : 0)), null, this)
    }
    $makeStyle(){
        let c = this
        if(!this.$v[T.SpinBox.VALIDATOR]){
            let validator = make(require('../IntValidator').IntValidator, this, {
                bottom: () => Math.min(c.from, c.to),
                top: () => Math.max(c.from, c.to),
            })
            this.$plain(T.SpinBox.VALIDATOR, validator)
        }
        let indicator = (button, plus) => {
            let r = make(Rectangle, null, {
                implicitWidth: 40,
                implicitHeight: 40,
                color: () => (button.pressed ? c.palette.mid : c.palette.button),
                'border.width': 0,
                'border.color': () => (c.enabled ? c.palette.text : c.palette.mid),
            })
            r.$bind(Item.HEIGHT, () => c.height, null, r)
            r.$bind(Item.X, plus ? () => (c.mirrored ? 0 : c.width - r.width) : () => (c.mirrored ? c.width - r.width : 0), null, r)
            let bar = (horizontal) => {
                let b = make(Rectangle, r, { color: () => (c.enabled ? c.palette.buttonText : c.palette.mid) })
                b.$bind(Item.WIDTH, () => (horizontal ? r.width / 3 : 2), null, b)
                b.$bind(Item.HEIGHT, () => (horizontal ? 2 : r.width / 3), null, b)
                b.$bind(Item.X, () => (r.width - b.width) / 2, null, b)
                b.$bind(Item.Y, () => (r.height - b.height) / 2, null, b)
            }
            bar(true)
            if(plus) bar(false)
            return r
        }
        for(let [button, plus] of [[this.up, true], [this.down, false]]){
            if(button.indicator) continue
            let item = indicator(button, plus)
            if(!this.$defaults) this.$defaults = new Set()
            this.$defaults.add(item)
            button.$write(T.IndicatorButton.INDICATOR, item)
        }
        this.$styleDelegate(T.Control.CONTENTITEM, () => {
            let input = make(require('../TextInput').TextInput, null, {
                z: 2,
                text: () => c.displayText,
                padding: 6,
                color: () => c.palette.text,
                selectionColor: () => c.palette.highlight,
                selectedTextColor: () => c.palette.highlightedText,
                horizontalAlignment: 4,
                verticalAlignment: 128,
                readOnly: () => !c.editable,
            })
            input.$bind(Item.CLIP, () => input.width < input.implicitWidth, null, input)
            c.font.$follow(input)
            let frame = make(Rectangle, input, { color: 'transparent', 'border.width': 2, visible: () => c.activeFocus })
            frame.border.$bind(Border.COLOR, () => c.palette.highlight, null, frame)
            frame.$bind(Item.WIDTH, () => input.width, null, frame)
            frame.$bind(Item.HEIGHT, () => input.height, null, frame)
            return input
        })
        this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, {
            implicitWidth: 140,
            color: () => (c.enabled ? c.palette.base : c.palette.button),
            'border.color': () => c.palette.button,
        }), -1)
    }
}
defineType(SpinBox, {})

// ---- Pane, Frame, GroupBox, Page, ToolBar, ToolSeparator, MenuSeparator, Container, TabBar, SwipeView ----
class Pane extends T.Pane {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
        this.padding = 12
    }
    $makeStyle(){ this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, { color: () => this.palette.window }), -1) }
}
defineType(Pane, {})

const frameBackground = (c) => make(Rectangle, null, { color: 'transparent', 'border.color': () => c.palette.mid })
class Frame extends T.Frame {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
        this.padding = 12
    }
    $makeStyle(){ this.$styleDelegate(T.Control.BACKGROUND, () => frameBackground(this), -1) }
}
defineType(Frame, {})

class GroupBox extends T.GroupBox {
    constructor(parent = null){
        super(parent)
        let c = this
        this.$bind(Item.IMPLICITWIDTH, () => Math.max(c.implicitBackgroundWidth + c.leftInset + c.rightInset, c.implicitContentWidth + c.leftPadding + c.rightPadding,
            c.implicitLabelWidth + c.leftPadding + c.rightPadding), null, this)
        this.$bind(Item.IMPLICITHEIGHT, () => Math.max(c.implicitBackgroundHeight + c.topInset + c.bottomInset, c.implicitContentHeight + c.topPadding + c.bottomPadding), null, this)
        this.spacing = 6
        this.padding = 12
        this.$bind(T.Control.TOPPADDING, () => c.padding + (c.implicitLabelWidth > 0 ? c.implicitLabelHeight + c.spacing : 0), null, this)
    }
    $makeStyle(){
        let c = this
        if(!this.$v[T.GroupBox.LABEL]){
            let label = make(Text, null, {
                x: () => c.leftPadding,
                width: () => c.availableWidth,
                text: () => c.title,
                color: () => c.palette.windowText,
                elide: Text.ElideRight,
                verticalAlignment: Text.AlignVCenter,
            })
            c.font.$follow(label)
            if(!this.$defaults) this.$defaults = new Set()
            this.$defaults.add(label)
            this.$setLabel(label)
        }
        let background = this.$styleDelegate(T.Control.BACKGROUND, () => frameBackground(c), -1)
        if(background){
            background.$ownSize = 3
            background.$bind(Item.Y, () => c.topPadding - c.bottomPadding, null, background)
            background.$bind(Item.WIDTH, () => c.width, null, background)
            background.$bind(Item.HEIGHT, () => c.height - c.topPadding + c.bottomPadding, null, background)
        }
    }
}
defineType(GroupBox, {})

class Page extends T.Page {
    constructor(parent = null){
        super(parent)
        let c = this
        this.$bind(Item.IMPLICITWIDTH, () => Math.max(c.implicitBackgroundWidth + c.leftInset + c.rightInset, c.implicitContentWidth + c.leftPadding + c.rightPadding,
            c.implicitHeaderWidth, c.implicitFooterWidth), null, this)
        this.$bind(Item.IMPLICITHEIGHT, () => Math.max(c.implicitBackgroundHeight + c.topInset + c.bottomInset, c.implicitContentHeight + c.topPadding + c.bottomPadding
            + (c.implicitHeaderHeight > 0 ? c.implicitHeaderHeight + c.spacing : 0) + (c.implicitFooterHeight > 0 ? c.implicitFooterHeight + c.spacing : 0)), null, this)
    }
    $makeStyle(){ this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, { color: () => this.palette.window }), -1) }
}
defineType(Page, {})

class ToolBar extends T.ToolBar {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
        // topPadding: SafeArea.margins.top ... (no safe area on a page)
        for(let id of [T.Control.TOPPADDING, T.Control.LEFTPADDING, T.Control.RIGHTPADDING, T.Control.BOTTOMPADDING]) this.$write(id, 0)
    }
    $makeStyle(){ this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, { implicitHeight: 40, color: () => this.palette.button }), -1) }
}
defineType(ToolBar, {})

class ToolSeparator extends T.ToolSeparator {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
        this.$bind(T.Control.PADDING, () => (this.vertical ? 6 : 2), null, this)
        this.$bind(T.Control.VERTICALPADDING, () => (this.vertical ? 2 : 6), null, this)
    }
    $makeStyle(){
        let c = this
        this.$styleDelegate(T.Control.CONTENTITEM, () => make(Rectangle, null, {
            implicitWidth: () => (c.vertical ? 1 : 30),
            implicitHeight: () => (c.vertical ? 30 : 1),
            color: () => c.palette.mid,
        }))
    }
}
defineType(ToolSeparator, {})

class MenuSeparator extends T.MenuSeparator {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
        this.padding = 2
        this.$bind(T.Control.VERTICALPADDING, () => this.padding + 4, null, this)
    }
    $makeStyle(){
        this.$styleDelegate(T.Control.CONTENTITEM, () => make(Rectangle, null, { implicitWidth: 188, implicitHeight: 1, color: () => this.palette.mid }))
    }
}
defineType(MenuSeparator, {})

class Container extends T.Container {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
    }
}
defineType(Container, {})

const { ListView } = require('../ListView')
class TabBar extends T.TabBar {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
        this.spacing = 1
    }
    $makeStyle(){
        let c = this
        this.$styleDelegate(T.Control.CONTENTITEM, () => {
            let view = make(ListView, null, {
                model: () => c.contentModel,
                currentIndex: () => c.currentIndex,
                spacing: () => c.spacing,
                orientation: ListView.Horizontal,
                boundsBehavior: 0,
                flickableDirection: 0xc,
                snapMode: ListView.SnapToItem,
                highlightMoveDuration: 0,
                highlightRangeMode: ListView.ApplyRange,
                preferredHighlightBegin: 40,
            })
            view.$bind(ListView.PREFERREDHIGHLIGHTEND, () => view.width - 40, null, view)
            return view
        })
        this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, { color: () => c.palette.window }), -1)
    }
}
defineType(TabBar, {})

class SwipeView extends T.SwipeView {
    constructor(parent = null){
        super(parent)
        implicitSize(this)
    }
    $makeStyle(){
        let c = this
        this.$styleDelegate(T.Control.CONTENTITEM, () => make(ListView, null, {
            model: () => c.contentModel,
            interactive: () => c.interactive,
            currentIndex: () => c.currentIndex,
            focus: () => c.focus,
            spacing: () => c.spacing,
            orientation: () => c.orientation,
            snapMode: ListView.SnapOneItem,
            boundsBehavior: 0,
            highlightRangeMode: ListView.StrictlyEnforceRange,
            preferredHighlightBegin: 0,
            preferredHighlightEnd: 0,
            highlightMoveDuration: 250,
            maximumFlickVelocity: () => 4 * (c.orientation === 1 ? c.width : c.height),
        }))
    }
}
defineType(SwipeView, {})

// ---- TextField, TextArea ----
// PlaceholderText (QQuickPlaceholderText): the placeholder of a text control, shown while it is empty
function placeholder(c, wrap){
    let text = make(Text, c, {
        x: () => c.leftPadding,
        y: () => c.topPadding,
        width: () => c.width - (c.leftPadding + c.rightPadding),
        height: () => c.height - (c.topPadding + c.bottomPadding),
        text: () => c.placeholderText,
        color: () => c.placeholderTextColor,
        verticalAlignment: () => c.verticalAlignment,
        visible: () => !c.length && !c.preeditText && (!c.activeFocus || c.horizontalAlignment !== 4),
        elide: Text.ElideRight,
    })
    if(wrap) text.$bind(Text.WRAPMODE, () => c.wrapMode, null, text)
    text.$delegateOf = c
    c.font.$follow(text)
    return text
}
function textColors(c){
    c.$bind(T.TextField.COLOR, () => c.palette.text, null, c)
    c.$bind(T.TextField.SELECTIONCOLOR, () => c.palette.highlight, null, c)
    c.$bind(T.TextField.SELECTEDTEXTCOLOR, () => c.palette.highlightedText, null, c)
    c.$bind(T.TextField.PLACEHOLDERTEXTCOLOR, () => c.palette.placeholderText, null, c)
}

class TextField extends T.TextField {
    constructor(parent = null){
        super(parent)
        let c = this
        this.$styleSize = true
        this.padding = 6
        this.$bind(T.TextField.LEFTPADDING, () => c.padding + 4, null, this)
        textColors(this)
        this.verticalAlignment = 128
        this.$placeholder = placeholder(this, false)
        let p = this.$placeholder
        this.$bind(Item.IMPLICITWIDTH, () => (c.implicitBackgroundWidth + c.leftInset + c.rightInset)
            || Math.max(c.contentWidth, p.implicitWidth) + c.leftPadding + c.rightPadding, null, this)
        this.$bind(Item.IMPLICITHEIGHT, () => Math.max(c.implicitBackgroundHeight + c.topInset + c.bottomInset,
            c.contentHeight + c.topPadding + c.bottomPadding, p.implicitHeight + c.topPadding + c.bottomPadding), null, this)
        engine.scheduleComponentComplete(this)
    }
    $componentComplete(){
        super.$componentComplete()
        if(this.$v[T.TextField.BACKGROUND]) return
        let c = this
        let background = make(Rectangle, null, {
            implicitWidth: 200,
            implicitHeight: 40,
            color: () => c.palette.base,
            'border.width': () => (c.activeFocus ? 2 : 1),
            'border.color': () => (c.activeFocus ? c.palette.highlight : c.palette.mid),
        })
        if(!this.$defaults) this.$defaults = new Set()
        this.$defaults.add(background)
        this.$setBackground(background)
    }
}
defineType(TextField, {})

class TextArea extends T.TextArea {
    constructor(parent = null){
        super(parent)
        let c = this
        this.$styleSize = true
        this.padding = 6
        this.$bind(T.TextArea.LEFTPADDING, () => c.padding + 4, null, this)
        c.$bind(T.TextArea.COLOR, () => c.palette.text, null, c)
        c.$bind(T.TextArea.SELECTIONCOLOR, () => c.palette.highlight, null, c)
        c.$bind(T.TextArea.SELECTEDTEXTCOLOR, () => c.palette.highlightedText, null, c)
        c.$bind(T.TextArea.PLACEHOLDERTEXTCOLOR, () => c.palette.placeholderText, null, c)
        this.$placeholder = placeholder(this, true)
        let p = this.$placeholder
        this.$bind(Item.IMPLICITWIDTH, () => Math.max(c.contentWidth + c.leftPadding + c.rightPadding, c.implicitBackgroundWidth + c.leftInset + c.rightInset,
            p.implicitWidth + c.leftPadding + c.rightPadding), null, this)
        this.$bind(Item.IMPLICITHEIGHT, () => Math.max(c.contentHeight + c.topPadding + c.bottomPadding, c.implicitBackgroundHeight + c.topInset + c.bottomInset,
            p.implicitHeight + c.topPadding + c.bottomPadding), null, this)
        engine.scheduleComponentComplete(this)
    }
    $componentComplete(){
        super.$componentComplete()
        if(this.$v[T.TextArea.BACKGROUND]) return
        let c = this
        let background = make(Rectangle, null, {
            visible: false,
            color: 'transparent',
            'border.width': () => (c.activeFocus ? 2 : 1),
            'border.color': () => (c.activeFocus ? c.palette.highlight : c.enabled ? c.palette.mid : c.palette.midlight),
        })
        if(!this.$defaults) this.$defaults = new Set()
        this.$defaults.add(background)
        this.$setBackground(background)
    }
}
defineType(TextArea, {})

// ---- Popup ----
const { transparent } = require('./impl')
// T.Overlay.modal / modeless: the dimming of the window under a popup
function dimmer(c, modal){
    return new (require('../../QtQml/Component').Component)(null, (parent) => make(Rectangle, parent, {
        color: () => transparent(c.palette.shadow, modal ? 0.5 : 0.12),
    }))
}
function popupStyle(c, background){
    implicitSize(c.$item)
    c.$styleDimmer = (modal) => dimmer(c, modal)
    let item = c.$item
    item.$makeStyle = () => {}
    if(background) item.$styleDelegate(T.Control.BACKGROUND, background, -1)
}
class Popup extends T.Popup {
    constructor(parent = null){
        super(parent)
        this.padding = 12
    }
    $makeStyle(){
        popupStyle(this, () => make(Rectangle, null, { color: () => this.palette.window, 'border.color': () => this.palette.dark }))
    }
}
defineType(Popup, {})

// ---- ComboBox ----
// the ListView of the popup: its delegates take the row on a click and highlight it on hover (QQuickComboBoxPrivate::createdItem)
class ComboListView extends ListView {
    $create(index, rows){
        let entry = super.$create(index, rows)
        if(entry && this.$combo) this.$combo.$connectDelegate(entry.item, () => entry.ctx.index)
        return entry
    }
}
defineType(ComboListView, {})
// delegate: ItemDelegate { width: ListView.view.width; text: model[control.textRole]; font.weight; highlighted; hoverEnabled }
function comboDelegate(c){
    return new (require('../../QtQml/Component').Component)(null, (parent, ctx) => {
        let d = new ItemDelegate(parent)
        let index = () => ctx.index
        apply(d, {
            width: () => (ctx.$view ? ctx.$view.width : 0),
            text: () => c.textAt(index()),
            'palette.text': () => c.palette.text,
            'palette.highlightedText': () => c.palette.highlightedText,
            'font.weight': () => (c.currentIndex === index() ? 600 : 400),
            highlighted: () => c.highlightedIndex === index(),
            hoverEnabled: () => c.hoverEnabled,
        })
        return d
    })
}
class ComboBox extends T.ComboBox {
    constructor(parent = null){
        super(parent)
        let c = this
        implicitSize(this, true)
        this.$bind(T.Control.LEFTPADDING, () => c.padding + (!c.mirrored || !c.indicator || !c.indicator.visible ? 0 : c.indicator.width + c.spacing), null, this)
        this.$bind(T.Control.RIGHTPADDING, () => c.padding + (c.mirrored || !c.indicator || !c.indicator.visible ? 0 : c.indicator.width + c.spacing), null, this)
    }
    $makeStyle(){
        let c = this
        if(!this.$v[T.ComboBox.DELEGATE]) this.$plain(T.ComboBox.DELEGATE, comboDelegate(this))
        let indicator = this.$styleDelegate(T.ComboBox.INDICATOR, () => styleImage('double-arrow', null, () => c.palette.dark, { opacity: () => (c.enabled ? 1 : 0.3) }))
        if(indicator){
            indicator.$bind(Item.X, () => (c.mirrored ? c.padding : c.width - indicator.width - c.padding), null, indicator)
            indicator.$bind(Item.Y, () => c.topPadding + (c.availableHeight - indicator.height) / 2, null, indicator)
            // the size of double-arrow.png (an image loaded later, or never without a page, has it already)
            indicator.$write(Item.IMPLICITWIDTH, 40)
            indicator.$write(Item.IMPLICITHEIGHT, 28)
        }
        this.$styleDelegate(T.Control.CONTENTITEM, () => {
            let field = new T.TextField(null)
            apply(field, {
                leftPadding: () => (!c.mirrored ? 12 : c.editable && field.activeFocus ? 3 : 1),
                rightPadding: () => (c.mirrored ? 12 : c.editable && field.activeFocus ? 3 : 1),
                topPadding: () => 6 - c.padding,
                bottomPadding: () => 6 - c.padding,
                text: () => (c.editable ? c.editText : c.displayText),
                enabled: () => c.editable,
                autoScroll: () => c.editable,
                readOnly: () => c.down,
                inputMethodHints: () => c.inputMethodHints,
                validator: () => c.validator,
                selectByMouse: () => c.selectTextByMouse,
                color: () => (c.editable ? c.palette.text : c.palette.buttonText),
                selectionColor: () => c.palette.highlight,
                selectedTextColor: () => c.palette.highlightedText,
                verticalAlignment: 128,
            })
            field.$bind(Item.IMPLICITHEIGHT, () => field.contentHeight + field.topPadding + field.bottomPadding, null, field)
            field.$styleSize = true
            c.font.$follow(field)
            let background = make(Rectangle, null, {
                visible: () => c.enabled && c.editable && !c.flat,
                'border.width': () => (field.activeFocus ? 2 : 1),
                'border.color': () => (field.activeFocus ? c.palette.highlight : c.palette.button),
                color: () => c.palette.base,
            })
            field.$setBackground(background)
            return field
        })
        this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, {
            implicitWidth: 140,
            implicitHeight: 40,
            color: () => (c.down ? c.palette.mid : c.palette.button),
            'border.color': () => (!c.editable && c.visualFocus ? c.palette.highlight : c.palette.buttonText),
            'border.width': () => (!c.editable && c.visualFocus ? 2 : 0),
            visible: () => !c.flat || c.down,
        }), -1)
        if(!this.$v[T.ComboBox.POPUP]){
            let popup = new T.Popup(this)
            let view = new ComboListView(null)
            view.$combo = this
            apply(view, {
                clip: true,
                model: () => c.model,
                delegate: () => c.delegate,
                currentIndex: () => c.highlightedIndex,
                highlightMoveDuration: 0,
            })
            view.$bind(Item.IMPLICITHEIGHT, () => view.contentHeight, null, view)
            let frame = make(Rectangle, view, { z: 10, color: 'transparent', 'border.color': () => c.palette.mid })
            frame.$bind(Item.WIDTH, () => view.width, null, frame)
            frame.$bind(Item.HEIGHT, () => view.height, null, frame)
            let indicator = new ScrollIndicator(null)
            view.ScrollIndicator.vertical = indicator
            popup.contentItem = view
            popup.$item.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, { color: () => c.palette.window }), -1)
            apply(popup, {
                y: () => c.height,
                width: () => c.width,
                height: () => Math.min(view.implicitHeight, (popup.$overlay() ? popup.$overlay().height : Infinity) - popup.topMargin - popup.bottomMargin),
                topMargin: 6,
                bottomMargin: 6,
            })
            this.$setPopup(popup)
        }
    }
}
defineType(ComboBox, {})

// ---- ToolTip, DialogButtonBox, Dialog, Menu, MenuItem, Drawer ----
class ToolTip extends T.ToolTip {
    constructor(parent = null){
        super(parent)
        let c = this
        this.$bind(T.Popup.X, () => (c.parent && c.parent.$isItem ? (c.parent.width - c.implicitWidth) / 2 : 0), null, this)
        this.$bind(T.Popup.Y, () => -c.implicitHeight - 3, null, this)
        this.margins = 6
        this.padding = 6
        this.closePolicy = 16 | 2 | 8
    }
    $makeStyle(){
        let c = this
        popupStyle(this, () => make(Rectangle, null, { 'border.color': () => c.palette.dark, color: () => c.palette.toolTipBase }))
        if(!this.$item.$v[T.Control.CONTENTITEM] || this.$item.$v[T.Control.CONTENTITEM] === this.$item.$ownContent){
            let text = make(Text, null, { text: () => c.text, wrapMode: Text.Wrap, color: () => c.palette.toolTipText })
            c.font.$follow(text)
            this.contentItem = text
        }
    }
}
defineType(ToolTip, {})
T.ToolTipAttached.$make = (parent) => new ToolTip(parent)

class DialogButtonBox extends T.DialogButtonBox {
    constructor(parent = null){
        super(parent)
        let c = this
        this.$bind(Item.IMPLICITWIDTH, () => Math.max(c.implicitBackgroundWidth + c.leftInset + c.rightInset,
            (c.count === 1 ? c.implicitContentWidth * 2 : c.implicitContentWidth) + c.leftPadding + c.rightPadding), null, this)
        this.$bind(Item.IMPLICITHEIGHT, () => Math.max(c.implicitBackgroundHeight + c.topInset + c.bottomInset, c.implicitContentHeight + c.topPadding + c.bottomPadding), null, this)
        this.spacing = 1
        this.padding = 12
        this.$bind(T.DialogButtonBox.ALIGNMENT, () => (c.count === 1 ? 2 : 0), null, this)
    }
    $makeButton(){ return super.$makeButton() }
    // delegate: Button { width: control.count === 1 ? control.availableWidth / 2 : undefined }: one button is half wide
    $updateLayout(){
        super.$updateLayout()
        let items = this.$v[T.Container.CONTENTMODEL].$items
        if(items.length === 1){
            items[0].$boxWidth = true
            items[0].$explicit |= 1
            items[0].$write(Item.WIDTH, this.availableWidth / 2)
        }
    }
    $makeStyle(){
        let c = this
        // delegate: Button { } (the standard buttons are made by $makeButton, as this delegate would)
        if(!this.$v[T.DialogButtonBox.DELEGATE]) this.$plain(T.DialogButtonBox.DELEGATE, new (require('../../QtQml/Component').Component)(null, (parent) => new Button(parent)))
        this.$styleDelegate(T.Control.CONTENTITEM, () => {
            let view = make(ListView, null, {
                model: () => c.contentModel,
                spacing: () => c.spacing,
                orientation: ListView.Horizontal,
                boundsBehavior: 0,
                snapMode: ListView.SnapToItem,
            })
            view.$bind(Item.IMPLICITWIDTH, () => view.contentWidth, null, view)
            return view
        })
        let background = this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, { implicitHeight: 40, x: 1, y: 1, color: () => c.palette.window }), -1)
        if(background){
            background.$ownSize = 3
            background.$bind(Item.WIDTH, () => c.width - 2, null, background)
            background.$bind(Item.HEIGHT, () => c.height - 2, null, background)
        }
        // contentWidth: (contentItem as ListView)?.contentWidth
        this.$bind(T.Container.CONTENTWIDTH, () => (c.contentItem && c.contentItem.contentWidth !== undefined ? c.contentItem.contentWidth : 0), null, this)
        // the delegate buttons are laid out by a ListView of the content model: the box keeps them sorted when made
        if(this.$pendingButtons){
            this.$pendingButtons = false
            this.$madeOnce = true
            this.$makeStandardButtons()
        }
    }
}
defineType(DialogButtonBox, {})

class Dialog extends T.Dialog {
    constructor(parent = null){
        super(parent)
        let c = this, item = this.$item
        item.$bind(Item.IMPLICITWIDTH, () => Math.max(item.implicitBackgroundWidth + item.leftInset + item.rightInset, item.implicitContentWidth + item.leftPadding + item.rightPadding,
            item.implicitHeaderWidth, item.implicitFooterWidth), null, item)
        item.$bind(Item.IMPLICITHEIGHT, () => Math.max(item.implicitBackgroundHeight + item.topInset + item.bottomInset, item.implicitContentHeight + item.topPadding + item.bottomPadding
            + (item.implicitHeaderHeight > 0 ? item.implicitHeaderHeight + item.spacing : 0) + (item.implicitFooterHeight > 0 ? item.implicitFooterHeight + item.spacing : 0)), null, item)
        this.padding = 12
    }
    $makeStyle(){
        let c = this, item = this.$item
        c.$styleDimmer = (modal) => dimmer(c, modal)
        item.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, { color: () => c.palette.window, 'border.color': () => c.palette.dark }), -1)
        if(!item.header){
            let header = new Label(null)
            apply(header, {
                text: () => c.title,
                visible: () => !!(header.parent && header.parent.parent && header.parent.parent instanceof T.Overlay && c.title),
                elide: Text.ElideRight,
                'font.bold': true,
                padding: 12,
            })
            let bg = make(Rectangle, null, { x: 1, y: 1, color: () => c.palette.window })
            bg.$bind(Item.WIDTH, () => header.width - 2, null, bg)
            bg.$bind(Item.HEIGHT, () => header.height - 1, null, bg)
            bg.$ownSize = 3
            header.background = bg
            this.header = header
        }
        if(!item.footer){
            let footer = new DialogButtonBox(null)
            footer.$bind(Item.VISIBLE, () => footer.count > 0, null, footer)
            this.footer = footer
        }
    }
}
defineType(Dialog, {})

class MenuItem extends T.MenuItem {
    constructor(parent = null){
        super(parent)
        implicitSize(this, true)
        this.padding = 6
        this.spacing = 6
        this.icon.width = 24
        this.icon.height = 24
    }
    $makeStyle(){
        let c = this
        let indicator = this.$styleDelegate(T.AbstractButton.INDICATOR, () => styleImage('check', null, () => c.palette.windowText, { visible: () => c.checked }))
        if(indicator){
            indicator.$bind(Item.X, () => (c.mirrored ? c.width - indicator.width - c.rightPadding : c.leftPadding), null, indicator)
            indicator.$bind(Item.Y, () => c.topPadding + (c.availableHeight - indicator.height) / 2, null, indicator)
            indicator.$write(Item.IMPLICITWIDTH, 28)
            indicator.$write(Item.IMPLICITHEIGHT, 28)
            // source: control.checkable ? check.png : ""
            indicator.$bind(Image.SOURCE, () => (c.checkable ? IMAGES.check(indicator.color) : ''), null, indicator)
        }
        // arrow: ColorImage { visible: control.subMenu; source: arrow-indicator.png when a sub menu }
        let arrow = this.$styleDelegate(T.MenuItem.ARROW, () => {
            let image = make(ColorImage, null, { width: 11, height: 18, defaultColor: '#353637', color: () => c.palette.windowText, visible: () => !!c.subMenu })
            image.$bind(Image.SOURCE, () => (c.subMenu ? svg(11, 18, '<path d="M1 1.2H3L10 9L3 16.8H1V15.3L6.6 9L1 2.7Z" fill="' + image.color + '"/>') : ''), null, image)
            image.$bind(Item.X, () => (c.mirrored ? c.leftPadding : c.width - image.width - c.rightPadding), null, image)
            image.$bind(Item.Y, () => c.topPadding + (c.availableHeight - image.height) / 2, null, image)
            return image
        })
        this.$styleDelegate(T.Control.CONTENTITEM, () => iconLabel(c, {
            leftPadding: () => (!c.mirrored ? (c.checkable && c.indicator ? c.indicator.width + c.spacing : 0) : (c.subMenu && c.arrow ? c.arrow.width + c.spacing : 0)),
            rightPadding: () => (c.mirrored ? (c.checkable && c.indicator ? c.indicator.width + c.spacing : 0) : (c.subMenu && c.arrow ? c.arrow.width + c.spacing : 0)),
            alignment: 1,
            defaultIconColor: () => c.palette.windowText,
            color: () => c.palette.windowText,
        }))
        let background = this.$styleDelegate(T.Control.BACKGROUND, () => make(Rectangle, null, {
            implicitWidth: 200,
            implicitHeight: 40,
            x: 1,
            y: 1,
            color: () => (c.down ? c.palette.midlight : c.highlighted ? c.palette.light : 'transparent'),
            'border.color': () => c.palette.dark,
            'border.width': 0,
        }), -1)
        if(background){
            background.$ownSize = 3
            background.$bind(Item.WIDTH, () => c.width - 2, null, background)
            background.$bind(Item.HEIGHT, () => c.height - 2, null, background)
        }
    }
}
defineType(MenuItem, {})

class Menu extends T.Menu {
    constructor(parent = null){
        super(parent)
        this.margins = 0
        this.overlap = 1
    }
    $makeStyle(){
        let c = this
        // delegate: MenuItem { }
        if(!this.$v[T.Menu.DELEGATE]) this.$plain(T.Menu.DELEGATE, new (require('../../QtQml/Component').Component)(null, (parent) => new MenuItem(parent)))
        popupStyle(this, () => make(Rectangle, null, { implicitWidth: 200, implicitHeight: 40, color: () => c.palette.window, 'border.color': () => c.palette.dark }))
        let view = make(ListView, null, {
            model: () => c.contentModel,
            clip: true,
            currentIndex: () => c.currentIndex,
        })
        view.$bind(Item.IMPLICITHEIGHT, () => view.contentHeight, null, view)
        view.$bind(ListView.INTERACTIVE, () => view.contentHeight + c.topPadding + c.bottomPadding > c.height, null, view)
        view.ScrollIndicator.vertical = new ScrollIndicator(null)
        this.contentItem = view
        // the items take the width of the menu (QQuickMenuPrivate::resizeItems)
        let resize = () => { for(let item of c.$items()) if(!(item.$explicit & 1)) item.$write(Item.WIDTH, view.width) }
        engine.connect(view, Item.SIGNAL_widthChanged, resize, null, engine.PRIORITY_NORMAL, this)
        engine.connect(this.$v[T.Menu.CONTENTMODEL], this.$v[T.Menu.CONTENTMODEL].constructor.SIGNAL_countChanged, resize, null, engine.PRIORITY_NORMAL, this)
        // the content size is the widest item (QQuickMenu: contentWidth of the ListView of items)
        view.$bind(Item.IMPLICITWIDTH, () => Math.max(0, ...c.$items().map(i => i.implicitWidth)), null, view)
    }
}
defineType(Menu, {})

class Drawer extends T.Drawer {
    constructor(parent = null){
        super(parent)
        let c = this
        implicitSize(this.$item)
        this.$bind(T.Popup.TOPPADDING, () => (c.edge === 8 ? 1 : 0), null, this)
        this.$bind(T.Popup.LEFTPADDING, () => (c.edge === 4 ? 1 : 0), null, this)
        this.$bind(T.Popup.RIGHTPADDING, () => (c.edge === 2 ? 1 : 0), null, this)
        this.$bind(T.Popup.BOTTOMPADDING, () => (c.edge === 1 ? 1 : 0), null, this)
    }
    $makeStyle(){
        let c = this
        // parent: T.Overlay.overlay
        let overlay = this.$overlay()
        if(overlay && this.$parent !== overlay) this.$setParent(overlay)
        // enter / exit: Transition { SmoothedAnimation { velocity: 5 } }
        let { Transition } = require('../Transition')
        if(!this.enter) this.$plain(T.Popup.ENTER, new Transition(this))
        if(!this.exit) this.$plain(T.Popup.EXIT, new Transition(this))
        c.$styleDimmer = (modal) => dimmer(c, modal)
        this.$item.$styleDelegate(T.Control.BACKGROUND, () => {
            let bg = make(Rectangle, null, { color: () => c.palette.window })
            let line = make(Rectangle, bg, { color: () => c.palette.dark })
            let horizontal = () => c.edge === 2 || c.edge === 4
            line.$bind(Item.WIDTH, () => (horizontal() ? 1 : bg.width), null, line)
            line.$bind(Item.HEIGHT, () => (horizontal() ? bg.height : 1), null, line)
            line.$bind(Item.X, () => (c.edge === 2 ? bg.width - 1 : 0), null, line)
            line.$bind(Item.Y, () => (c.edge === 1 ? bg.height - 1 : 0), null, line)
            return bg
        }, -1)
    }
}
defineType(Drawer, {})

module.exports = {
    Action: T.Action, ActionGroup: T.ActionGroup, ButtonGroup: T.ButtonGroup,
    ToolTip, DialogButtonBox, Dialog, Menu, MenuItem, Drawer,
    ComboBox,
    Popup, Overlay: T.Overlay,
    TextField, TextArea,
    Pane, Frame, GroupBox, Page, ToolBar, ToolSeparator, MenuSeparator, Container, TabBar, SwipeView,
    Slider, RangeSlider, ProgressBar, BusyIndicator, Dial, PageIndicator, SpinBox, ProgressBarImpl, BusyIndicatorImpl, DialImpl,
    ScrollBar, ScrollIndicator,
    Control, Label, AbstractButton, Button, ToolButton, RoundButton, CheckBox, RadioButton, Switch, DelayButton, TabButton,
    ItemDelegate, CheckDelegate, RadioDelegate, SwitchDelegate, IMAGES, styleImage, implicitSize, iconLabel, checkLabel,
}
