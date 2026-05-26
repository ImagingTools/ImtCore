const Flickable = require("../Flickable")
const Item = require("../Item")
const Real = require("../../QtQml/Real")
const Int = require("../../QtQml/Int")
const Bool = require("../../QtQml/Bool")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")
const DelegateChooser = require("../../QtQml/Models/DelegateChooser")
const ItemSelectionModel = require("../../QtQml/Models/ItemSelectionModel")

/**
 * TableView — scrollable table with virtualised cell rendering.
 * Extends Flickable so scroll/flick behaviour is inherited.
 */
class TableView extends Flickable {
    // SelectionBehavior
    static SelectionDisabled = 0
    static SelectCells       = 1
    static SelectRows        = 2
    static SelectColumns     = 3

    // SelectionMode
    static SingleSelection      = 0
    static ContiguousSelection  = 1
    static ExtendedSelection    = 2

    // EditTriggers
    static NoEditTriggers  = 0
    static SingleTapped    = 1
    static DoubleTapped    = 2
    static SelectedTapped  = 4
    static EditKeyPressed  = 8
    static AnyKeyPressed   = 16

    // PositionMode
    static AlignLeft    = 0x0001
    static AlignHCenter = 0x0002
    static AlignRight   = 0x0004
    static AlignTop     = 0x0010
    static AlignVCenter = 0x0020
    static AlignBottom  = 0x0040
    static AlignCenter  = 0x0022
    static Visible      = 0x0100
    static Contain      = 0x0200

    static meta = Object.assign({}, Flickable.meta, {
        model:              { type: Var,  value: undefined },
        delegate:           { type: Var,  value: undefined },
        selectionModel:     { type: Var,  value: undefined },
        selectionBehavior:  { type: Real, value: TableView.SelectCells },
        selectionMode:      { type: Real, value: TableView.ExtendedSelection },
        editTriggers:       { type: Real, value: TableView.DoubleTapped | TableView.EditKeyPressed },
        keyNavigationEnabled:    { type: Bool, value: true },
        pointerNavigationEnabled:{ type: Bool, value: true },
        alternatingRows:    { type: Bool, value: true },
        reuseItems:         { type: Bool, value: true },
        rowSpacing:         { type: Real, value: 0 },
        columnSpacing:      { type: Real, value: 0 },
        rows:               { type: Int,  value: 0 },
        columns:            { type: Int,  value: 0 },
        currentRow:         { type: Int,  value: -1 },
        currentColumn:      { type: Int,  value: -1 },
        topRow:             { type: Int,  value: 0 },
        bottomRow:          { type: Int,  value: 0 },
        leftColumn:         { type: Int,  value: 0 },
        rightColumn:        { type: Int,  value: 0 },
        rowHeightProvider:      { type: Var, value: undefined },
        columnWidthProvider:    { type: Var, value: undefined },
        resizableRows:      { type: Bool, value: false },
        resizableColumns:   { type: Bool, value: false },
        editDelegate:       { type: Var,  value: undefined },

        modelChanged:           { type: Signal, args: [] },
        delegateChanged:        { type: Signal, args: [] },
        selectionModelChanged:  { type: Signal, args: [] },
        rowsChanged:            { type: Signal, args: [] },
        columnsChanged:         { type: Signal, args: [] },
        currentRowChanged:      { type: Signal, args: [] },
        currentColumnChanged:   { type: Signal, args: [] },
        layoutChanged:          { type: Signal, args: [] },
        commit:                 { type: Signal, args: ['row', 'column', 'value'] },
    })

