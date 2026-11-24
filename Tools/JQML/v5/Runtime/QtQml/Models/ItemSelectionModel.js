// ItemSelectionModel of QtQml.Models: QItemSelectionModel, QItemSelection and QItemSelectionRange (qitemselectionmodel.cpp,
// Qt 6.12). The ranges keep persistent indexes, so a selection follows its items while rows and columns come and go.
const engine = require('../../Core/engine')
const { QtObject } = require('../../Core/QtObject')
const { defineType } = require('../../Core/type')
const { QModelIndex, QPersistentModelIndex, INVALID, sameIndex, indexLess, AbstractItemModel } = require('./ItemModel')
const QtEnums = require('../../Qt/enums')

const NoUpdate = 0x0, Clear = 0x1, Select = 0x2, Deselect = 0x4, Toggle = 0x8, Current = 0x10, Rows = 0x20, Columns = 0x40
const SelectCurrent = Select | Current, ToggleCurrent = Toggle | Current, ClearAndSelect = Clear | Select

const selectableAndEnabled = (flags) => !!(flags & QtEnums.ItemIsSelectable) && !!(flags & QtEnums.ItemIsEnabled)

// ---- QItemSelectionRange ------------------------------------------------------------------------------------------------
class QItemSelectionRange {
    constructor(topLeft = INVALID, bottomRight = topLeft){
        this.$tl = new QPersistentModelIndex(topLeft)
        this.$br = new QPersistentModelIndex(bottomRight)
    }
    get topLeft(){ return this.$tl.index }
    get bottomRight(){ return this.$br.index }
    get top(){ return this.$tl.row }
    get left(){ return this.$tl.column }
    get bottom(){ return this.$br.row }
    get right(){ return this.$br.column }
    get width(){ return this.$br.column - this.$tl.column + 1 }
    get height(){ return this.$br.row - this.$tl.row + 1 }
    get parent(){ return this.$tl.parent }
    get model(){ return this.$tl.model }
    get valid(){ return this.isValid() }
    isValid(){
        return this.$tl.valid && this.$br.valid && sameIndex(this.$tl.parent, this.$br.parent) && this.top <= this.bottom && this.left <= this.right
    }
    contains(index, column, parent){
        if(typeof index === 'number') return this.top <= index && index <= this.bottom && this.left <= column && column <= this.right && sameIndex(this.parent, parent || INVALID)
        return this.top <= index.row && index.row <= this.bottom && this.left <= index.column && index.column <= this.right && sameIndex(this.parent, index.parent)
    }
    intersects(other){
        if(this.model !== other.model) return false
        if(this.top > other.bottom || this.bottom < other.top) return false
        if(this.left > other.right || this.right < other.left) return false
        if(!sameIndex(this.parent, other.parent)) return false
        return this.isValid() && other.isValid()
    }
    intersected(other){
        if(this.model === other.model && sameIndex(this.parent, other.parent)){
            let model = this.model
            return new QItemSelectionRange(model.index(Math.max(this.top, other.top), Math.max(this.left, other.left), other.parent),
                model.index(Math.min(this.bottom, other.bottom), Math.min(this.right, other.right), other.parent))
        }
        return new QItemSelectionRange()
    }
    isEmpty(){
        if(!this.isValid() || !this.model) return true
        for(let column = this.left; column <= this.right; column++){
            for(let row = this.top; row <= this.bottom; row++){
                if(selectableAndEnabled(this.model.flags(this.model.index(row, column, this.parent)))) return false
            }
        }
        return true
    }
    indexes(){
        let result = []
        indexesFromRange(this, result)
        return result
    }
    equals(other){ return sameIndex(this.topLeft, other.topLeft) && sameIndex(this.bottomRight, other.bottomRight) }
    toString(){ return 'QItemSelectionRange(' + this.topLeft + ',' + this.bottomRight + ')' }
}

function indexesFromRange(range, result){
    if(!range.isValid() || !range.model) return
    let parent = range.parent
    for(let row = range.top; row <= range.bottom; row++){
        for(let column = range.left; column <= range.right; column++){
            let index = range.model.index(row, column, parent)
            if(selectableAndEnabled(range.model.flags(index))) result.push(index)
        }
    }
}

