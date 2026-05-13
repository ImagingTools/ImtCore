const QtObject = require("../QtQml/QtObject")
const Component = require("../QtQml/Component")
const Variant = require("../QtQml/Variant")
const Var = require("../QtQml/Var")
const Int = require("../QtQml/Int")
const Signal = require("../QtQml/Signal")


class MapItemView extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        add: { type: Var },
        autoFitViewport: { type: Var },
        delegate: { type:Variant, typeTarget:Component, value:undefined },
        model: { type: Var },
        remove: { type: Var },
        count: { type: Int, value: 0 },
   
        addChanged: { type: Signal },
        autoFitViewportChanged: { type: Signal },
        delegateChanged: { type: Signal },
        modelChanged: { type: Signal },
        removeChanged: { type: Signal },
        countChanged: { type: Signal },
    })


    __items = []

    __complete(){
        this.__initView(true)
        super.__complete()
    }

    SLOT_modelChanged(oldValue, newValue){
        this.__clear()
        
        if(oldValue && typeof oldValue === 'object' && !oldValue.__destroyed){
            oldValue.__removeViewListener(this)
        }

        if(newValue && typeof newValue === 'object'){
            newValue.__addViewListener(this)
        }

        this.__initView(this.__completed)
    }

    SLOT_delegateChanged(oldValue, newValue){
        this.__clear()
        this.__initView(this.__completed)
    }

    __clear(){
        let removed = this.__items
        this.__items = []

        for(let r of removed){
            if(r) r.destroy()
        }

        this.count = 0
    }

    __createItem(model){
        let item = this.delegate.createObject(this.parent, {model:model}, true)
        item.__updateFeature(true)

        return item
    }

    __initView(isCompleted){
        if(this.delegate && this.model && isCompleted){
            JQApplication.beginUpdate()
            JQApplication.updateLater(this)

            let countChanged = false

            if(typeof this.model === 'number'){
                if(this.count !== this.model){
                    countChanged = true
                    this.__self.count = this.model
                    this.count = this.model
                }

                for(let i = 0; i < this.model; i++){
                    let item = this.__createItem({index: i})
                    this.__items.push(item)
                }
            } else {
                if(this.count !== this.model.data.length){
                    countChanged = true
                    this.__self.count = this.model.data.length
                    this.count = this.model.data.length
                }

                for(let i = 0; i < this.model.data.length; i++){
                    let item = this.__createItem(this.model.data[i])
                    this.__items.push(item)
                }
            }

            if(countChanged) this.__proxy.countChanged()

            JQApplication.endUpdate()
        }
    }

    __updateView(changeSet){
        if(this.delegate && this.model && this.__completed){
            JQApplication.beginUpdate()
            JQApplication.updateLater(this.parent)

            if(this.model.data.length === this.__items.length) {
                for(let i = 0; i < this.model.data.length; i++){
                    this.__items[i].JQAbstractModel = this.model.data[i]
                }
            } else {
                let countChanged = false

                if(this.count !== this.model.data.length){
                    countChanged = true
                    this.__self.count = this.model.data.length
                }

                for(let change of changeSet){
                    let leftTop = change[0]
                    let bottomRight = change[1]
                    let role = change[2]

                    if(role === 'append'){
                        for(let i = leftTop; i < bottomRight; i++){
                            let item = this.__createItem(this.model.data[i])
                            this.__items[i] = item
                        }
                    } else if(role === 'insert'){
                        for(let i = leftTop; i < bottomRight; i++){
                            let item = this.__createItem(this.model.data[i])
                            this.__items.splice(i, 0, item)
                        }
                    } else if(role === 'remove'){
                        let removed = this.__items.splice(leftTop, bottomRight - leftTop)
                        for(let r of removed){
                            if(r) r.destroy()
                        }
                    }
                }

                if(countChanged) this.__proxy.countChanged()
            }

            JQApplication.endUpdate()
        }
    }

    __destroy(){
        if (this.model && typeof this.model === 'object' && !Array.isArray(this.model) && !this.model.__destroyed) {
            this.model.__removeViewListener(this)
        }
        this.__clear()
        super.__destroy()
    }
}



module.exports = MapItemView