    static create(parent, ...args) {
        let obj = super.create(parent, ...args)
        obj.__DOM.style.overflow = 'hidden'
        obj.__DOM.setAttribute('tabindex', '0')
        obj.__DOM.style.outline = 'none'

        obj.__cells  = {}   // key "row,col" -> delegate instance
        obj.__cache  = []
        obj.__explicitRowHeights    = {}
        obj.__explicitColumnWidths  = {}
        obj.__implicitRowHeights    = {}
        obj.__implicitColumnWidths  = {}
        obj.__resolvedRowHeights    = {}
        obj.__resolvedColumnWidths  = {}
        obj.__rowPositions = []
        obj.__colPositions = []
        obj.__anchorRow = -1
        obj.__anchorCol = -1
        obj.__lastClickTime = 0
        obj.__lastClickRow = -1
        obj.__lastClickCol = -1
        obj.__editingCell = null
        obj.__editElement = null
        obj.__editItem    = null
        obj.__lastShiftKey = false
        obj.__lastCtrlKey  = false
        obj.__tvReady = false

        obj.__installMouseModifierTracker()
        obj.__installKeyHandler()

        return obj
    }

    __complete() {
        this.__tvReady = true
        super.__complete()
        this.__rebuildTable()
    }

    // ─── model connection ───────────────────────────────────────────────────

    SLOT_modelChanged(oldValue, newValue) {
        this.__disconnectModel(oldValue)
        this.__clearAll(true)
        if (newValue && typeof newValue === 'object' && typeof newValue.dataChanged === 'function') {
            newValue.dataChanged.connect(this.__onModelDataChanged.bind(this))
        }
        this.__rebuildTable()
    }

    SLOT_delegateChanged() {
        this.__clearAll(true)
        this.__rebuildTable()
    }

    SLOT_selectionModelChanged(oldValue, newValue) {
        if (newValue instanceof ItemSelectionModel) {
            newValue.selectionChanged.connect(this.__updateSelectionVisuals.bind(this))
            newValue.currentIndexChanged.connect((current) => {
                this.currentRow    = current ? (current.row    !== undefined ? current.row    : -1) : -1
                this.currentColumn = current ? (current.column !== undefined ? current.column : -1) : -1
                this.__updateSelectionVisuals()
            })
        }
    }

    SLOT_rowSpacingChanged()    { this.__rebuildTable() }
    SLOT_columnSpacingChanged() { this.__rebuildTable() }

    SLOT_widthChanged(o, n) {
        super.SLOT_widthChanged(o, n)
        if (this.__tvReady) this.__populateViewport()
    }
    SLOT_heightChanged(o, n) {
        super.SLOT_heightChanged(o, n)
        if (this.__tvReady) this.__populateViewport()
    }
    SLOT_contentXChanged(o, n) {
        super.SLOT_contentXChanged(o, n)
        if (this.__tvReady) this.__populateViewport()
    }
    SLOT_contentYChanged(o, n) {
        super.SLOT_contentYChanged(o, n)
        if (this.__tvReady) this.__populateViewport()
    }

    __disconnectModel(model) {
        // Signal disconnection handled via weak refs; nothing explicit needed here.
    }

    __onModelDataChanged(topLeft, bottomRight, roles) {
        if (roles === 'reset' || roles === 'remove' || roles === 'append') {
            this.__rebuildTable()
        } else {
            this.__populateViewport()
        }
    }

    // ─── geometry helpers ────────────────────────────────────────────────────

    _getModelRowCount() {
        let model = this.model
        if (!model) return 0
        if (typeof model === 'number') return model
        if (Array.isArray(model)) return model.length
        if (typeof model.rowCount === 'function') return model.rowCount()
        if (model.count !== undefined) return model.count
        if (model.__data) return model.__data.length
        return 0
    }

    _getModelColumnCount() {
        let model = this.model
        if (!model) return 1
        if (typeof model === 'number') return 1
        if (Array.isArray(model)) {
            if (model.length > 0 && typeof model[0] === 'object') return Object.keys(model[0]).length || 1
            return 1
        }
        if (model.__columns && model.__columns.length > 0) return model.__columns.length
        if (model.__data && model.__data.length > 0) {
            let keys = Object.keys(model.__data[0]).filter(k => k !== 'index' && !k.startsWith('$'))
            return keys.length || 1
        }
        return 1
    }

