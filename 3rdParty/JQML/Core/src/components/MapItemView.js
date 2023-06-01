import {QtObject} from './QtObject'

export class MapItemView extends QtObject  {
    constructor(args) {
        super(args)
        
        this.$cP('add', null)
        this.$cP('autoFitViewport', null)
        this.$cP('delegate', null).connect(this.$delegateChanged.bind(this))
        this.$cP('model', null).connect(this.$modelChanged.bind(this))
        this.$cP('remove', null)
    }

    $modelChanged(){
        if(this.$model && typeof this.$model === 'object' && this.$model.$deps && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
        this.$model = this.model

        this.$updateView()
    }

    $delegateChanged(){
        this.$updateView()
    }

    $append(){}
    $insert(){}
    $remove(){}

    $recursiveIndex(obj, index){
        if(obj.$qmlClassName !== 'ListElement'){
            obj.$cP('index', index)
            obj.index = index
        }

        for(let child of obj.children){
            if(!child.$useModel && !child.$repeater && child.$qmlClassName !== 'ListElement')
            this.$recursiveIndex(child, index)
        }
    }

    $createElement(index){
        let obj = this.delegate.createObject ? this.delegate.createObject({parent: this.parent, index: index}) : this.delegate({parent: this.parent, index: index})
        obj.$mapView = this
        this.children.push(obj)
        this.$recursiveIndex(obj, index)
        return obj
    }

    $updateView(){
        if(this.model && this.delegate){
            if(!this.model){
                while(this.children.length){
                    let child = this.children.pop()
                    if(child) child.$destroy()
                }
                this.children = []
            }

            if(typeof this.model === 'number'){
                for(let i = 0; i < this.model; i++){ 
                    this.$createElement(i).$uP()
                }
                
            } else if (typeof this.model === 'object'){
                if(this.model.$deps) {
                    this.model.$deps[this.UID] = this
                    for(let i = 0; i < this.model.data.length; i++){
                        this.$createElement(i).$uP()
                    }
                } else {
                    for(let i = 0; i < this.model.length; i++){
                        this.$createElement(i).$uP()
                    }
                }
            }
        }
    }

    $destroy(){
        super.$destroy()
    }

}

QML.MapItemView = MapItemView