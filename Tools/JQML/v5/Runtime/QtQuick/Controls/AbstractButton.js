// The buttons of QtQuick.Templates (Qt 6.12): AbstractButton (qquickabstractbutton.cpp) and Button, ToolButton,
// RoundButton, CheckBox, RadioButton, Switch, DelayButton, TabButton, ItemDelegate, CheckDelegate, RadioDelegate,
// SwitchDelegate; ButtonGroup. Checked against Qt 6.12 (tests/diff/cases/abstractbutton_*.qml):
//  - a click: pressedChanged, downChanged, pressed, then pressedChanged, downChanged, (checkedChanged, toggled), released,
//    clicked; a press dragged out of the button and released there: canceled;
//  - pressAndHold after 800 ms takes the click only when something handles pressAndHold;
//  - autoRepeat: after autoRepeatDelay, every autoRepeatInterval: released, clicked, pressed;
//  - a double click: the second press gives doubleClicked and no clicked;
//  - Space presses and clicks a button that has the active focus (Return and Enter do not);
//  - toggle() only changes checked; click() and animateClick() go through press and release.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { QtObject } = require('../../Core/QtObject')
const { Item } = require('../Item')
const { Control, withSetters, StrongFocus, TabFocus, ClickFocus } = require('./Control')
const { Icon, TextBesideIcon } = require('./impl')
const QtEnums = require('../../Qt/enums')

const LeftButton = 1
const PRESS_AND_HOLD = 800      // QStyleHints::mousePressAndHoldInterval
const DRAG_THRESHOLD = 10       // QStyleHints::startDragDistance
const Unchecked = 0, PartiallyChecked = 1, Checked = 2

// macOS gives buttons the focus by Tab only (Qt 6.12: Button.focusPolicy is Qt.TabFocus there), the others by a click too
const isMac = () => typeof navigator !== 'undefined' && /Mac|iPhone|iPad|iPod/.test(String(navigator.platform || ''))
const buttonFocusPolicy = () => (isMac() ? TabFocus : StrongFocus)

class AbstractButton extends Control {
    constructor(parent = null){
        super(parent)
        this.$v[AbstractButton.ICON] = new Icon(this)
        this.$downSet = false       // down written by the QML (else it is pressed)
        this.$wasHeld = false
        this.$wasDoubleClick = false
        this.$holdTimer = null
        this.$repeatTimer = null
        this.$pressButtons = 0
        this.$write(Control.FOCUSPOLICY, buttonFocusPolicy())
    }

    // ---- pressed, down, checked ----
    $setPressed(value){
        if(this.$v[AbstractButton.PRESSED] === value) return
        this.$plain(AbstractButton.PRESSED, value)
        if(!this.$downSet) this.$plain(AbstractButton.DOWN, value)
    }
    $setDown(value){
        if(value === undefined){
            // down: undefined resets it to pressed
            this.$downSet = false
            this.$plain(AbstractButton.DOWN, this.$v[AbstractButton.PRESSED])
            return
        }
        this.$downSet = true
        this.$plain(AbstractButton.DOWN, !!value)
    }
    $setChecked(value){
        value = !!value
        if(this.$v[AbstractButton.CHECKED] === value) return
        if(value && !this.$v[AbstractButton.CHECKABLE]) this.$plain(AbstractButton.CHECKABLE, true)
        this.$plain(AbstractButton.CHECKED, value)
        let action = this.$v[AbstractButton.ACTION]
        if(action && action.checked !== value) action.checked = value
        this.$checkedChange()
    }
    // QQuickAbstractButton::buttonChange(ButtonCheckedChange): an exclusive button that is checked unchecks the others
    $checkedChange(){
        if(!this.$v[AbstractButton.CHECKED]) return
        let group = this.$group()
        if(group && group.$exclusive()) group.$checked(this)
        else if(this.$isAutoExclusive()){
            for(let other of this.$exclusiveButtons()) if(other !== this) other.$setChecked(false)
        }
    }
    // the ButtonGroup of the button (ButtonGroup.group), or null
    $group(){ return this.$buttonGroup || null }
    $isAutoExclusive(){ return !!this.$v[AbstractButton.AUTOEXCLUSIVE] && !this.$group() }
    // the auto exclusive buttons of the same parent (QQuickAbstractButtonPrivate::findExclusiveButtons)
    $exclusiveButtons(){
        let parent = this.$parent
        if(!parent || !parent.$children) return [this]
        return parent.$children.filter(child => child instanceof AbstractButton && child.$v[AbstractButton.CHECKABLE] && child.$isAutoExclusive())
    }
    // QQuickAbstractButtonPrivate::findCheckedButton
    $findCheckedButton(){
        let group = this.$group()
        if(group) return group.$exclusive() ? group.checkedButton : null
        if(!this.$isAutoExclusive()) return null
        return this.$exclusiveButtons().find(b => b.$v[AbstractButton.CHECKED]) || null
    }

