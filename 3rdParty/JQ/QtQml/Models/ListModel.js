const QtObject = require("../../QtQml/QtObject")
const AbstractItemModel = require("./AbstractItemModel")
const Real = require("../../QtQml/Real")
const Bool = require("../../QtQml/Bool")
const List = require("../../QtQml/List")
const Signal = require("../../QtQml/Signal")
const JQApplication = require("../../core/JQApplication")


class ListModel extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        count: {type:Real, value:0, signalName:'countChanged'},
        dynamicRoles: {type:Bool, value:false, signalName:'dynamicRolesChanged'},
        data: {type:List, signalName:'dataChanged'},

        countChanged: {type:Signal, slotName:'onCountChanged', args:[]},
        dynamicRolesChanged: {type:Signal, slotName:'onDynamicRolesChanged', args:[]},
        dataChanged: {type:Signal, slotName:'onDataChanged', args:['topLeft', 'bottomRight', 'roles']},
    })

    __views = []
    __changeSet = []

    __addViewListener(obj){
        if(this.__views.indexOf(obj) < 0) this.__views.push(obj)
    }

    __removeViewListener(obj){
        let index = this.__views.indexOf(obj)
        if(index >= 0) this.__views.splice(index, 1)
    }

    __beginUpdate(){
        this.dataChanged.blockSignal(true)
    }

    __endUpdate(){
        this.dataChanged.blockSignal(false)

        let changeSet = this.__changeSet
        this.__changeSet = []

        // this.count = this.data.length
        // while(this.__signals.length){
        //     this.__signals.shift()()
        // }

        for(let obj of this.__views){
            obj.__updateView(changeSet)
        } 

        this.dataChanged()
    }

    append(dict){
        JQApplication.updateLater(this)

        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

            this.__changeSet.push([this.data.length, this.data.length+dict.length, 'append'])
            for(let i = 0; i < dict.length; i++){
                // dict[i].index = this.data.length
                this.data.push(AbstractItemModel.create(this, this.data.length, dict[i]))
            }
		} else {
            this.__changeSet.push([this.data.length, this.data.length+1, 'append'])
            // dict.index = this.data.length
            this.data.push(AbstractItemModel.create(this, this.data.length, dict))
		}

        this.count = this.data.length
    }
    insert(index, dict){
        JQApplication.updateLater(this)

        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

            this.__changeSet.push([index, index+dict.length, 'insert'])
            for(let i = 0; i < dict.length; i++){
                // dict[i].index = index
                this.data.splice(i+index, 0, AbstractItemModel.create(this, i+index, dict[i]))
            }
		} else {
            this.__changeSet.push([index, index+1, 'insert'])
            // dict.index = index
            this.data.splice(index, 0, AbstractItemModel.create(this, index, dict))
		}

        this.count = this.data.length
    }
    remove(index, count = 1){
        JQApplication.updateLater(this)

        this.__changeSet.push([index, index+count, 'remove'])
        this.data.splice(index, count)

        this.count = this.data.length
    }
    get(index){
        return this.data[index]
    }
    set(){
        
    }
    clear(){
        JQApplication.updateLater(this)

        if(this.data.length) {
            this.__changeSet.push([0, this.data.length, 'remove'])
            this.data = []
        }

        this.count = this.data.length
    }

    setProperty(index, property, value){
        this.data[index][property] = value
    }

    onDataChanged(){
        
    }

    __destroy(){
        super.__destroy()
    }
}

module.exports = ListModel