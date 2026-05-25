const Item = require("../Item")
const Bool = require("../../QtQml/Bool")
const Int = require("../../QtQml/Int")
const Real = require("../../QtQml/Real")
const StringProp = require("../../QtQml/String")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

/**
 * TreeViewDelegate — standard delegate for TreeView cells.
 * Renders: indent spacer | expand arrow | text label
 * Properties are set by TreeView.__applyTreeProps.
 */
class TreeViewDelegate extends Item {
    static meta = Object.assign({}, Item.meta, {
        // Set by TreeView
        isTreeNode:   { type: Bool, value: false },
        expanded:     { type: Bool, value: false },
        hasChildren:  { type: Bool, value: false },
        depth:        { type: Int,  value: 0 },
        treeView:     { type: Var,  value: undefined },
        // Selection
        selected:     { type: Bool, value: false },
        current:      { type: Bool, value: false },
        editing:      { type: Bool, value: false },
        // Layout
        indentation:  { type: Real, value: 20 },
        leftMargin:   { type: Real, value: 4 },
        rightMargin:  { type: Real, value: 4 },
        // Content
        text:         { type: StringProp, value: '' },
        indicator:    { type: Var, value: undefined },
        contentItem:  { type: Var, value: undefined },

        isTreeNodeChanged:  { type: Signal, args: [] },
        expandedChanged:    { type: Signal, args: [] },
        hasChildrenChanged: { type: Signal, args: [] },
        depthChanged:       { type: Signal, args: [] },
        selectedChanged:    { type: Signal, args: [] },
        currentChanged:     { type: Signal, args: [] },

        clicked:       { type: Signal, args: [] },
        doubleClicked: { type: Signal, args: [] },
        'TableView.reused': { type: Signal, args: [] },
        'TableView.pooled': { type: Signal, args: [] },
    })

    static create(parent, properties = {}) {
        let obj = super.create(parent, properties)

        // Model context shortcuts
        let model   = properties.model   || {}
        let display = properties.modelData !== undefined ? properties.modelData : (model.display !== undefined ? model.display : '')
        let depth       = model.depth       || 0
        let isTreeNode  = !!model.isTreeNode
        let hasChildren = !!model.hasChildren
        let expanded    = !!model.expanded
        let col         = model.column || 0
        let row         = model.row    || 0

        obj.__tvdCol = col
        obj.__tvdRow = row

        // Container
        let container = document.createElement('div')
        container.style.cssText =
            'display:flex;align-items:center;height:100%;overflow:hidden;box-sizing:border-box;' +
            'font-size:13px;font-family:inherit;color:inherit;cursor:default;user-select:none;'
        obj.__container = container

        if (col === 0 && isTreeNode) obj.__buildTreeColumn(container, depth, hasChildren, expanded)

        let textEl = document.createElement('span')
        textEl.style.cssText = 'overflow:hidden;text-overflow:ellipsis;white-space:nowrap;flex:1;'
        textEl.textContent   = String(display !== null && display !== undefined ? display : '')
        container.appendChild(textEl)
        obj.__textEl = textEl

        obj.__DOM.appendChild(container)
        obj.__DOM.style.overflow = 'hidden'
        obj.__applyRowBg(row)
        obj.__applyBorder()

        return obj
    }