    // QQuickAbstractButtonPrivate::toggle: checked changes, toggled when it did
    $toggle(value){
        let was = this.$v[AbstractButton.CHECKED]
        this.$setChecked(value)
        if(was !== this.$v[AbstractButton.CHECKED]) engine.emit(this, AbstractButton.SIGNAL_toggled)
    }
    // QQuickAbstractButton::nextCheckState: a checkable button toggles, a checked exclusive one stays checked
    $nextCheckState(){
        if(this.$v[AbstractButton.CHECKABLE] && (!this.$v[AbstractButton.CHECKED] || this.$findCheckedButton() !== this)) this.$toggle(!this.$v[AbstractButton.CHECKED])
    }
    // QQuickAbstractButtonPrivate::trigger: the action, else clicked
    $trigger(){
        let action = this.$v[AbstractButton.ACTION]
        if(action && action.enabled !== false && action.$trigger) action.$trigger(this)
        else if(this.$v[Item.ENABLED]) engine.emit(this, AbstractButton.SIGNAL_clicked)
    }

    // ---- press, move, release (QQuickAbstractButtonPrivate::handlePress / handleMove / handleRelease) ----
    $handlePress(x, y){
        this.$clickFocus()
        this.$plain(AbstractButton.PRESSX, x)
        this.$plain(AbstractButton.PRESSY, y)
        this.$pressPoint = { x, y }
        this.$setPressed(true)
        engine.emit(this, AbstractButton.SIGNAL_pressed)
        if(this.$v[AbstractButton.AUTOREPEAT]) this.$startRepeatDelay()
        else this.$startPressAndHold()
    }
    $handleMove(x, y){
        this.$plain(AbstractButton.PRESSX, x)
        this.$plain(AbstractButton.PRESSY, y)
        this.$setPressed(this.$keepPressed() || this.$contains(x, y))
        if(!this.$v[AbstractButton.PRESSED] && this.$v[AbstractButton.AUTOREPEAT]) this.$stopPressRepeat()
        else if(this.$holdTimer && this.$pressPoint && (Math.abs(x - this.$pressPoint.x) > DRAG_THRESHOLD || Math.abs(y - this.$pressPoint.y) > DRAG_THRESHOLD)) this.$stopPressAndHold()
    }
    $handleRelease(x, y){
        let wasPressed = this.$v[AbstractButton.PRESSED]
        this.$plain(AbstractButton.PRESSX, x)
        this.$plain(AbstractButton.PRESSY, y)
        this.$setPressed(false)
        this.$pressButtons = 0
        if(!this.$wasHeld && (this.$keepPressed() || this.$contains(x, y))) this.$nextCheckState()
        if(wasPressed){
            engine.emit(this, AbstractButton.SIGNAL_released)
            if(!this.$wasHeld && !this.$wasDoubleClick) this.$trigger()
        } else {
            engine.emit(this, AbstractButton.SIGNAL_canceled)
        }
        if(this.$v[AbstractButton.AUTOREPEAT]) this.$stopPressRepeat()
        else this.$stopPressAndHold()
        this.$wasDoubleClick = false
    }
    $handleUngrab(){
        if(!this.$pressButtons) return
        this.$setPressed(false)
        this.$pressButtons = 0
        this.$stopPressRepeat()
        this.$stopPressAndHold()
        this.$wasDoubleClick = false
        engine.emit(this, AbstractButton.SIGNAL_canceled)
    }
    $contains(x, y){ return x >= 0 && y >= 0 && x <= this.$v[Item.WIDTH] && y <= this.$v[Item.HEIGHT] }
    // QQuickAbstractButtonPrivate::keepPressed: a Switch dragged, a DelayButton...
    $keepPressed(){ return false }

