// HorizontalHeaderView and VerticalHeaderView of QtQuick.Controls (qquickheaderview.cpp of Qt 6.12 and the Basic style's
// HorizontalHeaderView.qml / *HeaderViewDelegate.qml): a TableView synced to another one along its orientation, whose
// model is the header data of that one's model (QHeaderDataProxyModel). Its default delegate is the Basic style's: a control
// with a light background, a midlight border and the text of the textRole, centered, eight pixels of padding.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { TableView } = require('../TableView')
const { Item } = require('../Item')
const { Rectangle } = require('../Rectangle')
const { Text } = require('../Text')
const { Component } = require('../../QtQml/Component')
const { AbstractItemModel, AbstractTableModel, ListAccessorModel, INVALID, adaptModel } = require('../../QtQml/Models/ItemModel')
const { TableModel } = require('../../QtQml/Models/TableModel')
const QtEnums = require('../../Qt/enums')

// ---- QHeaderDataProxyModel: headerData() of a model as data() of a one-row (one-column) table ---------------------------
class HeaderDataProxyModel extends AbstractItemModel {
    constructor(orientation){
        super(null)
        this.$orientation = orientation
        this.$source = null
        this.$links = []
        this.$headerView = null
    }
    get sourceModel(){ return this.$source }
    setSourceModel(model){
        if(this.$source === model) return
        this.beginResetModel()
        for(let [object, signal, fn] of this.$links) engine.disconnect(object, signal, fn)
        this.$links = []
        this.$source = model
        if(model){
            let M = AbstractItemModel
            let link = (signal, fn) => {
                engine.connect(model, signal, fn)
                this.$links.push([model, signal, fn])
            }
            link(M.SIGNAL_headerDataChanged, (orientation, first, last) => {
                if(orientation !== this.$orientation) return
                if(orientation === QtEnums.Horizontal) engine.emitArgs(this, M.SIGNAL_dataChanged, [this.createIndex(0, first), this.createIndex(0, last), []])
                else engine.emitArgs(this, M.SIGNAL_dataChanged, [this.createIndex(first, 0), this.createIndex(last, 0), []])
            })
            // the changes of the source pass on as they are (QHeaderDataProxyModel connects the signals to its own)
            for(let name of ['modelAboutToBeReset', 'modelReset', 'rowsAboutToBeMoved', 'rowsMoved', 'rowsAboutToBeInserted', 'rowsInserted',
                'rowsAboutToBeRemoved', 'rowsRemoved', 'columnsAboutToBeMoved', 'columnsMoved', 'columnsAboutToBeInserted', 'columnsInserted',
                'columnsAboutToBeRemoved', 'columnsRemoved', 'layoutAboutToBeChanged', 'layoutChanged']){
                let signal = M['SIGNAL_' + name]
                link(signal, (...args) => engine.emitArgs(this, signal, args))
            }
        }
        this.endResetModel()
    }
    get orientation(){ return this.$orientation }
    setOrientation(orientation){
        if(orientation === this.$orientation) return
        this.beginResetModel()
        this.$orientation = orientation
        this.endResetModel()
    }
    index(row, column, parent = INVALID){ return this.hasIndex(row, column, parent) ? this.createIndex(row, column) : INVALID }
    parent(){ return INVALID }
    sibling(row, column){ return this.index(row, column) }
    $rowCount(parent = INVALID){
        if(parent && parent.valid) return 0
        return !this.$source ? -1 : this.$orientation === QtEnums.Horizontal ? 1 : this.$source.$rowCount(parent)
    }
    $columnCount(parent = INVALID){
        if(parent && parent.valid) return 0
        return !this.$source ? -1 : this.$orientation === QtEnums.Vertical ? 1 : this.$source.$columnCount(parent)
    }
    rowCount(parent){ return this.$rowCount(parent) }
    columnCount(parent){ return this.$columnCount(parent) }
    hasIndex(row, column, parent = INVALID){
        if(row < 0 || column < 0 || (parent && parent.valid)) return false
        return row < this.$rowCount(parent) && column < this.$columnCount(parent)
    }
    data(index, role = QtEnums.DisplayRole){
        if(!this.$source || !index || !this.hasIndex(index.row, index.column)) return undefined
        let section = this.$orientation === QtEnums.Vertical ? index.row : index.column
        return this.$source.headerData(section, this.$orientation, role)
    }
    setData(index, value, role = QtEnums.EditRole){
        if(!this.hasIndex(index.row, index.column)) return false
        let section = this.$orientation === QtEnums.Vertical ? index.row : index.column
        return this.$source.setHeaderData ? this.$source.setHeaderData(section, this.$orientation, value, role) : false
    }
    hasChildren(parent = INVALID){ return parent && parent.valid ? false : this.$rowCount(parent) > 0 && this.$columnCount(parent) > 0 }
    roleNames(){
        let names = this.$source ? this.$source.roleNames() : super.roleNames()
        let view = this.$headerView
        if(view){
            let textRole = view.$v[HeaderViewBase.TEXTROLE] || 'display'
            if(![...names.values()].includes(textRole)) console.warn("HeaderView: The 'textRole' property contains a role that doesn't exist in the model: " + textRole + '. Check your model\'s roleNames() implementation')
        }
        return names
    }
    $roleOf(name){
        let names = this.$source ? (this.$source.$roleNameCache || (this.$source.$roleNameCache = this.$source.roleNames())) : new Map(require('../../QtQml/Models/ItemModel').DEFAULT_ROLE_NAMES)
        for(let [role, roleName] of names) if(roleName === name) return role
        return -1
    }
}

