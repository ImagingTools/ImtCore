const { defineType } = require('../Core/type')
const { Qt } = require('../Qt/Qt')
const { ListModel } = require('../QtQml/Models/ListModel')
const { BaseClass } = require('./BaseClass')

class BaseModel extends ListModel {
    escapeSpecialChars(text){ return BaseClass.prototype.escapeSpecialChars(text) }
    getProperties(item){ return item.getProperties() }

    hasNullElements(){
        for(let i = 0; i < this.count; i++){
            let item = this.$rows[i].item
            if(item === null || item === undefined) return true
        }
        return false
    }

    toJson(){ return this.$serialize(true) }
    toGraphQL(){ return this.$serialize(false) }

    // Returns '' if a required array of an item is null or has null elements; scalars are not validated
    $serialize(json){
        let out = '['
        for(let i = 0; i < this.count; i++){
            if(i > 0) out += ','
            let item = this.$rows[i].item
            if(item === null || item === undefined){
                out += 'null'
                continue
            }
            let separator = '{'
            for(let key of this.getProperties(item)){
                let value = item[key]
                let serialized
                if(value == null){
                    if(!item.isArrayValueValid(key, value)) return ''
                    serialized = json ? ' null' : 'null'
                } else if(typeof value === 'object'){
                    if(!item.isArrayValueValid(key, value)) return ''
                    if(Array.isArray(value)){
                        serialized = '['
                        for(let k = 0; k < value.length; k++){
                            if(k !== 0) serialized += ', '
                            serialized += typeof value[k] === 'string' ? '"' + this.escapeSpecialChars(value[k]) + '"' : value[k]
                        }
                        serialized += ']'
                    } else if(json && typeof value.toJson === 'function'){
                        serialized = value.toJson()
                        if(serialized === '') return ''
                    } else if(!json){
                        serialized = value.toGraphQL()
                        if(serialized === '') return ''
                    } else {
                        continue
                    }
                } else {
                    serialized = typeof value === 'string' ? '"' + this.escapeSpecialChars(value) + '"' : value
                }
                out += separator + (json ? '"' + item.getJSONKeyForProperty(key) + '":' : item.getJSONKeyForProperty(key) + ':') + serialized
                separator = ','
            }
            out += separator === '{' ? '{}' : '}'
        }
        return out + ']'
    }

    isEqualWithModel(model){
        if(typeof this !== typeof model || this.count !== model.count) return false
        for(let i = 0; i < this.count; i++){
            let item1 = this.$rows[i].item, item2 = model.$rows[i].item
            let list2 = [...model.getProperties(item2)]
            for(let key of this.getProperties(item1)){
                if(list2.indexOf(key) < 0) return false
                if(typeof item1[key] !== typeof item2[key]) return false
                if(typeof item1[key] === 'object'){
                    if(!item1[key].isEqualWithModel(item2[key])) return false
                } else if(item1[key] !== item2[key]){
                    return false
                }
            }
        }
        return true
    }

    copyMe(){
        let result = new BaseModel(null)
        for(let i = 0; i < this.count; i++) result.addElement(this.$rows[i].item.copyMe())
        return result
    }

    copyFrom(source){
        for(let i = 0; i < source.count; i++) this.addElement(source.$rows[i].item.copyMe())
        return true
    }

    createFromJson(json){ return this.fromJSON(json) }
    fromJSON(json){ return this.fromObject(JSON.parse(json)) }

    fromObject(sourceObject){
        this.clear()
        for(let data of sourceObject){
            if(!data.__typename) continue
            let obj = Qt.createComponent(data.__typename + '.qml').createObject(this)
            obj.fromObject(data)
            this.addElement(obj)
        }
        this.finished()
    }

    $ownerChanged(){
        let owner = this.owner
        if(owner){
            if(owner._internal && owner._internal.isTransaction) owner._internal.countChanges++
            else owner.modelChanged([])
        }
    }

    addElement(element){
        element.owner = this.owner
        this.append({ item: element })
        this.$ownerChanged()
    }

    removeElement(index){
        this.remove(index)
        this.$ownerChanged()
    }

    insertElement(index, element){
        element.owner = this.owner
        this.insert(index, { item: element })
        this.$ownerChanged()
    }

    getItemsCount(){ return this.count }

    containsKey(key, index){
        if(index < 0 || index >= this.count) return false
        return this.$rows[index].item[key] != undefined
    }

    getData(key, index){ return this.$rows[index].item[key] }

    setProperty(index, propName, value){
        let item = this.$rows[index].item
        if(item[propName] !== value) item[propName] = value
    }

    swapItems(index1, index2){
        if(index1 < 0 || index1 >= this.count || index2 < 0 || index2 >= this.count) return false
        let item1 = this.get(index1).item.copyMe()
        let item2 = this.get(index2).item.copyMe()
        item1.owner = this.owner
        item2.owner = this.owner
        this.get(index1).item = item2
        this.get(index2).item = item1
        return true
    }
}

defineType(BaseModel, {
    props: {
        dynamicRoles: { kind: 'bool', initial: true },
        owner: { kind: 'var', initial: null },
    },
    signals: ['internalModelChanged(name,sender)', 'finished'],
    handlers: {
        ownerChanged(model){
            for(let i = 0; i < model.count; i++) model.$rows[i].item.owner = model.owner
        },
    },
})

module.exports = { BaseModel }
