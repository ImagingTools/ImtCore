import {QtObject} from './QtObject'

export class ListModel extends QtObject {
    constructor(parent) {
        super(parent)


        this.$createReadOnlyProperty('count', () => {return this.$data.length})

        this.$data = []
        this.$deps = {}
        this.$timer = null
    }
    $domCreate(){
        super.$domCreate()

    }

    append(dict){
        this.$data.push(dict)

        this.$notify()
    }
    clear(){
        this.$data = []

        this.$notify()
    }
    get(index){
        return index >= 0 && index < this.$data.length ? this.$data[index] : undefined
    }
    insert(index, dict){
        this.$data.splice(index, 0, dict);

        this.$notify()
    }
    set(index, dict){
        this.$data[index] = dict

        this.$notify()
    }
    move(from, to, n){

    }
    remove(index, count = 1){

    }
    setProperty(index, property, value){
        this.$data[index][property] = value

        this.$notify()
    }

    $notify(){
        if(this.$timer) clearTimeout(this.$timer)
        this.$timer = setTimeout(()=>{
            for(let key in this.$deps){
                this.$deps[key].$updateView()
            }
        }, 100)
        
    }

    // $destroy(){
    //     this.clear()
    //     this.$data = []
    //     this.$deps = {}
    //     super.$destroy()
    // }
}

QML.ListModel = ListModel