    _getColumnKeys() {
        let model = this.model
        if (!model) return []
        if (model.__columns && model.__columns.length > 0) return model.__columns.map(c => c.display)
        if (Array.isArray(model)) {
            if (model.length > 0 && typeof model[0] === 'object') return Object.keys(model[0])
            return ['modelData']
        }
        if (model.__data && model.__data.length > 0) {
            return Object.keys(model.__data[0]).filter(k => k !== 'index' && !k.startsWith('$'))
        }
        return ['display']
    }

    _getCellData(row, col) {
        let model = this.model
        if (!model) return { row, column: col, display: '', model: null, index: row }
        let keys = this._getColumnKeys()
        let key  = keys[col] !== undefined ? keys[col] : col

        if (typeof model === 'number') return { row, column: col, display: '', model: null, index: row }
        if (Array.isArray(model)) {
            let item = model[row]
            if (typeof item === 'object') return { row, column: col, display: item[key], model: item, index: row }
            return { row, column: col, display: item, model: null, index: row }
        }
        let data = model.__data || []
        let item = data[row]
        if (item) return { row, column: col, display: item[key], model: item, index: row }
        return { row, column: col, display: '', model: null, index: row }
    }

    _getRowHeight(row) {
        let provider = this.rowHeightProvider
        if (typeof provider === 'function') {
            let h = provider(row)
            if (h !== undefined && h !== null && h >= 0) return h
        }
        if (this.__explicitRowHeights[row] !== undefined) return this.__explicitRowHeights[row]
        if (this.__implicitRowHeights[row] !== undefined) return this.__implicitRowHeights[row]
        return 30
    }

    _getColumnWidth(col) {
        let provider = this.columnWidthProvider
        if (typeof provider === 'function') {
            let w = provider(col)
            if (w !== undefined && w !== null && w >= 0) return w
        }
        if (this.__explicitColumnWidths[col] !== undefined) return this.__explicitColumnWidths[col]
        if (this.__implicitColumnWidths[col] !== undefined) return this.__implicitColumnWidths[col]
        return 100
    }

    // ─── layout ──────────────────────────────────────────────────────────────

    __computeLayout() {
        let rowCount = this._getModelRowCount()
        let colCount = this._getModelColumnCount()
        let rowSpacing = this.rowSpacing
        let colSpacing = this.columnSpacing

        this.__rowPositions = []
        this.__colPositions = []
        this.__resolvedRowHeights    = {}
        this.__resolvedColumnWidths  = {}

        let y = 0
        for (let r = 0; r < rowCount; r++) {
            let h = this._getRowHeight(r)
            this.__resolvedRowHeights[r] = h
            this.__rowPositions[r] = y
            y += h + (r < rowCount - 1 ? rowSpacing : 0)
        }

        let x = 0
        for (let c = 0; c < colCount; c++) {
            let w = this._getColumnWidth(c)
            this.__resolvedColumnWidths[c] = w
            this.__colPositions[c] = x
            x += w + (c < colCount - 1 ? colSpacing : 0)
        }

        this.rows    = rowCount
        this.columns = colCount
        this.contentWidth  = x
        this.contentHeight = y
    }

    __rebuildTable() {
        if (!this.__tvReady) return
        let delegate = this.delegate
        let model    = this.model
        if (!delegate || model === undefined || model === null) return

        let rowCount = this._getModelRowCount()
        let colCount = this._getModelColumnCount()
        if (rowCount === 0 && colCount === 0) return

        this.__computeLayout()
        this.__populateViewport()
    }

