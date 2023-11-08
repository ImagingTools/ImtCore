const { ComplexObject } = require('../utils/base')
const { QVar, QReal } = require('../utils/properties')
const { QSignal } = require('../utils/signal')
var UID = 0
class QObject extends ComplexObject {
    static defaultProperties = {
        model: { type: QVar, value: undefined },
        index: { type: QReal, value: 0 },
        context: { type: QVar, value: undefined },
    }

    static defaultSignals = {
        completed: { params: [] },
        destroyed: { params: [] },
    }

    constructor(parent){
        super()
        this.$children = []
        
        if(parent) {
            this.UID = UID++
            UIDList[this.UID] = this
            
            // console.log(parent.constructor.name)
            if(parent.$flickable && this.constructor.name !== 'Component') {
                if(parent.getStatement('contentItem').get()){
                    parent = parent.getStatement('contentItem').get()
                }
                
            }

            if(parent.constructor.name !== 'Repeater' && parent.constructor.name !== 'ListView' && parent.constructor.name !== 'GridView') this.getStatement('model').setCompute(()=>{return this.parent().model})
            this.getStatement('index').setCompute(()=>{return this.parent().index})
            this.getStatement('context').setCompute(()=>{return this.parent().context})

            parent.addChild(this)
            this.setParent(parent)

            
            // this.getStatement('model').update()
        }
    }

    $complete(){
        if(this.$signals.completed) this.$signals.completed()
        for(let i = this.$children.length - 1; i >= 0; i--){
            this.$children[i].$complete()
        }
    }

    parent(){
        return this.$parent
    }

    children(){
        if(!this.$children) this.$children = []

        return this.$children
    }

    setParent(parent){
        this.$parent = parent
    }

    addChild(child){
        this.children().push(child)
    }

    $destroy(){
        delete UIDList[this.UID]

        for(let propName in this.$properties){
            if(this.$properties[propName].unsubscribe) this.$properties[propName].unsubscribe()
            if(this.$properties[propName].notify) this.$properties[propName].notify.destroy()
        }
        for(let sigName in this.$signals){
            this.$signals[sigName].destroy()
        }

        if(this.$signals.destroyed) this.$signals.destroyed()
        if(this.$parent) {
            let index = this.$parent.$children.indexOf(this)
            if(index >= 0) this.$parent.$children.splice(index, 1)
        }
        for(let i = this.$children.length - 1; i >= 0; i--){
            this.$children[i].$destroy()
        }
        for(let key in this){
            if(key === '$dom') this[key].remove()
            if(this[key].destroy) this[key].destroy()

            delete this[key]
        }
        
        
    }
}

module.exports.QObject = QObject