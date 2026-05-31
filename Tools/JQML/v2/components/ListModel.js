const { QtObject } = require('./QtObject')
const { ListElement } = require('./ListElement')
const { QReal, QData, QModelData } = require('../utils/properties')

class ListModel extends QtObject {
    static defaultProperties = {
        count: { type: QReal, value: 0, changed: '$countChanged' },
        data: { type: QData, value: undefined, changed: '$dataChanged' },
    }

    static defaultSignals = {
        $transaction: { params: ['sender', 'changeset'] },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.getProperty('data').value = []
    }

    $complete(){
        super.$complete()
        this.getStatement('count').reset(this.getStatement('data').get().length)
    }

    $countChanged(){

    }

    addData(data){
    }

    $dataChanged(topLeft, bottomRight, roles){
        if(roles){
            if(TransactionController.add(this)) {
                let change = this.$changeset[this.$changeset.length-1]
                if(change && change[0] === topLeft && change[1] === bottomRight && change[2] === 'insert' && roles === 'remove'){
                    this.$changeset.pop()
                } else {
                    this.$changeset.push([topLeft, bottomRight, roles])
                }
            } else {
                if(this.$signals.$transaction) this.$signals.$transaction(this, [[topLeft, bottomRight, roles]])
            }
        }
        

        let parent = this.getProperty('parent').get()
        if(parent instanceof ListModel){
            parent.getProperty('data').getNotify()()
        }
        // if(this.parent && this.parent instanceof ListModel) this.parent.getProperty('data').getNotify()()
        // console.log('DEBUG:::dataChanged', topLeft, bottomRight, roles)
    }

    $emitDataChanged(topLeft, bottomRight, roles){
        this.getStatement('data').getNotify()(topLeft, bottomRight, roles)
    }

    addResource(resource){
        super.addResource(resource)
        resource.getStatement('index').reset(this.getStatement('data').get().length)
        let obj = resource
        
        if(resource instanceof ListElement){
            this.getStatement('data').get().push(obj)
            this.getStatement('count').reset(this.getStatement('data').get().length)
        }
    }
    
    append(dict){
        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

      
            for(let i = 0; i < dict.length; i++){
                this.getStatement('data').get().push(new QModelData(this, dict[i], this.getStatement('data').get().length))
            }
		} else {
            this.getStatement('data').get().push(new QModelData(this, dict, this.getStatement('data').get().length))
		}
        
        let countProp = this.getStatement('count')
        let oldCount = countProp.value
        countProp.value = this.getStatement('data').get().length
        this.$emitDataChanged(this.getStatement('data').get().length-1, this.getStatement('data').get().length, 'append')
        if(oldCount !== countProp.value) {
            countProp.subscribersReset()
            if(countProp.notify) countProp.notify()
        }
    }
    clear(){
        let leftTop = 0
        let rightBottom = this.getStatement('data').get().length
        this.getStatement('data').value = []

        let countProp = this.getStatement('count')
        let oldCount = countProp.value
        countProp.value = 0
        this.$emitDataChanged(leftTop, rightBottom, 'remove')
        if(oldCount !== 0) {
            countProp.subscribersReset()
            if(countProp.notify) countProp.notify()
        }
    }
    get(index){
        return index >= 0 && index < this.getStatement('data').get().length ? this.getStatement('data').get()[index] : undefined
    }
    insert(index, dict){
        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

      
            for(let i = 0; i < dict.length; i++){
                this.getStatement('data').get().splice(index + i, 0, new QModelData(this, dict[i], index+i))
            }
		} else {
            this.getStatement('data').get().splice(index, 0, new QModelData(this, dict, index))
		}

        let data = this.getStatement('data').get()
        for(let i = 0; i < data.length; i++){
            data[i].index = i
        }
        
        let insertedCount = Array.isArray(dict) ? dict.length : 1
        let countProp = this.getStatement('count')
        let oldCount = countProp.value
        countProp.value = data.length
        this.$emitDataChanged(index, index + insertedCount, 'insert')
        if(oldCount !== data.length) {
            countProp.subscribersReset()
            if(countProp.notify) countProp.notify()
        }
    }
    set(index, dict){
        let data = this.getStatement('data').get()
        let existing = data[index]
        // If the existing element is a QModelData proxy, update its properties
        // in-place to preserve reactive subscriptions from delegate bindings.
        if (existing && existing.$lock !== undefined) {
            existing.$lock = true
            for (let key in dict) {
                if (key !== '$lock' && key !== 'index') {
                    existing[key] = dict[key]
                }
            }
            existing.$lock = false
        } else {
            data[index] = new QModelData(this, dict, index)
        }
        this.$emitDataChanged(index, index + 1, 'update')
    }
    move(from, to, n = 1){
        let data = this.getStatement('data').get()
        data.splice(to, 0, ...data.splice(from, n))

        for(let i = 0; i < data.length; i++){
            data[i].index = i
        }
        
        this.$emitDataChanged(Math.min(from, to), Math.max(from, to) + n, 'move')
    }
    remove(index, count = 1){
        this.getStatement('data').get().splice(index, count)
        let data = this.getStatement('data').get()
        for(let i = index; i < data.length; i++){
            data[i].index = i
        }
        let countProp = this.getStatement('count')
        let oldCount = countProp.value
        countProp.value = data.length
        
        this.$emitDataChanged(index, index+count, 'remove')
        if(oldCount !== data.length) {
            countProp.subscribersReset()
            if(countProp.notify) countProp.notify()
        }
    }
    setProperty(index, property, value){
        this.getStatement('data').get()[index][property] = value
        this.$emitDataChanged(index, index + 1, 'update')
    }

    $notify(){
        // if(this.$timer) clearTimeout(this.$timer)
        // this.$timer = setTimeout(()=>{
            for(let key in this.$deps){
                this.$deps[key].$updateView()
            }
        // }, 100)
        
    }
    toJSON(){
        let retVal = '['

        for (var i = 0; i < this.count; i++){
            var modelObject = this.get(i)
            if (i > 0) retVal += ","
            retVal += "{"

            var j = 0;
            for (var property in modelObject) {
                if (j > 0)
                    retVal += ","
                j++;
                retVal += "\"" + property + "\":"
                var modelVal = modelObject[property]
                if (modelVal === null)
                    retVal += "null"
                else if(typeof modelVal === 'object' && modelVal instanceof ListModel){
                    retVal += modelVal.toJSON()
                }
                else if(typeof modelVal === 'string' || modelVal instanceof String){
                    retVal += "\"" + modelVal + "\""
                }
                else if(typeof modelVal === 'object' && !(modelVal instanceof QtObject)){
                    retVal += JSON.stringify(modelVal)
                }else
                    retVal += modelVal
            }
            retVal += "}"
        }

        retVal += ']'
        return retVal
    }
    
}

module.exports.ListModel = ListModel