    $mousePress(x, y, button){
        if(button !== LeftButton) return false
        this.$pressButtons = button
        this.$handlePress(x, y)
        return true
    }
    $mouseMove(x, y){ if(this.$pressButtons) this.$handleMove(x, y) }
    $mouseRelease(x, y, button){ if(this.$pressButtons) this.$handleRelease(x, y) }
    $mouseDoubleClick(){
        engine.emit(this, AbstractButton.SIGNAL_doubleClicked)
        this.$wasDoubleClick = true
    }
    $ungrabMouse(){ this.$handleUngrab() }

    // ---- press and hold, auto repeat ----
    $startPressAndHold(){
        this.$wasHeld = false
        this.$stopPressAndHold()
        // only when something handles pressAndHold: else a long press is a click (QQuickAbstractButtonPrivate::isPressAndHoldConnected)
        if(!engine.hasSubscribers(this, AbstractButton.SIGNAL_pressAndHold)) return
        this.$holdTimer = setTimeout(() => {
            this.$holdTimer = null
            if(this.$f & engine.DESTROYED || !this.$v[AbstractButton.PRESSED]) return
            engine.batch(() => {
                this.$wasHeld = true
                engine.emit(this, AbstractButton.SIGNAL_pressAndHold)
            })
        }, PRESS_AND_HOLD)
    }
    $stopPressAndHold(){
        if(this.$holdTimer){
            clearTimeout(this.$holdTimer)
            this.$holdTimer = null
        }
    }
    $startRepeatDelay(){
        this.$stopPressRepeat()
        this.$repeatTimer = setTimeout(() => this.$startPressRepeat(), this.$v[AbstractButton.AUTOREPEATDELAY])
    }
    $startPressRepeat(){
        this.$stopPressRepeat()
        this.$repeatTimer = setInterval(() => {
            if(this.$f & engine.DESTROYED || !this.$v[AbstractButton.PRESSED]) return this.$stopPressRepeat()
            engine.batch(() => {
                engine.emit(this, AbstractButton.SIGNAL_released)
                this.$trigger()
                engine.emit(this, AbstractButton.SIGNAL_pressed)
            })
        }, this.$v[AbstractButton.AUTOREPEATINTERVAL])
        this.$repeatInterval = true
    }
    $stopPressRepeat(){
        if(this.$repeatTimer){
            clearTimeout(this.$repeatTimer)
            clearInterval(this.$repeatTimer)
            this.$repeatTimer = null
        }
    }

    // ---- keys: Space (QPlatformTheme::ButtonPressKeys) ----
    $isPressKey(event){ return event.key === QtEnums.Key_Space || event.key === QtEnums.Key_Select }
    $keyEvent(event, kind){
        if(!this.$isPressKey(event)) return 0
        if(kind === 'pressed'){
            if(event.isAutoRepeat) return 1
            this.$pressPoint = { x: this.$v[Item.WIDTH] / 2, y: this.$v[Item.HEIGHT] / 2 }
            this.$setPressed(true)
            if(this.$v[AbstractButton.AUTOREPEAT]) this.$startRepeatDelay()
            engine.emit(this, AbstractButton.SIGNAL_pressed)
            return 1
        }
        if(!this.$v[AbstractButton.PRESSED]) return 0
        this.$setPressed(false)
        this.$nextCheckState()
        engine.emit(this, AbstractButton.SIGNAL_released)
        this.$trigger()
        if(this.$v[AbstractButton.AUTOREPEAT]) this.$stopPressRepeat()
        else this.$stopPressAndHold()
        return 1
    }

