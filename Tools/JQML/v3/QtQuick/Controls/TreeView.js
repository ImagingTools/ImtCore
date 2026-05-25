const TableView = require("./TableView")
const Var = require("../../QtQml/Var")
const Real = require("../../QtQml/Real")
const Int = require("../../QtQml/Int")
const Bool = require("../../QtQml/Bool")
const Signal = require("../../QtQml/Signal")
const ItemSelectionModel = require("../../QtQml/Models/ItemSelectionModel")

/**
 * TreeView — extends TableView with hierarchical expand/collapse support.
 * The model items may have a `children` array to define sub-items.
 */
class TreeView extends TableView {
    static meta = Object.assign({}, TableView.meta, {
        rootIndex:    { type: Var,  value: undefined },
        editTriggers: { type: Real, value: TableView.EditKeyPressed },

        rootIndexChanged: { type: Signal, args: [] },
        collapsed:        { type: Signal, args: ['row', 'recursively'] },
        expanded:         { type: Signal, args: ['row', 'depth'] },
    })

    static create(parent, ...args) {
        let obj = super.create(parent, ...args)
        obj.__treeData    = []   // flat array of visible nodes
        obj.__expandedSet = new Set()
        return obj
    }

    SLOT_modelChanged(o, n) {
        this.__treeData = []
        this.__expandedSet.clear()
        super.SLOT_modelChanged(o, n)
    }

    __rebuildTable() {
        if (!this.__tvReady) return
        // Clear all cells before rebuilding because tree structure changes row→node mapping
        for (let key in this.__cells) { this.__toCache(this.__cells[key]); delete this.__cells[key] }
        this.__cache = []

        this.__treeData = []
        let model = this.model
        if (model) this.__flattenTree(model, null, 0, '')

        this.__computeLayout()
        this.__populateViewport()
    }

    __flattenTree(model, parentKey, depth, pathPrefix) {
        let items = this.__getTreeItems(model)
        if (!items) return
        for (let i = 0; i < items.length; i++) {
            let item      = items[i]
            let nodePath  = pathPrefix + '/' + i
            let children  = this.__getChildren(item)
            let hasChildren = !!(children && children.length > 0)
            let expanded  = this.__expandedSet.has(nodePath)

            let node = { item, depth, hasChildren, expanded, nodePath, parentPath: pathPrefix, modelIndex: i, flatIndex: this.__treeData.length }
            this.__treeData.push(node)

            if (hasChildren && expanded) this.__flattenTree(children, nodePath, depth + 1, nodePath)
        }
    }

    __getTreeItems(model) {
        if (!model) return null
        if (Array.isArray(model)) return model
        if (model.__data && Array.isArray(model.__data)) return model.__data
        return null
    }

    __getChildren(item) {
        if (!item) return null
        if (item.children && Array.isArray(item.children)) return item.children
        if (item.$children && Array.isArray(item.$children)) return item.$children
        return null
    }

    // ─── override row/col count ─────────────────────────────────────────────

    _getModelRowCount()    { return this.__treeData.length }

    _getModelColumnCount() {
        let model = this.model
        if (!model) return 1
        let items = this.__getTreeItems(model)
        if (items && items.length > 0) {
            let first = items[0]
            if (typeof first === 'object') {
                let keys = Object.keys(first).filter(k => k !== 'children' && k !== '$children' && k !== 'index' && !k.startsWith('$'))
                return Math.max(keys.length, 1)
            }
        }
        return 1
    }

    _getColumnKeys() {
        let model = this.model
        if (!model) return ['display']
        let items = this.__getTreeItems(model)
        if (items && items.length > 0) {
            let first = items[0]
            if (typeof first === 'object') {
                return Object.keys(first).filter(k => k !== 'children' && k !== '$children' && k !== 'index' && !k.startsWith('$'))
            }
        }
        return ['display']
    }

    _getCellData(row, col) {
        let node = this.__treeData[row]
        if (!node) return { row, column: col, display: '', isTreeNode: false, expanded: false, hasChildren: false, depth: 0, model: null, index: row }

        let keys = this._getColumnKeys()
        let key  = keys[col] !== undefined ? keys[col] : col
        let item = node.item
        let display = typeof item === 'object' && item !== null ? (item[key] !== undefined ? item[key] : '') : item

        return { row, column: col, display, model: item, index: row, isTreeNode: col === 0, expanded: node.expanded, hasChildren: node.hasChildren, depth: node.depth }
    }