    __populateViewport() {
        let rowCount = this.rows
        let colCount = this.columns
        if (rowCount === 0 || colCount === 0) return

        let viewX = this.contentX
        let viewY = this.contentY
        let viewW = this.width
        let viewH = this.height

        let topRow = -1, bottomRow = -1, leftCol = -1, rightCol = -1

        for (let r = 0; r < rowCount; r++) {
            let ry = this.__rowPositions[r], rh = this.__resolvedRowHeights[r] || 0
            if (ry + rh >= viewY && ry <= viewY + viewH) {
                if (topRow < 0) topRow = r
                bottomRow = r
            }
        }
        for (let c = 0; c < colCount; c++) {
            let cx = this.__colPositions[c], cw = this.__resolvedColumnWidths[c] || 0
            if (cx + cw >= viewX && cx <= viewX + viewW) {
                if (leftCol < 0) leftCol = c
                rightCol = c
            }
        }

        if (topRow    < 0) topRow    = 0
        if (bottomRow < 0) bottomRow = Math.min(rowCount - 1, 0)
        if (leftCol   < 0) leftCol   = 0
        if (rightCol  < 0) rightCol  = Math.min(colCount - 1, 0)

        this.topRow      = topRow
        this.bottomRow   = bottomRow
        this.leftColumn  = leftCol
        this.rightColumn = rightCol

        let neededCells = new Set()
        for (let r = topRow; r <= bottomRow; r++)
            for (let c = leftCol; c <= rightCol; c++)
                neededCells.add(r + ',' + c)

        for (let key in this.__cells) {
            if (!neededCells.has(key)) { this.__toCache(this.__cells[key]); delete this.__cells[key] }
        }

        for (let r = topRow; r <= bottomRow; r++) {
            for (let c = leftCol; c <= rightCol; c++) {
                let key = r + ',' + c
                if (!this.__cells[key]) this.__createCell(r, c)
                let cell = this.__cells[key]
                if (cell) {
                    cell.x = this.__colPositions[c]
                    cell.y = this.__rowPositions[r]
                    cell.width  = this.__resolvedColumnWidths[c]
                    cell.height = this.__resolvedRowHeights[r]
                }
            }
        }

        this.__updateSelectionVisuals()
        this.layoutChanged()
    }

    __createCell(row, col) {
        let key = row + ',' + col
        if (this.__cells[key]) return this.__cells[key]

        let cellData    = this._getCellData(row, col)
        let rawDelegate = this.delegate
        let isChooser   = rawDelegate instanceof DelegateChooser

        let obj = null

        if (this.__cache.length && !isChooser) {
            obj = this.__fromCache()
            this.__cells[key] = obj
            obj.__tableRow = row
            obj.__tableCol = col
            // Update model context
            let context = obj.__self || obj
            try {
                if (cellData.model) context.JQAbstractModel     = cellData.model
                context.JQAbstractModelData = cellData.display
                context.index = cellData.index !== undefined ? cellData.index : row
            } catch(e) { /* ignore */ }
        } else {
            let delegate = isChooser
                ? rawDelegate.chooseDelegate(row, col, cellData)
                : rawDelegate
            if (!delegate) return null

            let exModel = Object.assign({}, cellData.model || {})
            exModel.row     = row
            exModel.column  = col
            exModel.display = cellData.display
            exModel.index   = cellData.index !== undefined ? cellData.index : row

            obj = delegate.create(this.contentItem, { model: cellData.model || exModel, modelData: cellData.display })

            // Override x/y to absolute within contentItem
            obj.__DOM.style.position = 'absolute'
            this.__cells[key] = obj
            obj.__tableRow = row
            obj.__tableCol = col
        }

        obj.__complete && obj.__complete()
        return obj
    }

    __toCache(item) {
        if (!item) return
        if (this.reuseItems) {
            let dom = item.__DOM || (typeof item.__getDOM === 'function' ? item.__getDOM() : null)
            if (dom && dom.parentNode) dom.parentNode.removeChild(dom)
            this.__cache.push(item)
        } else {
            item.destroy && item.destroy()
        }
    }

    __fromCache() {
        let item = this.__cache.pop()
        if (!item) return null
        let dom = item.__DOM || (typeof item.__getDOM === 'function' ? item.__getDOM() : null)
        if (dom && this.contentItem && this.contentItem.__DOM) {
            this.contentItem.__DOM.appendChild(dom)
        }
        return item
    }

