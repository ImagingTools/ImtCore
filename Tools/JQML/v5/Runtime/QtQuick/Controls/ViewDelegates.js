// TableViewDelegate and TreeViewDelegate of QtQuick.Controls (qquicktableviewdelegate.cpp, qquicktreeviewdelegate.cpp and the
// Basic style's TableViewDelegate.qml / TreeViewDelegate.qml, Qt 6.12): the cell of a view with the required properties a
// view gives it, a background that shows the alternating rows, the selection and the current cell, the text of the display
// role, and an editor (TableView.editDelegate) that writes the EditRole back when it commits. A press on the cell is the
// view's (its delegates ignore it while pointerNavigationEnabled); the indicator of a tree node toggles the node.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Item } = require('../Item')
const { Rectangle } = require('../Rectangle')
const { Text } = require('../Text')
const { TextInput } = require('../TextInput')
const { Image } = require('../Image')
const { FocusScope } = require('../FocusScope')
const { MouseArea } = require('../MouseArea')
const { Component } = require('../../QtQml/Component')
const { TableView, TableViewAttached } = require('../TableView')
const QtEnums = require('../../Qt/enums')

// the Basic style's palette (QQuickBasicTheme, as Qt 6.12 gives it with QT_QUICK_CONTROLS_STYLE=Basic)
const P = {
    base: '#ffffff', alternateBase: '#f7f7f7', highlight: '#0066ff', highlightedText: '#090909', buttonText: '#26282a',
    windowText: '#26282a',
}

// Basic/images/arrow-indicator.png (11 x 18): a chevron pointing right, 3 px thick, as a vector of the same outline
const ARROW_WIDTH = 11, ARROW_HEIGHT = 18
const arrowIndicator = (color) => 'data:image/svg+xml,' + encodeURIComponent(
    `<svg xmlns="http://www.w3.org/2000/svg" width="${ARROW_WIDTH}" height="${ARROW_HEIGHT}" viewBox="0 0 ${ARROW_WIDTH} ${ARROW_HEIGHT}">`
    + `<path d="M1 1.2H3L10 9L3 16.8H1V15.3L6.6 9L1 2.7Z" fill="${color}"/></svg>`)

// the editor of the Basic delegates: a field over the cell, its text all selected, committed with setData(EditRole)
function editDelegate(control){
    return new Component(null, (parent, ctx) => {
        let scope = new FocusScope(parent)
        scope.$bind(Item.WIDTH, () => (scope.parent ? scope.parent.width : 0), null)
        scope.$bind(Item.HEIGHT, () => (scope.parent ? scope.parent.height : 0), null)
        let field = new TextInput(scope)
        field.$bind(Item.X, () => (control.$label ? control.$label.x : 0), null)
        field.$bind(Item.WIDTH, () => scope.width - field.x, null)
        field.$bind(Item.HEIGHT, () => scope.height, null)
        field.verticalAlignment = TextInput.AlignVCenter
        let model = ctx.model
        let edit = model.edit
        field.text = edit !== undefined && edit !== null ? String(edit) : model.display !== undefined && model.display !== null ? String(model.display) : ''
        field.focus = true
        engine.queueCall(() => { if(!(field.$f & engine.DESTROYED)) field.selectAll() })
        let attached = scope.TableView
        engine.connect(attached, TableViewAttached.SIGNAL_commit, () => {
            let view = control.$view()
            let m = view && view.$selectionSourceModel()
            if(!m) return
            // the row and column of the cell's context (TreeViewDelegate has no column property)
            let index = view.index(ctx.$cellRow, ctx.$cellColumn)
            if(!m.setData(index, field.text, QtEnums.EditRole)) console.warn('The model does not allow setting the EditRole data.')
        })
        return scope
    })
}

class ViewDelegateBase extends Item {
    constructor(parent = null){
        super(parent)
        let background = new Rectangle(this)
        background.$bind(Item.WIDTH, () => this.width, null)
        background.$bind(Item.HEIGHT, () => this.height, null)
        let label = new Text(this)
        label.elide = Text.ElideRight
        label.verticalAlignment = Text.AlignVCenter
        label.$bind(Item.HEIGHT, () => this.height, null)
        label.$bind(Item.VISIBLE, () => !this.editing, null)
        label.$bind(Text.TEXT, () => {
            let model = this.model
            let display = model ? model.display : undefined
            return display === undefined || display === null ? '' : String(display)
        }, null)
        label.$bind(Text.COLOR, () => (this.highlighted ? P.highlightedText : P.buttonText), null)
        background.$bind(Rectangle.COLOR, () => {
            let view = this.$view()
            if(this.highlighted) return P.highlight
            return view && view.alternatingRows && this.row % 2 !== 0 ? P.alternateBase : P.base
        }, null)
        background.border.$bind(require('../Rectangle').Border.WIDTH, () => (this.current ? 2 : 0), null)
        background.border.$bind(require('../Rectangle').Border.COLOR, () => (this.current ? P.highlight : P.windowText), null)
        this.$background = background
        this.$label = label
        this.TableView.editDelegate = editDelegate(this)
    }
    get background(){ return this.$background }
    get contentItem(){ return this.$label }
}
defineType(ViewDelegateBase, {
    props: {
        model: 'var', row: 'int', current: 'bool', selected: 'bool', editing: 'bool', highlighted: 'bool',
        padding: 'real', spacing: 'real',
    },
})

