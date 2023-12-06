const { QtObject } = require('./QtObject')
const { ListElement } = require('./ListElement')
const { QReal, QData, QModelData } = require('../utils/properties')

class ListModel extends QtObject {
    static defaultProperties = {
        count: { type: QReal, value: 0, changed: '$countChanged' },
        data: { type: QData, value: undefined, changed: '$dataChanged' },
    }

    static defaultSignals = {
        
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.getProperty('data').reset([])
    }

    $complete(){
        super.$complete()
        this.getStatement('count').reset(this.getStatement('data').get().length)
    }

    $countChanged(){

    }

    $dataChanged(topLeft, bottomRight, roles){
        // console.log('DEBUG:::dataChanged', topLeft, bottomRight, roles)
    }

    addResource(resource){
        super.addResource(resource)
        resource.getStatement('index').reset(this.getStatement('data').get().length)
        let obj = resource
        
        if(resource instanceof ListElement)
        this.getStatement('data').get().push(obj)
    }
    
    append(dict){
        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

      
            for(let i = 0; i < dict.length; i++){
                this.getStatement('data').get().push(new QModelData(dict[i], this.getStatement('data').get().length))
            }
		} else {
            this.getStatement('data').get().push(new QModelData(dict, this.getStatement('data').get().length))
		}
        
        this.getStatement('count').reset(this.getStatement('data').get().length)
        this.getStatement('data').getNotify()(this.getStatement('data').get().length-1, this.getStatement('data').get().length, 'append')

    }
    clear(){
        this.getStatement('data').reset([])
        
        // for(let key in this.$deps){
        //     this.$deps[key].$clear()
        //     this.$deps[key].$updateView()
        // }
        this.getStatement('count').reset(0)
        this.getStatement('data').getNotify()()
    }
    get(index){
        return index >= 0 && index < this.getStatement('data').get().length ? this.getStatement('data').get()[index] : undefined
    }
    insert(index, dict){
        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

      
            for(let i = 0; i < dict.length; i++){
                this.getStatement('data').get().splice(index, 0, new QModelData(dict[i], this.getStatement('data').get().length))
            }
		} else {
            this.getStatement('data').get().splice(index, 0, new QModelData(dict, this.getStatement('data').get().length))
		}
        
        this.getStatement('count').reset(this.getStatement('data').get().length)
        this.getStatement('data').getNotify()(index, index+1, 'insert')
    }
    set(index, dict){
        this.getStatement('data').get()[index] = dict
        this.getStatement('count').reset(this.getStatement('data').get().length)
        // this.$notify()
        // this.dataChanged(index)
    }
    move(from, to, n){

    }
    remove(index, count = 1){
        this.getStatement('data').get().splice(index, count)
        this.getStatement('count').reset(this.getStatement('data').get().length)
        
        // for(let key in this.$deps){
        //     this.$deps[key].$remove(index, count)
        // }
        // this.dataChanged(index, index+count)
        this.getStatement('data').getNotify()(index, count, 'remove')
    }
    setProperty(index, property, value){
        this.getStatement('data').get()[index][property] = value
        this.getStatement('count').reset(this.getStatement('data').get().length)

        // this.dataChanged(index)
        // this.$notify()
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
        let retVal = ''
        if(this.count > 1){
            retVal += '{'
        } else {
            retVal += '['
        }

        for (var i = 0; i < this.count; i++){
            var modelObject = this.get(i)
            if (i > 0) retVal += ","
            if (this.count > 1) retVal += "{"

            var j = 0;
            for (var property in modelObject) {
                if (j > 0)
                    retVal += ","
                j++;
                retVal += "\"" + property + "\":"
                var modelVal = modelObject[property]
                if (modelVal === null)
                    modelVal += "null"
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
            if (this.count > 1) retVal += "}"
        }

        if(this.count > 1){
            retVal += '}'
        } else {
            retVal += ']'
        }
        return retVal
    }
    
}

module.exports.ListModel = ListModel