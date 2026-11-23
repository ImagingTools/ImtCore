// The attached Accessible of QtQuick (QQuickAccessibleAttached, Qt 6.12): role, name, description and the states a
// screen reader reads, and the actions it can ask for. JQML keeps them (no accessibility tree of the page is made from
// them yet); the roles are those of QAccessible::Role.
const { defineType } = require('../Core/type')
const { GroupObject } = require('../Core/QtObject')
const { Item } = require('./Item')

const ROLES = {
    NoRole: 0x0, TitleBar: 0x1, MenuBar: 0x2, ScrollBar: 0x3, Grip: 0x4, Sound: 0x5, Cursor: 0x6, Caret: 0x7, AlertMessage: 0x8,
    Window: 0x9, Client: 0xa, PopupMenu: 0xb, MenuItem: 0xc, ToolTip: 0xd, Application: 0xe, Document: 0xf, Pane: 0x10,
    Chart: 0x11, Dialog: 0x12, Border: 0x13, Grouping: 0x14, Separator: 0x15, ToolBar: 0x16, StatusBar: 0x17, Table: 0x18,
    ColumnHeader: 0x19, RowHeader: 0x1a, Column: 0x1b, Row: 0x1c, Cell: 0x1d, Link: 0x1e, HelpBalloon: 0x1f, Assistant: 0x20,
    List: 0x21, ListItem: 0x22, Tree: 0x23, TreeItem: 0x24, PageTab: 0x25, PropertyPage: 0x26, Indicator: 0x27, Graphic: 0x28,
    StaticText: 0x29, EditableText: 0x2a, Button: 0x2b, PushButton: 0x2b, CheckBox: 0x2c, RadioButton: 0x2d, ComboBox: 0x2e,
    ProgressBar: 0x30, Dial: 0x31, HotkeyField: 0x32, Slider: 0x33, SpinBox: 0x34, Canvas: 0x35, Animation: 0x36,
    Equation: 0x37, ButtonDropDown: 0x38, ButtonMenu: 0x39, ButtonDropGrid: 0x3a, Whitespace: 0x3b, PageTabList: 0x3c,
    Clock: 0x3d, Splitter: 0x3e, LayeredPane: 0x80, Terminal: 0x81, Desktop: 0x82, Paragraph: 0x83, WebDocument: 0x84,
    Section: 0x85, Notification: 0x86, ActionList: 0x87, Heading: 0x88, ColorChooser: 0x404, Footer: 0x40e, Form: 0x410,
    UserRole: 0x0000ffff,
}

class AccessibleAttached extends GroupObject {}
defineType(AccessibleAttached, {
    props: {
        role: 'int', name: 'string', description: 'string', id: 'string', ignored: 'bool', labelledBy: 'var', labelFor: 'var',
        checkable: 'bool', checked: 'bool', editable: 'bool', focusable: 'bool', focused: { kind: 'bool', readonly: true },
        multiLine: 'bool', readOnly: 'bool', selected: 'bool', selectable: 'bool', pressed: 'bool', checkStateMixed: 'bool',
        defaultButton: 'bool', passwordEdit: 'bool', selectableText: 'bool', searchEdit: 'bool',
    },
    signals: ['pressAction', 'toggleAction', 'increaseAction', 'decreaseAction', 'scrollUpAction', 'scrollDownAction',
        'scrollLeftAction', 'scrollRightAction', 'previousPageAction', 'nextPageAction'],
})
Object.assign(AccessibleAttached.prototype, ROLES)

// the type Accessible: its attached properties and the roles (Accessible.Button)
class Accessible extends GroupObject {}
defineType(Accessible, {})
Object.assign(Accessible, ROLES)

const prop = Item.$type.propByName.get('Accessible')
prop.group = AccessibleAttached
Object.defineProperty(Item.prototype, 'Accessible', {
    get(){ return this.$v[prop.id] || (this.$v[prop.id] = new AccessibleAttached(this)) },
    configurable: true,
})
// Accessible.announce(message, politeness) (Qt 6.8): nothing to say to without an accessibility tree
Accessible.announce = function(){}

// ---- EnterKey (QQuickEnterKeyAttached, Qt 6.4): the kind of the enter key an on-screen keyboard shows for an item ----
class EnterKeyAttached extends GroupObject {}
defineType(EnterKeyAttached, { props: { type: 'int' } })
class EnterKey extends GroupObject {}
defineType(EnterKey, {})
{
    const prop = Item.$type.propByName.get('EnterKey')
    prop.group = EnterKeyAttached
    Object.defineProperty(Item.prototype, 'EnterKey', {
        get(){ return this.$v[prop.id] || (this.$v[prop.id] = new EnterKeyAttached(this)) },
        configurable: true,
    })
}

module.exports = { Accessible, AccessibleAttached, EnterKey, EnterKeyAttached }
