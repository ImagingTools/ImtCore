import { ListElement } from './ListElement'
import {QtObject} from './QtObject'
// import { Signal } from '../utils/Signal'

export class ListModel extends QtObject {
    constructor(args) {
        super(args)
        this.$cP('count', 0, this.$countChanged)
        this.$cP('data', [])

        this.$deps = {}
        this.$timer = null

        // this.$cS('dataChanged', 'topLeft', 'bottomRight', 'keyRoles')
        // this.dataChanged.connect(this.$dataChanged.bind(this))
    }
    $domCreate(){
        super.$domCreate()

    }
    $countChanged(){
        for(let i = 0; i < this.data.length; i++){
            this.data[i].index = i
        }
    }
    // $dataChanged(topLeft, bottomRight, keyRoles){
    //     // for(let key in this.$deps){
    //     //     this.$deps[key].$updateData(topLeft)
    //     // }
    //     this.dataChanged()
    //     // console.log('DEBUG::dataChanged', topLeft, bottomRight, keyRoles)
    // }
    $dataChanged(ignore = []){
        ignore.push(this)
        let parent = this.parent
        if(parent && ignore.indexOf(parent) < 0 && parent.$dataChanged){
            parent.$dataChanged(ignore)
        }
        
        if(this.dataChanged) this.dataChanged()

        for(let child of this.children){
            if(child && ignore.indexOf(child) < 0 && child.$dataChanged){
                child.$dataChanged(ignore)
            }
        }
    }
    $modelChanged(ignore = []){
        ignore.push(this)
        let parent = this.parent
        if(parent && ignore.indexOf(parent) < 0 && parent.$modelChanged){
            parent.$modelChanged(ignore)
        }
        
        if(this.modelChanged) this.modelChanged()

        for(let child of this.children){
            if(child && ignore.indexOf(child) < 0 && child.$modelChanged){
                child.$modelChanged(ignore)
            }
        }
    }
    append(dict){
        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

            for(let i = 0; i < dict.length; i++){
                let listElement = new ListElement({parent:this,appended:true})
                listElement.$cP('index', i)
                for(let key in dict[i]){
                    listElement.$cP(key, dict[i][key])
                    // listElement.$p[key].getSignal().connect(this.$modelChanged.bind(this))
                }
                this.data.push(listElement)
            }
		} else {
            if(dict.$qmlClassName === 'ListElement'){
                dict.$cP('index', this.data.length)
                this.data.push(dict)
            } else {
                let listElement = new ListElement({parent:this,appended:true})
                listElement.$cP('index', this.data.length)
                for(let key in dict){
                    listElement.$cP(key, dict[key])
                    // listElement.$p[key].getSignal().connect(this.$modelChanged.bind(this))
                }
                this.data.push(listElement)
            }
			
		}
        
        this.count = this.data.length

        for(let key in this.$deps){
            this.$deps[key].$append(this.data[this.data.length])
        }
    }
    clear(){
        this.data = []
        
        for(let key in this.$deps){
            this.$deps[key].$clear()
            this.$deps[key].$updateView()
        }
        this.count = 0
    }
    get(index){
        return index >= 0 && index < this.data.length ? this.data[index] : undefined
    }
    insert(index, dict){
        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

            for(let i = 0; i < dict.length; i++){
                let listElement = new ListElement({parent:this,appended:true})
                listElement.$cP('index', i)
                for(let key in dict[i]){
                    listElement.$cP(key, dict[i][key])
                    // listElement.$p[key].getSignal().connect(this.$modelChanged.bind(this))
                }
                this.data.splice(index + i, 0, listElement)
            }
			// Array.prototype.splice.apply(this.data, index, 0, new Proxy(dict, handler))
		} else {
			if(dict.$qmlClassName === 'ListElement'){
                dict.$cP('index', index)
                this.data.splice(index, 0, dict)
            } else {
                let listElement = new ListElement({parent:this,appended:true})
                listElement.$cP('index', index)
                for(let key in dict){
                    listElement.$cP(key, dict[key])
                    // listElement.$p[key].getSignal().connect(this.$modelChanged.bind(this))
                }
                this.data.splice(index, 0, listElement)
            }
		}
        
        this.count = this.data.length
        for(let key in this.$deps){
            this.$deps[key].$insert(index)
        }
        
        // this.$notify()
    }
    set(index, dict){
        this.data[index] = dict
        this.count = this.data.length
        // this.$notify()
        this.dataChanged(index)
    }
    move(from, to, n){

    }
    remove(index, count = 1){
        this.data.splice(index, count)
        this.count = this.data.length
        for(let key in this.$deps){
            this.$deps[key].$remove(index, count)
        }
        this.dataChanged(index, index+count)
    }
    setProperty(index, property, value){
        this.data[index][property] = value
        this.count = this.data.length

        this.dataChanged(index)
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
                else if(typeof modelVal === 'object' && modelVal.$qmlClassName === "ListModel"){
                    retVal += modelVal.toJSON()
                }
                else if(typeof modelVal === 'string' || modelVal instanceof String){
                    retVal += "\"" + modelVal + "\""
                }
                else if(typeof modelVal === 'object' && !modelVal.$qmlClassName){
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

    $destroy(){
        this.clear()
        this.$data = []
        this.$deps = {}
        super.$destroy()
    }
}

QML.ListModel = ListModel