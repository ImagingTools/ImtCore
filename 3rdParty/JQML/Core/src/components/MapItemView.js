import {QtObject} from './QtObject'
import {transform} from 'ol/proj'

export class MapItemView extends QtObject  {
    constructor(args) {
        super(args)
        
        this.$cP('add', null)
        this.$cP('autoFitViewport', null)
        this.$cP('delegate', null, this.$delegateChanged)
        this.$cP('model', null, this.$modelChanged)
        this.$cP('remove', null)
    }

    $modelChanged(){
        if(this.$model && typeof this.$model === 'object' && this.$model.$deps && this.$model.$deps[this.UID]) delete this.$model.$deps[this.UID]
        this.$model = this.model
        // for(let child of this.children){
        //     child.$destroy()
        // }
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
            // if(!this.model){
                while(this.children.length){
                    let child = this.children.pop()
                    if(child) child.$destroy()
                }
                this.children = []
            // }

            if(typeof this.model === 'number'){
                for(let i = 0; i < this.model; i++){ 
                    let obj = this.$createElement(i)
                    obj.$uP()
                    if(this.parent.mapReady){
                        obj.$updateFeature(true)
                    }
                }
                
            } else if (typeof this.model === 'object'){
                if(this.model.$deps) {
                    this.model.$deps[this.UID] = this
                    for(let i = 0; i < this.model.data.length; i++){
                        let obj = this.$createElement(i)
                        obj.$uP()
                        if(this.parent.mapReady){
                            obj.$updateFeature(true)
                        }
                    }
                } else {
                    for(let i = 0; i < this.model.length; i++){
                        let obj = this.$createElement(i)
                        obj.$uP()
                        if(this.parent.mapReady){
                            obj.$updateFeature(true)
                        }
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