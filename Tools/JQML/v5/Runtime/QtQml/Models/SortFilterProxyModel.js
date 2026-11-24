// SortFilterProxyModel of QtQml.Models (qqmlsortfilterproxymodel.cpp, Qt 6.10+) with its filters and sorters. Checked
// against Qt 6.12 (tests/diff/cases/sortfilterproxymodel.qml):
//  - the rows of the source model (model / sourceModel) that every enabled filter accepts (a filter that is inverted the
//    others); ordered by the enabled sorters by priority (the lower first): the first that tells the rows apart decides,
//    with its sortOrder, else the order of the source (QQmlSorterCompositor::lessThan);
//  - filtered and sorted once complete, and again when the source, a filter or a sorter changes (dynamicSortFilter);
//  - ValueFilter: the role roleName equal to value (any value of the role without one); FunctionFilter: its filter(...)
//    method, called with the values of the roles its parameters name; RoleSorter: the values of roleName compared as
//    QVariant::compare; StringSorter: as strings, by a collator (caseSensitivity, numericMode, ignorePunctuation, locale);
//    FunctionSorter: its compare(lhs, rhs) method with objects of the roles of the rows.
// The views see the proxy as a list model: its rows are those of the source (modelRows).
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { QtObject } = require('../../Core/QtObject')

const AscendingOrder = 0, DescendingOrder = 1
const CaseInsensitive = 0, CaseSensitive = 1

// the rows of a source model as { count, row(i), data(i) } and its role names
function sourceRows(model){
    const { modelRows } = require('./ModelContext')
    return modelRows(model)
}
function roleNamesOf(model, rows){
    const { ListModel } = require('./ListModel')
    if(model instanceof ListModel){
        let names = new Set(model.$roles ? model.$roles.keys() : [])
        for(let i = 0; i < Math.min(rows.count, 1); i++) for(let key of Object.keys(rows.row(i) || {})) names.add(key)
        return [...names]
    }
    if(model && model.$isProxyModel) return model.$roleNames()
    if(rows.count){
        let first = rows.row(0)
        if(first && typeof first === 'object') return Object.keys(first)
    }
    return ['modelData', 'display']
}
// the value of a role of a source row (QAbstractItemModel::data)
function roleValue(rows, i, role){
    let row = rows.row(i)
    if(row && typeof row === 'object' && role in row) return row[role]
    if(role === 'modelData' || role === 'display'){
        let data = rows.data(i)
        return data && typeof data === 'object' && !Array.isArray(data) && role in data ? data[role] : data
    }
    return undefined
}
// QVariant::compare: numbers, strings (code points), booleans, dates; other types unordered (0 with a flag)
function variantCompare(a, b){
    if(a === undefined || b === undefined || a === null || b === null) return NaN
    if(typeof a === 'number' && typeof b === 'number') return a < b ? -1 : a > b ? 1 : 0
    if(typeof a === 'boolean' && typeof b === 'boolean') return a === b ? 0 : a ? 1 : -1
    if(a instanceof Date && b instanceof Date) return a - b < 0 ? -1 : a - b > 0 ? 1 : 0
    if(typeof a === 'string' && typeof b === 'string') return a < b ? -1 : a > b ? 1 : 0
    if(typeof a === 'number' && typeof b === 'string' || typeof a === 'string' && typeof b === 'number'){
        let x = Number(a), y = Number(b)
        if(!isNaN(x) && !isNaN(y)) return x < y ? -1 : x > y ? 1 : 0
    }
    return NaN
}

// ---- filters ----
class FilterBase extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$plain(FilterBase.ENABLED, true)
    }
    $plain(id, value){ QtObject.prototype.$write.call(this, id, value) }
    $write(id, value){
        let old = this.$v[id]
        super.$write(id, value)
        if(old !== this.$v[id]) this.invalidate()
    }
    // QQmlFilterBase::invalidate: the proxy filters again
    invalidate(){ engine.emit(this, FilterBase.SIGNAL_invalidateModel) }
    $accepts(){ return true }
}
defineType(FilterBase, { props: { enabled: 'bool', inverted: 'bool', column: 'int' }, signals: ['invalidateModel'] })

class RoleFilter extends FilterBase {}
defineType(RoleFilter, { props: { roleName: 'string' } })

class ValueFilter extends RoleFilter {
    // QQmlValueFilter::filterAcceptsRowInternal
    $accepts(proxy, rows, i){
        let role = this.$v[RoleFilter.ROLENAME]
        if(!role) return true
        if(!proxy.$roleNames().includes(role)){
            if(!this.$warned){ console.warn('Provided role name ' + role + " doesn't exist in the model"); this.$warned = true }
            return false
        }
        let value = roleValue(rows, i, role)
        let wanted = this.$v[ValueFilter.VALUE]
        if(value === undefined) return false
        return wanted === undefined || wanted === null || wanted == value
    }
}
defineType(ValueFilter, { props: { value: 'var' } })

