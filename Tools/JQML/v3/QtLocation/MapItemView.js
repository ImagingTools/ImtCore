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


    __items = {}
    __changeSet = []

    __complete(){
        this.__initView(true)
        super.__complete()
    }

    SLOT_modelChanged(oldValue, newValue) {
        this.__clear()

        if (oldValue && typeof oldValue === 'object' && !Array.isArray(oldValue) && !oldValue.__destroyed) {
            oldValue.__removeViewListener(this)
        }

        if (newValue && typeof newValue === 'object' && !Array.isArray(newValue)) {
            newValue.__addViewListener(this)
        }

        this.__initView(true)
    }

    SLOT_delegateChanged() {
        this.__clear()
        this.__initView(this.__completed)
    }

    __clear() {
        this.blockSignals(true)
        this.__changeSet = []

        let removed = this.__items
        this.__items = {}

        for (let r in removed) {
            if (removed[r]) removed[r].destroy()
        }

        this.blockSignals(false)

        this.count = 0
    }

    __createItem(index) {
        let properties = {}

        if (Array.isArray(this.model)) {
            properties.modelData = this.model[index]
            properties.model = { index: index }
        } else if (typeof this.model === 'object') {
            properties.model = this.model.data[index]
        } else {
            properties.model = { index: index }
        }

        let item = null
        item = this.delegate.createObject(this.parent, properties, true)

        this.__items[index] = item
        item.__updateFeature(true)

        return item
    }

    __updateChangedSet(changeSet) {
        this.__changeSet.push(changeSet)
        if (this.model && typeof this.model === 'object') {
            this.count = this.model.count
        }
    }

    __normalizeItemsIndex() {
        let temp = this.__items
        this.__items = {}
        for(let i in temp){
            let index = temp[i].index
            if(index >= 0 && Number.isFinite(index))
            this.__items[index] = temp[i]
        }
    }

    __initView(isCompleted) {
        if (this.delegate && this.model && isCompleted) {
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

            if (length === 0) return

            let countChanged = false

            if (this.count !== length) {
                countChanged = true
            }

            this.__self.count = length

            JQApplication.beginUpdate()
            JQApplication.updateLater(this)

            for (let i = 0; i < length; i++) {
                this.__createItem(i)
            }

            if (countChanged) this.countChanged()

            JQApplication.endUpdate()
        }
    }

    __updateView() {
        if (this.delegate && this.model && this.__completed) {
            this.__updating = true
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

            if (length === 0 && Object.keys(this.__items).length === 0) return

            JQApplication.beginUpdate()
            JQApplication.updateLater(this)

            let countChanged = false

            if (this.count !== length) {
                countChanged = true
            }

            this.__self.count = length

            let changeSet = this.__changeSet
            this.__changeSet = []

            if(changeSet.length > 0){
                let i = 0
                while(i < changeSet.length - 1){
                    if(changeSet[i][0] === changeSet[i+1][0] && changeSet[i][1] === changeSet[i+1][1] && 
                        (changeSet[i][2] === 'append' || changeSet[i][2] === 'insert') && changeSet[i+1][2] === 'remove'){
                            changeSet.splice(i, 2)
                    } else {
                        i++
                    }
                }
            }

            for (let change of changeSet) {
                let leftTop = change[0]
                let bottomRight = change[1]
                let role = change[2]

                if (role === 'append') {
                    for (let i = leftTop; i < bottomRight; i++) {
                        this.__createItem(i)
                    }
                } else if (role === 'insert') {
                    this.__normalizeItemsIndex()
                    
                    for (let i = leftTop; i < bottomRight; i++) {
                        this.__createItem(i)
                    }
                } else if (role === 'remove') {
                    let leftTopItem = this.__items[leftTop]
                    let bottomRightItem = this.__items[bottomRight]

                    for(let i = leftTop; i < bottomRight; i++){
                        if(this.__items[i]){
                           this.__items[i].destroy()
                            delete this.__items[i] 
                        }
                        
                    }

                    this.__normalizeItemsIndex()
                }
            }

            if (countChanged) this.countChanged()

            JQApplication.endUpdate()
            delete this.__updating
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