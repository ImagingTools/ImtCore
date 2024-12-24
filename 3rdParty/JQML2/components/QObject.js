const { ComplexObject } = require('../utils/base')
const { QVar, QReal, QList } = require('../utils/properties')
const { QSignal } = require('../utils/signal')
const { Qt } = require('../utils/Qt')

var UID = 0
class QObject extends ComplexObject {
    static defaultProperties = {
        model: { type: QVar, value: undefined, signalWithout: true },
        model_: { type: QVar, value: undefined, signalWithout: true },

        modelData: { type: QVar, value: undefined, signalWithout: true },
        modelData_: { type: QVar, value: undefined, signalWithout: true },
        parentModelData_: { type: QVar, value: undefined, signalWithout: true },

        index: { type: QReal, value: 0 },
        context: { type: QVar, value: undefined },
        parent: { type: QVar, value: undefined, changed: '$parentChanged' },
        children: { type: QList, changed: '$childrenChanged' },
        resources: { type: QList, changed: '$resourcesChanged' },
        data: { type: QList, changed: '$dataChanged' },
    }

    static defaultSignals = {
        'Component.completed': { params: [] },
        'Component.destruction': { params: [] },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        // this.$children = []
        // this.$resources = []
        // this.$data = []

        // this.getProperty('children').reset([])
        // this.getProperty('resources').reset([])
        // this.getProperty('data').reset([])
        
        if(parent) {
            this.UID = UID++
            UIDList[this.UID] = this
            
            Object.defineProperty(this, 'modelData', {
                get:()=>{
                    let property = this.getPropertyValue('parentModelData_').getProperty('modelData_')
                    let caller = global.queueLink[global.queueLink.length-1]
                    if(caller) caller.subscribe(property)

                    return property.get()
                },
                // set:(val)=>{
                //     this.getPropertyValue('parentModelData_').getProperty('modelData_').reset(val)
                // },
                configurable: true,
            })

            if(parent.$flickable && !(this instanceof Component)){
                if(parent.getStatement('contentItem').get()){
                    parent = parent.getStatement('contentItem').get()
                }
            } else if(parent.$repeater && !(this instanceof Component)){
                parent = parent.getProperty('parent').get()
            }

            this.getStatement('model_').setCompute(()=>{return this.parent.model_})
            this.getStatement('parentModelData_').setCompute(()=>{return this.parent.parentModelData_ })

            if(!(this instanceof MapItemView) && !(this instanceof Repeater) && !(this instanceof ListView) && !(this instanceof GridView) && !(this instanceof ListElement)) {
                this.getStatement('model').setCompute(()=>{return this.parent.model_})
            }

            if(!(this instanceof ListElement)) {
                this.getStatement('index').setCompute(()=>{return this.parent.index})
                if(!(this instanceof Shortcut)) this.getStatement('context').reset(mainRoot)
            }

            if(exModel){
                if('$modelData' in exModel){
                    this.getStatement('modelData_').reset(exModel['$modelData'])
                } else {
                    let keys = Object.keys(exModel)
                    if(keys.length === 1){
                        this.getStatement('modelData_').reset(exModel[keys[0]])
                    }
                }

                this.getStatement('parentModelData_').reset(this)

                this.getStatement('model').reset(exModel)
                this.getStatement('model_').reset(exModel)
                this.getStatement('index').setCompute(()=>{return exModel.index})
                this.getStatement('index').update()
            }
            
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
        if(this.$completed) return

        this.$completed = true
        if(this.$signals['Component.completed']) this.$signals['Component.completed']()
        let data = this.getProperty('data').get()
        for(let i = data.length - 1; i >= 0; i--){
            if(data[i] instanceof QObject) data[i].$complete()
        }
    }

    $childrenChanged(topLeft, bottomRight, roles){
        if(roles === 'append'){
            for(let index = topLeft; index < bottomRight; index++){
                this.getProperty('children').get()[index].getProperty('parent').reset(this)
            }
        }  
    }

    $resourcesChanged(topLeft, bottomRight, roles){
        if(roles === 'append'){
            for(let index = topLeft; index < bottomRight; index++){
                this.getProperty('resources').get()[index].getProperty('parent').reset(this)
            }
        }
    }

    $dataChanged(topLeft, bottomRight, roles){
        if(roles === 'append'){
            for(let index = topLeft; index < bottomRight; index++){
                this.getProperty('data').get()[index].getProperty('parent').reset(this)
            }
        }
    }

    // children(){
    //     if(!this.$children) this.$children = []

    //     return this.$children
    // }

    $parentChanged(){
        if(this.$parent) {
            let index = this.$parent.getProperty('children').get().indexOf(this)
            if(index >= 0) this.$parent.getProperty('children').get().splice(index, 1)
            index = this.$parent.getProperty('resources').get().indexOf(this)
            if(index >= 0) this.$parent.getProperty('resources').get().splice(index, 1)
            index = this.$parent.getProperty('data').get().indexOf(this)
            if(index >= 0) this.$parent.getProperty('data').get().splice(index, 1)
        }
        this.$parent = this.getProperty('parent').get()
        if(this instanceof Item){
            this.getProperty('parent').get().addChild(this)
        } else {
            this.getProperty('parent').get().addResource(this)
        }
        this.getProperty('parent').get().addData(this)
    }

    setParent(parent){
        this.getProperty('parent').reset(parent)
    }

    addChild(child){
        if(this.getProperty('children').get().indexOf(child) < 0) this.getProperty('children').get().push(child)
    }

    addResource(resource){
        if(this.getProperty('resources').get().indexOf(resource) < 0) this.getProperty('resources').get().push(resource)
    }

    addData(data){
        if(this.getProperty('data').get().indexOf(data) < 0) this.getProperty('data').get().push(data)
    }

    createComponent(namespace, path, parent){
        return Qt.createComponent(namespace, path, parent)
    }

    destroy(){
        delete UIDList[this.UID]
        delete this.UID

        if(this.$signals['Component.destruction']) this.$signals['Component.destruction']()

        if(this.parent) {
            let index = this.parent.getProperty('children').get().indexOf(this)
            if(index >= 0) this.parent.getProperty('children').get().splice(index, 1)

            index = this.parent.getProperty('resources').get().indexOf(this)
            if(index >= 0) this.parent.getProperty('resources').get().splice(index, 1)

            index = this.parent.getProperty('data').get().indexOf(this)
            if(index >= 0) this.parent.getProperty('data').get().splice(index, 1)
        }

        let data = this.getProperty('data').get()
        for(let i = data.length - 1; i >= 0; i--){
            if(data[i] instanceof QObject) data[i].destroy()
        } 

        TransactionController.add1(this)
        // RemoveList.push(this)
    }

    $free(){
        for(let propName in this.$properties){
            this.$properties[propName].destroy()
        }
        for(let sigName in this.$signals){
            this.$signals[sigName].destroy()
        }

        for(let key in this){
            delete this[key]
        }
    }

}

module.exports.QObject = QObject