class FunctionFilter extends FilterBase {
    // QQmlFunctionFilter::filterAcceptsRowInternal: filter(<the roles its parameters name>)
    $accepts(proxy, rows, i){
        let fn = this.filter
        if(typeof fn !== 'function') return true
        let params = (this.constructor.$methodParams && this.constructor.$methodParams.filter) || []
        let roles = proxy.$roleNames()
        for(let name of params){
            if(!roles.includes(name)){
                if(!this.$warned){ console.warn('Parameter specified in the filter method ' + name + " doesn't exist in the model"); this.$warned = true }
                return true
            }
        }
        let args = params.map(name => roleValue(rows, i, name))
        if(args.some(v => v === undefined)) return false
        return !!fn.apply(this, args)
    }
}
defineType(FunctionFilter, {})

// ---- sorters ----
class SorterBase extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$plain(SorterBase.ENABLED, true)
    }
    $plain(id, value){ QtObject.prototype.$write.call(this, id, value) }
    $write(id, value){
        let old = this.$v[id]
        super.$write(id, value)
        if(old !== this.$v[id]) this.invalidate()
    }
    invalidate(){ engine.emit(this, SorterBase.SIGNAL_invalidateModel) }
    $compare(){ return NaN }
}
defineType(SorterBase, { props: { enabled: 'bool', sortOrder: 'int', priority: 'int', column: 'int' }, signals: ['invalidateModel'] })

class RoleSorter extends SorterBase {
    $role(proxy){
        let role = this.$v[RoleSorter.ROLENAME]
        if(!proxy.$roleNames().includes(role)){
            if(role && !this.$warned){ console.warn('Provided role name ' + role + " doesn't exist in the model"); this.$warned = true }
            return null
        }
        return role
    }
    $compare(proxy, rows, a, b){
        let role = this.$role(proxy)
        if(role === null) return NaN
        return variantCompare(roleValue(rows, a, role), roleValue(rows, b, role))
    }
}
defineType(RoleSorter, { props: { roleName: 'string' } })

class StringSorter extends RoleSorter {
    $compare(proxy, rows, a, b){
        let role = this.$role(proxy)
        if(role === null) return NaN
        let locale = this.$v[StringSorter.LOCALE]
        let name = locale && locale.name ? String(locale.name).replace('_', '-') : undefined
        let collator = new Intl.Collator(name, {
            sensitivity: this.$v[StringSorter.CASESENSITIVITY] === CaseSensitive ? 'variant' : 'accent',
            numeric: !!this.$v[StringSorter.NUMERICMODE],
            ignorePunctuation: !!this.$v[StringSorter.IGNOREPUNCTUATION],
        })
        let str = (v) => (v === undefined || v === null ? '' : String(v))
        let r = collator.compare(str(roleValue(rows, a, role)), str(roleValue(rows, b, role)))
        return r < 0 ? -1 : r > 0 ? 1 : 0
    }
}
defineType(StringSorter, { props: { caseSensitivity: 'int', ignorePunctuation: 'bool', locale: 'var', numericMode: 'bool' } })

class FunctionSorter extends SorterBase {
    // QQmlFunctionSorter::compare: compare(lhs, rhs) with the roles of the rows (sort() of before 6.12 too)
    $compare(proxy, rows, a, b){
        let fn = typeof this.compare === 'function' ? this.compare : typeof this.sort === 'function' ? this.sort : null
        if(!fn) return NaN
        let data = (i) => { let o = {}; for(let role of proxy.$roleNames()) o[role] = roleValue(rows, i, role); return o }
        let r = Number(fn.call(this, data(a), data(b)))
        return r < 0 ? -1 : r > 0 ? 1 : r === 0 ? 0 : NaN
    }
}
defineType(FunctionSorter, {})