// ---- the Basic style's delegate (HorizontalHeaderViewDelegate.qml / VerticalHeaderViewDelegate.qml) ----------------------
const PADDING = 8
// the Basic palette of Qt 6.12 (QT_QUICK_CONTROLS_STYLE=Basic): light, midlight, windowText; a highlighted (selected)
// section looks the same in the Basic style
const LIGHT = '#f6f6f6', MIDLIGHT = '#e4e4e4', WINDOW_TEXT = '#26282a'

// HeaderViewDelegate (QQuickHeaderViewDelegate, Qt 6.10): a control (background and contentItem, padding) with the
// required properties a header gives
class HeaderViewDelegate extends Item {
    get background(){ return this.$background || null }
    get contentItem(){ return this.$contentItem || null }
}
defineType(HeaderViewDelegate, {
    props: {
        headerView: 'var', model: 'var', index: 'int', row: 'int', column: 'int', selected: 'bool', current: 'bool',
        containsDrag: 'bool', highlighted: 'bool', padding: 'real',
    },
})
HeaderViewDelegate.$qmlRequired = ['headerView', 'model', 'index', 'row', 'column', 'selected', 'current', 'containsDrag']

function defaultDelegate(){
    return new Component(null, (parent, ctx) => {
        let control = new HeaderViewDelegate(parent)
        control.padding = PADDING
        // background: Rectangle { border.color: palette.midlight; color: palette.light }
        let background = new Rectangle(control)
        background.$bind(Item.WIDTH, () => control.width, null)
        background.$bind(Item.HEIGHT, () => control.height, null)
        background.border.color = MIDLIGHT
        background.color = LIGHT
        // contentItem: Label { text: control.model[control.headerView.textRole] }; the model object of a list accessor (a
        // JS array, a number) gives its modelData for any name, so "" (the textRole of such a model) shows the value
        let label = new Text(control)
        let view = ctx.$view
        label.$bind(Text.TEXT, () => {
            let role = view.$v[HeaderViewBase.TEXTROLE]
            let value = ctx.$role(role || 'modelData')
            return value === undefined || value === null ? '' : String(value)
        }, null)
        label.horizontalAlignment = Text.AlignHCenter
        label.verticalAlignment = Text.AlignVCenter
        label.$bind(Item.X, () => control.padding, null)
        label.$bind(Item.Y, () => control.padding, null)
        label.$bind(Item.WIDTH, () => Math.max(0, control.width - 2 * control.padding), null)
        label.$bind(Item.HEIGHT, () => Math.max(0, control.height - 2 * control.padding), null)
        label.color = WINDOW_TEXT
        // same as AbstractButton.qml: the content and the padding
        control.$bind(Item.IMPLICITWIDTH, () => label.implicitWidth + 2 * control.padding, null)
        control.$bind(Item.IMPLICITHEIGHT, () => label.implicitHeight + 2 * control.padding, null)
        // highlighted: selected
        control.$bind(HeaderViewDelegate.HIGHLIGHTED, () => control.selected, null)
        control.$background = background
        control.$contentItem = label
        return control
    })
}