// ---- QItemSelection: a list of ranges -----------------------------------------------------------------------------------
class QItemSelection extends Array {
    static get [Symbol.species](){ return Array }
    static of(...ranges){
        let s = new QItemSelection()
        for(let r of ranges) s.push(r)
        return s
    }
    static from(ranges){ return QItemSelection.of(...ranges) }
    constructor(topLeft, bottomRight){
        super()
        if(topLeft !== undefined && typeof topLeft === 'object') this.select(topLeft, bottomRight)
    }
    // QItemSelection::select: the range between two indexes, in any order
    select(topLeft, bottomRight){
        if(!topLeft || !bottomRight || !topLeft.valid || !bottomRight.valid) return
        if(topLeft.model !== bottomRight.model || !sameIndex(topLeft.parent, bottomRight.parent)){
            console.warn("Can't select indexes from different model or with different parents")
            return
        }
        if(topLeft.row > bottomRight.row || topLeft.column > bottomRight.column){
            let top = Math.min(topLeft.row, bottomRight.row), bottom = Math.max(topLeft.row, bottomRight.row)
            let left = Math.min(topLeft.column, bottomRight.column), right = Math.max(topLeft.column, bottomRight.column)
            this.push(new QItemSelectionRange(topLeft.sibling(top, left), bottomRight.sibling(bottom, right)))
            return
        }
        this.push(new QItemSelectionRange(topLeft, bottomRight))
    }
    contains(index){
        if(!index || !index.valid || !selectableAndEnabled(index.model.flags(index))) return false
        return this.some(range => range.contains(index))
    }
    indexes(){
        let result = []
        for(let range of this) indexesFromRange(range, result)
        return result
    }
    isEmpty(){ return this.length === 0 }
    clone(){ return QItemSelection.from(this) }
    removeInvalid(){
        for(let i = this.length - 1; i >= 0; i--) if(!this[i].isValid()) this.splice(i, 1)
        return this
    }
    // QItemSelection::merge
    merge(other, command){
        if(!other.length || !(command & Select || command & Deselect || command & Toggle)) return
        let newSelection = new QItemSelection()
        let intersections = new QItemSelection()
        for(let range of other){
            if(!range.isValid()) continue
            newSelection.push(range)
            for(let t = 0; t < this.length; t++) if(range.intersects(this[t])) intersections.push(this[t].intersected(range))
        }
        for(let i = 0; i < intersections.length; i++){
            for(let t = 0; t < this.length;){
                if(this[t].intersects(intersections[i])){
                    QItemSelection.split(this[t], intersections[i], this)
                    this.splice(t, 1)
                } else t++
            }
            for(let n = 0; command & Toggle && n < newSelection.length;){
                if(newSelection[n].intersects(intersections[i])){
                    QItemSelection.split(newSelection[n], intersections[i], newSelection)
                    newSelection.splice(n, 1)
                } else n++
            }
        }
        if(!(command & Deselect)) for(let r of newSelection) this.push(r)
    }
    // QItemSelection::split: range minus other, appended to result
    static split(range, other, result){
        if(!sameIndex(range.parent, other.parent) || range.model !== other.model) return
        let parent = other.parent, model = range.model
        let top = range.top, left = range.left, bottom = range.bottom, right = range.right
        if(other.top > top){
            result.push(new QItemSelectionRange(model.index(top, left, parent), model.index(other.top - 1, right, parent)))
            top = other.top
        }
        if(other.bottom < bottom){
            result.push(new QItemSelectionRange(model.index(other.bottom + 1, left, parent), model.index(bottom, right, parent)))
            bottom = other.bottom
        }
        if(other.left > left){
            result.push(new QItemSelectionRange(model.index(top, left, parent), model.index(bottom, other.left - 1, parent)))
            left = other.left
        }
        if(other.right < right){
            result.push(new QItemSelectionRange(model.index(top, other.right + 1, parent), model.index(bottom, right, parent)))
            right = other.right
        }
    }
    equals(other){ return this.length === other.length && this.every((r, i) => r.equals(other[i])) }
    toString(){ return '[' + this.map(String).join(', ') + ']' }
}

