import { ListElement } from './ListElement'
import {QtObject} from './QtObject'
// import { Signal } from '../utils/Signal'

export class ListModel extends QtObject {
    constructor(args) {
        super(args)
        this.$cP('count', 0).connect(this.$countChanged.bind(this))
        this.$cP('data', [])

        this.$deps = {}
        this.$timer = null

        this.$cS('dataChanged', 'topLeft', 'bottomRight', 'keyRoles').connect(this.$dataChanged.bind(this))
    }
    $domCreate(){
        super.$domCreate()

    }
    $countChanged(){
        for(let i = 0; i < this.data.length; i++){
            this.data[i].index = i
        }
    }
    $dataChanged(topLeft, bottomRight, keyRoles){
        // for(let key in this.$deps){
        //     this.$deps[key].$updateData(topLeft)
        // }
        this.$p.data.signal()
        // console.log('DEBUG::dataChanged', topLeft, bottomRight, keyRoles)
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
                let listElement = new ListElement({})
                listElement.$cP('index', i)
                for(let key in dict[i]){
                    listElement.$cP(key, dict[i][key])
                }
                this.data.push(listElement)
            }
		} else {
            if(dict.$qmlClassName === 'ListElement'){
                dict.$cP('index', this.data.length)
                this.data.push(dict)
            } else {
                let listElement = new ListElement({})
                listElement.$cP('index', this.data.length)
                for(let key in dict){
                    listElement.$cP(key, dict[key])
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
                let listElement = new ListElement({})
                listElement.$cP('index', i)
                for(let key in dict[i]){
                    listElement.$cP(key, dict[i][key])
                }
                this.data.splice(index + i, 0, listElement)
            }
			// Array.prototype.splice.apply(this.data, index, 0, new Proxy(dict, handler))
		} else {
			if(dict.$qmlClassName === 'ListElement'){
                dict.$cP('index', this.data.length)
                this.data.splice(dict)
            } else {
                let listElement = new ListElement({})
                listElement.$cP('index', this.data.length)
                for(let key in dict){
                    listElement.$cP(key, dict[key])
                }
                this.data.splice(listElement)
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
        this.$dataChanged(index)
    }
    move(from, to, n){

    }
    remove(index, count = 1){
        this.data.splice(index, count)
        this.count = this.data.length
        for(let key in this.$deps){
            this.$deps[key].$remove(index, count)
        }
        this.$dataChanged(index, index+count)
    }
    setProperty(index, property, value){
        this.data[index][property] = value
        this.count = this.data.length

        this.$dataChanged(index)
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
        return ''
    }

    // $destroy(){
    //     this.clear()
    //     this.$data = []
    //     this.$deps = {}
    //     super.$destroy()
    // }
}

QML.ListModel = ListModel