import {QtObject} from './QtObject'

export class ListModel extends QtObject {
    constructor(parent) {
        super(parent)


        this.$cP('count', 0)

        this.$data = []
        this.$deps = {}
        this.$timer = null
    }
    $domCreate(){
        super.$domCreate()

    }

    append(dict){
        // if(Array.isArray(dict)){
        //     this.$data = dict
        // } else {
        //     this.$data.push(dict)
        // }
        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return
			Array.prototype.push.apply(this.$data, dict)
		} else {
			this.$data.push(dict)
		}
        this.count = this.$data.length
        this.$notify()
    }
    clear(){
        this.$data = []
        this.count = 0
        this.$notify()
    }
    get(index){
        return index >= 0 && index < this.$data.length ? this.$data[index] : undefined
    }
    insert(index, dict){
        this.$data.splice(index, 0, dict);
        this.count = this.$data.length
        this.$notify()
    }
    set(index, dict){
        this.$data[index] = dict
        this.count = this.$data.length
        this.$notify()
    }
    move(from, to, n){

    }
    remove(index, count = 1){
        this.$data.splice(index, count)
        this.count = this.$data.length
        this.$notify()
    }
    setProperty(index, property, value){
        this.$data[index][property] = value
        this.count = this.$data.length
        this.$notify()
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