const selectionIsEmpty = (selection) => selection.every(range => range.isEmpty())

// ---- ItemSelectionModel -------------------------------------------------------------------------------------------------
class ItemSelectionModel extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$ranges = new QItemSelection()
        this.$currentSelection = new QItemSelection()
        this.$currentCommand = NoUpdate
        this.$current = new QPersistentModelIndex(INVALID)
        this.$model = null
        this.$links = []
        this.$saved = null
    }

    $finalize(){
        this.$ranges.merge(this.$currentSelection, this.$currentCommand)
        if(this.$currentSelection.length) this.$currentSelection = new QItemSelection()
    }

    // ---- the properties (hasSelection, currentIndex, selection, selectedIndexes notify with the signals) ----
    get hasSelection(){
        engine.track(this, ItemSelectionModel.SIGNAL_selectionChanged)
        if(this.$currentCommand & (Toggle | Deselect)){
            let sel = this.$ranges.clone()
            sel.merge(this.$currentSelection, this.$currentCommand)
            return !selectionIsEmpty(sel)
        }
        return !(selectionIsEmpty(this.$ranges) && selectionIsEmpty(this.$currentSelection))
    }
    get currentIndex(){
        engine.track(this, ItemSelectionModel.SIGNAL_currentChanged)
        return this.$current.index
    }
    get selection(){
        engine.track(this, ItemSelectionModel.SIGNAL_selectionChanged)
        let selected = this.$ranges.clone()
        selected.merge(this.$currentSelection, this.$currentCommand)
        return selected.removeInvalid()
    }
    get selectedIndexes(){
        engine.track(this, ItemSelectionModel.SIGNAL_selectionChanged)
        let selected = this.$ranges.clone()
        selected.merge(this.$currentSelection, this.$currentCommand)
        return selected.indexes()
    }

    // ---- select ----
    select(selection, command){
        if(!this.$model){
            console.warn('QItemSelectionModel: Selecting when no model has been set will result in a no-op.')
            return
        }
        if(selection instanceof QModelIndex){
            let s = new QItemSelection(selection, selection)
            selection = s
        }
        if(command === NoUpdate) return
        let sel = selection instanceof QItemSelection ? selection : QItemSelection.from(Array.from(selection || []))
        this.$ranges.removeInvalid()
        let old = this.$ranges.clone()
        old.merge(this.$currentSelection, this.$currentCommand)
        if(command & Rows || command & Columns) sel = this.$expandSelection(sel, command)
        if(command & Clear){
            this.$ranges = new QItemSelection()
            this.$currentSelection = new QItemSelection()
        }
        if(!(command & Current)) this.$finalize()
        if(command & Toggle || command & Select || command & Deselect){
            this.$currentCommand = command
            this.$currentSelection = sel
        }
        let newSelection = this.$ranges.clone()
        newSelection.merge(this.$currentSelection, this.$currentCommand)
        this.$emitSelectionChanged(newSelection, old)
    }

    $expandSelection(selection, command){
        if(!selection.length && !(command & Rows || command & Columns)) return selection
        let expanded = new QItemSelection()
        let model = this.$model
        if(command & Rows){
            for(let range of selection){
                let parent = range.parent
                expanded.merge(new QItemSelection(model.index(range.top, 0, parent), model.index(range.bottom, model.$columnCount(parent) - 1, parent)), Select)
            }
        }
        if(command & Columns){
            for(let range of selection){
                let parent = range.parent
                expanded.merge(new QItemSelection(model.index(0, range.left, parent), model.index(model.$rowCount(parent) - 1, range.right, parent)), Select)
            }
        }
        return expanded
    }

    // QItemSelectionModel::emitSelectionChanged: what left the selection and what came into it
    $emitSelectionChanged(newSelection, oldSelection){
        if((!oldSelection.length && !newSelection.length) || oldSelection.equals(newSelection)) return
        if(!oldSelection.length || !newSelection.length){
            engine.emitArgs(this, ItemSelectionModel.SIGNAL_selectionChanged, [newSelection, oldSelection])
            return
        }
        let deselected = oldSelection.clone(), selected = newSelection.clone()
        for(let o = 0; o < deselected.length;){
            let advance = true
            for(let s = 0; s < selected.length && o < deselected.length;){
                if(deselected[o].equals(selected[s])){
                    deselected.splice(o, 1)
                    selected.splice(s, 1)
                    advance = false
                } else s++
            }
            if(advance) o++
        }
        let intersections = new QItemSelection()
        for(let o = 0; o < deselected.length; o++){
            for(let s = 0; s < selected.length; s++){
                if(deselected[o].intersects(selected[s])) intersections.push(deselected[o].intersected(selected[s]))
            }
        }
        for(let i = 0; i < intersections.length; i++){
            for(let o = 0; o < deselected.length;){
                if(deselected[o].intersects(intersections[i])){
                    QItemSelection.split(deselected[o], intersections[i], deselected)
                    deselected.splice(o, 1)
                } else o++
            }
            for(let s = 0; s < selected.length;){
                if(selected[s].intersects(intersections[i])){
                    QItemSelection.split(selected[s], intersections[i], selected)
                    selected.splice(s, 1)
                } else s++
            }
        }
        if(selected.length || deselected.length) engine.emitArgs(this, ItemSelectionModel.SIGNAL_selectionChanged, [selected, deselected])
    }

    clear(){
        this.clearSelection()
        this.clearCurrentIndex()
    }

    clearCurrentIndex(){
        let previous = this.$current.index
        this.$current = new QPersistentModelIndex(INVALID)
        if(previous.valid){
            for(let signal of [ItemSelectionModel.SIGNAL_currentChanged, ItemSelectionModel.SIGNAL_currentRowChanged, ItemSelectionModel.SIGNAL_currentColumnChanged]){
                engine.emitArgs(this, signal, [INVALID, previous])
            }
        }
    }

    // QItemSelectionModel::reset: clear() with the signals blocked
    reset(){
        this.$ranges = new QItemSelection()
        this.$currentSelection = new QItemSelection()
        this.$currentCommand = NoUpdate
        this.$current = new QPersistentModelIndex(INVALID)
    }

    clearSelection(){
        if(!this.$ranges.length && !this.$currentSelection.length) return
        this.select(new QItemSelection(), Clear)
    }

    setCurrentIndex(index, command){
        if(!this.$model){
            console.warn('QItemSelectionModel: Setting the current index when no model has been set will result in a no-op.')
            return
        }
        index = index || INVALID
        let previous = this.$current.index
        if(sameIndex(index, previous)){
            if(command !== NoUpdate) this.select(index, command)
            return
        }
        this.$current = new QPersistentModelIndex(index)
        if(command !== NoUpdate) this.select(this.$current.index, command)
        let current = this.$current.index
        engine.emitArgs(this, ItemSelectionModel.SIGNAL_currentChanged, [current, previous])
        if(current.row !== previous.row || !sameIndex(current.parent, previous.parent)) engine.emitArgs(this, ItemSelectionModel.SIGNAL_currentRowChanged, [current, previous])
        if(current.column !== previous.column || !sameIndex(current.parent, previous.parent)) engine.emitArgs(this, ItemSelectionModel.SIGNAL_currentColumnChanged, [current, previous])
    }

    isSelected(index){
        if(!index || this.$model !== index.model || !index.valid) return false
        let selected = this.$ranges.some(range => range.contains(index))
        if(this.$currentSelection.length){
            if(this.$currentCommand & Deselect && selected) selected = !this.$currentSelection.contains(index)
            else if(this.$currentCommand & Toggle) selected = selected !== this.$currentSelection.contains(index)
            else if(this.$currentCommand & Select && !selected) selected = this.$currentSelection.contains(index)
        }
        return selected ? selectableAndEnabled(this.$model.flags(index)) : false
    }

    // every selectable item of the row is selected
    isRowSelected(row, parent = INVALID){
        let model = this.$model
        if(!model) return false
        let columns = model.$columnCount(parent)
        let any = false
        for(let column = 0; column < columns; column++){
            let index = model.index(row, column, parent)
            if(!selectableAndEnabled(model.flags(index))) continue
            if(!this.isSelected(index)) return false
            any = true
        }
        return any
    }

    isColumnSelected(column, parent = INVALID){
        let model = this.$model
        if(!model) return false
        let rows = model.$rowCount(parent)
        let any = false
        for(let row = 0; row < rows; row++){
            let index = model.index(row, column, parent)
            if(!selectableAndEnabled(model.flags(index))) continue
            if(!this.isSelected(index)) return false
            any = true
        }
        return any
    }

    rowIntersectsSelection(row, parent = INVALID){
        let model = this.$model
        if(!model) return false
        for(let column = 0; column < model.$columnCount(parent); column++) if(this.isSelected(model.index(row, column, parent))) return true
        return false
    }

    columnIntersectsSelection(column, parent = INVALID){
        let model = this.$model
        if(!model) return false
        for(let row = 0; row < model.$rowCount(parent); row++) if(this.isSelected(model.index(row, column, parent))) return true
        return false
    }

    selectedRows(column = 0){
        let indexes = [], seen = new Set()
        for(let range of this.selection){
            let parent = range.parent
            for(let row = range.top; row <= range.bottom; row++){
                let key = row + ':' + parentKey(parent)
                if(seen.has(key)) continue
                seen.add(key)
                if(this.isRowSelected(row, parent)) indexes.push(this.$model.index(row, column, parent))
            }
        }
        return indexes
    }

    selectedColumns(row = 0){
        let indexes = [], seen = new Set()
        for(let range of this.selection){
            let parent = range.parent
            for(let column = range.left; column <= range.right; column++){
                let key = column + ':' + parentKey(parent)
                if(seen.has(key)) continue
                seen.add(key)
                if(this.isColumnSelected(column, parent)) indexes.push(this.$model.index(row, column, parent))
            }
        }
        return indexes
    }

    // ---- the model (QItemSelectionModelPrivate::initModel) ----
    $setModel(model){
        if(model === this.$model) return
        if(this.$model){
            this.reset()
            this.$disconnectAll()
        }
        this.$model = model
        if(model){
            let M = AbstractItemModel
            this.$connect(model, M.SIGNAL_rowsAboutToBeRemoved, (p, s, e) => this.$rowsAboutToBeRemoved(p, s, e))
            this.$connect(model, M.SIGNAL_columnsAboutToBeRemoved, (p, s, e) => this.$columnsAboutToBeRemoved(p, s, e))
            this.$connect(model, M.SIGNAL_rowsAboutToBeInserted, (p, s, e) => this.$rowsAboutToBeInserted(p, s, e))
            this.$connect(model, M.SIGNAL_columnsAboutToBeInserted, (p, s, e) => this.$columnsAboutToBeInserted(p, s, e))
            this.$connect(model, M.SIGNAL_rowsAboutToBeMoved, () => this.$layoutAboutToBeChanged(0))
            this.$connect(model, M.SIGNAL_columnsAboutToBeMoved, () => this.$layoutAboutToBeChanged(0))
            this.$connect(model, M.SIGNAL_rowsMoved, () => this.$layoutChanged(0))
            this.$connect(model, M.SIGNAL_columnsMoved, () => this.$layoutChanged(0))
            this.$connect(model, M.SIGNAL_layoutAboutToBeChanged, (parents, hint) => this.$layoutAboutToBeChanged(hint))
            this.$connect(model, M.SIGNAL_layoutChanged, (parents, hint) => this.$layoutChanged(hint))
            this.$connect(model, M.SIGNAL_modelReset, () => this.reset())
        }
    }

    $connect(object, signal, fn){
        engine.connect(object, signal, fn)
        this.$links.push([object, signal, fn])
    }

    $disconnectAll(){
        for(let [object, signal, fn] of this.$links) engine.disconnect(object, signal, fn)
        this.$links = []
    }

    $rowsAboutToBeRemoved(parent, start, end){
        this.$finalize()
        let current = this.$current.index
        if(current.valid && sameIndex(parent, current.parent) && current.row >= start && current.row <= end){
            let old = current, model = this.$model
            let next = start > 0 ? model.index(start - 1, old.column, parent)
                : end < model.$rowCount(parent) - 1 ? model.index(end + 1, old.column, parent) : INVALID
            this.$current = new QPersistentModelIndex(next)
            engine.emitArgs(this, ItemSelectionModel.SIGNAL_currentChanged, [next, old])
            engine.emitArgs(this, ItemSelectionModel.SIGNAL_currentRowChanged, [next, old])
            if(next.column !== old.column) engine.emitArgs(this, ItemSelectionModel.SIGNAL_currentColumnChanged, [next, old])
        }
        let deselected = new QItemSelection(), newParts = new QItemSelection()
        let indexesChanged = false
        let model = this.$model
        for(let i = 0; i < this.$ranges.length;){
            let it = this.$ranges[i]
            if(!sameIndex(it.topLeft.parent, parent)){
                let itParent = it.topLeft.parent
                while(itParent.valid && !sameIndex(itParent.parent, parent)) itParent = itParent.parent
                if(itParent.valid && start <= itParent.row && itParent.row <= end){
                    deselected.push(it)
                    this.$ranges.splice(i, 1)
                } else {
                    if(itParent.valid && end < itParent.row) indexesChanged = true
                    i++
                }
            } else if(start <= it.bottom && it.bottom <= end && start <= it.top && it.top <= end){
                deselected.push(it)
                this.$ranges.splice(i, 1)
            } else if(start <= it.top && it.top <= end){
                deselected.push(new QItemSelectionRange(it.topLeft, model.index(end, it.right, it.parent)))
                i++
            } else if(start <= it.bottom && it.bottom <= end){
                deselected.push(new QItemSelectionRange(model.index(start, it.left, it.parent), it.bottomRight))
                i++
            } else if(it.top < start && end < it.bottom){
                let removed = new QItemSelectionRange(model.index(start, it.left, it.parent), model.index(end, it.right, it.parent))
                deselected.push(removed)
                QItemSelection.split(it, removed, newParts)
                this.$ranges.splice(i, 1)
            } else if(end < it.top){
                indexesChanged = true
                i++
            } else i++
        }
        for(let r of newParts) this.$ranges.push(r)
        if(deselected.length || indexesChanged) engine.emitArgs(this, ItemSelectionModel.SIGNAL_selectionChanged, [new QItemSelection(), deselected])
    }

    $columnsAboutToBeRemoved(parent, start, end){
        let current = this.$current.index
        let model = this.$model
        if(current.valid && sameIndex(parent, current.parent) && current.column >= start && current.column <= end){
            let old = current
            let next = start > 0 ? model.index(old.row, start - 1, parent)
                : end < model.$columnCount() - 1 ? model.index(old.row, end + 1, parent) : INVALID
            this.$current = new QPersistentModelIndex(next)
            engine.emitArgs(this, ItemSelectionModel.SIGNAL_currentChanged, [next, old])
            if(next.row !== old.row) engine.emitArgs(this, ItemSelectionModel.SIGNAL_currentRowChanged, [next, old])
            engine.emitArgs(this, ItemSelectionModel.SIGNAL_currentColumnChanged, [next, old])
        }
        this.select(new QItemSelection(model.index(0, start, parent), model.index(model.$rowCount(parent) - 1, end, parent)), Deselect)
        this.$finalize()
    }

    $columnsAboutToBeInserted(parent, start){
        this.$finalize()
        let split = []
        let model = this.$model
        for(let i = 0; i < this.$ranges.length;){
            let it = this.$ranges[i]
            if(it.isValid() && sameIndex(it.parent, parent) && it.left < start && it.right >= start){
                let left = new QItemSelectionRange(it.topLeft, model.index(it.bottom, start - 1, it.parent))
                let right = new QItemSelectionRange(model.index(it.top, start, it.parent), it.bottomRight)
                this.$ranges.splice(i, 1)
                split.push(left, right)
            } else i++
        }
        for(let r of split) this.$ranges.push(r)
    }

    $rowsAboutToBeInserted(parent, start){
        this.$finalize()
        let split = []
        let indexesChanged = false
        let model = this.$model
        for(let i = 0; i < this.$ranges.length;){
            let it = this.$ranges[i]
            if(it.isValid() && sameIndex(it.parent, parent) && it.top < start && it.bottom >= start){
                let top = new QItemSelectionRange(it.topLeft, model.index(start - 1, it.right, it.parent))
                let bottom = new QItemSelectionRange(model.index(start, it.left, it.parent), it.bottomRight)
                this.$ranges.splice(i, 1)
                split.push(top, bottom)
            } else {
                if(it.isValid() && sameIndex(it.parent, parent) && it.top >= start) indexesChanged = true
                i++
            }
        }
        for(let r of split) this.$ranges.push(r)
        if(indexesChanged) engine.emitArgs(this, ItemSelectionModel.SIGNAL_selectionChanged, [new QItemSelection(), new QItemSelection()])
    }

    // the selection as single persistent indexes while the layout changes, merged into ranges again afterwards
    $layoutAboutToBeChanged(){
        let single = (selection) => {
            let list = []
            for(let index of selection.indexes()) list.push(new QPersistentModelIndex(index))
            return list
        }
        this.$saved = { ranges: single(this.$ranges), current: single(this.$currentSelection) }
    }

    $layoutChanged(){
        let saved = this.$saved
        this.$saved = null
        if(!saved || (!saved.ranges.length && !saved.current.length)) return
        let order = (list) => list.slice().sort((a, b) => {
            let pa = a.parent, pb = b.parent
            if(!sameIndex(pa, pb)) return indexLess(pa, pb) ? -1 : 1
            return indexLess(a.index, b.index) ? -1 : indexLess(b.index, a.index) ? 1 : 0
        })
        this.$ranges = mergeIndexes(order(saved.ranges))
        this.$currentSelection = mergeIndexes(order(saved.current))
    }

    $disposeNative(){
        this.$disconnectAll()
        super.$disposeNative && super.$disposeNative()
    }
}

