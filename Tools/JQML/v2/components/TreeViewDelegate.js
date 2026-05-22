const { Item } = require('./Item')
const { QVar, QReal, QBool, QInt, QString } = require('../utils/properties')

/**
 * TreeViewDelegate — standard delegate for TreeView (Qt6 API).
 *
 * Renders a tree row with:
 *   - indentation based on depth
 *   - expand/collapse indicator (arrow) for nodes with children
 *   - text content from display role
 *   - selection / current highlighting
 *
 * Properties mirror Qt6 TreeViewDelegate:
 *   current, selected, editing, isTreeNode, expanded, hasChildren,
 *   depth, indentation, leftMargin, rightMargin, treeView
 */
class TreeViewDelegate extends Item {

    static defaultProperties = {
        // ── Required tree properties (set by TreeView.$createCell) ──
        isTreeNode:   { type: QBool, value: false },
        expanded:     { type: QBool, value: false, changed: '$expandedVisualChanged' },
        hasChildren:  { type: QBool, value: false },
        depth:        { type: QInt,  value: 0 },
        treeView:     { type: QVar,  value: null },

        // ── Selection / focus ──
        selected: { type: QBool, value: false, changed: '$selectedChanged' },
        current:  { type: QBool, value: false, changed: '$currentChanged' },
        editing:  { type: QBool, value: false },

        // ── Layout tuning ──
        indentation: { type: QReal, value: 20 },
        leftMargin:  { type: QReal, value: 4 },
        rightMargin: { type: QReal, value: 4 },

        // ── Content ──
        text:        { type: QString, value: '' },

        // ── Custom indicator / contentItem (Component slots) ──
        indicator:   { type: QVar, value: null },
        contentItem: { type: QVar, value: null },
    }