    __buildTreeColumn(container, depth, hasChildren, expanded) {
        let indent = this.indentation || 20
        let lm     = this.leftMargin  || 4

        if (lm > 0) {
            let el = document.createElement('span')
            el.style.cssText = 'display:inline-block;flex-shrink:0;width:' + lm + 'px;'
            container.appendChild(el)
            this.__leftMarginEl = el
        }

        let indentEl = document.createElement('span')
        indentEl.style.cssText = 'display:inline-block;flex-shrink:0;width:' + (depth * indent) + 'px;'
        container.appendChild(indentEl)
        this.__indentEl = indentEl

        let indicatorWrap = document.createElement('span')
        indicatorWrap.style.cssText =
            'display:inline-flex;align-items:center;justify-content:center;width:16px;height:16px;flex-shrink:0;'
        container.appendChild(indicatorWrap)
        this.__indicatorWrap = indicatorWrap

        if (hasChildren) {
            let arrow = document.createElement('span')
            arrow.style.cssText =
                'display:inline-block;font-size:10px;cursor:pointer;user-select:none;transition:transform 0.15s ease;'
            arrow.textContent    = '\u25B6'
            if (expanded) arrow.style.transform = 'rotate(90deg)'
            indicatorWrap.appendChild(arrow)
            this.__arrowEl = arrow

            arrow.addEventListener('click', (e) => {
                e.stopPropagation()
                let tv = this.treeView
                if (tv && typeof tv.toggleExpanded === 'function') tv.toggleExpanded(this.__tableRow)
            })
        }

        let spacer = document.createElement('span')
        spacer.style.cssText = 'display:inline-block;flex-shrink:0;width:4px;'
        container.appendChild(spacer)
    }

    // Called by TreeView on cell reuse
    __updateCell(data) {
        let col = data.column !== undefined ? data.column : this.__tvdCol
        let row = data.row    !== undefined ? data.row    : this.__tvdRow
        this.__tvdCol = col; this.__tvdRow = row

        if (this.__textEl) this.__textEl.textContent = String(data.display !== null && data.display !== undefined ? data.display : '')

        if (col === 0 && data.isTreeNode) {
            this.__rebuildTreeColumn(data.depth || 0, !!data.hasChildren, !!data.expanded)
        } else {
            this.__clearTreeColumn()
        }
        this.__applyRowBg(row)
        this.__applyBg()
        this.__applyOutline()
    }

    __rebuildTreeColumn(depth, hasChildren, expanded) {
        let indent = this.indentation || 20
        if (this.__indentEl) this.__indentEl.style.width = (depth * indent) + 'px'
        if (this.__indicatorWrap) {
            this.__indicatorWrap.innerHTML = ''
            if (hasChildren) {
                let arrow = document.createElement('span')
                arrow.style.cssText = 'display:inline-block;font-size:10px;cursor:pointer;user-select:none;transition:transform 0.15s ease;'
                arrow.textContent   = '\u25B6'
                if (expanded) arrow.style.transform = 'rotate(90deg)'
                this.__indicatorWrap.appendChild(arrow)
                this.__arrowEl = arrow
                arrow.addEventListener('click', (e) => {
                    e.stopPropagation()
                    let tv = this.treeView
                    if (tv && typeof tv.toggleExpanded === 'function') tv.toggleExpanded(this.__tableRow)
                })
            } else { this.__arrowEl = null }
        }
    }

    __clearTreeColumn() {
        if (this.__indentEl)    this.__indentEl.style.width = '0px'
        if (this.__indicatorWrap) this.__indicatorWrap.innerHTML = ''
        if (this.__leftMarginEl) this.__leftMarginEl.style.width = '0px'
        this.__arrowEl = null
    }

    SLOT_expandedChanged(o, n) {
        if (this.__arrowEl) this.__arrowEl.style.transform = n ? 'rotate(90deg)' : 'rotate(0deg)'
    }
    SLOT_selectedChanged() { this.__applyBg() }
    SLOT_currentChanged()  { this.__applyOutline() }

    __applyRowBg(row) { this.__evenRow = ((row !== null && row !== undefined ? row : this.__tvdRow || 0) % 2 === 0) }

    __applyBg() {
        this.__DOM.style.background = this.selected ? '#264f78' : (this.__evenRow ? '#2d2d2d' : '#252526')
    }

    __applyOutline() {
        if (this.current) {
            this.__DOM.style.outline = '1px solid #007acc'
            this.__DOM.style.outlineOffset = '-1px'
        } else {
            this.__DOM.style.outline = 'none'
            this.__DOM.style.outlineOffset = ''
        }
    }

    __applyBorder() { this.__DOM.style.borderBottom = '1px solid #3c3c3c' }
}

module.exports = TreeViewDelegate
