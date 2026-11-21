// Action, ActionGroup and ButtonGroup of QtQuick.Templates (qquickaction.cpp, qquickactiongroup.cpp, qquickbuttongroup.cpp,
// Qt 6.12):
//  - Action: text, icon, enabled, checkable, checked, shortcut; trigger(source) toggles a checkable action and emits
//    toggled, then triggered; a button with an action takes its text, icon, enabled, checkable and checked, and a click
//    triggers it;
//  - ActionGroup: its actions exclusive (checking one unchecks the others), checkedAction, triggered(action);
//  - ButtonGroup: its buttons exclusive (checkedButton), clicked(button), checkState of a non-exclusive group;
//    ButtonGroup.group / ActionGroup.group attached to buttons / actions.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { QtObject, GroupObject } = require('../../Core/QtObject')
const { Item } = require('../Item')
const { Icon } = require('./impl')
const { AbstractButton } = require('./AbstractButton')

// ---- Action (QQuickAction) ----
class Action extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$v[Action.ICON] = new Icon(this)
        this.$plain(Action.ENABLED, true)
        this.$buttons = new Set()
        this.$shortcut = null
    }
    $plain(id, value){ QtObject.prototype.$write.call(this, id, value) }
    // QQuickActionPrivate::trigger
    trigger(source = null, doToggle = true){
        if(!this.$v[Action.ENABLED]) return
        engine.batch(() => {
            if(doToggle && this.$v[Action.CHECKABLE] && !(this.$v[Action.CHECKED] && this.$group && this.$group.$v[ActionGroup.EXCLUSIVE])) this.toggle(source)
            engine.emitArgs(this, Action.SIGNAL_triggered, [source])
        })
    }
    toggle(source = null){
        if(!this.$v[Action.ENABLED]) return
        engine.batch(() => {
            this.checked = !this.$v[Action.CHECKED]
            engine.emitArgs(this, Action.SIGNAL_toggled, [source])
        })
    }
    $write(id, value){
        if(id === Action.CHECKED){
            value = !!value
            if(value && !this.$v[Action.CHECKABLE]) QtObject.prototype.$write.call(this, Action.CHECKABLE, true)
            QtObject.prototype.$write.call(this, id, value)
            if(value && this.$group) this.$group.$checked(this)
            for(let button of this.$buttons) if(button.checked !== value) button.$setChecked(value)
            return
        }
        QtObject.prototype.$write.call(this, id, value)
        if(id === Action.SHORTCUT) this.$updateShortcut()
        for(let button of this.$buttons) button.$actionChanged(this)
    }
    // shortcut: a Shortcut of the window that triggers the action
    $updateShortcut(){
        let sequence = this.$v[Action.SHORTCUT]
        if(this.$shortcut){
            this.$shortcut.destroy()
            this.$shortcut = null
        }
        if(sequence === undefined || sequence === null || sequence === '') return
        let { Shortcut } = require('../Shortcut')
        let shortcut = this.$shortcut = new Shortcut(this)
        shortcut.sequence = sequence
        shortcut.context = 1
        engine.connect(shortcut, Shortcut.SIGNAL_activated, () => this.trigger(this), null, engine.PRIORITY_NORMAL, this)
    }
    // the button that shows the action (QQuickAbstractButtonPrivate::setAction)
    $bindButton(button){
        this.$buttons.add(button)
        button.$actionChanged(this)
    }
    $unbindButton(button){ this.$buttons.delete(button) }
    // a button triggers it: the button toggled itself already (QQuickActionPrivate::trigger(source, doToggle false))
    $trigger(button){
        // the button toggled the action's checked: its toggled, then triggered
        if(this.$v[Action.CHECKABLE] && this.$v[Action.ENABLED]) engine.emitArgs(this, Action.SIGNAL_toggled, [button])
        this.trigger(button, false)
    }
}
defineType(Action, {
    props: { text: 'string', icon: { group: Icon }, enabled: 'bool', checked: 'bool', checkable: 'bool', shortcut: 'var', ActionGroup: { group: null } },
    signals: ['triggered(source)', 'toggled(source)'],
})

