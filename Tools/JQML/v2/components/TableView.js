const { Item } = require('./Item')
const { Flickable } = require('./Flickable')
const { QVar, QReal, QBool, QAutoGeometry } = require('../utils/properties')
const { ListModel } = require('./ListModel')
const { ItemSelectionModel } = require('./ItemSelectionModel')
const { DelegateChooser } = require('./DelegateChooser')

class TableView extends Flickable {
    // SelectionBehavior
    static SelectionDisabled = 0
    static SelectCells = 1
    static SelectRows = 2
    static SelectColumns = 3

    // SelectionMode
    static SingleSelection = 0
    static ContiguousSelection = 1
    static ExtendedSelection = 2

    // EditTriggers
    static NoEditTriggers = 0
    static SingleTapped = 1
    static DoubleTapped = 2
    static SelectedTapped = 4
    static EditKeyPressed = 8
    static AnyKeyPressed = 16

    // PositionMode
    static AlignLeft = 0x0001
    static AlignHCenter = 0x0002
    static AlignRight = 0x0004
    static AlignTop = 0x0010
    static AlignVCenter = 0x0020
    static AlignBottom = 0x0040
    static AlignCenter = 0x0022
    static Visible = 0x0100
    static Contain = 0x0200

    static defaultProperties = {
        model: { type: QVar, value: undefined, changed: '$modelChanged' },
        delegate: { type: QVar, changed: '$delegateChanged' },
        selectionModel: { type: QVar, value: undefined, changed: '$selectionModelChanged' },
        selectionBehavior: { type: QReal, value: TableView.SelectCells, changed: '$selectionBehaviorChanged' },
        selectionMode: { type: QReal, value: TableView.ExtendedSelection },
        editTriggers: { type: QReal, value: TableView.DoubleTapped | TableView.EditKeyPressed },
        keyNavigationEnabled: { type: QBool, value: true },
        pointerNavigationEnabled: { type: QBool, value: true },
        alternatingRows: { type: QBool, value: true },
        reuseItems: { type: QBool, value: true },
        animate: { type: QBool, value: true },
        rowSpacing: { type: QReal, value: 0, changed: '$spacingChanged' },
        columnSpacing: { type: QReal, value: 0, changed: '$spacingChanged' },
        rows: { type: QReal, value: 0 },
        columns: { type: QReal, value: 0 },
        currentRow: { type: QReal, value: -1 },
        currentColumn: { type: QReal, value: -1 },
        topRow: { type: QReal, value: 0 },
        bottomRow: { type: QReal, value: 0 },
        leftColumn: { type: QReal, value: 0 },
        rightColumn: { type: QReal, value: 0 },
        contentWidth: { type: QAutoGeometry, value: 0, changed: '$contentWidthChanged' },
        contentHeight: { type: QAutoGeometry, value: 0, changed: '$contentHeightChanged' },
        rowHeightProvider: { type: QVar, value: undefined },
        columnWidthProvider: { type: QVar, value: undefined },
        resizableRows: { type: QBool, value: false },
        resizableColumns: { type: QBool, value: false },
        editDelegate: { type: QVar, value: undefined },
    }