// HorizontalHeaderViewDelegate / VerticalHeaderViewDelegate of QtQuick.Controls.Basic (Qt 6.10): the delegate above as a type,
// declared in a header (delegate: HorizontalHeaderViewDelegate { }); the text from its required model and headerView
class BasicHeaderViewDelegate extends HeaderViewDelegate {
    constructor(parent = null){
        super(parent)
        let control = this
        control.padding = PADDING
        let background = new Rectangle(control)
        background.$bind(Item.WIDTH, () => control.width, null)
        background.$bind(Item.HEIGHT, () => control.height, null)
        background.border.color = MIDLIGHT
        background.color = LIGHT
        let label = new Text(control)
        label.$bind(Text.TEXT, () => {
            let model = control.model, view = control.headerView
            // (the header the delegate is in, when its required headerView is not given)
            for(let it = control.$parent; !view && it; it = it.$parent) if(it instanceof HeaderViewBase) view = it
            if(!model) return ''
            let role = view ? view.textRole : ''
            let value = model[role || '']
            if(value === undefined) value = model.modelData
            return value === undefined || value === null ? '' : String(value)
        }, null)
        label.horizontalAlignment = Text.AlignHCenter
        label.verticalAlignment = Text.AlignVCenter
        label.$bind(Item.X, () => control.padding, null)
        label.$bind(Item.Y, () => control.padding, null)
        label.$bind(Item.WIDTH, () => Math.max(0, control.width - 2 * control.padding), null)
        label.$bind(Item.HEIGHT, () => Math.max(0, control.height - 2 * control.padding), null)
        label.color = WINDOW_TEXT
        control.$bind(Item.IMPLICITWIDTH, () => label.implicitWidth + 2 * control.padding, null)
        control.$bind(Item.IMPLICITHEIGHT, () => label.implicitHeight + 2 * control.padding, null)
        control.$bind(HeaderViewDelegate.HIGHLIGHTED, () => control.selected, null)
        control.$background = background
        control.$contentItem = label
    }
}
defineType(BasicHeaderViewDelegate, {})
class HorizontalHeaderViewDelegate extends BasicHeaderViewDelegate {}
defineType(HorizontalHeaderViewDelegate, {})
class VerticalHeaderViewDelegate extends BasicHeaderViewDelegate {}
defineType(VerticalHeaderViewDelegate, {})

// ---- QQuickHeaderViewBase -----------------------------------------------------------------------------------------------
class HeaderViewBase extends TableView {
    constructor(parent, orientation){
        super(parent)
        this.$proxy = new HeaderDataProxyModel(orientation)
        this.$proxy.$headerView = this
        this.$modelExplicitlySet = false
        this.$v[TableView.SYNCDIRECTION] = orientation
        this.$v[TableView.DELEGATE] = defaultDelegate()
        this.$drag = null
    }
    $orientation(){ return this.$proxy.orientation }

    // QQuickHeaderViewBasePrivate::setModelImpl: a table model is shown through the proxy, any other model as it is
    $setModelValue(value){
        this.$modelExplicitlySet = value !== undefined
        let table = value instanceof AbstractTableModel || value instanceof TableModel ? value : null
        if(table){
            if(table !== this.$proxy.sourceModel){
                this.$proxy.setSourceModel(table)
                this.$assignedModel = this.$proxy
                this.$needsModelSynchronization = true
                this.$scheduleRebuildTable(1)
            }
            return
        }
        this.$proxy.setSourceModel(null)
        super.$setModelValue(value)
    }
    $modelValue(){ return this.$proxy.sourceModel ? this.$proxy : super.$modelValue() }

    // QQuickHeaderViewBasePrivate::syncModel: without a model of its own, the header of the sync view's model
    $syncModel(){
        let syncView = this.$v[TableView.SYNCVIEW]
        if(!this.$modelExplicitlySet && syncView){
            let assigned = syncView.$assignedModel
            let source = assigned instanceof AbstractItemModel ? assigned : assigned && assigned.$rows && assigned.$addView ? adaptModel(assigned).model : null
            if(source !== this.$proxy.sourceModel){
                this.$proxy.setSourceModel(source)
                this.$assignedModel = this.$proxy
                this.$needsModelSynchronization = true
                engine.emitArgs(this, TableView.SIGNAL_modelChanged, [])
            }
        }
        super.$syncModel()
        let aim = this.$tableModel.model
        this.$isTransposed = this.$orientation() === QtEnums.Horizontal ? !aim || aim.$columnCount() === 1 : false
        // the display role of an item model; a list accessor (a JS array, a number) keeps "" and shows its modelData
        if(!this.$v[HeaderViewBase.TEXTROLE] && aim && !(aim instanceof ListAccessorModel)) this.$v[HeaderViewBase.TEXTROLE] = 'display'
    }

