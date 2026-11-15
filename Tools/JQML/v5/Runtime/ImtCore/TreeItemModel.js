// TreeItemModel of com.imtcore.imtqml, ported from imtbase/CTreeItemModel.cpp.
// Rows are plain objects with keys in insertion order (Item::m_keys); a value can be a nested model.
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { ListModel } = require('../QtQml/Models/ListModel')

class TreeItemModel extends ListModel {
    constructor(parent = null){
        super(parent)
        this.$isArray = false
        this.$updateEnabled = true
        this.$transaction = 0
        this.$changes = 0
        this.$queryParams = {}
        this.$parentModel = parent instanceof TreeItemModel ? parent : null
    }

    // OnModelChanged(): counted inside a transaction, otherwise emitted and passed to the parent model
    $changed(){
        if(this.$transaction > 0){
            this.$changes++
            return
        }
        if(this.$updateEnabled) engine.emitArgs(this, TreeItemModel.SIGNAL_modelChanged, [])
        let parent = this.$parentModel
        if(parent && parent.$updateEnabled) parent.$changed()
    }

    $adopt(value){
        if(value instanceof TreeItemModel) value.$parentModel = this
    }

    beginChanges(){
        this.$transaction++
        return true
    }

    endChanges(){
        this.$transaction--
        if(this.$changes > 0 && this.$transaction === 0){
            this.$changed()
            this.$changes = 0
            return true
        }
        return false
    }

    insertNewItem(index){
        if(index === undefined) index = this.$rows.length
        else if(index < 0 || index > this.$rows.length) return -1
        this.$rows.splice(index, 0, {})
        if(this.$rows.length > 1) this.$isArray = true
        if(this.$updateEnabled){
            this.$notify('insert', index, 1)
            this.$changed()
        }
        return index
    }

    insertNewItemWithParameters(index, map){
        if(index < 0 || index > this.$rows.length) return
        let row = {}
        for(let key in map){
            row[key] = map[key]
            this.$adopt(map[key])
        }
        this.$rows.splice(index, 0, row)
        this.$notify('insert', index, 1)
        this.$changed()
    }

    removeItem(index){
        if(index < 0 || index > this.$rows.length - 1) return -1
        this.$rows.splice(index, 1)
        if(this.$updateEnabled) this.$notify('remove', index, 1)
        if(!this.$rows.length) this.$isArray = false
        if(this.$updateEnabled) this.$changed()
        return index
    }

    swapItems(index1, index2){
        let count = this.$rows.length
        if(index1 < 0 || index1 >= count || index2 < 0 || index2 >= count) return false
        if(index1 === index2) return true
        let row = this.$rows[index1]
        this.$rows[index1] = this.$rows[index2]
        this.$rows[index2] = row
        if(this.$updateEnabled) this.$notify('reset', 0, count)
        return true
    }

    addTreeModel(key, index = 0){
        if(this.$updateEnabled) this.beginChanges()
        try {
            if(!this.$rows.length && index === 0) this.insertNewItem()
            if(index < 0 || index > this.$rows.length - 1) return null
            let model = this.getTreeItemModel(key, index)
            if(!model){
                model = new TreeItemModel(this)
                this.setData(key, model, index)
            } else {
                model.clear()
            }
            return model
        } finally {
            if(this.$updateEnabled) this.endChanges()
        }
    }

    setExternTreeModel(key, model, index = 0){
        if(this.$updateEnabled) this.beginChanges()
        try {
            if(!this.$rows.length && index === 0) this.insertNewItem()
            if(index < 0 || index > this.$rows.length - 1) return false
            this.$adopt(model)
            this.setData(key, model, index)
            return true
        } finally {
            if(this.$updateEnabled) this.endChanges()
        }
    }

    setData(key, value, index = 0){
        if(this.$updateEnabled) this.beginChanges()
        try {
            if(!this.$rows.length && index === 0) this.insertNewItem()
            if(index < 0 || index > this.$rows.length - 1) return false
            let row = this.$rows[index]
            if(Object.prototype.hasOwnProperty.call(row, key) && row[key] === value) return true
            row[key] = value
            if(value instanceof TreeItemModel && !value.$parentModel) value.$parentModel = this
            if(this.$updateEnabled){
                this.$notify('change', index, 1, [key])
                this.$changed()
            }
            return true
        } finally {
            if(this.$updateEnabled) this.endChanges()
        }
    }

    removeData(key, index = 0){
        if(index < 0 || index > this.$rows.length - 1) return false
        delete this.$rows[index][key]
        if(this.$updateEnabled){
            this.$notify('change', index, 1, [key])
            this.$changed()
        }
        return true
    }

    getData(key, index = 0){
        let row = this.$rows[index]
        return row && Object.prototype.hasOwnProperty.call(row, key) ? row[key] : undefined
    }

    getParent(){ return this.$parentModel }
    isTreeModel(key, index = 0){ return this.getTreeItemModel(key, index) !== null }

    containsKey(key, index = 0){
        let row = this.$rows[index]
        return !!row && Object.prototype.hasOwnProperty.call(row, key)
    }

    isValidData(key, index = 0){ return this.getData(key, index) !== undefined }

    getTreeItemModel(key, index = 0){
        let value = this.getData(key, index)
        return value instanceof TreeItemModel ? value : null
    }

    getModelFromItem(index = 0){
        if(this.$rows.length <= index) return null
        let model = new TreeItemModel(null)
        return model.copyItemDataFromModel(0, this, index) ? model : null
    }

    getItemsCount(){ return this.$rows.length }