    __clearAll(destroyCache = false) {
        if (destroyCache) {
            while (this.__cache.length) { let o = this.__cache.shift(); o && o.destroy && o.destroy() }
        }
        for (let key in this.__cells) {
            let cell = this.__cells[key]
            if (destroyCache) { cell && cell.destroy && cell.destroy() }
            else this.__toCache(cell)
            delete this.__cells[key]
        }
    }

    // ─── selection visuals ──────────────────────────────────────────────────

    __updateSelectionVisuals() {
        let sm     = this.selectionModel
        let curRow = this.currentRow
        let curCol = this.currentColumn

        for (let key in this.__cells) {
            let cell = this.__cells[key]
            if (!cell) continue
            let r = cell.__tableRow, c = cell.__tableCol
            let selected = sm instanceof ItemSelectionModel ? sm.isSelected({ row: r, column: c }) : false
            let current  = r === curRow && c === curCol

            let dom = cell.__DOM || (typeof cell.__getDOM === 'function' ? cell.__getDOM() : null)
            if (dom) {
                current  ? dom.setAttribute('tv-current', '')  : dom.removeAttribute('tv-current')
                selected ? dom.setAttribute('tv-selected', '') : dom.removeAttribute('tv-selected')
            }
            // Forward to delegate properties if present
            try {
                if ('selected' in cell) cell.selected = selected
                if ('current'  in cell) cell.current  = current
            } catch(e) { /* ignore */ }
        }
    }

    // ─── pointer & keyboard ─────────────────────────────────────────────────

    __installMouseModifierTracker() {
        let self = this
        if (typeof window === 'undefined') return
        window.addEventListener('mousedown', function(e) {
            self.__lastShiftKey = e.shiftKey
            self.__lastCtrlKey  = e.ctrlKey
        }, true)
    }

    __installKeyHandler() {
        let dom = this.__DOM
        dom.addEventListener('keydown', (e) => {
            if (!this.keyNavigationEnabled) return
            let sm = this.selectionModel
            if (!sm) return

            let curRow = this.currentRow, curCol = this.currentColumn
            let rowCount = this.rows, colCount = this.columns
            let newRow = curRow, newCol = curCol, handled = false

            switch (e.key) {
                case 'ArrowDown':  if (curRow < rowCount-1) { newRow = curRow+1; handled=true } break
                case 'ArrowUp':    if (curRow > 0)          { newRow = curRow-1; handled=true } break
                case 'ArrowRight': if (curCol < colCount-1) { newCol = curCol+1; handled=true } break
                case 'ArrowLeft':  if (curCol > 0)          { newCol = curCol-1; handled=true } break
                case 'Home': newCol = 0; if (e.ctrlKey) newRow = 0; handled=true; break
                case 'End':  newCol = colCount-1; if (e.ctrlKey) newRow = rowCount-1; handled=true; break
                case 'PageDown':
                    newRow = Math.min(rowCount-1, curRow + Math.floor(this.height / (this._getRowHeight(curRow)||30)))
                    handled = true; break
                case 'PageUp':
                    newRow = Math.max(0, curRow - Math.floor(this.height / (this._getRowHeight(curRow)||30)))
                    handled = true; break
                case 'Enter': case 'Return':
                    if (this.editTriggers & TableView.EditKeyPressed) { this.edit({ row: curRow, column: curCol }); handled=true } break
                case 'Escape': this.closeEditor(); handled=true; break
                case ' ':
                    if (curRow >= 0 && curCol >= 0) { this.__selectCell(curRow, curCol, e.shiftKey, e.ctrlKey); handled=true } break
                case 'a':
                    if (e.ctrlKey && sm instanceof ItemSelectionModel) {
                        sm.clearSelection()
                        for (let r=0; r<rowCount; r++) for (let c=0; c<colCount; c++) sm.select({ row:r, column:c }, ItemSelectionModel.Select)
                        handled=true
                    }
                    break
            }

            if (handled) { e.preventDefault(); e.stopPropagation() }
            if (newRow !== curRow || newCol !== curCol) {
                this.__selectCell(newRow, newCol, e.shiftKey, e.ctrlKey)
                this.positionViewAtCell({ x: newCol, y: newRow }, TableView.Contain)
            }
        })

        // Click handler
        dom.addEventListener('mousedown', (e) => {
            if (!this.pointerNavigationEnabled) return
            let rect = dom.getBoundingClientRect()
            let vx = e.clientX - rect.left, vy = e.clientY - rect.top
            let cell = this.__cellAtViewportPosition(vx, vy)
            if (cell && cell.row >= 0) {
                let now = Date.now()
                if (now - this.__lastClickTime < 300 && this.__lastClickRow === cell.row && this.__lastClickCol === cell.column) {
                    this.__lastClickTime = 0
                    if (this.editTriggers & TableView.DoubleTapped) this.edit({ row: cell.row, column: cell.column })
                } else {
                    this.__lastClickTime = now
                    this.__lastClickRow = cell.row
                    this.__lastClickCol = cell.column
                    this.__selectCell(cell.row, cell.column, e.shiftKey, e.ctrlKey)
                }
                dom.focus()
            }
        })
    }