    $syncSyncView(){
        if(this.$v[TableView.SYNCDIRECTION] !== this.$orientation()){
            console.warn('HeaderView: Setting syncDirection other than Qt::' + (this.$orientation() === QtEnums.Horizontal ? 'Horizontal' : 'Vertical') + ' is invalid.')
            this.$v[TableView.SYNCDIRECTION] = this.$orientation()
        }
        super.$syncSyncView()
    }

    $selectionSourceModel(){ return this.$proxy }

    // QQuickHeaderViewBasePrivate::initItemCallback: the required headerView and model
    $initNewItem(ctx, item){
        super.$initNewItem(ctx, item)
        ctx.$setTableRequired('headerView', this)
        ctx.$setTableRequired('model', ctx.model)
    }

    $logicalRowIndex(visual){ return this.$orientation() === QtEnums.Horizontal ? visual : super.$logicalRowIndex(visual) }
    $logicalColumnIndex(visual){ return this.$orientation() === QtEnums.Vertical ? visual : super.$logicalColumnIndex(visual) }
    $visualRowIndex(logical){ return this.$orientation() === QtEnums.Horizontal ? logical : super.$visualRowIndex(logical) }
    $visualColumnIndex(logical){ return this.$orientation() === QtEnums.Vertical ? logical : super.$visualColumnIndex(logical) }

    // QQuickTableViewSectionDragHandler: a section dragged onto another one moves there (movableColumns / movableRows)
    $movable(){ return false }
    $mousePress(x, y, button, buttons, modifiers){
        let took = super.$mousePress(x, y, button, buttons, modifiers)
        if(this.$movable() && button === 1 && this.$resize.state === 'listening'){
            let cell = this.cellAtPosition({ x: x + this.$contentX(), y: y + this.$contentY() })
            let horizontal = this.$orientation() === QtEnums.Horizontal
            let source = horizontal ? cell.x : cell.y
            if(source >= 0) this.$drag = { source, x, y, dragging: false }
            return true
        }
        return took
    }
    $mouseMove(x, y, buttons){
        let d = this.$drag
        if(d){
            if(!d.dragging && (Math.abs(x - d.x) > 10 || Math.abs(y - d.y) > 10)) d.dragging = true
            if(d.dragging) return
        }
        super.$mouseMove(x, y, buttons)
    }
    $mouseRelease(x, y){
        let d = this.$drag
        this.$drag = null
        if(d && d.dragging){
            let cell = this.cellAtPosition({ x: x + this.$contentX(), y: y + this.$contentY() }, true)
            let destination = this.$orientation() === QtEnums.Horizontal ? cell.x : cell.y
            if(destination >= 0 && destination !== d.source){
                if(this.$orientation() === QtEnums.Horizontal) this.moveColumn(d.source, destination)
                else this.moveRow(d.source, destination)
            }
            return
        }
        super.$mouseRelease(x, y)
    }
}
defineType(HeaderViewBase, {
    props: {
        textRole: 'string',
    },
})

class HorizontalHeaderView extends HeaderViewBase {
    constructor(parent = null){
        super(parent, QtEnums.Horizontal)
        this.$v[TableView.FLICKABLEDIRECTION] = 1    // Flickable.HorizontalFlick
        this.$v[TableView.RESIZABLECOLUMNS] = true
        // implicitWidth: syncView ? syncView.width : 0; implicitHeight: Math.max(1, contentHeight)
        this.$bind(Item.IMPLICITWIDTH, () => { let s = this.syncView; return s ? s.width : 0 }, null)
        this.$bind(Item.IMPLICITHEIGHT, () => Math.max(1, this.contentHeight), null)
    }
    $movable(){ return !!this.$v[HorizontalHeaderView.MOVABLECOLUMNS] }
}
defineType(HorizontalHeaderView, { props: { movableColumns: 'bool' } })

class VerticalHeaderView extends HeaderViewBase {
    constructor(parent = null){
        super(parent, QtEnums.Vertical)
        this.$v[TableView.FLICKABLEDIRECTION] = 2    // Flickable.VerticalFlick
        this.$v[TableView.RESIZABLEROWS] = true
        this.$bind(Item.IMPLICITWIDTH, () => Math.max(1, this.contentWidth), null)
        this.$bind(Item.IMPLICITHEIGHT, () => { let s = this.syncView; return s ? s.height : 0 }, null)
    }
    $movable(){ return !!this.$v[VerticalHeaderView.MOVABLEROWS] }
}
defineType(VerticalHeaderView, { props: { movableRows: 'bool' } })

module.exports = { HorizontalHeaderView, VerticalHeaderView, HeaderViewBase, HeaderDataProxyModel, HeaderViewDelegate, HorizontalHeaderViewDelegate, VerticalHeaderViewDelegate }