    // ---- methods ----
    // QQuickAbstractButton::accessiblePressAction / accessibleToggleAction
    accessiblePressAction(){ engine.batch(() => this.$trigger()) }
    accessibleToggleAction(){ engine.batch(() => this.$toggle(!this.$v[AbstractButton.CHECKED])) }
    toggle(){ engine.batch(() => this.$setChecked(!this.$v[AbstractButton.CHECKED])) }
    click(){
        if(!this.$v[Item.ENABLED]) return
        let x = this.$v[Item.WIDTH] / 2, y = this.$v[Item.HEIGHT] / 2
        engine.batch(() => {
            this.$handlePress(x, y)
            this.$handleRelease(x, y)
        })
    }
    // a press now and the release 100 ms later (QQuickAbstractButton::animateClick)
    animateClick(){
        if(!this.$v[Item.ENABLED]) return
        let x = this.$v[Item.WIDTH] / 2, y = this.$v[Item.HEIGHT] / 2
        engine.batch(() => this.$handlePress(x, y))
        setTimeout(() => {
            if(this.$f & engine.DESTROYED) return
            engine.batch(() => this.$handleRelease(x, y))
        }, 100)
    }

    // the indicator of a check box, a radio button...: a delegate as the background (a child, its implicit size)
    $delegateChanged(id){
        if(id === AbstractButton.INDICATOR) this.$delegateImplicitChanged(id)
        else super.$delegateChanged(id)
    }
    $delegateImplicitChanged(id){
        if(id === AbstractButton.INDICATOR){
            let indicator = this.$v[AbstractButton.INDICATOR]
            this.$plain(AbstractButton.IMPLICITINDICATORWIDTH, indicator ? indicator.$v[Item.IMPLICITWIDTH] : 0)
            this.$plain(AbstractButton.IMPLICITINDICATORHEIGHT, indicator ? indicator.$v[Item.IMPLICITHEIGHT] : 0)
            return
        }
        super.$delegateImplicitChanged(id)
    }

    // an Action: its text, icon, enabled, checkable and checked are the button's (QQuickAbstractButtonPrivate::setAction)
    $setAction(action){
        let old = this.$v[AbstractButton.ACTION]
        if(old === action) return
        if(old && old.$unbindButton) old.$unbindButton(this)
        this.$plain(AbstractButton.ACTION, action || null)
        if(action && action.$bindButton) action.$bindButton(this)
    }

    $componentComplete(){
        super.$componentComplete()
    }
}

defineType(AbstractButton, {
    props: {
        text: 'string',
        down: 'bool',
        pressed: { kind: 'bool', readonly: true },
        checked: 'bool',
        checkable: 'bool',
        autoExclusive: 'bool',
        autoRepeat: 'bool',
        indicator: { kind: 'var', initial: null },
        icon: { group: Icon },
        display: { kind: 'int', initial: TextBesideIcon },
        action: { kind: 'var', initial: null },
        autoRepeatDelay: { kind: 'int', initial: 300 },
        autoRepeatInterval: { kind: 'int', initial: 100 },
        pressX: { kind: 'real', readonly: true },
        pressY: { kind: 'real', readonly: true },
        implicitIndicatorWidth: { kind: 'real', readonly: true },
        implicitIndicatorHeight: { kind: 'real', readonly: true },
    },
    signals: ['pressed', 'released', 'canceled', 'clicked', 'pressAndHold', 'doubleClicked', 'toggled'],
})
AbstractButton.$setters = withSetters(Control, [
    [AbstractButton.DOWN, (b, v) => b.$setDown(v)],
    [AbstractButton.CHECKED, (b, v) => b.$setChecked(v)],
    [AbstractButton.INDICATOR, (b, v) => b.$setDelegate(AbstractButton.INDICATOR, v)],
    [AbstractButton.ACTION, (b, v) => b.$setAction(v)],
])
Object.assign(AbstractButton, { IconOnly: 0, TextOnly: 1, TextBesideIcon: 2, TextUnderIcon: 3 })