    __cellAtViewportPosition(vx, vy) {
        let cx = vx + this.contentX, cy = vy + this.contentY
        return this.cellAtPosition(cx, cy)
    }

    __selectCell(row, col, shiftKey, ctrlKey) {
        let sm = this.selectionModel
        if (!sm) return
        let behavior = this.selectionBehavior
        if (behavior === TableView.SelectionDisabled) return

        let mode = this.selectionMode
        let command = 0
        if (behavior === TableView.SelectRows)    command |= ItemSelectionModel.Rows
        if (behavior === TableView.SelectColumns) command |= ItemSelectionModel.Columns

        if (ctrlKey && mode === TableView.ExtendedSelection) {
            command |= ItemSelectionModel.Toggle | ItemSelectionModel.Current
            this.__anchorRow = row; this.__anchorCol = col
        } else if (shiftKey && mode !== TableView.SingleSelection) {
            let ar = this.__anchorRow >= 0 ? this.__anchorRow : this.currentRow
            let ac = this.__anchorCol >= 0 ? this.__anchorCol : this.currentColumn
            if (ar >= 0 && ac >= 0 && sm instanceof ItemSelectionModel) {
                sm.clearSelection()
                for (let r = Math.min(ar,row); r <= Math.max(ar,row); r++) {
                    for (let c = Math.min(ac,col); c <= Math.max(ac,col); c++) {
                        sm.select({ row:r, column:c }, ItemSelectionModel.Select | (behavior===TableView.SelectRows ? ItemSelectionModel.Rows : 0))
                    }
                }
                sm.setCurrentIndex({ row, column: col }, ItemSelectionModel.NoUpdate)
                return
            }
            command |= ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Current
        } else {
            command = ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Current
            if (behavior === TableView.SelectRows)    command |= ItemSelectionModel.Rows
            if (behavior === TableView.SelectColumns) command |= ItemSelectionModel.Columns
            this.__anchorRow = row; this.__anchorCol = col
        }

        if (sm instanceof ItemSelectionModel) sm.select({ row, column: col }, command)
        else { this.currentRow = row; this.currentColumn = col; this.__updateSelectionVisuals() }
    }

    // ─── public API ─────────────────────────────────────────────────────────

    index(row, column) { return { row, column } }

    cellAtPosition(x, y, includeSpacing) {
        let rowCount = this.rows, colCount = this.columns
        let foundRow = -1, foundCol = -1
        for (let r = 0; r < rowCount; r++) {
            let ry = this.__rowPositions[r], rh = this.__resolvedRowHeights[r] || 0
            let sp = includeSpacing ? this.rowSpacing/2 : 0
            if (y >= ry-sp && y < ry+rh+sp) { foundRow = r; break }
        }
        for (let c = 0; c < colCount; c++) {
            let cx = this.__colPositions[c], cw = this.__resolvedColumnWidths[c] || 0
            let sp = includeSpacing ? this.columnSpacing/2 : 0
            if (x >= cx-sp && x < cx+cw+sp) { foundCol = c; break }
        }
        if (foundRow >= 0 && foundCol >= 0) return { x: foundCol, y: foundRow, row: foundRow, column: foundCol }
        return { x: -1, y: -1, row: -1, column: -1 }
    }