// a button with an action: its text, icon, enabled, checkable and checked are the action's
AbstractButton.prototype.$actionChanged = function(action){
    if(this.$v[AbstractButton.ACTION] !== action) return
    engine.batch(() => {
        if(action.$v[Action.TEXT] !== undefined && action.$v[Action.TEXT] !== '') this.$plain(AbstractButton.TEXT, action.$v[Action.TEXT])
        this.$plain(AbstractButton.CHECKABLE, !!action.$v[Action.CHECKABLE])
        if(this.$v[AbstractButton.CHECKED] !== !!action.$v[Action.CHECKED]) this.$setChecked(!!action.$v[Action.CHECKED])
        this.enabled = !!action.$v[Action.ENABLED]
        let icon = action.$v[Action.ICON], own = this.$v[AbstractButton.ICON]
        if(icon && own){
            for(let name of ['name', 'source', 'color']) if(icon[name]) own[name] = icon[name]
        }
    })
}
{
    // the checked of a button with an action is the action's (QQuickAbstractButtonPrivate::buttonChange)
    let base = AbstractButton.prototype.$setChecked
    AbstractButton.prototype.$setChecked = function(value){
        base.call(this, value)
        let action = this.$v[AbstractButton.ACTION]
        if(action && action.checked !== !!value) action.checked = !!value
    }
}

// ---- ActionGroup (QQuickActionGroup) ----
class ActionGroupAttached extends GroupObject {
    $write(id, value){
        let old = this.$v[id]
        super.$write(id, value)
        if(id === ActionGroupAttached.GROUP){
            if(old && old !== value) old.removeAction(this.$owner)
            if(value) value.addAction(this.$owner)
        }
    }
}
defineType(ActionGroupAttached, { props: { group: { kind: 'var', initial: null } } })
{
    let prop = Action.$type.propByName.get('ActionGroup')
    prop.group = ActionGroupAttached
    Object.defineProperty(Action.prototype, 'ActionGroup', {
        get(){ return this.$v[prop.id] || (this.$v[prop.id] = new ActionGroupAttached(this)) },
        configurable: true,
    })
}

class ActionGroup extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$list = []
        this.$plain(ActionGroup.EXCLUSIVE, true)
        this.$plain(ActionGroup.ENABLED, true)
        engine.scheduleComponentComplete(this)
    }
    $plain(id, value){ QtObject.prototype.$write.call(this, id, value) }
    $componentComplete(){
        super.$componentComplete()
        for(let child of this.$children || []) if(child instanceof Action) this.addAction(child)
    }
    addAction(action){
        if(!action || this.$list.includes(action)) return
        this.$list.push(action)
        action.$group = this
        engine.connect(action, Action.SIGNAL_triggered, () => engine.emitArgs(this, ActionGroup.SIGNAL_triggered, [action]), null, engine.PRIORITY_NORMAL, this)
        if(action.checked) this.$checked(action)
        engine.emit(this, ActionGroup.SIGNAL_actionsChanged)
    }
    removeAction(action){
        let i = this.$list.indexOf(action)
        if(i < 0) return
        this.$list.splice(i, 1)
        if(action.$group === this) action.$group = null
        if(this.$v[ActionGroup.CHECKEDACTION] === action) this.$plain(ActionGroup.CHECKEDACTION, null)
        engine.emit(this, ActionGroup.SIGNAL_actionsChanged)
    }
    $checked(action){
        if(!this.$v[ActionGroup.EXCLUSIVE]) return
        for(let other of this.$list) if(other !== action && other.checked) other.checked = false
        this.$plain(ActionGroup.CHECKEDACTION, action)
    }
}
defineType(ActionGroup, {
    props: {
        checkedAction: { kind: 'var', initial: null }, enabled: 'bool', exclusive: 'bool',
        actions: { kind: 'var', get(){ engine.track(this, ActionGroup.SIGNAL_actionsChanged); return this.$list.slice() } },
    },
    signals: ['triggered(action)'],
})

// ---- ButtonGroup (QQuickButtonGroup) ----
class ButtonGroupAttached extends GroupObject {
    $write(id, value){
        let old = this.$v[id]
        super.$write(id, value)
        if(id === ButtonGroupAttached.GROUP){
            if(old && old !== value) old.removeButton(this.$owner)
            if(value) value.addButton(this.$owner)
        }
    }
}
defineType(ButtonGroupAttached, { props: { group: { kind: 'var', initial: null } } })

