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
        super.__beginUpdate()
    }

    __endUpdate(){
        this.dataChanged.blockSignal(false)

        let changeSet = this.__changeSet
        this.__changeSet = []

        for(let obj of this.__views){
            obj.__updateView(changeSet)
        } 

        this.dataChanged()
        super.__endUpdate()
    }

    append(dict){
        JQApplication.updateLater(this)

        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

            this.__changeSet.push([this.data.length, this.data.length+dict.length, 'append'])
            for(let i = 0; i < dict.length; i++){
                this.data.opush(AbstractItemModel.create(this, this.data.length, dict[i]))
            }
		} else {
            this.__changeSet.push([this.data.length, this.data.length+1, 'append'])
            this.data.opush(AbstractItemModel.create(this, this.data.length, dict))
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
                this.data.osplice(i+index, 0, AbstractItemModel.create(this, i+index, dict[i]))
            }
		} else {
            this.__changeSet.push([index, index+1, 'insert'])
            this.data.osplice(index, 0, AbstractItemModel.create(this, index, dict))
		}

        this.count = this.data.length
    }
    remove(index, count = 1){
        JQApplication.updateLater(this)

        this.__changeSet.push([index, index+count, 'remove'])
        this.data.osplice(index, count)

        this.count = this.data.length
    }
    get(index){
        return this.data[index]
    }
    set(){
        
    }
    clear(){
        this.remove(0, this.count)
    }

    setProperty(index, property, value){
        this.data[index][property] = value
    }

    onDataChanged(){

    }

    __propogate(){
        
    }

    __destroy(){
        this.clear()
        super.__destroy()
    }
}

ListModel.initialize()

module.exports = ListModel