    static defaultSignals = {
        clicked: { params: [] },
        doubleClicked: { params: [] },
        'TableView.reused': { params: [] },
        'TableView.pooled': { params: [] },
    }

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)

        // ── Extract initial model data ──
        let depth       = exModel ? (exModel.depth || 0)        : 0
        let isTreeNode  = exModel ? !!exModel.isTreeNode        : false
        let hasChildren = exModel ? !!exModel.hasChildren       : false
        let expanded    = exModel ? !!exModel.expanded          : false
        let display     = exModel ? (exModel.display != null ? String(exModel.display) : '') : ''
        let col         = exModel ? (exModel.column || 0)       : 0
        let row         = exModel ? (exModel.row || 0)          : 0

        this.$tvdCol = col
        this.$tvdRow = row

        // ── Root container — flex row ──
        let container = document.createElement('div')
        container.style.cssText =
            'display:flex;align-items:center;height:100%;overflow:hidden;box-sizing:border-box;' +
            'font-size:13px;font-family:inherit;color:inherit;cursor:default;user-select:none;'
        this.$container = container

        // ── Build tree column (col 0) content ──
        if (col === 0 && isTreeNode) {
            this._buildTreeColumn(container, depth, hasChildren, expanded)
        }

        // ── Text label ──
        let textEl = document.createElement('span')
        textEl.style.cssText =
            'overflow:hidden;text-overflow:ellipsis;white-space:nowrap;flex:1;'
        textEl.textContent = display
        container.appendChild(textEl)
        this.$textEl = textEl

        this.$dom.appendChild(container)

        // ── Default styling ──
        this.$dom.style.overflow = 'hidden'
        this._applyRowBackground(row)
        this._applyBorder()

        // ── Selection / current highlight ──
        this.getProperty('selected').getNotify().connect(() => {
            this._applyBackground()
        })
        this.getProperty('current').getNotify().connect(() => {
            this._applyOutline()
        })
    }

    // ── Build tree-column-specific elements ──
    _buildTreeColumn(container, depth, hasChildren, expanded) {
        let indentation = this.getPropertyValue('indentation')
        let leftMargin  = this.getPropertyValue('leftMargin')

        // Left margin spacer
        if (leftMargin > 0) {
            let lm = document.createElement('span')
            lm.style.cssText = 'display:inline-block;flex-shrink:0;width:' + leftMargin + 'px;'
            container.appendChild(lm)
            this.$leftMarginEl = lm
        }

        // Depth indentation
        let indent = document.createElement('span')
        indent.style.cssText = 'display:inline-block;flex-shrink:0;width:' + (depth * indentation) + 'px;'
        container.appendChild(indent)
        this.$indentEl = indent

        // Expand/collapse indicator
        let indicatorWrap = document.createElement('span')
        indicatorWrap.style.cssText =
            'display:inline-flex;align-items:center;justify-content:center;' +
            'width:16px;height:16px;flex-shrink:0;'
        container.appendChild(indicatorWrap)
        this.$indicatorWrap = indicatorWrap

        if (hasChildren) {
            let arrow = document.createElement('span')
            arrow.style.cssText =
                'display:inline-block;font-size:10px;cursor:pointer;user-select:none;' +
                'transition:transform 0.15s ease;'
            arrow.textContent = '\u25B6' // ▶
            if (expanded) arrow.style.transform = 'rotate(90deg)'
            indicatorWrap.appendChild(arrow)
            this.$arrowEl = arrow

            arrow.addEventListener('click', (e) => {
                e.stopPropagation()
                let tv = this.getPropertyValue('treeView')
                if (tv && typeof tv.toggleExpanded === 'function') {
                    tv.toggleExpanded(this.$tableRow)
                }
            })
        }

        // Spacing between indicator and content
        let spacing = document.createElement('span')
        spacing.style.cssText = 'display:inline-block;flex-shrink:0;width:4px;'
        container.appendChild(spacing)
        this.$spacingEl = spacing
    }

    // ── $updateCell — called by TreeView on cell reuse ──
    $updateCell(data) {
        let col         = data.column !== undefined ? data.column : this.$tvdCol
        let row         = data.row    !== undefined ? data.row    : this.$tvdRow
        let depth       = data.depth       || 0
        let isTreeNode  = !!data.isTreeNode
        let hasChildren = !!data.hasChildren
        let expanded    = !!data.expanded
        let display     = data.display != null ? String(data.display) : ''

        this.$tvdCol = col
        this.$tvdRow = row

        // Update text
        if (this.$textEl) this.$textEl.textContent = display

        // Rebuild tree column if structure changed
        if (col === 0 && isTreeNode) {
            this._rebuildTreeColumn(depth, hasChildren, expanded)
        } else {
            this._clearTreeColumn()
        }

        // Row background
        this._applyRowBackground(row)
        this._applyBackground()
        this._applyOutline()
    }

    _rebuildTreeColumn(depth, hasChildren, expanded) {
        let indentation = this.getPropertyValue('indentation')

        // Update indentation width
        if (this.$indentEl) {
            this.$indentEl.style.width = (depth * indentation) + 'px'
        }

        // Update arrow
        if (this.$indicatorWrap) {
            this.$indicatorWrap.innerHTML = ''
            if (hasChildren) {
                let arrow = document.createElement('span')
                arrow.style.cssText =
                    'display:inline-block;font-size:10px;cursor:pointer;user-select:none;' +
                    'transition:transform 0.15s ease;'
                arrow.textContent = '\u25B6'
                if (expanded) arrow.style.transform = 'rotate(90deg)'
                this.$indicatorWrap.appendChild(arrow)
                this.$arrowEl = arrow

                arrow.addEventListener('click', (e) => {
                    e.stopPropagation()
                    let tv = this.getPropertyValue('treeView')
                    if (tv && typeof tv.toggleExpanded === 'function') {
                        tv.toggleExpanded(this.$tableRow)
                    }
                })
            } else {
                this.$arrowEl = null
            }
        }
    }

    _clearTreeColumn() {
        // Remove tree-specific elements if this cell is no longer the tree column
        if (this.$indentEl) { this.$indentEl.style.width = '0px' }
        if (this.$indicatorWrap) { this.$indicatorWrap.innerHTML = '' }
        if (this.$leftMarginEl) { this.$leftMarginEl.style.width = '0px' }
        if (this.$spacingEl) { this.$spacingEl.style.width = '0px' }
        this.$arrowEl = null
    }

    // ── Visual property change handlers ──

    $expandedVisualChanged() {
        if (this.$arrowEl) {
            let exp = this.getPropertyValue('expanded')
            this.$arrowEl.style.transform = exp ? 'rotate(90deg)' : 'rotate(0deg)'
        }
    }

    $selectedChanged() {
        this._applyBackground()
    }

    $currentChanged() {
        this._applyOutline()
    }

    // ── Styling helpers ──

    _applyRowBackground(row) {
        this.$evenRow = (row != null ? row : this.$tvdRow || 0) % 2 === 0
    }

    _applyBackground() {
        let selected = this.getPropertyValue('selected')
        if (selected) {
            this.$dom.style.background = '#264f78'
        } else {
            this.$dom.style.background = this.$evenRow ? '#2d2d2d' : '#252526'
        }
    }

    _applyOutline() {
        let current = this.getPropertyValue('current')
        if (current) {
            this.$dom.style.outline = '1px solid #007acc'
            this.$dom.style.outlineOffset = '-1px'
        } else {
            this.$dom.style.outline = 'none'
            this.$dom.style.outlineOffset = ''
        }
    }

    _applyBorder() {
        this.$dom.style.borderBottom = '1px solid #3c3c3c'
    }
}

module.exports.TreeViewDelegate = TreeViewDelegate
