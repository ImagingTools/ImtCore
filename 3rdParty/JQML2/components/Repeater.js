const { Item } = require('./Item')
const { QVar, QReal } = require('../utils/properties')

class Repeater extends Item {
    static defaultProperties = {
        model: { type: QVar, value: undefined, changed: 'modelChanged' },
        delegate: { type: QVar, changed: 'delegateChanged' },
        count: { type: QReal, value: 0 },
    }

    static defaultSignals = {
        itemAdded: { params: ['index', 'item'] },
        itemRemoved: { params: ['index', 'item'] },
    }
    constructor(parent){
        super(parent)
        
        this.setStyle({
            display: 'none'
        })

        this.$items = []
    }

    modelChanged(){
        this.updateView()
    }

    delegateChanged(){
        this.updateView()
    }

    updateView(){
        if(!this.getPropertyValue('delegate') || this.getPropertyValue('model') === undefined || this.getPropertyValue('model') === null) return

        for(let item of this.$items){
            item.$destroy()
        }
        this.$items = []

        this.parent().preventAutoUpdateGeometry = true
        if(typeof this.getPropertyValue('model') === 'number'){
            for(let i = 0; i < this.getPropertyValue('model'); i++){
                let obj = this.delegate.get().createObject(this.parent(), this.$exCtx)
                obj.getStatement('index').reset(i)
                obj.getStatement('model').reset({index: i})
                for(let update of updateList.splice(0, updateList.length)){
                    update()
                }
                obj.$complete()
                this.$items.push(obj)
            }
        } else {
            for(let model of this.getPropertyValue('model').getPropertyValue('data')){
                let obj = this.delegate.get().createObject(this.parent(), this.$exCtx)
                obj.getStatement('index').setCompute(()=>{obj.getStatement('index').subscribe(model.getStatement('index')); return model.getStatement('index').get()})
                obj.getStatement('index').update()
                obj.getStatement('model').reset(model)
                for(let update of updateList.splice(0, updateList.length)){
                    update()
                }
                obj.$complete()
                this.$items.push(obj)
            }
        }
        delete this.parent().preventAutoUpdateGeometry
        if(this.parent().updateGeometry) this.parent().updateGeometry()
    }

}

module.exports.Repeater = Repeater