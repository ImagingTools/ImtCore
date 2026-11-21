// The Basic style of QtQuick.Controls (Qt 6.12, Controls/Basic/*.qml) over the templates: the paddings, the implicit size
// and the delegates of each control, written as the QML of the style is (make(): a function is a binding). The delegates
// are made when the control is complete and only when its QML does not give its own (deferred, as in Qt).
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Item } = require('../Item')
const { Rectangle } = require('../Rectangle')
const { Image } = require('../Image')
const { Text } = require('../Text')
const T = require('./Templates')
const { make, blend, IconLabel, CheckLabel, PaddedRectangle, ColorImage, IconOnly, TextUnderIcon } = require('./impl')
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

module.exports = {
    Control, Label, AbstractButton, Button, ToolButton, RoundButton, CheckBox, RadioButton, Switch, DelayButton, TabButton,
    ItemDelegate, CheckDelegate, RadioDelegate, SwitchDelegate, IMAGES, styleImage, implicitSize, iconLabel, checkLabel,
}
