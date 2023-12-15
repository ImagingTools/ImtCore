const { QtObject } = require('./QtObject')
const { QBorder, QReal, QBool, QVar, QColor } = require('../utils/properties')

class MapItemView extends QtObject {
    static defaultProperties = {
        add: { type: QVar, value: undefined },
        autoFitViewport: { type: QVar, value: undefined },
        delegate: { type: QVar, value: undefined, changed: '$delegateChanged' },
        model: { type: QVar, value: undefined, changed: '$modelChanged' },
        remove: { type: QVar, value: undefined },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.$exCtx = exCtx

        this.$items = {
            length: new QInt(0)
        }
        this.$items.length.getNotify().connect(()=>{
            this.updateView()
        })

    }

    $modelDataChanged(leftTop, bottomRight, roles){
        if(roles === 'remove'){
            for(let i = leftTop; i < bottomRight; i++){
                if(this.$items[i]){
                    this.$items[i].$destroy()
                    delete this.$items[i]
                }
            }
        }
        // console.log('DEBUG:::', leftTop, bottomRight, roles)
    }
    $disconnectModel(){
        if(this.$model && this.$model instanceof ListModel && this.$model.UID){
            this.$model.getProperty('data').getNotify().disconnect(this, this.$modelDataChanged)
        }
    }
    $connectModel(model){
        if(model && model instanceof ListModel){
            this.$model = model
            model.getProperty('data').getNotify().connect(this, this.$modelDataChanged)
        }
    }

    $modelChanged(){
        this.$disconnectModel()
        for(let key in this.$items){
            if(key !== 'length') {
                this.$items[key].$destroy()
                delete this.$items[key]
            }
        }
        this.$items.length.reset(0)
        if(typeof this.getPropertyValue('model') === 'number'){     
            this.$items.length.setCompute(()=>{this.$items.length.subscribe(this.getProperty('model')); return this.getPropertyValue('model')})
        } else {
            this.$connectModel(this.getPropertyValue('model'))
            this.$items.length.setCompute(()=>{this.$items.length.subscribe(this.getPropertyValue('model').getProperty('data')); return this.getPropertyValue('model').getPropertyValue('data').length}) 
        }
        this.$items.length.update()
    }

    $visibleChanged(){

    }

    $delegateChanged(){
        for(let key in this.$items){
            if(key !== 'length') {
                this.$items[key].$destroy()
                delete this.$items[key]
            }
        }
        this.updateView()
    }

    createElement(index){
        if(this.$items[index]) return this.$items[index]
        let ctx = new ContextController(this.delegate.get().$exCtx,this.$exCtx)
        let createObject = this.getStatement('delegate').get().createObject
        let cls = this.getStatement('delegate').get().constructor

        if(typeof this.getPropertyValue('model') === 'number'){
            let obj = createObject ? createObject(this.getStatement('contentItem').get(),ctx, {index: index}) : new cls(this.getStatement('contentItem').get(),ctx, {index: index})
            // obj.getStatement('index').reset(index)
            // obj.getStatement('model').reset({index: index})
            this.$items[index] = obj
        } else {
            let model = this.getPropertyValue('model').getPropertyValue('data')[index]
            let obj = createObject ? createObject(this.getStatement('contentItem').get(),ctx, model) : new cls(this.getStatement('contentItem').get(),ctx, model)
            // obj.getStatement('index').setCompute(()=>{return model.index})
            // obj.getStatement('index').update()
            // obj.getStatement('model').reset(model)
            this.$items[index] = obj   
        }
        for(let update of updateList.splice(0, updateList.length)){
            update()
        }
        
        this.$items[index].$complete()
        return this.$items[index]
    }

    updateView(){
        if(!this.getPropertyValue('delegate') || this.getPropertyValue('model') === undefined || this.getPropertyValue('model') === null) return
        

        for(let i = 0; i < this.$items.length.get(); i++){
            this.createElement(i).$updateFeature(true)
        }
       
        // this.getProperty('count').reset(this.$items.length.get())
    }


    $destroy(){
        this.$disconnectModel()
        this.$items.length.unsubscribe()

        for(let key in this.$items){
            if(key !== 'length') {
                if(this.$items[key].UID){
                    this.$items[key].$destroy()
                }
                
                delete this.$items[key]
            }
        }
        
        super.$destroy()
    }
   
}

module.exports.MapItemView = MapItemView