    getKeys(index = 0){
        let row = this.$rows[index]
        return row ? Object.keys(row) : []
    }

    clear(){
        let count = this.$rows.length
        this.$rows = []
        if(this.$updateEnabled){
            this.$notify('reset', 0, count)
            this.$changed()
        }
    }

    isArray(){ return this.$isArray }
    setIsArray(value){ this.$isArray = !!value }

    setQueryParam(key, value){ this.$queryParams[key] = value }
    getQueryParam(key){ return this.$queryParams[key] }
    takeQueryParam(key){
        let value = this.$queryParams[key]
        delete this.$queryParams[key]
        return value
    }
    getQueryParams(){ return this.$queryParams }
    clearQueryParams(){ this.$queryParams = {} }

    setUpdateEnabled(value){ this.$updateEnabled = !!value }

    refresh(){ this.$notify('reset', 0, this.$rows.length) }

    copy(source){
        if(!source) return false
        this.$rows = source.$rows.map(row => {
            let copy = {}
            for(let key of Object.keys(row)){
                let value = row[key]
                if(value instanceof TreeItemModel){
                    let sub = new TreeItemModel(this)
                    sub.copy(value)
                    copy[key] = sub
                } else {
                    copy[key] = value
                }
            }
            return copy
        })
        this.$queryParams = Object.assign({}, source.$queryParams)
        this.$isArray = source.$isArray
        this.$write(TreeItemModel.STATE, source.$v[TreeItemModel.STATE])
        this.$notify('reset', 0, this.$rows.length)
        this.$changed()
        return true
    }

    copyMe(){
        let model = new TreeItemModel(null)
        return model.copy(this) ? model : null
    }

    copyItemDataFromModel(index, source, sourceIndex = 0){
        if(this.$updateEnabled) this.beginChanges()
        try {
            this.removeItem(index)
            this.insertNewItem(index)
            let result = true
            for(let key of source.getKeys(sourceIndex)){
                let value = source.getData(key, sourceIndex)
                if(value instanceof TreeItemModel){
                    let child = this.addTreeModel(key, index)
                    result = result && child.copy(value)
                } else {
                    result = result && this.setData(key, value, index)
                }
            }
            return result
        } finally {
            if(this.$updateEnabled) this.endChanges()
        }
    }

    copyItemDataToModel(index, target, targetIndex = 0){
        return target ? target.copyItemDataFromModel(targetIndex, this, index) : false
    }

    // Compares nested models and int / string / bool values, like IsEqualWithModel
    isEqualWithModel(model){
        if(!model || model.$rows.length !== this.$rows.length) return false
        for(let i = 0; i < model.$rows.length; i++){
            let mine = this.$rows[i], theirs = model.$rows[i]
            let keys = Object.keys(theirs)
            if(Object.keys(mine).length !== keys.length) return false
            for(let key of keys){
                if(!Object.prototype.hasOwnProperty.call(mine, key)) return false
                let source = theirs[key], value = mine[key]
                if(source instanceof TreeItemModel){
                    if(!(value instanceof TreeItemModel) || !source.isEqualWithModel(value)) return false
                } else if(typeof source !== 'object' && source !== value){
                    return false
                }
            }
        }
        return true
    }

    createFromJson(json){
        if(this.$updateEnabled) this.beginChanges()
        try {
            this.clear()
            let document
            try {
                document = JSON.parse(json)
            } catch(error) {
                console.error('Error during parsing JSON document: ' + error.message + ' content: ' + json)
                return false
            }
            if(Array.isArray(document)){
                for(let value of document){
                    if(Array.isArray(value)) return false
                    let index = this.insertNewItem()
                    if(value && typeof value === 'object'){
                        if(!this.$parse(value, index)) return false
                    } else {
                        this.setData('', value, index)
                    }
                }
                return true
            }
            if(document && typeof document === 'object') return this.$parse(document, 0)
            return false
        } finally {
            if(this.$updateEnabled) this.endChanges()
        }
    }

    // ParseRecursive: objects become nested models, arrays nested array models
    $parse(object, index){
        for(let key of Object.keys(object)){
            let value = object[key]
            if(Array.isArray(value)){
                let model = this.addTreeModel(key, index)
                if(!model) return false
                model.setIsArray(true)
                for(let item of value){
                    if(Array.isArray(item)) return false
                    let childIndex = model.insertNewItem()
                    if(item && typeof item === 'object'){
                        if(!model.$parse(item, childIndex)) return false
                    } else {
                        model.setData('', item, childIndex)
                    }
                }
            } else if(value && typeof value === 'object'){
                let model = this.addTreeModel(key, index)
                if(!model) return false
                if(Object.keys(value).length && !model.$parse(value, 0)) return false
            } else {
                this.setData(key, value, index)
            }
        }
        return true
    }

    // SerializeRecursive: an array when isArray or more than one row, otherwise the first row as an object
    toJson(){
        return JSON.stringify(this.$serialize())
    }

    $serialize(){
        let rowValue = (row) => {
            let keys = Object.keys(row)
            if(keys.length === 1 && keys[0] === '') return valueOf(row[''])
            let object = {}
            for(let key of keys) object[key] = valueOf(row[key])
            return object
        }
        let valueOf = (value) => value instanceof TreeItemModel ? value.$serialize() : (value === undefined ? null : value)
        if(this.$isArray || this.$rows.length > 1) return this.$rows.map(rowValue)
        return this.$rows.length ? rowValue(this.$rows[0]) : {}
    }
}

defineType(TreeItemModel, {
    props: { state: 'string' },
    signals: ['modelChanged'],
})

module.exports = { TreeItemModel }
