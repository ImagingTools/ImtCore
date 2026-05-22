const { QtObject } = require('./QtObject')
const { QVar, QInt, QBool } = require('../utils/properties')

/**
 * ModelIndex — analog of QModelIndex for tree/table model navigation.
 *
 * A ModelIndex uniquely identifies a node in a hierarchical model.
 * It stores the model reference, row within that model, and a link
 * to the parent index (forming a path from root to node).
 *
 * Usage:
 *   var idx = ModelIndex.create(model, row, parentIndex, childrenKey)
 *   idx.data('name')         // read field from model
 *   idx.setData('name', 'x') // write field to model
 *   idx.parent()             // parent ModelIndex or null
 *   idx.depth()              // 0 for root
 *   idx.path()               // [0, 2, 1] — row path from root
 *   idx.child(0)             // first child ModelIndex
 *   idx.childCount()         // number of children
 *   idx.equals(other)        // structural equality
 */
class ModelIndex extends QtObject {

    static defaultProperties = {
        row:    { type: QInt,  value: -1 },
        column: { type: QInt,  value: 0 },
        valid:  { type: QBool, value: false },
    }

    static defaultSignals = {}

    // ── Factory (preferred way to create) ──

    static create(model, row, parentIndex, childrenKey) {
        var idx = new ModelIndex(null, null, null)
        idx.$model = model || null
        idx.$parentIndex = parentIndex || null
        idx.$childrenKey = childrenKey || 'children'
        idx.getProperty('row').reset(row !== undefined && row !== null ? row : -1)
        idx.getProperty('valid').reset(model != null && row >= 0)
        return idx
    }

    // ── Invalid singleton ──

    static _invalid = null
    static invalid() {
        if (!ModelIndex._invalid) {
            ModelIndex._invalid = new ModelIndex(null, null, null)
        }
        return ModelIndex._invalid
    }

    // ── Constructor ──

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)
        this.$model = null
        this.$parentIndex = null
        this.$childrenKey = 'children'

        if (exModel) {
            if (exModel.model !== undefined)      this.$model = exModel.model
            if (exModel.parentIndex !== undefined) this.$parentIndex = exModel.parentIndex
            if (exModel.childrenKey !== undefined) this.$childrenKey = exModel.childrenKey
            if (exModel.row !== undefined) {
                this.getProperty('row').reset(exModel.row)
                this.getProperty('valid').reset(this.$model != null && exModel.row >= 0)
            }
        }
    }

    // ── Query ──

    isValid() {
        return this.$model != null && this.getPropertyValue('row') >= 0
    }

    parent() {
        return this.$parentIndex
    }

    model() {
        return this.$model
    }

    /**
     * Read a field from the model at this index's row.
     */
    data(key) {
        if (!this.isValid()) return undefined
        var m = this.$model
        var r = this.getPropertyValue('row')
        if (m.GetData) return m.GetData(key, r)
        if (m.get) {
            var item = m.get(r)
            return item ? item[key] : undefined
        }
        return undefined
    }

    /**
     * Write a field to the model at this index's row.
     */
    setData(key, value) {
        if (!this.isValid()) return false
        var m = this.$model
        var r = this.getPropertyValue('row')
        if (m.SetData) return m.SetData(key, value, r)
        if (m.get) {
            var item = m.get(r)
            if (item) { item[key] = value; return true }
        }
        return false
    }

    /**
     * Get the child sub-model (TreeItemModel) at this node, or null.
     */
    childModel() {
        if (!this.isValid()) return null
        var m = this.$model
        var r = this.getPropertyValue('row')
        var key = this.$childrenKey
        if (m.GetTreeItemModel) return m.GetTreeItemModel(key, r)
        if (m.get) {
            var item = m.get(r)
            return item ? item[key] : null
        }
        return null
    }

    /**
     * Number of children at this node.
     */
    childCount() {
        var cm = this.childModel()
        if (!cm) return 0
        return cm.GetItemsCount ? cm.GetItemsCount() : (cm.count || 0)
    }

    /**
     * Create a ModelIndex for the i-th child of this node.
     */
    child(row, column) {
        var cm = this.childModel()
        if (!cm) return ModelIndex.invalid()
        var count = cm.GetItemsCount ? cm.GetItemsCount() : (cm.count || 0)
        if (row < 0 || row >= count) return ModelIndex.invalid()
        return ModelIndex.create(cm, row, this, this.$childrenKey)
    }

    /**
     * Create a ModelIndex for a sibling at the same level.
     */
    sibling(row, column) {
        if (!this.isValid()) return ModelIndex.invalid()
        var count = this.$model.GetItemsCount
            ? this.$model.GetItemsCount()
            : (this.$model.count || 0)
        if (row < 0 || row >= count) return ModelIndex.invalid()
        return ModelIndex.create(this.$model, row, this.$parentIndex, this.$childrenKey)
    }

    /**
     * Depth in the tree (0 = root level).
     */
    depth() {
        var d = 0
        var p = this.$parentIndex
        while (p && p.isValid()) { d++; p = p.$parentIndex }
        return d
    }

    /**
     * Path from root as array of row indices: [rootRow, ..., thisRow]
     */
    path() {
        var result = []
        var idx = this
        while (idx && idx.isValid()) {
            result.unshift(idx.getPropertyValue('row'))
            idx = idx.$parentIndex
        }
        return result
    }

    /**
     * String key for use in Sets/Maps — serialized path.
     */
    key() {
        return this.path().join('/')
    }

    /**
     * Structural equality: same row path in the same model hierarchy.
     */
    equals(other) {
        if (!other) return !this.isValid()
        if (!(other instanceof ModelIndex)) return false
        if (!this.isValid() && !other.isValid()) return true
        if (!this.isValid() || !other.isValid()) return false
        if (this.$model !== other.$model) return false
        if (this.getPropertyValue('row') !== other.getPropertyValue('row')) return false
        // Compare parent chain
        var p1 = this.$parentIndex
        var p2 = other.$parentIndex
        if (!p1 && !p2) return true
        if (!p1 || !p2) return false
        return p1.equals(p2)
    }

    /**
     * Checks whether `other` is a descendant of this index.
     */
    isAncestorOf(other) {
        if (!other || !this.isValid()) return false
        var p = other.$parentIndex
        while (p && p.isValid()) {
            if (p.equals(this)) return true
            p = p.$parentIndex
        }
        return false
    }
}

module.exports.ModelIndex = ModelIndex
