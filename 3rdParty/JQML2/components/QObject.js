const { ComplexObject } = require('../utils/base')
const { QVar, QReal, QModelData } = require('../utils/properties')
const { QSignal } = require('../utils/signal')
const { Qt } = require('../utils/Qt')

var UID = 0
class QObject extends ComplexObject {
    static defaultProperties = {
        model: { type: QVar, value: undefined, signalWithout: true },
        index: { type: QReal, value: 0 },
        context: { type: QVar, value: undefined },
    }

    static defaultSignals = {
        'Component.completed': { params: [] },
        'Component.destruction': { params: [] },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.$children = []
        this.$resources = []
        this.$data = []
        
        if(parent) {
            this.UID = UID++
            UIDList[this.UID] = this
            
            // console.log(parent.constructor.name)
            // if(parent.$flickable && this.constructor.name !== 'Component') {
            //     if(parent.getStatement('contentItem').get()){
            //         parent = parent.getStatement('contentItem').get()
            //     }
                
            // }

            if(parent.$flickable && !(this instanceof Component)){
                if(parent.getStatement('contentItem').get()){
                    parent = parent.getStatement('contentItem').get()
                }
            } else if(parent.$repeater && !(this instanceof Component)){
                parent = parent.parent
            }
            if(!(this instanceof MapItemView) && !(this instanceof Repeater) && !(this instanceof ListView) && !(this instanceof GridView) && !(this instanceof ListElement)) this.getStatement('model').setCompute(()=>{return this.parent.model})

            if(!(this instanceof ListElement)) {
                this.getStatement('index').setCompute(()=>{return this.parent.index})
                this.getStatement('context').setCompute(()=>{return this.parent.context})
            }

            if(exModel){
                this.getStatement('model').reset(exModel)
                this.getStatement('index').setCompute(()=>{return exModel.index})
                this.getStatement('index').update()
            }
            
            

            if(this instanceof Item){
                parent.addChild(this)
            } else {
                parent.addResource(this)
            }
            parent.addData(this)
            
            this.setParent(parent)

            
            // this.getStatement('model').update()
        }

        // if(this.constructor.name !== 'Repeater' && this.constructor.name !== 'ListView' && this.constructor.name !== 'GridView'){
        //     this.createProperty(model)
        //     if(this.constructor.name !== 'ListElement'){

        //     }
        // }

    }

    $complete(){
        if(this.$signals['Component.completed']) this.$signals['Component.completed']()
        for(let i = this.$data.length - 1; i >= 0; i--){
            this.$data[i].$complete()
        }
    }

    children(){
        if(!this.$children) this.$children = []

        return this.$children
    }

    setParent(parent){
        this.parent = parent
    }

    addChild(child){
        this.children().push(child)
    }

    addResource(resource){
        this.$resources.push(resource)
    }

    addData(data){
        this.$data.push(data)
    }

    createComponent(namespace, path, parent){
        return Qt.createComponent(namespace, path, parent)
    }

    $destroy(){
        delete UIDList[this.UID]

        if(this.$signals['Component.destruction']) this.$signals['Component.destruction']()

        if(this.parent) {
            let index = this.parent.$children.indexOf(this)
            if(index >= 0) this.parent.$children.splice(index, 1)

            index = this.parent.$resources.indexOf(this)
            if(index >= 0) this.parent.$resources.splice(index, 1)

            index = this.parent.$data.indexOf(this)
            if(index >= 0) this.parent.$data.splice(index, 1)
        }
        for(let i = this.$data.length - 1; i >= 0; i--){
            this.$data[i].$destroy()
        }
        
        for(let key in this){
            if(!(key in this.$properties) && !(key in this.$signals) && this[key] && this[key].clearDependsSignal){
                this[key].clearDependsSignal()
            }
        }
        for(let propName in this.$properties){
            if(this.$properties[propName].unsubscribe) this.$properties[propName].unsubscribe()
            if(this.$properties[propName].notify) this.$properties[propName].notify.destroy()
        }
        for(let sigName in this.$signals){
            this.$signals[sigName].destroy()
        }

        
        // this.$dom.remove()
        for(let key in this){
            // if(key === '$dom') this[key].remove()
            // if(this[key].destroy) this[key].destroy()

            delete this[key]
        }
        
        
    }
}

module.exports.QObject = QObject