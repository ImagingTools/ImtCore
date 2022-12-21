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
    append(dict){
        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return
			Array.prototype.push.apply(this.data, dict)
		} else {
			this.data.push(dict)
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
			Array.prototype.splice.apply(this.data, index, 0, dict)
		} else {
			this.data.splice(index, 0, dict)
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

    // $destroy(){
    //     this.clear()
    //     this.$data = []
    //     this.$deps = {}
    //     super.$destroy()
    // }
}

QML.ListModel = ListModel