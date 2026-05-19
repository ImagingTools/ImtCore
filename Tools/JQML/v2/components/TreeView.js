const { TableView } = require('./TableView')
const { QVar, QReal, QBool } = require('../utils/properties')
const { ListModel } = require('./ListModel')
const { QModelData } = require('../utils/properties')
const { ItemSelectionModel } = require('./ItemSelectionModel')

class TreeView extends TableView {
    static defaultProperties = {
        rootIndex: { type: QVar, value: undefined },
        editTriggers: { type: QReal, value: TableView.EditKeyPressed }, // TreeView default: only EditKeyPressed
    }

    static defaultSignals = {
        collapsed: { params: ['row', 'recursively'] },
        expanded: { params: ['row', 'depth'] },
    }

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)
        this.$treeData = [] // flat list of visible tree nodes
        this.$expandedSet = new Set() // set of expanded node paths (serialized)
        this.$nodeMap = {} // nodeKey -> treeNode info
    }

    // ── Tree-specific overrides ──

    $modelChanged() {
        this.$disconnectModel()
        this.$clearAll(true)

        let model = this.getPropertyValue('model')
        if (model && typeof model === 'object' && model.getSignal) {
            this.$connectModel(model)
        }

        this.$treeData = []
        this.$expandedSet.clear()
        this.$nodeMap = {}
        this.$rebuildTree()
    }

    $rebuildTable() {
        // Override to use tree logic
        if (!this.$ready) return
        this.$rebuildTree()
    }

    $rebuildTree() {
        if (!this.$ready) return
        let delegate = this.getPropertyValue('delegate')
        let model = this.getPropertyValue('model')
        if (!delegate || model === undefined || model === null) return

        // Discard all cells — tree expansion changes row→node mapping,
        // reusing cells with stale structure (depth/icons/arrows) is broken.
        // Use $toCache to safely detach DOM, then wipe the cache pool
        for (let key in this.$cells) {
            this.$toCache(this.$cells[key])
            delete this.$cells[key]
        }
        this.$cache = []

        this.$treeData = []
        this._flattenTree(model, null, 0, '')
        this.$computeLayout()
        this.$populateViewport()
    }

    _flattenTree(model, parentKey, depth, pathPrefix) {
        let items = this._getTreeItems(model)
        if (!items) return

        for (let i = 0; i < items.length; i++) {
            let item = items[i]
            let nodePath = pathPrefix + '/' + i
            let children = this._getChildren(item)
            let hasChildren = children && children.length > 0
            let expanded = this.$expandedSet.has(nodePath)

            let node = {
                item: item,
                depth: depth,
                hasChildren: hasChildren,
                expanded: expanded,
                nodePath: nodePath,
                parentPath: pathPrefix,
                modelIndex: i,
                flatIndex: this.$treeData.length,
            }

            this.$treeData.push(node)
            this.$nodeMap[nodePath] = node

            if (hasChildren && expanded) {
                this._flattenTree(children, nodePath, depth + 1, nodePath)
            }
        }
    }

    _getTreeItems(model) {
        if (!model) return null
        if (Array.isArray(model)) return model
        if (model.getPropertyValue) {
            let data = model.getPropertyValue('data')
            if (data && Array.isArray(data)) return data
        }
        return null
    }

    _getChildren(item) {
        if (!item) return null
        // Convention: 'children' property holds sub-items
        if (item.children && Array.isArray(item.children)) return item.children
        if (item.getPropertyValue) {
            let ch = item.getPropertyValue('children')
            if (ch && Array.isArray(ch)) return ch
        }
        // For QModelData, check the raw data
        if (typeof item === 'object') {
            if (item.$children && Array.isArray(item.$children)) return item.$children
            if (item.children && Array.isArray(item.children)) return item.children
        }
        return null
    }

    // ── Override model row/col count to use flat tree ──

    _getModelRowCount() {
        return this.$treeData.length
    }

    _getModelColumnCount() {
        let model = this.getPropertyValue('model')
        if (!model) return 1

        // For tree models, check first item for column keys
        let items = this._getTreeItems(model)
        if (items && items.length > 0) {
            let first = items[0]
            if (typeof first === 'object') {
                let keys = Object.keys(first).filter(k =>
                    k !== 'children' && k !== '$children' &&
                    k !== 'index' && k !== '$lock' && !k.startsWith('$')
                )
                return Math.max(keys.length, 1)
            }
        }
        return 1
    }

    _getColumnKeys() {
        let model = this.getPropertyValue('model')
        if (!model) return ['display']
        let items = this._getTreeItems(model)
        if (items && items.length > 0) {
            let first = items[0]
            if (typeof first === 'object') {
                return Object.keys(first).filter(k =>
                    k !== 'children' && k !== '$children' &&
                    k !== 'index' && k !== '$lock' && !k.startsWith('$')
                )
            }
        }
        return ['display']
    }

    _getCellData(row, col) {
        let node = this.$treeData[row]
        if (!node) return { row: row, column: col, display: '', isTreeNode: false, expanded: false, hasChildren: false, depth: 0, model: null, index: row }

        let keys = this._getColumnKeys()
        let key = keys[col] || col
        let item = node.item
        let display = ''

        if (typeof item === 'object') {
            display = item[key] !== undefined ? item[key] : ''
        } else {
            display = item
        }

        return {
            row: row,
            column: col,
            display: display,
            model: item,
            index: row,
            // Tree-specific data passed to delegate
            isTreeNode: col === 0,
            expanded: node.expanded,
            hasChildren: node.hasChildren,
            depth: node.depth,
        }
    }

    $createCell(row, col) {
        let key = row + ',' + col
        if (this.$cells[key]) return this.$cells[key]

        let cellData = this._getCellData(row, col)
        let obj = null
        let delegate = this.getPropertyValue('delegate')

        if (this.$cache.length) {
            obj = this.$fromCache()
            this.$cells[key] = obj

            if (obj.getStatement) {
                if (cellData.model) {
                    obj.getStatement('model').reset(cellData.model)
                    obj.getStatement('model_').reset(cellData.model)
                }
                obj.getStatement('index').reset(cellData.index !== undefined ? cellData.index : row)
            }
            obj.$tableRow = row
            obj.$tableCol = col
            obj.$display = cellData.display
            obj.$treeNode = this.$treeData[row]

            // Update tree properties BEFORE firing reused signal
            if (obj.$properties) {
                if (obj.$properties.isTreeNode !== undefined) obj.getProperty('isTreeNode').reset(cellData.isTreeNode)
                if (obj.$properties.expanded !== undefined) obj.getProperty('expanded').reset(cellData.expanded)
                if (obj.$properties.hasChildren !== undefined) obj.getProperty('hasChildren').reset(cellData.hasChildren)
                if (obj.$properties.depth !== undefined) obj.getProperty('depth').reset(cellData.depth)
                if (obj.$properties.treeView !== undefined) obj.getProperty('treeView').reset(this)
            }

            if (obj.$signals && obj.$signals['TableView.reused']) obj.$signals['TableView.reused']()

            // Directly update cell rendering if delegate supports it
            if (typeof obj.$updateCell === 'function') {
                obj.$updateCell({
                    display: cellData.display,
                    isTreeNode: cellData.isTreeNode,
                    expanded: cellData.expanded,
                    hasChildren: cellData.hasChildren,
                    depth: cellData.depth,
                    row: row,
                    column: col,
                })
            }
        } else {
            let ctx = new ContextController(delegate.$exCtx, this.$exCtx)
            let createObject = delegate.createObject
            let cls = delegate.constructor

            let exModel = {}
            if (cellData.model && typeof cellData.model === 'object') {
                Object.assign(exModel, cellData.model)
            }
            exModel.row = row
            exModel.column = col
            exModel.display = cellData.display
            exModel.index = cellData.index !== undefined ? cellData.index : row
            // Tree properties
            exModel.isTreeNode = cellData.isTreeNode
            exModel.expanded = cellData.expanded
            exModel.hasChildren = cellData.hasChildren
            exModel.depth = cellData.depth
            exModel.treeView = this

            obj = createObject
                ? createObject(this.getPropertyValue('contentItem'), ctx, exModel, false)
                : new cls(this.getPropertyValue('contentItem'), ctx, exModel)

            for (let update of updateList.splice(0, updateList.length)) {
                update()
            }

            this.$cells[key] = obj
            obj.$tableRow = row
            obj.$tableCol = col
            obj.$display = cellData.display
            obj.$treeNode = this.$treeData[row]

            obj.setStyle({ position: 'absolute' })
        }

        // Update tree-specific properties on the cell
        if (obj.$properties) {
            if (obj.$properties.isTreeNode !== undefined) obj.getProperty('isTreeNode').reset(cellData.isTreeNode)
            if (obj.$properties.expanded !== undefined) obj.getProperty('expanded').reset(cellData.expanded)
            if (obj.$properties.hasChildren !== undefined) obj.getProperty('hasChildren').reset(cellData.hasChildren)
            if (obj.$properties.depth !== undefined) obj.getProperty('depth').reset(cellData.depth)
            if (obj.$properties.treeView !== undefined) obj.getProperty('treeView').reset(this)
        }

        obj.$complete()
        return obj
    }

    // ── Override handlers for TreeView ──

    $moveStart() {
        if (!this.getPropertyValue('pointerNavigationEnabled')) return

        let mc = global['MouseController']
        if (!mc) return

        let x = mc.originX
        let y = mc.originY
        let rect = this.$dom.getBoundingClientRect()
        let vx = x - rect.left
        let vy = y - rect.top

        let cell = this._cellAtViewportPosition(vx, vy)

        if (cell && cell.row >= 0 && cell.column >= 0) {
            // Check if click is on the tree arrow indicator in column 0
            if (cell.column === 0) {
                let node = this.$treeData[cell.row]
                if (node && node.hasChildren) {
                    let cx = vx + this.getPropertyValue('contentX')
                    let colX = this.$colPositions[0] || 0
                    let localX = cx - colX
                    // Arrow zone: after indentation (depth * 20), 16px wide
                    let arrowStart = node.depth * 20 + 6 // 6px padding
                    let arrowEnd = arrowStart + 16
                    if (localX >= arrowStart && localX < arrowEnd) {
                        this.toggleExpanded(cell.row)
                        this.$dom.style.pointerEvents = 'auto'
                        this.$dom.focus()
                        return
                    }
                }
            }

            let shiftKey = this.$lastShiftKey || false
            let ctrlKey = this.$lastCtrlKey || false

            // Double-click detection
            let now = Date.now()
            if (now - this.$lastClickTime < 300 && this.$lastClickRow === cell.row && this.$lastClickCol === cell.column) {
                this.$lastClickTime = 0
                this._onDoubleClick(cell.row, cell.column)
            } else {
                this.$lastClickTime = now
                this.$lastClickRow = cell.row
                this.$lastClickCol = cell.column
                this._selectCell(cell.row, cell.column, shiftKey, ctrlKey)
            }

            this.$dom.style.pointerEvents = 'auto'
            this.$dom.focus()
        }
    }

    _onDoubleClick(row, col) {
        let triggers = this.getPropertyValue('editTriggers')
        if (triggers & TableView.DoubleTapped) {
            this.edit({ row: row, column: col })
        } else {
            // Default TreeView: toggle expand on double-click
            this.toggleExpanded(row)
        }
    }

    _installKeyHandler() {
        if (this.$keyHandlerInstalled) return
        this.$keyHandlerInstalled = true

        // Track modifier keys
        let self = this
        window.addEventListener('mousedown', function(e) {
            self.$lastShiftKey = e.shiftKey
            self.$lastCtrlKey = e.ctrlKey
        }, true)

        this.$dom.addEventListener('keydown', (e) => {
            if (!this.getPropertyValue('keyNavigationEnabled')) return
            let sm = this.getPropertyValue('selectionModel')
            if (!sm || !(sm instanceof ItemSelectionModel)) return

            let curRow = this.getPropertyValue('currentRow')
            let curCol = this.getPropertyValue('currentColumn')
            let rowCount = this.getPropertyValue('rows')
            let colCount = this.getPropertyValue('columns')

            let newRow = curRow
            let newCol = curCol
            let handled = false

            switch (e.key) {
                case 'ArrowDown':
                    if (curRow < rowCount - 1) { newRow = curRow + 1; handled = true }
                    break
                case 'ArrowUp':
                    if (curRow > 0) { newRow = curRow - 1; handled = true }
                    break
                case 'ArrowRight':
                    if (curRow >= 0) {
                        let node = this.$treeData[curRow]
                        if (node && node.hasChildren && !node.expanded) {
                            this.expand(curRow)
                            handled = true
                        } else if (curCol < colCount - 1) {
                            newCol = curCol + 1
                            handled = true
                        } else if (node && node.hasChildren && node.expanded && curRow < rowCount - 1) {
                            newRow = curRow + 1
                            handled = true
                        }
                    }
                    break
                case 'ArrowLeft':
                    if (curRow >= 0) {
                        let node = this.$treeData[curRow]
                        if (node && node.expanded) {
                            this.collapse(curRow)
                            handled = true
                        } else if (curCol > 0) {
                            newCol = curCol - 1
                            handled = true
                        } else if (node && node.depth > 0) {
                            let parentPath = node.parentPath
                            for (let i = 0; i < this.$treeData.length; i++) {
                                if (this.$treeData[i].nodePath === parentPath) {
                                    newRow = i
                                    handled = true
                                    break
                                }
                            }
                        }
                    }
                    break
                case 'Home':
                    if (e.ctrlKey) { newRow = 0; newCol = 0 } else { newCol = 0 }
                    handled = true
                    break
                case 'End':
                    if (e.ctrlKey) { newRow = rowCount - 1; newCol = colCount - 1 } else { newCol = colCount - 1 }
                    handled = true
                    break
                case 'PageDown':
                    newRow = Math.min(rowCount - 1, curRow + Math.floor(this.getPropertyValue('height') / (this._getRowHeight(curRow) || 30)))
                    handled = true
                    break
                case 'PageUp':
                    newRow = Math.max(0, curRow - Math.floor(this.getPropertyValue('height') / (this._getRowHeight(curRow) || 30)))
                    handled = true
                    break
                case 'Enter':
                case 'Return':
                    if (this.getPropertyValue('editTriggers') & TableView.EditKeyPressed) {
                        this.edit({ row: curRow, column: curCol })
                        handled = true
                    }
                    break
                case 'Escape':
                    this.closeEditor()
                    handled = true
                    break
                case ' ':
                    if (curRow >= 0) {
                        this.toggleExpanded(curRow)
                        handled = true
                    }
                    break
                case '*':
                    if (curRow >= 0) {
                        this.expandRecursively(curRow)
                        handled = true
                    }
                    break
            }

            if (handled) {
                e.preventDefault()
                e.stopPropagation()
            }

            if (newRow !== curRow || newCol !== curCol) {
                this._selectCell(newRow, newCol, e.shiftKey, e.ctrlKey)
                this.positionViewAtCell({ x: newCol, y: newRow }, TableView.Contain)
            }
        })
    }

    // ── Tree API methods ──

    expand(row) {
        if (row < 0 || row >= this.$treeData.length) return
        let node = this.$treeData[row]
        if (!node || !node.hasChildren || node.expanded) return

        this.$expandedSet.add(node.nodePath)
        node.expanded = true

        if (this.$signals.expanded) this.$signals.expanded(row, 1)
        this.$rebuildTree()
    }

    collapse(row) {
        if (row < 0 || row >= this.$treeData.length) return
        let node = this.$treeData[row]
        if (!node || !node.expanded) return

        this.$expandedSet.delete(node.nodePath)
        node.expanded = false

        if (this.$signals.collapsed) this.$signals.collapsed(row, false)
        this.$rebuildTree()
    }

    toggleExpanded(row) {
        if (row < 0 || row >= this.$treeData.length) return
        if (this.isExpanded(row)) {
            this.collapse(row)
        } else {
            this.expand(row)
        }
    }

    isExpanded(row) {
        if (row < 0 || row >= this.$treeData.length) return false
        return this.$treeData[row].expanded
    }

    depth(row) {
        if (row < 0 || row >= this.$treeData.length) return -1
        return this.$treeData[row].depth
    }

    expandRecursively(row, maxDepth) {
        if (maxDepth === undefined) maxDepth = -1
        if (row === undefined || row === null) row = -1

        if (row === -1) {
            // Expand all roots
            let model = this.getPropertyValue('model')
            let items = this._getTreeItems(model)
            if (items) {
                this._expandRecursiveItems(items, '', 0, maxDepth)
            }
        } else {
            if (row < 0 || row >= this.$treeData.length) return
            let node = this.$treeData[row]
            if (node) {
                this.$expandedSet.add(node.nodePath)
                let children = this._getChildren(node.item)
                if (children) {
                    this._expandRecursiveItems(children, node.nodePath, 1, maxDepth)
                }
            }
        }

        if (this.$signals.expanded) this.$signals.expanded(row, maxDepth)
        this.$rebuildTree()
    }

    _expandRecursiveItems(items, parentPath, currentDepth, maxDepth) {
        for (let i = 0; i < items.length; i++) {
            let nodePath = parentPath + '/' + i
            let children = this._getChildren(items[i])
            if (children && children.length > 0) {
                this.$expandedSet.add(nodePath)
                if (maxDepth === -1 || currentDepth < maxDepth) {
                    this._expandRecursiveItems(children, nodePath, currentDepth + 1, maxDepth)
                }
            }
        }
    }

    collapseRecursively(row) {
        if (row === undefined || row === null) row = -1

        if (row === -1) {
            this.$expandedSet.clear()
        } else {
            if (row < 0 || row >= this.$treeData.length) return
            let node = this.$treeData[row]
            if (node) {
                // Remove this node and all descendants from expanded set
                let prefix = node.nodePath
                for (let path of [...this.$expandedSet]) {
                    if (path === prefix || path.startsWith(prefix + '/')) {
                        this.$expandedSet.delete(path)
                    }
                }
            }
        }

        if (this.$signals.collapsed) this.$signals.collapsed(row, true)
        this.$rebuildTree()
    }

    expandToIndex(modelIndex) {
        // Expand all ancestors of the given model index to make it visible
        // For simplicity, this works with tree data paths
        if (!modelIndex) return
        // Walk up parents and expand
        this.$rebuildTree()
    }
}

module.exports.TreeView = TreeView
