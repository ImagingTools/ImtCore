const { QtObject } = require('./QtObject')
const { ListElement } = require('./ListElement')
const { QReal, QVar } = require('../utils/properties')

class ListModel extends QtObject {
    static defaultProperties = {
        count: { type: QReal, value: 0, changed: '$countChanged' },
        data: { type: QVar, value: undefined, changed: '$dataChanged' },
    }

    static defaultSignals = {
        
    }

    constructor(parent){
        super(parent)
        this.getProperty('data').reset([])
    }

    $complete(){
        super.$complete()
        this.getStatement('count').reset(this.getStatement('data').get().length)
    }

    $countChanged(){

    }

    $dataChanged(){

    }

    addChild(child){
        super.addChild(child)
        child.getStatement('index').reset(this.getStatement('data').get().length)
        this.getStatement('data').get().push(child)
    }
    
    append(dict){
        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

            for(let i = 0; i < dict.length; i++){
                let listElement = new ListElement(this, false)
                // listElement.getStatement('index').reset(i)
                for(let key in dict[i]){
                    listElement.createProperty(key, QVar, dict[i][key])
                    // listElement.getStatement(key).reset(dict[i][key])
                }
                // this.getStatement('data').get().push(listElement)
            }
		} else {
            let keys = Object.keys(dict)
            let listElement = new ListElement(this, false)
            // listElement.getStatement('index').reset(this.getStatement('data').get().length)
            for(let key of keys){
                listElement.createProperty(key, QVar, dict[key])
                // listElement.getStatement(key).reset(dict[key])
            }
            // this.getStatement('data').get().push(listElement)
		}
        
        this.getStatement('count').reset(this.getStatement('data').get().length)
        this.getStatement('data').getNotify()()
        // for(let key in this.$deps){
        //     this.$deps[key].$append(this.getStatement('data').get()[this.getStatement('data').get().length])
        // }
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
                let listElement = new ListElement(this, false)
                listElement.getStatement('index').reset(i)
                for(let key in dict[i]){
                    listElement.createProperty(key, QVar, dict[i][key])
                    // listElement.getStatement(key).reset(dict[i][key])
                }
                this.getStatement('data').get().splice(index + i, 0, listElement)
            }
		} else {
            let keys = Object.keys(dict)
            let listElement = new ListElement(this, false)
            listElement.getStatement('index').reset(this.getStatement('data').get().length)
            for(let key of keys){
                listElement.createProperty(key, QVar, dict[key])
                // listElement.getStatement(key).reset(dict[key])
            }
            this.getStatement('data').get().splice(index, 0, listElement)
		}

    
        this.getStatement('count').reset(this.getStatement('data').get().length)
        this.getStatement('data').getNotify()()
        // for(let key in this.$deps){
        //     this.$deps[key].$insert(index)
        // }
        
        // this.$notify()
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
        this.getStatement('data').getNotify()()
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