// ---- Button, ToolButton, RoundButton ----
class Button extends AbstractButton {}
defineType(Button, { props: { flat: 'bool', highlighted: 'bool' } })
class ToolButton extends Button {}
defineType(ToolButton, {})
// QQuickRoundButton: radius, half the smaller side unless set
class RoundButton extends Button {
    constructor(parent = null){
        super(parent)
        this.$radiusSet = false
    }
    $write(id, value){
        super.$write(id, value)
        if((id === Item.WIDTH || id === Item.HEIGHT) && !this.$radiusSet) this.$plain(RoundButton.RADIUS, Math.min(this.$v[Item.WIDTH], this.$v[Item.HEIGHT]) / 2)
    }
}
defineType(RoundButton, { props: { radius: 'real' } })
RoundButton.$setters = withSetters(Button, [[RoundButton.RADIUS, (b, v) => {
    b.$radiusSet = v !== undefined
    b.$plain(RoundButton.RADIUS, v === undefined ? Math.min(b.$v[Item.WIDTH], b.$v[Item.HEIGHT]) / 2 : v)
}]])

// ---- CheckBox (QQuickCheckBox): checkable, a checkState; tristate cycles Unchecked, PartiallyChecked, Checked ----
class CheckBox extends AbstractButton {
    constructor(parent = null){
        super(parent)
        this.$plain(AbstractButton.CHECKABLE, true)
    }
    $setCheckState(state){
        state = Number(state)
        if(state === PartiallyChecked && !this.$v[this.constructor.TRISTATE]) this.$plain(this.constructor.TRISTATE, true)
        if(this.$v[this.constructor.CHECKSTATE] === state) return
        let wasChecked = this.$v[AbstractButton.CHECKED]
        this.$plain(this.constructor.CHECKSTATE, state)
        if(state !== PartiallyChecked) super.$setChecked(state === Checked)
        else if(wasChecked) super.$setChecked(false)
    }
    $setChecked(value){ this.$setCheckState(value ? Checked : Unchecked) }
    // QQuickCheckBox::nextCheckState: the function the QML gives (nextCheckState: function() {...}), else the next state
    $nextCheckState(){
        let custom = this.$v[this.constructor.NEXTCHECKSTATE]
        if(typeof custom === 'function'){
            this.$toggleState(custom.call(this))
            return
        }
        if(this.$v[this.constructor.TRISTATE]) this.$toggleState((this.$v[this.constructor.CHECKSTATE] + 1) % 3)
        else super.$nextCheckState()
    }
    $toggleState(state){
        let was = this.$v[this.constructor.CHECKSTATE]
        this.$setCheckState(state)
        if(was !== this.$v[this.constructor.CHECKSTATE]) engine.emit(this, AbstractButton.SIGNAL_toggled)
    }
}
defineType(CheckBox, { props: { tristate: 'bool', checkState: 'int', nextCheckState: 'var' } })
CheckBox.$setters = withSetters(AbstractButton, [
    [CheckBox.CHECKSTATE, (b, v) => b.$setCheckState(v)],
    [AbstractButton.CHECKED, (b, v) => b.$setChecked(v)],
])

// ---- RadioButton: checkable and auto exclusive ----
class RadioButton extends AbstractButton {
    constructor(parent = null){
        super(parent)
        this.$plain(AbstractButton.CHECKABLE, true)
        this.$plain(AbstractButton.AUTOEXCLUSIVE, true)
    }
}
defineType(RadioButton, {})