    itemAtCell(cell) {
        let key = (cell.y !== undefined ? cell.y : cell.row) + ',' + (cell.x !== undefined ? cell.x : cell.column)
        return this.__cells[key] || null
    }

    rowAtIndex(modelIndex)    { return modelIndex ? (modelIndex.row    !== undefined ? modelIndex.row    : -1) : -1 }
    columnAtIndex(modelIndex) { return modelIndex ? (modelIndex.column !== undefined ? modelIndex.column : -1) : -1 }

    columnWidth(col) { return this.__resolvedColumnWidths[col] !== undefined ? this.__resolvedColumnWidths[col] : -1 }
    rowHeight(row)   { return this.__resolvedRowHeights[row]   !== undefined ? this.__resolvedRowHeights[row]   : -1 }

    setColumnWidth(col, size) { this.__explicitColumnWidths[col] = size }
    setRowHeight(row, size)   { this.__explicitRowHeights[row]   = size }
    clearColumnWidths() { this.__explicitColumnWidths = {} }
    clearRowHeights()   { this.__explicitRowHeights   = {} }

    isColumnLoaded(col) { return col >= this.leftColumn  && col <= this.rightColumn }
    isRowLoaded(row)    { return row >= this.topRow      && row <= this.bottomRow   }

    forceLayout() { this.__computeLayout(); this.__populateViewport() }

    positionViewAtCell(cell, mode, offset, subRect) {
        let col = cell.x !== undefined ? cell.x : (cell.column !== undefined ? cell.column : 0)
        let row = cell.y !== undefined ? cell.y : (cell.row    !== undefined ? cell.row    : 0)
        let ox  = offset ? (offset.x || 0) : 0
        let oy  = offset ? (offset.y || 0) : 0

        let cellX = this.__colPositions[col] || 0, cellY = this.__rowPositions[row] || 0
        let cellW = this.__resolvedColumnWidths[col] || 0, cellH = this.__resolvedRowHeights[row] || 0
        let vw = this.width, vh = this.height
        let cx = this.contentX, cy = this.contentY

        if (mode & TableView.Contain) {
            if (cellX < cx) cx = cellX
            else if (cellX + cellW > cx + vw) cx = cellX + cellW - vw
            if (cellY < cy) cy = cellY
            else if (cellY + cellH > cy + vh) cy = cellY + cellH - vh
        } else if (mode & TableView.Visible) {
            if (cellX + cellW <= cx || cellX >= cx + vw) cx = cellX
            if (cellY + cellH <= cy || cellY >= cy + vh) cy = cellY
        } else {
            if (mode & TableView.AlignLeft)    cx = cellX
            if (mode & TableView.AlignRight)   cx = cellX + cellW - vw
            if (mode & TableView.AlignHCenter) cx = cellX + cellW/2 - vw/2
            if (mode & TableView.AlignTop)     cy = cellY
            if (mode & TableView.AlignBottom)  cy = cellY + cellH - vh
            if (mode & TableView.AlignVCenter) cy = cellY + cellH/2 - vh/2
        }

        cx = Math.max(0, Math.min(cx + ox, Math.max(0, this.contentWidth  - vw)))
        cy = Math.max(0, Math.min(cy + oy, Math.max(0, this.contentHeight - vh)))
        this.contentX = cx
        this.contentY = cy
    }

    positionViewAtRow(row, mode, offset)       { this.positionViewAtCell({ x: 0, y: row }, mode, { x: 0, y: offset||0 }) }
    positionViewAtColumn(col, mode, offset)    { this.positionViewAtCell({ x: col, y: 0 }, mode, { x: offset||0, y: 0 }) }