    __createCell(row, col) {
        let key = row + ',' + col
        if (this.__cells[key]) return this.__cells[key]

        let cellData = this._getCellData(row, col)
        let delegate = this.delegate
        if (!delegate) return null

        let exModel = Object.assign({}, cellData.model && typeof cellData.model === 'object' ? cellData.model : {})
        exModel.row = row; exModel.column = col; exModel.display = cellData.display
        exModel.index = cellData.index !== undefined ? cellData.index : row
        exModel.isTreeNode  = cellData.isTreeNode
        exModel.expanded    = cellData.expanded
        exModel.hasChildren = cellData.hasChildren
        exModel.depth       = cellData.depth
        exModel.treeView    = this.__proxy || this

        let obj = delegate.create(this.contentItem, { model: cellData.model || exModel, modelData: cellData.display })
        obj.__DOM.style.position = 'absolute'
        this.__cells[key] = obj
        obj.__tableRow = row
        obj.__tableCol = col

        // Forward tree properties
        this.__applyTreeProps(obj, cellData)
        obj.__complete && obj.__complete()
        return obj
    }

    __applyTreeProps(obj, cellData) {
        try {
            if ('isTreeNode'  in obj) obj.isTreeNode  = cellData.isTreeNode
            if ('expanded'    in obj) obj.expanded    = cellData.expanded
            if ('hasChildren' in obj) obj.hasChildren = cellData.hasChildren
            if ('depth'       in obj) obj.depth       = cellData.depth
            if ('treeView'    in obj) obj.treeView    = this.__proxy || this
        } catch(e) { /* ignore */ }
    }

    // ─── keyboard override ──────────────────────────────────────────────────

    __installKeyHandler() {
        super.__installKeyHandler()
        // Intercept ArrowLeft/Right for expand/collapse
        let dom = this.__DOM
        dom.addEventListener('keydown', (e) => {
            let curRow = this.currentRow
            if (e.key === 'ArrowRight') {
                let node = this.__treeData[curRow]
                if (node && node.hasChildren && !node.expanded) { this.expand(curRow); e.stopImmediatePropagation(); e.preventDefault() }
            } else if (e.key === 'ArrowLeft') {
                let node = this.__treeData[curRow]
                if (node && node.expanded) { this.collapse(curRow); e.stopImmediatePropagation(); e.preventDefault() }
            } else if (e.key === ' ') {
                this.toggleExpanded(curRow); e.stopImmediatePropagation(); e.preventDefault()
            } else if (e.key === '*') {
                this.expandRecursively(curRow); e.stopImmediatePropagation(); e.preventDefault()
            }
        }, true)  // capture phase so we intercept before parent handler
    }

    // ─── tree API ────────────────────────────────────────────────────────────

    expand(row) {
        if (row < 0 || row >= this.__treeData.length) return
        let node = this.__treeData[row]
        if (!node || !node.hasChildren || node.expanded) return
        this.__expandedSet.add(node.nodePath)
        node.expanded = true
        this.expanded(row, 1)
        this.__rebuildTable()
    }

    collapse(row) {
        if (row < 0 || row >= this.__treeData.length) return
        let node = this.__treeData[row]
        if (!node || !node.expanded) return
        this.__expandedSet.delete(node.nodePath)
        node.expanded = false
        this.collapsed(row, false)
        this.__rebuildTable()
    }

    toggleExpanded(row) {
        if (this.isExpanded(row)) this.collapse(row)
        else this.expand(row)
    }

    isExpanded(row) {
        if (row < 0 || row >= this.__treeData.length) return false
        return this.__treeData[row].expanded
    }

    depth(row) {
        if (row < 0 || row >= this.__treeData.length) return -1
        return this.__treeData[row].depth
    }

    expandRecursively(row, maxDepth) {
        if (maxDepth === undefined) maxDepth = -1
        if (row === undefined || row === null) row = -1

        if (row === -1) {
            let model = this.model
            let items = this.__getTreeItems(model)
            if (items) this.__expandRecursiveItems(items, '', 0, maxDepth)
        } else {
            if (row < 0 || row >= this.__treeData.length) return
            let node = this.__treeData[row]
            if (node) {
                this.__expandedSet.add(node.nodePath)
                let children = this.__getChildren(node.item)
                if (children) this.__expandRecursiveItems(children, node.nodePath, 1, maxDepth)
            }
        }
        this.expanded(row, maxDepth)
        this.__rebuildTable()
    }

    __expandRecursiveItems(items, parentPath, currentDepth, maxDepth) {
        for (let i = 0; i < items.length; i++) {
            let nodePath = parentPath + '/' + i
            let children = this.__getChildren(items[i])
            if (children && children.length > 0) {
                this.__expandedSet.add(nodePath)
                if (maxDepth === -1 || currentDepth < maxDepth) {
                    this.__expandRecursiveItems(children, nodePath, currentDepth+1, maxDepth)
                }
            }
        }
    }

    collapseRecursively(row) {
        if (row === undefined || row === null) row = -1
        if (row === -1) {
            this.__expandedSet.clear()
        } else {
            if (row < 0 || row >= this.__treeData.length) return
            let node = this.__treeData[row]
            if (node) {
                let prefix = node.nodePath
                for (let path of [...this.__expandedSet]) {
                    if (path === prefix || path.startsWith(prefix + '/')) this.__expandedSet.delete(path)
                }
            }
        }
        this.collapsed(row, true)
        this.__rebuildTable()
    }
}

module.exports = TreeView
