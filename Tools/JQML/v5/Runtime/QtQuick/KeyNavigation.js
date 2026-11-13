// KeyNavigation attached property (QQuickKeyNavigationAttached): arrow and tab keys move the active focus
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { GroupObject } = require('../Core/QtObject')
const QtEnums = require('../Qt/enums')
const { Item } = require('./Item')

const OPPOSITE = { left: 'right', right: 'left', up: 'down', down: 'up', tab: 'backtab', backtab: 'tab' }
const BY_KEY = new Map([
    [QtEnums.Key_Left, 'left'], [QtEnums.Key_Right, 'right'], [QtEnums.Key_Up, 'up'],
    [QtEnums.Key_Down, 'down'], [QtEnums.Key_Tab, 'tab'], [QtEnums.Key_Backtab, 'backtab'],
])

class KeyNavigation extends GroupObject {
    constructor(owner){
        super(owner)
        this.$set = new Set()   // directions assigned explicitly, not filled in by the other item
    }

    // keyPressed / keyReleased: a key with a target is accepted, the press also moves the focus
    $filterKey(event, kind, post){
        if(post !== (this.$v[KeyNavigation.PRIORITY] === KeyNavigation.AfterItem)) return false
        let direction = BY_KEY.get(event.key)
        let target = direction && this.$v[KeyNavigation[direction.toUpperCase()]]
        if(!target) return false
        if(kind === 'pressed') focusNavigation(target, direction)
        return true
    }
}

// setFocusNavigation: the first visible and enabled item following the same direction from the target
function focusNavigation(target, direction){
    let isEnabled = require('./KeyDelivery').isEnabled
    let id = KeyNavigation[direction.toUpperCase()]
    let visited = []
    for(let item = target; item;){
        if(item.$v[Item.VISIBLE] && isEnabled(item)){
            item.forceActiveFocus()
            return
        }
        let navigation = item.$v[Item.KEYNAVIGATION]
        let next = navigation && navigation.$v[id]
        if(!next || next === target || visited.includes(next)) return
        visited.push(item)
        item = next
    }
}

// setLeft and the others: the target navigates back unless its own direction was assigned
function direction(name){
    return {
        kind: 'var',
        initial: null,
        effect(navigation, item){
            navigation.$set.add(name)
            if(!(item instanceof Item)) return
            let other = item.KeyNavigation
            let back = OPPOSITE[name]
            if(other.$set.has(back)) return
            let id = KeyNavigation[back.toUpperCase()]
            let old = other.$v[id]
            if(old === navigation.$owner) return
            other.$v[id] = navigation.$owner
            engine.emit(other, KeyNavigation.$type.props[id].signal, old, navigation.$owner)
        },
    }
}

defineType(KeyNavigation, {
    props: {
        left: direction('left'),
        right: direction('right'),
        up: direction('up'),
        down: direction('down'),
        tab: direction('tab'),
        backtab: direction('backtab'),
        priority: { kind: 'int', initial: 1 },
    },
})
Object.assign(KeyNavigation, { BeforeItem: 0, AfterItem: 1 })

module.exports = { KeyNavigation }