    edit(modelIndex) {
        if (!modelIndex) return
        let row = modelIndex.row, col = modelIndex.column
        if (row < 0 || col < 0) return
        this.closeEditor()

        let cellData    = this._getCellData(row, col)
        let editDelegate = this.editDelegate

        if (editDelegate) {
            let exModel = Object.assign({}, cellData.model || {})
            exModel.row = row; exModel.column = col; exModel.display = cellData.display; exModel.editing = true

            let obj = editDelegate.create(this.contentItem, { model: exModel })
            obj.__DOM.style.position = 'absolute'
            obj.__DOM.style.zIndex   = '1000'
            obj.x = this.__colPositions[col] || 0
            obj.y = this.__rowPositions[row] || 0
            obj.width  = this.__resolvedColumnWidths[col] || 100
            obj.height = this.__resolvedRowHeights[row]   || 30
            obj.__complete && obj.__complete()
            this.__editItem = obj
        } else {
            this.__createDefaultEditor(row, col, cellData)
        }
        this.__editingCell = modelIndex
    }

    __createDefaultEditor(row, col, cellData) {
        let px = this.__colPositions[col] || 0, py = this.__rowPositions[row] || 0
        let pw = this.__resolvedColumnWidths[col] || 100, ph = this.__resolvedRowHeights[row] || 30

        let input = document.createElement('input')
        input.type = 'text'
        input.value = cellData.display !== undefined ? String(cellData.display) : ''
        input.style.cssText =
            'position:absolute;left:'+px+'px;top:'+py+'px;width:'+pw+'px;height:'+ph+'px;' +
            'box-sizing:border-box;border:2px solid #0078d4;outline:none;padding:2px 4px;' +
            'font:inherit;z-index:1000;background:#1e1e1e;color:#d4d4d4;'

        let contentDOM = this.contentItem.__DOM
        contentDOM.appendChild(input)
        this.__editElement = input
        input.focus(); input.select()

        input.addEventListener('keydown', (e) => {
            if (e.key === 'Enter' || e.key === 'Return') { e.preventDefault(); this.__commitEdit(row, col, input.value) }
            else if (e.key === 'Escape') { e.preventDefault(); this.closeEditor() }
            else if (e.key === 'Tab') {
                e.preventDefault(); this.__commitEdit(row, col, input.value)
                let cc = this.columns, rc = this.rows
                if (e.shiftKey) { if (col > 0) this.edit({row,column:col-1}); else if (row > 0) this.edit({row:row-1,column:cc-1}) }
                else { if (col < cc-1) this.edit({row,column:col+1}); else if (row < rc-1) this.edit({row:row+1,column:0}) }
            }
        })
        input.addEventListener('blur', () => {
            if (this.__editingCell && this.__editElement === input) this.__commitEdit(row, col, input.value)
        })
    }

    __commitEdit(row, col, value) {
        let model = this.model
        if (model && typeof model.setModelData === 'function') model.setModelData(row, col, value)
        else if (Array.isArray(model)) {
            let keys = this._getColumnKeys(), key = keys[col]
            if (key && model[row] && typeof model[row] === 'object') model[row][key] = value
        }
        this.commit(row, col, value)
        this.closeEditor()
        this.__rebuildTable()
    }

    closeEditor() {
        let el = this.__editElement
        if (el) { this.__editElement = null; if (el.parentNode) el.parentNode.removeChild(el) }
        let item = this.__editItem
        if (item) { this.__editItem = null; item.destroy && item.destroy() }
        this.__editingCell = null
    }

    moveColumn(src, dst) {
        let w1 = this.__resolvedColumnWidths[src], w2 = this.__resolvedColumnWidths[dst]
        this.__resolvedColumnWidths[src] = w2; this.__resolvedColumnWidths[dst] = w1
        this.forceLayout()
    }
    moveRow(src, dst) {
        let h1 = this.__resolvedRowHeights[src], h2 = this.__resolvedRowHeights[dst]
        this.__resolvedRowHeights[src] = h2; this.__resolvedRowHeights[dst] = h1
        this.forceLayout()
    }

    __destroy() {
        this.__clearAll(true)
        super.__destroy()
    }
}

module.exports = TableView
