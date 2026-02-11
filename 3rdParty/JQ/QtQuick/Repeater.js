const Item = require("./Item")
const Component = require("../QtQml/Component")
const Variant = require("../QtQml/Variant")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")
const JQApplication = require("../core/JQApplication")

class Repeater extends Item {
    static meta = Object.assign({}, Item.meta, {
        model: {type: Var, value:undefined, signalName:'modelChanged'},
        delegate: {type:Variant, typeTarget:Component, value:undefined, signalName:'delegateChanged'},
        count: {type: Real, value:0, signalName:'countChanged'},
        
        modelChanged: {type:Signal, slotName:'onModelChanged', args:[]},
        delegateChanged: {type:Signal, slotName:'onDelegateChanged', args:[]},
        countChanged: {type:Signal, slotName:'onCountChanged', args:[]},

        itemAdded: {type:Signal, slotName:'onItemAdded', args:['item']},
        itemRemoved: {type:Signal, slotName:'onItemRemoved', args:['item']},
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__DOM.classList.add('Repeater')

        return obj
    }

    __items = []

    __updatePrimaryProperties(){
        super.__updatePrimaryProperties()
        this.__updateProperty('delegate')
        this.__initView(true)
    }

    __complete(){
        this.__initView(true)
        super.__complete()
    }

    itemAt(index){
        return this.__items[index]
    }

    SLOT_modelChanged(oldValue, newVlaue) {
        this.__clear()

        if (oldValue && typeof oldValue === 'object' && !Array.isArray(oldValue) && !oldValue.__destroyed) {
            oldValue.__removeViewListener(this)
        }

        if (newVlaue && typeof newVlaue === 'object' && !Array.isArray(newVlaue)) {
            newVlaue.__addViewListener(this)
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
            this.__proxy.itemRemoved(r)
            if(r) r.destroy()
        }
    }

    __createItem(index) {
        if(this.__items[index]) return this.__items[index]

        let properties = {}

        if (Array.isArray(this.model)) {
            properties.modelData = this.model[index]
            properties.model = { index: index }
        } else if (typeof this.model === 'object') {
            properties.model = this.model.data[index]
        } else {
            properties.model = { index: index }
        }

        let item = this.delegate.createObject(this.parent, properties, true)

        this.__items[index] = item

        this.__proxy.itemAdded(item)

        return item
    }

    __initView(isCompleted) {
        if (this.delegate && this.model && isCompleted) {
            let length = 0
            if (Array.isArray(this.model)) {
                length = this.model.length
            } else if (typeof this.model === 'object') {
                if(this.model.count === this.count) return
                length = this.model.count
            } else if (typeof this.model === 'number') {
                length = this.model
            } else {
                return
            }

            if (length === 0) return

            let countChanged = false

            if (this.count !== length) {
                countChanged = true
            }

            this.__self.count = length

            JQApplication.beginUpdate()

            for (let i = 0; i < length; i++) {
                this.__createItem(i)
            }

            JQApplication.endUpdate()

            if (countChanged) this.countChanged()
        }
    }

    __updateView(changeSet) {
        if (this.delegate && this.model && this.__completed) {
            let length = 0
            if (Array.isArray(this.model)) {
                length = this.model.length
            } else if (typeof this.model === 'object') {
                length = this.model.count
            } else if (typeof this.model === 'number') {
                length = this.model
            } else {
                return
            }

            // if (length === 0) return

            let countChanged = false

            if (this.count !== length) {
                countChanged = true
            }

            this.__self.count = length

            JQApplication.beginUpdate()

            for (let change of changeSet) {
                let leftTop = change[0]
                let bottomRight = change[1]
                let role = change[2]

                if (role === 'append') {
                    for (let i = leftTop; i < bottomRight; i++) {
                        this.__createItem(i)
                    }
                } else if (role === 'insert') {
                    for (let i = leftTop; i < bottomRight; i++) {
                        this.__items.splice(i, 0, undefined)
                        this.__createItem(i)
                    }
                } else if (role === 'remove') {
                    let removed = this.__items.splice(leftTop, bottomRight - leftTop)
                    for (let r of removed) {
                        this.itemRemoved(r)
                        if(r) r.destroy()
                    }
                }
            }

            JQApplication.endUpdate()

            if (countChanged) this.countChanged()
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



module.exports = Repeater