// ---- the proxy ----
class SortFilterProxyModel extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$plain(SortFilterProxyModel.DYNAMICSORTFILTER, true)
        this.$plain(SortFilterProxyModel.FILTERS, [])
        this.$plain(SortFilterProxyModel.SORTERS, [])
        this.$source = null
        this.$map = []           // proxy row -> source row
        this.$views = null
        this.$complete = false
        engine.scheduleComponentComplete(this)
    }
    get $isProxyModel(){ return true }
    $plain(id, value){ QtObject.prototype.$write.call(this, id, value) }
    $componentComplete(){
        if(super.$componentComplete) super.$componentComplete()
        this.$complete = true
        this.$update()
    }
    $setSource(model){
        if(this.$source === model) return
        if(this.$source && this.$source.$removeView) this.$source.$removeView(this)
        this.$source = model || null
        if(model && model.$addView) model.$addView(this)
        this.$plain(SortFilterProxyModel.MODEL, model || null)
        this.$plain(SortFilterProxyModel.SOURCEMODEL, model || null)
        this.$update()
    }
    // the source changed (a row in, out, moved or changed): filtered and sorted again when dynamicSortFilter
    $modelChanged(kind){
        if(kind !== 'change' || this.$v[SortFilterProxyModel.DYNAMICSORTFILTER]) this.$update()
        else this.$notify('change', 0, this.$map.length)
    }
    $roleNames(){ return roleNamesOf(this.$source, sourceRows(this.$source)) }
    $watch(list, kind){
        let key = kind + 'Watched'
        if(!this[key]) this[key] = new Set()
        for(let item of list){
            if(this[key].has(item)) continue
            this[key].add(item)
            engine.connect(item, item.constructor.SIGNAL_invalidateModel, () => this.$update(), null, engine.PRIORITY_NORMAL, this)
            let enabled = item.constructor.SIGNAL_enabledChanged
            if(enabled !== undefined) engine.connect(item, enabled, () => this.$update(), null, engine.PRIORITY_NORMAL, this)
        }
    }
    $filters(){ return (this.$v[SortFilterProxyModel.FILTERS] || []).filter(f => f instanceof FilterBase) }
    $sorters(){ return (this.$v[SortFilterProxyModel.SORTERS] || []).filter(s => s instanceof SorterBase) }
    // QSortFilterProxyModel: the accepted rows, sorted (stable)
    $update(){
        if(!this.$complete) return
        let rows = sourceRows(this.$source)
        let filters = this.$filters().filter(f => f.$v[FilterBase.ENABLED])
        let sorters = this.$sorters().filter(s => s.$v[SorterBase.ENABLED])
            .map((s, i) => ({ s, i })).sort((x, y) => x.s.$v[SorterBase.PRIORITY] - y.s.$v[SorterBase.PRIORITY] || x.i - y.i).map(x => x.s)
        this.$watch(this.$filters(), 'filters')
        this.$watch(this.$sorters(), 'sorters')
        let map = []
        for(let i = 0; i < rows.count; i++){
            if(filters.every(f => f.$v[FilterBase.INVERTED] !== f.$accepts(this, rows, i))) map.push(i)
        }
        if(sorters.length){
            map.sort((a, b) => {
                for(let s of sorters){
                    let r = s.$compare(this, rows, a, b)
                    if(r < 0 || r > 0) return s.$v[SorterBase.SORTORDER] === AscendingOrder ? r : -r
                }
                return a - b
            })
        }
        let old = this.$map
        if(old.length === map.length && old.every((v, i) => v === map[i])){
            this.$map = map
            this.$notify('change', 0, map.length)
            return
        }
        this.$map = []
        if(old.length) this.$notify('remove', 0, old.length)
        this.$map = map
        if(map.length) this.$notify('insert', 0, map.length)
        engine.emit(this, SortFilterProxyModel.SIGNAL_rowCountChanged)
    }
    $notify(kind, index, count){
        if(!count && kind === 'change') return
        if(this.$views) for(let view of this.$views.slice()) view.$modelChanged(kind, index, count, null, -1)
    }
    $addView(view){
        if(!this.$views) this.$views = []
        if(!this.$views.includes(view)) this.$views.push(view)
    }
    $removeView(view){ if(this.$views) this.$views = this.$views.filter(v => v !== view) }
    // the list of the views: row i is the source row it maps to
    $rowsView(){
        let rows = sourceRows(this.$source), map = this.$map
        return { count: map.length, row: (i) => rows.row(map[i]), data: (i) => rows.data(map[i]) }
    }
    rowCount(){ engine.track(this, SortFilterProxyModel.SIGNAL_rowCountChanged); return this.$map.length }
    columnCount(){ return 1 }
    index(row, column = 0){ return { row, column, valid: row >= 0 && row < this.$map.length, model: this } }
    roleNames(){ let out = {}; this.$roleNames().forEach((name, i) => { out[256 + i] = name }); return out }
    mapToSource(row){ return typeof row === 'number' ? (this.$map[row] === undefined ? -1 : this.$map[row]) : row }
    mapFromSource(row){ return typeof row === 'number' ? this.$map.indexOf(row) : row }
    data(index, role){
        let row = typeof index === 'number' ? index : index && index.row
        let rows = sourceRows(this.$source)
        let name = typeof role === 'string' ? role : this.$roleNames()[(role || 256) - 256]
        return row >= 0 && row < this.$map.length ? roleValue(rows, this.$map[row], name || 'display') : undefined
    }
    invalidate(){ this.$update() }
    invalidateSorter(){ this.$update() }
    $disposeNative(){
        if(this.$source && this.$source.$removeView) this.$source.$removeView(this)
        if(super.$disposeNative) super.$disposeNative()
    }
}
defineType(SortFilterProxyModel, {
    props: {
        model: { kind: 'var', initial: null }, sourceModel: { kind: 'var', initial: null }, filters: 'var', sorters: 'var',
        dynamicSortFilter: 'bool', recursiveFiltering: 'bool', autoAcceptChildRows: 'bool',
    },
    signals: ['rowCountChanged'],
})
{
    let write = SortFilterProxyModel.prototype.$write
    SortFilterProxyModel.prototype.$write = function(id, value){
        if(id === SortFilterProxyModel.MODEL || id === SortFilterProxyModel.SOURCEMODEL) return this.$setSource(value)
        write.call(this, id, value)
        if(id === SortFilterProxyModel.FILTERS || id === SortFilterProxyModel.SORTERS) this.$update()
    }
}

module.exports = { SortFilterProxyModel, FilterBase, RoleFilter, ValueFilter, FunctionFilter, SorterBase, RoleSorter, StringSorter, FunctionSorter }
