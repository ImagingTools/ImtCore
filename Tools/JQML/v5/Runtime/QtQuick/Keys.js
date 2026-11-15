// Keys attached property (QQuickKeysAttached): the key handlers of an item, a grouped property made on first use
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { GroupObject } = require('../Core/QtObject')
const QtEnums = require('../Qt/enums')

// keyToSignal of qquickitem.cpp; Return and Enter share the key name here, the DOM code tells them apart
const KEY_SIGNALS = new Map([
    [QtEnums.Key_Left, 'leftPressed'], [QtEnums.Key_Right, 'rightPressed'], [QtEnums.Key_Up, 'upPressed'],
    [QtEnums.Key_Down, 'downPressed'], [QtEnums.Key_Tab, 'tabPressed'], [QtEnums.Key_Backtab, 'backtabPressed'],
    [QtEnums.Key_Asterisk, 'asteriskPressed'], [QtEnums.Key_NumberSign, 'numberSignPressed'],
    [QtEnums.Key_Escape, 'escapePressed'], [QtEnums.Key_Delete, 'deletePressed'], [QtEnums.Key_Space, 'spacePressed'],
    [QtEnums.Key_Back, 'backPressed'], [QtEnums.Key_Cancel, 'cancelPressed'], [QtEnums.Key_Menu, 'menuPressed'],
])
for(let digit = 0; digit <= 9; digit++) KEY_SIGNALS.set(0x30 + digit, 'digit' + digit + 'Pressed')

function keySignal(event){
    if(event.key === QtEnums.Key_Return) return event.$code === 'NumpadEnter' ? 'enterPressed' : 'returnPressed'
    return KEY_SIGNALS.get(event.key)
}

class Keys extends GroupObject {
    // QQuickKeysAttached::keyPressed: forwardTo first, then the handler of the key (accepted unless it says
    // otherwise), then onPressed while the event is not accepted
    $keyPressed(event){
        let deliverTo = require('./KeyDelivery').deliverTo
        for(let target of this.$v[Keys.FORWARDTO] || []){
            if(target && target !== this.$owner && deliverTo(target, event, 'pressed')) return true
        }
        event.accepted = false
        let name = keySignal(event)
        let id = name && Keys.$type.signalByName.get(name)
        if(id !== undefined && engine.hasSubscribers(this, id)){
            event.accepted = true
            engine.emitArgs(this, id, [event])
        }
        if(!event.accepted) engine.emitArgs(this, Keys.SIGNAL_pressed, [event])
        return event.accepted
    }

    $filterKey(event, kind, post){
        if(!this.$v[Keys.ENABLED] || post !== (this.$v[Keys.PRIORITY] === Keys.AfterItem)) return false
        return kind === 'pressed' ? this.$keyPressed(event) : this.$keyReleased(event)
    }

    $keyReleased(event){
        let deliverTo = require('./KeyDelivery').deliverTo
        for(let target of this.$v[Keys.FORWARDTO] || []){
            if(target && target !== this.$owner && deliverTo(target, event, 'released')) return true
        }
        event.accepted = false
        engine.emitArgs(this, Keys.SIGNAL_released, [event])
        return event.accepted
    }
}

defineType(Keys, {
    props: {
        enabled: { kind: 'bool', initial: true },
        priority: 'int',
        forwardTo: 'var',
    },
    signals: ['pressed(event)', 'released(event)', 'shortcutOverride(event)', 'returnPressed(event)', 'enterPressed(event)']
        .concat([...new Set(KEY_SIGNALS.values())].map(name => name + '(event)')),
})
Object.assign(Keys, { BeforeItem: 0, AfterItem: 1 })

module.exports = { Keys }