class ButtonGroup extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$list = []
        this.$plain(ButtonGroup.EXCLUSIVE, true)
    }
    $plain(id, value){ QtObject.prototype.$write.call(this, id, value) }
    $setButtons(list){
        engine.batch(() => {
            for(let button of this.$list.slice()) this.removeButton(button)
            for(let button of list || []) this.addButton(button)
        })
    }
    addButton(button){
        if(!button || this.$list.includes(button)) return
        this.$list.push(button)
        button.$buttonGroup = this
        engine.connect(button, AbstractButton.SIGNAL_clicked, () => engine.emitArgs(this, ButtonGroup.SIGNAL_clicked, [button]), null, engine.PRIORITY_NORMAL, this)
        engine.connect(button, AbstractButton.SIGNAL_checkedChanged, () => this.$updateState(), null, engine.PRIORITY_NORMAL, this)
        if(button.checked) this.$checked(button)
        this.$updateState()
        engine.emit(this, ButtonGroup.SIGNAL_buttonsChanged)
    }
    removeButton(button){
        let i = this.$list.indexOf(button)
        if(i < 0) return
        this.$list.splice(i, 1)
        if(button.$buttonGroup === this) button.$buttonGroup = null
        if(this.$v[ButtonGroup.CHECKEDBUTTON] === button) this.$plain(ButtonGroup.CHECKEDBUTTON, null)
        this.$updateState()
        engine.emit(this, ButtonGroup.SIGNAL_buttonsChanged)
    }
    $exclusive(){ return !!this.$v[ButtonGroup.EXCLUSIVE] }
    // a button of an exclusive group checked: the others unchecked
    $checked(button){
        if(!this.$exclusive()) return
        for(let other of this.$list) if(other !== button && other.checked) other.$setChecked(false)
        this.$plain(ButtonGroup.CHECKEDBUTTON, button)
    }
    get checkedButton(){
        engine.track(this, ButtonGroup.SIGNAL_checkedButtonChanged)
        return this.$v[ButtonGroup.CHECKEDBUTTON]
    }
    set checkedButton(button){ if(button) button.checked = true }
    // QQuickButtonGroup::checkState: of a non-exclusive group, the checked buttons
    $updateState(){
        let checked = this.$list.filter(b => b.checked).length
        this.$plain(ButtonGroup.CHECKSTATE, !checked ? 0 : checked === this.$list.length ? 2 : 1)
        if(this.$exclusive()){
            let current = this.$list.find(b => b.checked) || null
            this.$plain(ButtonGroup.CHECKEDBUTTON, current)
        }
    }
    $write(id, value){
        if(id === ButtonGroup.BUTTONS) return this.$setButtons(value)
        if(id === ButtonGroup.CHECKSTATE && !this.$updatingState){
            QtObject.prototype.$write.call(this, id, value)
            if(value === 2 || value === 0) for(let b of this.$list) b.checked = value === 2
            return
        }
        QtObject.prototype.$write.call(this, id, value)
    }
}
defineType(ButtonGroup, {
    props: {
        checkedButton: { kind: 'var', initial: null }, exclusive: 'bool', checkState: 'int',
        buttons: { kind: 'var', get(){ engine.track(this, ButtonGroup.SIGNAL_buttonsChanged); return this.$list.slice() } },
    },
    signals: ['clicked(button)'],
})
ButtonGroup.prototype.$updateState = (function(base){ return function(){ this.$updatingState = true; try { base.call(this) } finally { this.$updatingState = false } } })(ButtonGroup.prototype.$updateState)

// ButtonGroup.group / ActionGroup.group on an item
for(let [name, Group] of [['ButtonGroup', ButtonGroupAttached], ['ActionGroup', ActionGroupAttached]]){
    let prop = Item.$type.propByName.get(name)
    if(!prop) continue
    prop.group = Group
    Object.defineProperty(Item.prototype, name, {
        get(){ return this.$v[prop.id] || (this.$v[prop.id] = new Group(this)) },
        configurable: true,
    })
}

module.exports = { Action, ActionGroup, ActionGroupAttached, ButtonGroup, ButtonGroupAttached }