// ---- Switch (QQuickSwitch): checkable, a position 0..1 the handle shows; the handle can be dragged ----
class Switch extends AbstractButton {
    constructor(parent = null){
        super(parent)
        this.$plain(AbstractButton.CHECKABLE, true)
    }
    $setChecked(value){
        super.$setChecked(value)
        this.$setPosition(this.$v[AbstractButton.CHECKED] ? 1 : 0)
    }
    $setPosition(value){
        value = Math.max(0, Math.min(1, Number(value)))
        this.$plain(this.constructor.POSITION, value)
        this.$plain(this.constructor.VISUALPOSITION, this.mirrored ? 1 - value : value)
    }
    // QQuickSwitch::handleMove: a drag beyond the threshold moves the handle
    $handleMove(x, y){
        super.$handleMove(x, y)
        if(!this.$pressPoint) return
        if(!this.$dragging && Math.abs(x - this.$pressPoint.x) > DRAG_THRESHOLD) this.$dragging = true
        if(this.$dragging) this.$setPosition(this.$positionAt(x))
    }
    $positionAt(x){
        let indicator = this.$v[AbstractButton.INDICATOR]
        if(!indicator || indicator.$v[Item.WIDTH] <= 0) return this.$v[this.constructor.POSITION]
        return (x - indicator.$v[Item.X]) / indicator.$v[Item.WIDTH]
    }
    $handleRelease(x, y){
        let dragging = this.$dragging
        this.$dragging = false
        if(dragging){
            // QQuickSwitch::nextCheckState: a drag checks by where the handle is
            let checked = this.$v[this.constructor.POSITION] > 0.5
            this.$nextByDrag = checked
        }
        super.$handleRelease(x, y)
        this.$nextByDrag = undefined
        this.$setPosition(this.$v[AbstractButton.CHECKED] ? 1 : 0)
    }
    $nextCheckState(){
        if(this.$nextByDrag !== undefined){
            this.$toggle(this.$nextByDrag)
            return
        }
        super.$nextCheckState()
    }
    $keepPressed(){ return !!this.$dragging }
    positionAt(x){ return Math.max(0, Math.min(1, this.$positionAt(x))) }
}
defineType(Switch, { props: { position: 'real', visualPosition: { kind: 'real', readonly: true } } })
Switch.$setters = withSetters(AbstractButton, [
    [Switch.POSITION, (b, v) => b.$setPosition(v)],
    [AbstractButton.CHECKED, (b, v) => b.$setChecked(v)],
])

// ---- DelayButton (QQuickDelayButton): checkable; held for delay ms its progress reaches 1, it is checked and activated ----
class DelayButton extends AbstractButton {
    constructor(parent = null){
        super(parent)
        this.$plain(AbstractButton.CHECKABLE, true)
        this.$progressTimer = null
    }
    // the transition of progress: to 1 in delay * (1 - progress) while pressed, back to 0 faster on a release
    $animateProgress(to){
        if(this.$progressTimer) clearInterval(this.$progressTimer)
        let from = this.$v[DelayButton.PROGRESS]
        let delay = this.$v[DelayButton.DELAY]
        let duration = to === 1 ? delay * (1 - from) : 0.3 * delay * from
        if(duration <= 0){
            this.$setProgress(to)
            return
        }
        let start = Date.now()
        this.$progressTimer = setInterval(() => {
            if(this.$f & engine.DESTROYED) return clearInterval(this.$progressTimer)
            let t = Math.min(1, (Date.now() - start) / duration)
            engine.batch(() => this.$setProgress(from + (to - from) * t))
            if(t >= 1){
                clearInterval(this.$progressTimer)
                this.$progressTimer = null
            }
        }, 16)
    }
    $setProgress(value){
        value = Math.max(0, Math.min(1, value))
        if(this.$v[DelayButton.PROGRESS] === value) return
        this.$plain(DelayButton.PROGRESS, value)
        // QQuickDelayButton::setProgress: at 1, checked and activated
        if(value === 1 && !this.$v[AbstractButton.CHECKED]){
            this.$setChecked(true)
            engine.emit(this, DelayButton.SIGNAL_activated)
        }
    }
    $handlePress(x, y){
        super.$handlePress(x, y)
        if(!this.$v[AbstractButton.CHECKED]) this.$animateProgress(1)
    }
    $handleRelease(x, y){
        super.$handleRelease(x, y)
        if(!this.$v[AbstractButton.CHECKED]) this.$animateProgress(0)
    }
    // a DelayButton is checked only by its progress (QQuickDelayButton::nextCheckState: a checked one unchecks)
    $nextCheckState(){
        if(this.$v[AbstractButton.CHECKED]){
            this.$toggle(false)
            this.$setProgress(0)
        }
    }
}
defineType(DelayButton, { props: { delay: { kind: 'int', initial: 300 }, progress: { kind: 'real', readonly: true }, transition: { kind: 'var', initial: null } }, signals: ['activated'] })

