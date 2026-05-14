const { QtObject } = require('./QtObject')
const { QVar, QReal, QBool } = require('../utils/properties')

class SortFilterProxyModel extends QtObject {
    static AscendingOrder = 0
    static DescendingOrder = 1

    // Qt::CaseSensitivity
    static CaseInsensitive = 0
    static CaseSensitive = 1

    static defaultProperties = {
        sourceModel: { type: QVar, value: null, changed: '$sourceModelChanged' },
        filterRole: { type: QVar, value: 'display' },
        filterString: { type: QVar, value: '', changed: '$filterChanged' },
        filterRegularExpression: { type: QVar, value: null, changed: '$filterChanged' },
        filterCaseSensitivity: { type: QReal, value: SortFilterProxyModel.CaseInsensitive, changed: '$filterChanged' },
        sortRole: { type: QVar, value: 'display' },
        sortOrder: { type: QReal, value: SortFilterProxyModel.AscendingOrder, changed: '$sortChanged' },
        sortColumn: { type: QReal, value: -1, changed: '$sortChanged' },
        sortEnabled: { type: QBool, value: true, changed: '$sortChanged' },
        count: { type: QReal, value: 0 },
        data: { type: QVar, value: undefined },
    }

    static defaultSignals = {
        $transaction: { params: ['sender', 'changeset'] },
    }

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)
        this.$proxyToSource = []  // proxy index -> source index
        this.$columns = null      // delegated from source model
        this.$sourceData = null
    }

    $complete() {
        super.$complete()
        this.$invalidate()
    }

    // ── Source model connection ──

    $sourceModelChanged() {
        if (this.$connectedSource) {
            let src = this.$connectedSource
            if (src.getSignal && src.getSignal('$transaction')) {
                src.getSignal('$transaction').disconnect(this, this.$onSourceTransaction)
            }
        }
        let source = this.getPropertyValue('sourceModel')
        this.$connectedSource = source
        if (source && source.getSignal && source.getSignal('$transaction')) {
            source.getSignal('$transaction').connect(this, this.$onSourceTransaction)
        }
        // Delegate $columns from source
        if (source && source.$columns) {
            this.$columns = source.$columns
        }
        this.$invalidate()
    }

    $onSourceTransaction() {
        this.$invalidate()
    }

    $filterChanged() {
        this.$invalidate()
    }

    $sortChanged() {
        this.$invalidate()
    }

    // ── Core filter/sort logic ──

    $invalidate() {
        let source = this.getPropertyValue('sourceModel')
        if (!source) {
            this.$proxyToSource = []
            this.$sourceData = null
            this._updateOutput()
            return
        }

        // Get source data array
        let sourceData = null
        if (source.getPropertyValue) {
            sourceData = source.getPropertyValue('data')
            if (!sourceData) {
                let rows = source.getPropertyValue('rows')
                if (rows && Array.isArray(rows)) sourceData = rows
            }
        }
        if (Array.isArray(source)) {
            sourceData = source
        }
        if (!sourceData || !Array.isArray(sourceData)) {
            this.$proxyToSource = []
            this.$sourceData = null
            this._updateOutput()
            return
        }

        this.$sourceData = sourceData

        // Step 1: Filter
        let filtered = []
        let filterString = this.getPropertyValue('filterString')
        let filterRegex = this.getPropertyValue('filterRegularExpression')
        let filterRole = this.getPropertyValue('filterRole')
        let caseSensitive = this.getPropertyValue('filterCaseSensitivity') === SortFilterProxyModel.CaseSensitive
        let hasFilter = (filterString && filterString.length > 0) || filterRegex

        for (let i = 0; i < sourceData.length; i++) {
            if (hasFilter) {
                let item = sourceData[i]
                let value = ''
                if (item && typeof item === 'object') {
                    value = item[filterRole] !== undefined ? String(item[filterRole]) : ''
                } else {
                    value = String(item)
                }

                if (filterRegex) {
                    let re = filterRegex instanceof RegExp ? filterRegex : new RegExp(filterRegex, caseSensitive ? '' : 'i')
                    if (!re.test(value)) continue
                } else if (filterString) {
                    let sv = caseSensitive ? value : value.toLowerCase()
                    let sf = caseSensitive ? filterString : filterString.toLowerCase()
                    if (sv.indexOf(sf) < 0) continue
                }
            }
            filtered.push(i)
        }

        // Step 2: Sort
        let sortEnabled = this.getPropertyValue('sortEnabled')
        let sortColumn = this.getPropertyValue('sortColumn')
        let sortRole = this.getPropertyValue('sortRole')
        let sortOrder = this.getPropertyValue('sortOrder')

        if (sortEnabled && sortColumn >= 0) {
            // Determine the sort key from column
            let sortKey = sortRole
            if (this.$columns && this.$columns.length > sortColumn) {
                let colDef = this.$columns[sortColumn]
                if (colDef && colDef.getPropertyValue) {
                    sortKey = colDef.getPropertyValue('display') || sortRole
                }
            } else if (sourceData.length > 0 && typeof sourceData[0] === 'object') {
                let keys = Object.keys(sourceData[0]).filter(function(k) {
                    return k !== 'index' && k !== '$lock' && k[0] !== '$'
                })
                if (keys[sortColumn]) sortKey = keys[sortColumn]
            }

            filtered.sort(function(a, b) {
                let itemA = sourceData[a]
                let itemB = sourceData[b]
                let valA = itemA && typeof itemA === 'object' ? itemA[sortKey] : itemA
                let valB = itemB && typeof itemB === 'object' ? itemB[sortKey] : itemB

                if (valA === undefined) valA = ''
                if (valB === undefined) valB = ''

                let result = 0
                if (typeof valA === 'number' && typeof valB === 'number') {
                    result = valA - valB
                } else {
                    result = String(valA).localeCompare(String(valB))
                }

                return sortOrder === SortFilterProxyModel.DescendingOrder ? -result : result
            })
        }

        this.$proxyToSource = filtered
        this._updateOutput()
    }

    _updateOutput() {
        let source = this.getPropertyValue('sourceModel')
        let sourceData = this.$sourceData

        let proxyData = []
        for (let i = 0; i < this.$proxyToSource.length; i++) {
            let si = this.$proxyToSource[i]
            if (sourceData && sourceData[si]) {
                proxyData.push(sourceData[si])
            }
        }

        this.getProperty('data').value = proxyData
        this.getProperty('count').reset(proxyData.length)

        if (this.$signals.$transaction) {
            this.$signals.$transaction(this, [['proxy', 'proxy', 'rebuild']])
        }
    }

    // ── Public API ──

    invalidateFilter() {
        this.$invalidate()
    }

    invalidateSort() {
        this.$invalidate()
    }

    sort(column, order) {
        if (order !== undefined) this.getProperty('sortOrder').reset(order)
        this.getProperty('sortColumn').reset(column)
        // $sortChanged triggers $invalidate
    }

    rowCount() {
        return this.$proxyToSource.length
    }

    // Map proxy row to source row
    mapToSource(proxyRow) {
        if (proxyRow < 0 || proxyRow >= this.$proxyToSource.length) return -1
        return this.$proxyToSource[proxyRow]
    }

    // Map source row to proxy row
    mapFromSource(sourceRow) {
        for (let i = 0; i < this.$proxyToSource.length; i++) {
            if (this.$proxyToSource[i] === sourceRow) return i
        }
        return -1
    }

    // Get data at proxy row
    getData(row, columnIndex) {
        let sourceRow = this.mapToSource(row)
        if (sourceRow < 0) return undefined
        let source = this.getPropertyValue('sourceModel')
        if (source && source.getData) {
            return source.getData(sourceRow, columnIndex)
        }
        if (this.$sourceData && this.$sourceData[sourceRow]) {
            if (columnIndex !== undefined && this.$columns && this.$columns.length > 0) {
                let colName = this.$columns[columnIndex]
                if (colName && colName.getPropertyValue) colName = colName.getPropertyValue('display')
                return this.$sourceData[sourceRow][colName]
            }
            return this.$sourceData[sourceRow]
        }
        return undefined
    }

    // Proxy setModelData — delegates to source model
    setModelData(row, columnIndex, value) {
        let sourceRow = this.mapToSource(row)
        if (sourceRow < 0) return false
        let source = this.getPropertyValue('sourceModel')
        if (source && source.setModelData) {
            return source.setModelData(sourceRow, columnIndex, value)
        }
        return false
    }

    // get item at proxy index
    get(index) {
        let sourceRow = this.mapToSource(index)
        if (sourceRow < 0) return undefined
        let source = this.getPropertyValue('sourceModel')
        if (source && source.get) return source.get(sourceRow)
        if (this.$sourceData) return this.$sourceData[sourceRow]
        return undefined
    }

    getColumnName(columnIndex) {
        let source = this.getPropertyValue('sourceModel')
        if (source && source.getColumnName) return source.getColumnName(columnIndex)
        return undefined
    }

    columnCount() {
        let source = this.getPropertyValue('sourceModel')
        if (source && source.columnCount) return source.columnCount()
        if (this.$columns) return this.$columns.length
        return 0
    }
}

module.exports.SortFilterProxyModel = SortFilterProxyModel