class TableViewDelegate extends ViewDelegateBase {
    constructor(parent = null){
        super(parent)
        // same as AbstractButton.qml: the content and the padding (0 in T.TableViewDelegate), the background has no
        // implicit size
        this.$label.$bind(Item.X, () => this.leftPadding, null)
        this.$label.$bind(Item.WIDTH, () => Math.max(0, this.width - this.leftPadding - this.rightPadding), null)
        this.$bind(Item.IMPLICITWIDTH, () => this.$label.implicitWidth + this.leftPadding + this.rightPadding, null)
        this.$bind(Item.IMPLICITHEIGHT, () => this.$label.implicitHeight + this.topPadding + this.bottomPadding, null)
        this.$bind(ViewDelegateBase.HIGHLIGHTED, () => this.selected, null)
    }
    $view(){ return this.tableView }
}
defineType(TableViewDelegate, {
    props: {
        tableView: 'var', column: 'int',
        leftPadding: 'real', rightPadding: 'real', topPadding: 'real', bottomPadding: 'real',
    },
})
TableViewDelegate.$qmlRequired = ['tableView', 'current', 'selected', 'editing', 'row', 'column', 'model']

class TreeViewDelegate extends ViewDelegateBase {
    constructor(parent = null){
        super(parent)
        this.$v[ViewDelegateBase.SPACING] = 4
        // the indicator: an arrow in a 20 x 40 area at depth * indentation, turned down while expanded
        let indicator = new Item(this)
        indicator.implicitWidth = 20
        indicator.implicitHeight = 40
        indicator.$bind(Item.X, () => this.leftMargin + this.depth * this.indentation, null)
        indicator.$bind(Item.Y, () => (this.height - indicator.height) / 2, null)
        indicator.$bind(Item.VISIBLE, () => this.isTreeNode && this.hasChildren, null)
        // ColorImage { source: ".../images/arrow-indicator.png"; color: control.palette.windowText }: the 11 x 18 chevron
        // of the Basic style, drawn in windowText (also on a highlighted row)
        let arrow = new Image(indicator)
        arrow.width = ARROW_WIDTH
        arrow.height = ARROW_HEIGHT
        arrow.source = arrowIndicator(P.windowText)
        arrow.$bind(Item.X, () => (indicator.width - arrow.width) / 2, null)
        arrow.$bind(Item.Y, () => (indicator.height - arrow.height) / 2, null)
        arrow.$bind(Item.ROTATION, () => (this.expanded ? 90 : 0), null)
        let area = new MouseArea(indicator)
        area.$bind(Item.WIDTH, () => indicator.width, null)
        area.$bind(Item.HEIGHT, () => indicator.height, null)
        // QQuickTreeViewDelegatePrivate::toggleExpanded
        engine.connect(area, MouseArea.SIGNAL_clicked, () => {
            let view = this.treeView
            if(view && view.pointerNavigationEnabled) view.toggleExpanded(this.row)
        })
        this.$indicator = indicator
        // __contentIndent: depth * indentation and the indicator with the spacing on a tree node
        let indent = () => (!this.isTreeNode ? 0 : this.depth * this.indentation + indicator.width + this.spacing)
        this.$label.$bind(Item.X, () => this.leftMargin + indent(), null)
        this.$label.$bind(Item.WIDTH, () => Math.max(0, this.width - this.$label.x - this.rightMargin), null)
        this.$bind(Item.IMPLICITWIDTH, () => this.leftMargin + indent() + this.$label.implicitWidth + this.rightMargin, null)
        this.$bind(Item.IMPLICITHEIGHT, () => Math.max(indicator.height, this.$label.implicitHeight) * 1.25, null)
        this.$bind(ViewDelegateBase.HIGHLIGHTED, () => {
            let view = this.treeView
            return this.selected || this.current || !!(view && (view.selectionBehavior === TableView.SelectRows
                || view.selectionBehavior === TableView.SelectionDisabled) && this.row === view.currentRow)
        }, null)
    }
    $view(){ return this.treeView }
    get indicator(){ return this.$indicator }
}
defineType(TreeViewDelegate, {
    props: {
        treeView: 'var', isTreeNode: 'bool', hasChildren: 'bool', expanded: 'bool', depth: 'int',
        indentation: { kind: 'real', initial: 20 },
        leftMargin: { kind: 'real', initial: 4 }, rightMargin: { kind: 'real', initial: 4 },
    },
})
TreeViewDelegate.$qmlRequired = ['treeView', 'isTreeNode', 'hasChildren', 'expanded', 'depth', 'current', 'selected', 'editing', 'row', 'model']

module.exports = { TableViewDelegate, TreeViewDelegate }