    static defaultSignals = {
        layoutChanged: { params: [] },
        commit: { params: ['row', 'column', 'value'] },
    }

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)
        this.$exCtx = exCtx
        this.$cells = {} // key: "row,col" -> delegate instance
        this.$cache = []
        this.$explicitRowHeights = {}
        this.$explicitColumnWidths = {}
        this.$implicitRowHeights = {}
        this.$implicitColumnWidths = {}
        this.$resolvedRowHeights = {}
        this.$resolvedColumnWidths = {}
        this.$rowPositions = []
        this.$colPositions = []
        this.$model = null
        this.$editDelegate = null
        this.$editingCell = null
        this.$lastClickTime = 0
        this.$lastClickRow = -1
        this.$lastClickCol = -1
        this.$anchorRow = -1
        this.$anchorCol = -1

        this.setStyle({
            overflow: 'hidden',
        })
        this.getPropertyValue('contentItem').setStyle({
            position: 'relative',
        })

        this.$dom.setAttribute('tabindex', '0')
        this.$dom.style.outline = 'none'
        this._installKeyHandler()
    }

    $complete() {
        this.$ready = true
        if (this.$completed) return
        this.$completed = true

        if (this.$signals['Component.completed']) this.$signals['Component.completed']()
        let data = this.getProperty('data').get()
        for (let i = data.length - 1; i >= 0; i--) {
            if (data[i] instanceof QObject) data[i].$complete()
        }
        this.$anchorsUpdate()
        this.$rebuildTable()
    }

    // ── Model connection ──

    $disconnectModel() {
        if (this.$model && this.$model.getSignal && this.$model.getSignal('$transaction')) {
            this.$model.getSignal('$transaction').disconnect(this, this.$transaction)
        }
    }

    $connectModel(model) {
        if (model && model.getSignal && model.getSignal('$transaction')) {
            this.$model = model
            model.getSignal('$transaction').connect(this, this.$transaction)
        }
    }

    $transaction(sender, changeset) {
        if (changeset && sender === this.getPropertyValue('model')) {
            this.$rebuildTable()
        }
    }

    $modelChanged() {
        this.$disconnectModel()
        this.$clearAll(true)

        let model = this.getPropertyValue('model')
        if (model && typeof model === 'object' && model.getSignal) {
            this.$connectModel(model)
        }
        this.$rebuildTable()
    }

    $delegateChanged() {
        this.$clearAll(true)
        this.$rebuildTable()
    }

    $selectionModelChanged() {
        let sm = this.getPropertyValue('selectionModel')
        if (sm && sm instanceof ItemSelectionModel) {
            sm.getSignal('selectionChanged').connect(this, this.$onSelectionChanged)
            sm.getSignal('currentChanged').connect(this, this.$onCurrentChanged)
        }
    }

    $selectionBehaviorChanged() {}

    $spacingChanged() {
        this.$rebuildTable()
    }

    $onSelectionChanged() {
        this.$updateSelectionVisuals()
    }

    $onCurrentChanged(current, previous) {
        if (current) {
            this.getProperty('currentRow').reset(current.row !== undefined ? current.row : -1)
            this.getProperty('currentColumn').reset(current.column !== undefined ? current.column : -1)
        } else {
            this.getProperty('currentRow').reset(-1)
            this.getProperty('currentColumn').reset(-1)
        }
        this.$updateSelectionVisuals()
    }

    // ── Geometry helpers ──

    _getModelRowCount() {
        let model = this.getPropertyValue('model')
        if (!model) return 0
        if (typeof model === 'number') return model
        if (Array.isArray(model)) return model.length
        if (model.getPropertyValue) {
            let count = model.getPropertyValue('count')
            if (count !== undefined) return count
            let data = model.getPropertyValue('data')
            if (data) return data.length
        }
        return 0
    }

    _getModelColumnCount() {
        let model = this.getPropertyValue('model')
        if (!model) return 1
        if (typeof model === 'number') return 1
        if (Array.isArray(model)) {
            if (model.length > 0 && typeof model[0] === 'object') {
                return Object.keys(model[0]).length || 1
            }
            return 1
        }
        if (model.$columns && model.$columns.length > 0) return model.$columns.length
        if (model.getPropertyValue) {
            let data = model.getPropertyValue('data')
            if (data && data.length > 0) {
                let first = data[0]
                if (first && typeof first === 'object') {
                    let keys = Object.keys(first).filter(k => k !== 'index' && k !== '$lock' && !k.startsWith('$'))
                    return keys.length || 1
                }
            }
        }
        return 1
    }

    _getColumnKeys() {
        let model = this.getPropertyValue('model')
        if (!model) return []
        if (model.$columns && model.$columns.length > 0) {
            return model.$columns.map(c => c.getPropertyValue('display'))
        }
        if (Array.isArray(model)) {
            if (model.length > 0 && typeof model[0] === 'object') {
                return Object.keys(model[0])
            }
            return ['modelData']
        }
        if (model.getPropertyValue) {
            let data = model.getPropertyValue('data')
            if (data && data.length > 0) {
                let first = data[0]
                if (first && typeof first === 'object') {
                    return Object.keys(first).filter(k => k !== 'index' && k !== '$lock' && !k.startsWith('$'))
                }
            }
        }
        return ['display']
    }

    _getCellData(row, col) {
        let model = this.getPropertyValue('model')
        if (!model) return undefined
        let keys = this._getColumnKeys()
        let key = keys[col] || col

        if (typeof model === 'number') return { row: row, column: col, display: '' }
        if (Array.isArray(model)) {
            let item = model[row]
            if (typeof item === 'object') {
                return { row: row, column: col, display: item[key], model: item, index: row }
            }
            return { row: row, column: col, display: item, index: row }
        }
        if (model.getPropertyValue) {
            let data = model.getPropertyValue('data')
            if (data && data[row]) {
                let item = data[row]
                let display = item[key]
                return { row: row, column: col, display: display, model: item, index: row }
            }
        }
        return { row: row, column: col, display: '' }
    }

    _getRowHeight(row) {
        let provider = this.getPropertyValue('rowHeightProvider')
        if (typeof provider === 'function') {
            let h = provider(row)
            if (h !== undefined && h !== null && h >= 0) return h
        }
        if (this.$explicitRowHeights[row] !== undefined) {
            return this.$explicitRowHeights[row]
        }
        if (this.$implicitRowHeights[row] !== undefined) {
            return this.$implicitRowHeights[row]
        }
        return 30 // default row height
    }

    _getColumnWidth(col) {
        let provider = this.getPropertyValue('columnWidthProvider')
        if (typeof provider === 'function') {
            let w = provider(col)
            if (w !== undefined && w !== null && w >= 0) return w
        }
        if (this.$explicitColumnWidths[col] !== undefined) {
            return this.$explicitColumnWidths[col]
        }
        if (this.$implicitColumnWidths[col] !== undefined) {
            return this.$implicitColumnWidths[col]
        }
        return 100 // default column width
    }

    // ── Layout computation ──

    $computeLayout() {
        let rowCount = this._getModelRowCount()
        let colCount = this._getModelColumnCount()
        let rowSpacing = this.getPropertyValue('rowSpacing')
        let colSpacing = this.getPropertyValue('columnSpacing')

        this.$rowPositions = []
        this.$colPositions = []
        this.$resolvedRowHeights = {}
        this.$resolvedColumnWidths = {}

        let y = 0
        for (let r = 0; r < rowCount; r++) {
            let h = this._getRowHeight(r)
            this.$resolvedRowHeights[r] = h
            this.$rowPositions[r] = y
            y += h + (r < rowCount - 1 ? rowSpacing : 0)
        }

        let x = 0
        for (let c = 0; c < colCount; c++) {
            let w = this._getColumnWidth(c)
            this.$resolvedColumnWidths[c] = w
            this.$colPositions[c] = x
            x += w + (c < colCount - 1 ? colSpacing : 0)
        }

        this.getProperty('rows').reset(rowCount)
        this.getProperty('columns').reset(colCount)

        let totalWidth = x
        let totalHeight = y

        let cw = this.getProperty('contentWidth')
        if (cw.value !== totalWidth) {
            cw.value = totalWidth
            cw.getNotify()()
        }
        let ch = this.getProperty('contentHeight')
        if (ch.value !== totalHeight) {
            ch.value = totalHeight
            ch.getNotify()()
        }
    }

    // ── Table build ──

    $rebuildTable() {
        if (!this.$ready) return
        let delegate = this.getPropertyValue('delegate')
        let model = this.getPropertyValue('model')
        if (!delegate || model === undefined || model === null) return

        let rowCount = this._getModelRowCount()
        let colCount = this._getModelColumnCount()
        if (rowCount === 0 || colCount === 0) return

        this.$computeLayout()
        this.$populateViewport()
    }

    $populateViewport() {
        let rowCount = this.getPropertyValue('rows')
        let colCount = this.getPropertyValue('columns')
        let viewX = this.getPropertyValue('contentX')
        let viewY = this.getPropertyValue('contentY')
        let viewW = this.getPropertyValue('width')
        let viewH = this.getPropertyValue('height')

        let topRow = -1, bottomRow = -1, leftCol = -1, rightCol = -1

        // Find visible rows
        for (let r = 0; r < rowCount; r++) {
            let ry = this.$rowPositions[r]
            let rh = this.$resolvedRowHeights[r] || 0
            if (ry + rh >= viewY && ry <= viewY + viewH) {
                if (topRow < 0) topRow = r
                bottomRow = r
            }
        }

        // Find visible columns
        for (let c = 0; c < colCount; c++) {
            let cx = this.$colPositions[c]
            let cw = this.$resolvedColumnWidths[c] || 0
            if (cx + cw >= viewX && cx <= viewX + viewW) {
                if (leftCol < 0) leftCol = c
                rightCol = c
            }
        }

        if (topRow < 0) topRow = 0
        if (bottomRow < 0) bottomRow = Math.min(rowCount - 1, 0)
        if (leftCol < 0) leftCol = 0
        if (rightCol < 0) rightCol = Math.min(colCount - 1, 0)

        this.getProperty('topRow').value = topRow
        this.getProperty('bottomRow').value = bottomRow
        this.getProperty('leftColumn').value = leftCol
        this.getProperty('rightColumn').value = rightCol

        // Mark which cells should be visible
        let neededCells = new Set()
        for (let r = topRow; r <= bottomRow; r++) {
            for (let c = leftCol; c <= rightCol; c++) {
                neededCells.add(r + ',' + c)
            }
        }

        // Remove cells no longer needed
        for (let key in this.$cells) {
            if (!neededCells.has(key)) {
                this.$toCache(this.$cells[key])
                delete this.$cells[key]
            }
        }

        // Create/reuse cells that are needed
        for (let r = topRow; r <= bottomRow; r++) {
            for (let c = leftCol; c <= rightCol; c++) {
                let key = r + ',' + c
                if (!this.$cells[key]) {
                    this.$createCell(r, c)
                }
                let cell = this.$cells[key]
                if (cell) {
                    let px = this.$colPositions[c]
                    let py = this.$rowPositions[r]
                    let pw = this.$resolvedColumnWidths[c]
                    let ph = this.$resolvedRowHeights[r]
                    cell.getProperty('x').reset(px)
                    cell.getProperty('y').reset(py)
                    cell.getProperty('width').reset(pw)
                    cell.getProperty('height').reset(ph)
                }
            }
        }

        this.$updateSelectionVisuals()
        if (this.$signals.layoutChanged) this.$signals.layoutChanged()
    }

    _resolveDelegate(row, col, cellData) {
        let delegate = this.getPropertyValue('delegate')
        if (delegate instanceof DelegateChooser) {
            return delegate.$chooseDelegate(row, col, cellData) || null
        }
        return delegate
    }

    $createCell(row, col) {
        let key = row + ',' + col
        if (this.$cells[key]) return this.$cells[key]

        let cellData = this._getCellData(row, col)
        let obj = null
        let rawDelegate = this.getPropertyValue('delegate')
        let isChooser = rawDelegate instanceof DelegateChooser

        if (this.$cache.length && !isChooser) {
            obj = this.$fromCache()
            this.$cells[key] = obj

            // Update model data
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

            if (obj.$signals && obj.$signals['TableView.reused']) obj.$signals['TableView.reused']()
        } else {
            let delegate = isChooser ? this._resolveDelegate(row, col, cellData) : rawDelegate
            if (!delegate) return null
            let ctx = new ContextController(delegate.$exCtx, this.$exCtx)
            let createObject = delegate.createObject
            let cls = delegate.constructor

            let exModel = cellData.model || {}
            exModel.row = row
            exModel.column = col
            exModel.display = cellData.display
            exModel.index = cellData.index !== undefined ? cellData.index : row

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

            obj.setStyle({ position: 'absolute' })
        }

        obj.$complete()
        return obj
    }

    $toCache(item) {
        if (!item) return
        if (this.getPropertyValue('reuseItems')) {
            if (item.$dom) {
                this.getPropertyValue('contentItem').$dom.removeChild(item.$dom)
            }
            this.$cache.push(item)
            if (item.$signals && item.$signals['TableView.pooled']) item.$signals['TableView.pooled']()
        } else {
            item.destroy()
        }
    }

    $fromCache() {
        let item = this.$cache.pop()
        if (item && item.$dom) {
            this.getPropertyValue('contentItem').$dom.appendChild(item.$dom)
        }
        return item
    }

    $clearAll(destroyCache = false) {
        if (destroyCache) {
            while (this.$cache.length) {
                let obj = this.$cache.shift()
                if (obj) obj.destroy()
            }
        }

        for (let key in this.$cells) {
            let cell = this.$cells[key]
            if (destroyCache) {
                if (cell) cell.destroy()
            } else {
                this.$toCache(cell)
            }
            delete this.$cells[key]
        }
    }

    // ── Selection visuals ──

    $updateSelectionVisuals() {
        let sm = this.getPropertyValue('selectionModel')
        let curRow = this.getPropertyValue('currentRow')
        let curCol = this.getPropertyValue('currentColumn')

        for (let key in this.$cells) {
            let cell = this.$cells[key]
            if (!cell) continue
            let r = cell.$tableRow
            let c = cell.$tableCol
            let selected = false
            let current = false

            if (sm && sm instanceof ItemSelectionModel) {
                selected = sm.isSelected({ row: r, column: c })
                current = (r === curRow && c === curCol)
            }

            // Set properties if they exist on the delegate
            if (cell.getProperty && cell.$properties.selected !== undefined) {
                cell.getProperty('selected').reset(selected)
            }
            if (cell.getProperty && cell.$properties.current !== undefined) {
                cell.getProperty('current').reset(current)
            }

            // Apply visual styling
            if (cell.$dom) {
                if (current) {
                    cell.$dom.setAttribute('tv-current', '')
                } else {
                    cell.$dom.removeAttribute('tv-current')
                }
                if (selected) {
                    cell.$dom.setAttribute('tv-selected', '')
                } else {
                    cell.$dom.removeAttribute('tv-selected')
                }
            }
        }
    }

    // ── Scroll handlers ──

    $contentXChanged() {
        if (this.UID) this.getPropertyValue('contentItem').getProperty('x').reset(-this.getPropertyValue('contentX'))
        this.$populateViewport()
    }

    $contentYChanged() {
        if (this.UID) this.getPropertyValue('contentItem').getProperty('y').reset(-this.getPropertyValue('contentY'))
        this.$populateViewport()
    }

    $contentWidthChanged() {
        if (this.UID) this.getPropertyValue('contentItem').getProperty('width').reset(this.getPropertyValue('contentWidth'))
        if (this.getPropertyValue('contentWidth') < this.getPropertyValue('width')) {
            this.getProperty('contentX').reset(0)
        }
    }

    $contentHeightChanged() {
        if (this.UID) this.getPropertyValue('contentItem').getProperty('height').reset(this.getPropertyValue('contentHeight'))
        if (this.getPropertyValue('contentHeight') < this.getPropertyValue('height')) {
            this.getProperty('contentY').reset(0)
        }
    }

    $widthChanged() {
        super.$widthChanged()
        if (this.$ready) this.$populateViewport()
    }

    $heightChanged() {
        super.$heightChanged()
        if (this.$ready) this.$populateViewport()
    }

    // ── Pointer / keyboard interaction ──

    // Called by MouseController when this Flickable is the mousedown target
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
            let shiftKey = false
            let ctrlKey = false
            // Read modifier keys from MouseController
            // MC computes modifiers but stores on MouseArea.mouse, not on itself
            // We need to read them from the raw DOM event — but we don't have the event here
            // Instead, check the MC's pressed event modifiers by looking at the DOM state
            // We'll use a flag set from our own window listener
            shiftKey = this.$lastShiftKey || false
            ctrlKey = this.$lastCtrlKey || false

            // Double-click detection
            let now = Date.now()
            if (now - this.$lastClickTime < 300 && this.$lastClickRow === cell.row && this.$lastClickCol === cell.column) {
                // Double click
                this.$lastClickTime = 0
                this._onDoubleClick(cell.row, cell.column)
            } else {
                this.$lastClickTime = now
                this.$lastClickRow = cell.row
                this.$lastClickCol = cell.column
                this._selectCell(cell.row, cell.column, shiftKey, ctrlKey)
            }

            // Focus for keyboard
            this.$dom.style.pointerEvents = 'auto'
            this.$dom.focus()
        }
    }

    _onDoubleClick(row, col) {
        let triggers = this.getPropertyValue('editTriggers')
        if (triggers & TableView.DoubleTapped) {
            this.edit({ row: row, column: col })
        }
    }

    _installKeyHandler() {
        if (this.$keyHandlerInstalled) return
        this.$keyHandlerInstalled = true

        // Track modifier keys via window listener (since MC doesn't expose them for Flickable)
        let self = this
        window.addEventListener('mousedown', function(e) {
            self.$lastShiftKey = e.shiftKey
            self.$lastCtrlKey = e.ctrlKey
        }, true) // capture phase — fires before MC's handler

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
                    if (curCol < colCount - 1) { newCol = curCol + 1; handled = true }
                    break
                case 'ArrowLeft':
                    if (curCol > 0) { newCol = curCol - 1; handled = true }
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
                case 'Tab':
                    if (this.$editingCell) {
                        this.closeEditor()
                        if (curCol < colCount - 1) { newCol = curCol + 1 }
                        else if (curRow < rowCount - 1) { newRow = curRow + 1; newCol = 0 }
                        handled = true
                    }
                    break
                case ' ':
                    if (curRow >= 0 && curCol >= 0) {
                        this._selectCell(curRow, curCol, e.shiftKey, e.ctrlKey)
                        handled = true
                    }
                    break
                case 'a':
                    if (e.ctrlKey) {
                        for (let r = 0; r < rowCount; r++) {
                            for (let c = 0; c < colCount; c++) {
                                sm.select({ row: r, column: c }, ItemSelectionModel.Select)
                            }
                        }
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

    _cellAtViewportPosition(vx, vy) {
        let cx = vx + this.getPropertyValue('contentX')
        let cy = vy + this.getPropertyValue('contentY')
        return this.cellAtPosition(cx, cy)
    }

    _selectCell(row, col, shiftKey, ctrlKey) {
        let sm = this.getPropertyValue('selectionModel')
        if (!sm || !(sm instanceof ItemSelectionModel)) return

        let behavior = this.getPropertyValue('selectionBehavior')
        let mode = this.getPropertyValue('selectionMode')
        let modelIndex = { row: row, column: col }

        if (behavior === TableView.SelectionDisabled) return

        let command = ItemSelectionModel.NoUpdate

        if (behavior === TableView.SelectRows) {
            command = command | ItemSelectionModel.Rows
        } else if (behavior === TableView.SelectColumns) {
            command = command | ItemSelectionModel.Columns
        }

        if (ctrlKey && mode === TableView.ExtendedSelection) {
            command = command | ItemSelectionModel.Toggle | ItemSelectionModel.Current
            this.$anchorRow = row
            this.$anchorCol = col
        } else if (shiftKey && (mode === TableView.ContiguousSelection || mode === TableView.ExtendedSelection)) {
            // Range selection from anchor to clicked
            let anchorRow = this.$anchorRow >= 0 ? this.$anchorRow : this.getPropertyValue('currentRow')
            let anchorCol = this.$anchorCol >= 0 ? this.$anchorCol : this.getPropertyValue('currentColumn')
            if (anchorRow >= 0 && anchorCol >= 0) {
                sm.clearSelection()
                let r1 = Math.min(anchorRow, row), r2 = Math.max(anchorRow, row)
                let c1 = Math.min(anchorCol, col), c2 = Math.max(anchorCol, col)
                for (let r = r1; r <= r2; r++) {
                    for (let c = c1; c <= c2; c++) {
                        sm.select({ row: r, column: c }, ItemSelectionModel.Select | (behavior === TableView.SelectRows ? ItemSelectionModel.Rows : 0))
                    }
                }
                sm.setCurrentIndex(modelIndex, ItemSelectionModel.NoUpdate)
                return
            }
            command = command | ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Current
        } else {
            command = ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Current
            if (behavior === TableView.SelectRows) command |= ItemSelectionModel.Rows
            if (behavior === TableView.SelectColumns) command |= ItemSelectionModel.Columns
            this.$anchorRow = row
            this.$anchorCol = col
        }

        sm.select(modelIndex, command)
    }

    // ── Public API methods ──

    index(row, column) {
        return { row: row, column: column }
    }

    cellAtPosition(x, y, includeSpacing) {
        let rowCount = this.getPropertyValue('rows')
        let colCount = this.getPropertyValue('columns')

        let foundRow = -1
        let foundCol = -1

        for (let r = 0; r < rowCount; r++) {
            let ry = this.$rowPositions[r]
            let rh = this.$resolvedRowHeights[r]
            let spacing = includeSpacing ? this.getPropertyValue('rowSpacing') / 2 : 0
            if (y >= ry - spacing && y < ry + rh + spacing) {
                foundRow = r
                break
            }
        }

        for (let c = 0; c < colCount; c++) {
            let cx = this.$colPositions[c]
            let cw = this.$resolvedColumnWidths[c]
            let spacing = includeSpacing ? this.getPropertyValue('columnSpacing') / 2 : 0
            if (x >= cx - spacing && x < cx + cw + spacing) {
                foundCol = c
                break
            }
        }

        if (foundRow >= 0 && foundCol >= 0) return { x: foundCol, y: foundRow, row: foundRow, column: foundCol }
        return { x: -1, y: -1, row: -1, column: -1 }
    }

    itemAtCell(cell) {
        let key = (cell.y !== undefined ? cell.y : cell.row) + ',' + (cell.x !== undefined ? cell.x : cell.column)
        return this.$cells[key] || null
    }

    rowAtIndex(modelIndex) {
        return modelIndex ? (modelIndex.row !== undefined ? modelIndex.row : -1) : -1
    }

    columnAtIndex(modelIndex) {
        return modelIndex ? (modelIndex.column !== undefined ? modelIndex.column : -1) : -1
    }

    columnWidth(column) {
        return this.$resolvedColumnWidths[column] !== undefined ? this.$resolvedColumnWidths[column] : -1
    }

    rowHeight(row) {
        return this.$resolvedRowHeights[row] !== undefined ? this.$resolvedRowHeights[row] : -1
    }

    implicitColumnWidth(column) {
        return this.$implicitColumnWidths[column] !== undefined ? this.$implicitColumnWidths[column] : -1
    }

    implicitRowHeight(row) {
        return this.$implicitRowHeights[row] !== undefined ? this.$implicitRowHeights[row] : -1
    }

    explicitColumnWidth(column) {
        return this.$explicitColumnWidths[column] !== undefined ? this.$explicitColumnWidths[column] : -1
    }

    explicitRowHeight(row) {
        return this.$explicitRowHeights[row] !== undefined ? this.$explicitRowHeights[row] : -1
    }

    setColumnWidth(column, size) {
        this.$explicitColumnWidths[column] = size
    }

    setRowHeight(row, size) {
        this.$explicitRowHeights[row] = size
    }

    clearColumnWidths() {
        this.$explicitColumnWidths = {}
    }

    clearRowHeights() {
        this.$explicitRowHeights = {}
    }

    isColumnLoaded(column) {
        let leftCol = this.getPropertyValue('leftColumn')
        let rightCol = this.getPropertyValue('rightColumn')
        return column >= leftCol && column <= rightCol
    }

    isRowLoaded(row) {
        let topRow = this.getPropertyValue('topRow')
        let bottomRow = this.getPropertyValue('bottomRow')
        return row >= topRow && row <= bottomRow
    }

    forceLayout() {
        this.$computeLayout()
        this.$populateViewport()
    }

    positionViewAtCell(cell, mode, offset, subRect) {
        let col = cell.x !== undefined ? cell.x : (cell.column !== undefined ? cell.column : 0)
        let row = cell.y !== undefined ? cell.y : (cell.row !== undefined ? cell.row : 0)
        let ox = offset ? (offset.x || 0) : 0
        let oy = offset ? (offset.y || 0) : 0

        let cellX = this.$colPositions[col] || 0
        let cellY = this.$rowPositions[row] || 0
        let cellW = this.$resolvedColumnWidths[col] || 0
        let cellH = this.$resolvedRowHeights[row] || 0

        let vw = this.getPropertyValue('width')
        let vh = this.getPropertyValue('height')

        let cx = this.getPropertyValue('contentX')
        let cy = this.getPropertyValue('contentY')

        if (mode & TableView.Contain || mode === TableView.Contain) {
            // Ensure visible
            if (cellX < cx) cx = cellX
            else if (cellX + cellW > cx + vw) cx = cellX + cellW - vw
            if (cellY < cy) cy = cellY
            else if (cellY + cellH > cy + vh) cy = cellY + cellH - vh
        } else if (mode & TableView.Visible || mode === TableView.Visible) {
            if (cellX + cellW <= cx || cellX >= cx + vw) {
                cx = cellX
            }
            if (cellY + cellH <= cy || cellY >= cy + vh) {
                cy = cellY
            }
        } else {
            if (mode & TableView.AlignLeft) cx = cellX
            if (mode & TableView.AlignRight) cx = cellX + cellW - vw
            if (mode & TableView.AlignHCenter) cx = cellX + cellW / 2 - vw / 2
            if (mode & TableView.AlignTop) cy = cellY
            if (mode & TableView.AlignBottom) cy = cellY + cellH - vh
            if (mode & TableView.AlignVCenter) cy = cellY + cellH / 2 - vh / 2
        }

        cx += ox
        cy += oy

        // Clamp
        let maxX = Math.max(0, this.getPropertyValue('contentWidth') - vw)
        let maxY = Math.max(0, this.getPropertyValue('contentHeight') - vh)
        cx = Math.max(0, Math.min(cx, maxX))
        cy = Math.max(0, Math.min(cy, maxY))

        this.getProperty('contentX').reset(cx)
        this.getProperty('contentY').reset(cy)
    }

    positionViewAtRow(row, mode, offset) {
        this.positionViewAtCell({ x: 0, y: row }, mode, { x: 0, y: offset || 0 })
    }

    positionViewAtColumn(column, mode, offset) {
        this.positionViewAtCell({ x: column, y: 0 }, mode, { x: offset || 0, y: 0 })
    }

    edit(modelIndex) {
        if (!modelIndex) return
        let row = modelIndex.row
        let col = modelIndex.column
        if (row < 0 || col < 0) return

        // Close any existing editor
        this.closeEditor()

        let cellData = this._getCellData(row, col)
        let editDelegate = this.getPropertyValue('editDelegate')

        if (editDelegate) {
            // Custom edit delegate
            let ctx = new ContextController(editDelegate.$exCtx, this.$exCtx)
            let createObject = editDelegate.createObject
            let cls = editDelegate.constructor

            let exModel = cellData.model || {}
            exModel.row = row
            exModel.column = col
            exModel.display = cellData.display
            exModel.editing = true
            exModel.index = cellData.index !== undefined ? cellData.index : row

            let obj = createObject
                ? createObject(this.getPropertyValue('contentItem'), ctx, exModel, false)
                : new cls(this.getPropertyValue('contentItem'), ctx, exModel)

            for (let update of updateList.splice(0, updateList.length)) {
                update()
            }

            let px = this.$colPositions[col] || 0
            let py = this.$rowPositions[row] || 0
            let pw = this.$resolvedColumnWidths[col] || 100
            let ph = this.$resolvedRowHeights[row] || 30
            obj.getProperty('x').reset(px)
            obj.getProperty('y').reset(py)
            obj.getProperty('width').reset(pw)
            obj.getProperty('height').reset(ph)
            obj.setStyle({ position: 'absolute', zIndex: '1000' })
            obj.$complete()
            this.$editItem = obj
        } else {
            // Default: create an <input> overlay
            this._createDefaultEditor(row, col, cellData)
        }

        this.$editingCell = modelIndex
    }

    _createDefaultEditor(row, col, cellData) {
        let px = this.$colPositions[col] || 0
        let py = this.$rowPositions[row] || 0
        let pw = this.$resolvedColumnWidths[col] || 100
        let ph = this.$resolvedRowHeights[row] || 30

        let input = document.createElement('input')
        input.type = 'text'
        input.value = cellData.display !== undefined ? String(cellData.display) : ''
        input.style.cssText =
            'position:absolute;' +
            'left:' + px + 'px;top:' + py + 'px;' +
            'width:' + pw + 'px;height:' + ph + 'px;' +
            'box-sizing:border-box;border:2px solid #0078d4;' +
            'outline:none;padding:2px 4px;font:inherit;' +
            'z-index:1000;background:#1e1e1e;color:#d4d4d4;'

        this.getPropertyValue('contentItem').$dom.appendChild(input)
        this.$editElement = input
        input.focus()
        input.select()

        let self = this
        input.addEventListener('keydown', function(e) {
            if (e.key === 'Enter' || e.key === 'Return') {
                e.preventDefault()
                self._commitEdit(row, col, input.value)
            } else if (e.key === 'Escape') {
                e.preventDefault()
                self.closeEditor()
            } else if (e.key === 'Tab') {
                e.preventDefault()
                self._commitEdit(row, col, input.value)
                let colCount = self.getPropertyValue('columns')
                let rowCount = self.getPropertyValue('rows')
                if (e.shiftKey) {
                    if (col > 0) self.edit({ row: row, column: col - 1 })
                    else if (row > 0) self.edit({ row: row - 1, column: colCount - 1 })
                } else {
                    if (col < colCount - 1) self.edit({ row: row, column: col + 1 })
                    else if (row < rowCount - 1) self.edit({ row: row + 1, column: 0 })
                }
            }
        })

        input.addEventListener('blur', function() {
            if (self.$editingCell && self.$editElement === input) {
                self._commitEdit(row, col, input.value)
            }
        })
    }

    _commitEdit(row, col, value) {
        let model = this.getPropertyValue('model')
        if (model && model.setModelData) {
            model.setModelData(row, col, value)
        } else if (model && Array.isArray(model)) {
            let keys = this._getColumnKeys()
            let key = keys[col]
            if (key && model[row] && typeof model[row] === 'object') {
                model[row][key] = value
            }
        }

        if (this.$signals.commit) this.$signals.commit(row, col, value)
        this.closeEditor()
        this.$rebuildTable()
    }

    closeEditor() {
        let el = this.$editElement
        if (el) {
            this.$editElement = null
            if (el.parentNode) el.parentNode.removeChild(el)
        }
        let item = this.$editItem
        if (item) {
            this.$editItem = null
            if (item.$dom && item.$dom.parentNode) item.$dom.parentNode.removeChild(item.$dom)
        }
        this.$editingCell = null
    }

    moveColumn(source, destination) {
        // Basic implementation: swap column positions in display
        let w1 = this.$resolvedColumnWidths[source]
        let w2 = this.$resolvedColumnWidths[destination]
        this.$resolvedColumnWidths[source] = w2
        this.$resolvedColumnWidths[destination] = w1
        this.forceLayout()
    }

    moveRow(source, destination) {
        let h1 = this.$resolvedRowHeights[source]
        let h2 = this.$resolvedRowHeights[destination]
        this.$resolvedRowHeights[source] = h2
        this.$resolvedRowHeights[destination] = h1
        this.forceLayout()
    }

    destroy() {
        this.$clearAll(true)
        super.destroy()
    }
}

module.exports.TableView = TableView
