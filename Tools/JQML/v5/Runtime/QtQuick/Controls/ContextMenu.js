// ContextMenu of QtQuick.Controls (qquickcontextmenu.cpp, Qt 6.9+). Checked against Qt 6.12 (tests/diff/cases/contextmenu.qml):
// the attached ContextMenu.menu and requested(position) of an item; a right press that no item takes (a MouseArea of the
// right button takes it) goes to the topmost item under it with a ContextMenu that has a menu or a handler of requested:
// requested(the point in that item), then the menu, a child of the item, pops up at the point.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { GroupObject } = require('../../Core/QtObject')
const { Item } = require('../Item')
const pointer = require('../PointerDelivery')

class ContextMenuAttached extends GroupObject {}
defineType(ContextMenuAttached, { props: { menu: { kind: 'var', initial: null } }, signals: ['requested(position)'] })

const prop = Item.$type.propByName.get('ContextMenu')
prop.group = ContextMenuAttached
Object.defineProperty(Item.prototype, 'ContextMenu', {
    get(){ return this.$v[prop.id] || (this.$v[prop.id] = new ContextMenuAttached(this)) },
    configurable: true,
})

// QQuickContextMenu::event(QEvent::ContextMenu)
pointer.setContextMenuHandler((x, y, items) => {
    for(let item of items){
        let attached = item.$v[prop.id]
        if(!attached) continue
        let menu = attached.$v[ContextMenuAttached.MENU]
        let requested = engine.hasSubscribers(attached, ContextMenuAttached.SIGNAL_requested)
        if(!menu && !requested) continue
        let p = pointer.scenePos(item)
        let position = { x: x - p.x, y: y - p.y }
        if(requested) engine.emitArgs(attached, ContextMenuAttached.SIGNAL_requested, [require('../../Qt/Qt').Qt.point(position.x, position.y)])
        menu = attached.$v[ContextMenuAttached.MENU]
        if(menu && menu.popup) menu.popup(item, position.x, position.y)
        return
    }
})

// the type ContextMenu: its attached properties only (uncreatable)
class ContextMenu extends GroupObject {}
defineType(ContextMenu, {})

module.exports = { ContextMenu, ContextMenuAttached }