// ---- TabButton: checkable and auto exclusive (a TabBar makes the current one checked) ----
class TabButton extends AbstractButton {
    constructor(parent = null){
        super(parent)
        this.$plain(AbstractButton.CHECKABLE, true)
        this.$plain(AbstractButton.AUTOEXCLUSIVE, true)
    }
}
defineType(TabButton, {})

// ---- ItemDelegate and its checkable kinds ----
// QQuickItemDelegate: no focus of its own (Qt 6.12: focusPolicy is Qt.NoFocus)
class ItemDelegate extends AbstractButton {
    constructor(parent = null){
        super(parent)
        this.$write(Control.FOCUSPOLICY, 0)
    }
}
defineType(ItemDelegate, { props: { highlighted: 'bool' } })
class CheckDelegate extends ItemDelegate {
    constructor(parent = null){
        super(parent)
        this.$plain(AbstractButton.CHECKABLE, true)
    }
}
const copyMethods = (to, from, names) => { for(let name of names) Object.defineProperty(to.prototype, name, Object.getOwnPropertyDescriptor(from.prototype, name)) }
copyMethods(CheckDelegate, CheckBox, ['$setCheckState', '$setChecked', '$nextCheckState', '$toggleState'])
defineType(CheckDelegate, { props: { tristate: 'bool', checkState: 'int', nextCheckState: 'var' } })
CheckDelegate.$setters = withSetters(ItemDelegate, [
    [CheckDelegate.CHECKSTATE, (b, v) => b.$setCheckState(v)],
    [AbstractButton.CHECKED, (b, v) => b.$setChecked(v)],
])
class RadioDelegate extends ItemDelegate {
    constructor(parent = null){
        super(parent)
        this.$plain(AbstractButton.CHECKABLE, true)
        this.$plain(AbstractButton.AUTOEXCLUSIVE, true)
    }
}
defineType(RadioDelegate, {})
class SwitchDelegate extends ItemDelegate {
    constructor(parent = null){
        super(parent)
        this.$plain(AbstractButton.CHECKABLE, true)
    }
}
copyMethods(SwitchDelegate, Switch, ['$setChecked', '$setPosition', '$handleMove', '$positionAt', '$handleRelease', '$nextCheckState', '$keepPressed', 'positionAt'])
defineType(SwitchDelegate, { props: { position: 'real', visualPosition: { kind: 'real', readonly: true } } })
SwitchDelegate.$setters = withSetters(ItemDelegate, [
    [SwitchDelegate.POSITION, (b, v) => b.$setPosition(v)],
    [AbstractButton.CHECKED, (b, v) => b.$setChecked(v)],
])

module.exports = {
    AbstractButton, Button, ToolButton, RoundButton, CheckBox, RadioButton, Switch, DelayButton, TabButton,
    ItemDelegate, CheckDelegate, RadioDelegate, SwitchDelegate,
    Unchecked, PartiallyChecked, Checked, buttonFocusPolicy,
}