const parentKey = (parent) => (parent && parent.valid ? parent.row + '.' + parent.column + '.' + String(parent.$id) : '-')

// QItemSelectionModelPrivate mergeIndexes: runs along the columns, then the runs of the same columns down the rows
function mergeIndexes(indexes){
    let colSpans = []
    for(let i = 0; i < indexes.length;){
        let tl = indexes[i]
        if(!tl.valid){ i++; continue }
        let br = tl
        while(++i < indexes.length){
            let next = indexes[i]
            if(!next.valid) continue
            if(sameIndex(next.parent, br.parent) && next.row === br.row && next.column === br.column + 1) br = next
            else break
        }
        colSpans.push([tl.index, br.index])
    }
    let rowSpans = new QItemSelection()
    for(let i = 0; i < colSpans.length;){
        let [tl, br] = colSpans[i]
        let prevTl = tl
        while(++i < colSpans.length){
            let [nextTl, nextBr] = colSpans[i]
            if(!sameIndex(nextTl.parent, tl.parent)) break
            if(nextTl.column === prevTl.column && nextBr.column === br.column && nextTl.row === prevTl.row + 1 && nextBr.row === br.row + 1){
                br = nextBr
                prevTl = nextTl
            } else break
        }
        rowSpans.push(new QItemSelectionRange(tl, br))
    }
    return rowSpans
}

defineType(ItemSelectionModel, {
    props: {
        model: { kind: 'var', effect: (ism, value) => ism.$setModelValue(value) },
    },
    signals: ['selectionChanged(selected,deselected)', 'currentChanged(current,previous)', 'currentRowChanged(current,previous)',
        'currentColumnChanged(current,previous)'],
})
Object.assign(ItemSelectionModel, { NoUpdate, Clear, Select, Deselect, Toggle, Current, Rows, Columns, SelectCurrent, ToggleCurrent, ClearAndSelect })

// the model property takes an item model, or a ListModel through its shared item model (as the views read it)
ItemSelectionModel.prototype.$setModelValue = function(value){
    const { adaptModel } = require('./ItemModel')
    let adaptor = value ? adaptModel(value) : null
    this.$setModel(adaptor ? adaptor.model : null)
}

module.exports = { ItemSelectionModel, QItemSelection, QItemSelectionRange, NoUpdate, Clear, Select, Deselect, Toggle, Current, Rows, Columns, SelectCurrent, ToggleCurrent, ClearAndSelect }
