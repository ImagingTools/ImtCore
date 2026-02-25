const QtObject = require("../../QtQml/QtObject")
const ListElement = require("./ListElement")
const AbstractItemModel = require("./AbstractItemModel")
const Real = require("../../QtQml/Real")
const Bool = require("../../QtQml/Bool")
const List = require("../../QtQml/List")
const Signal = require("../../QtQml/Signal")
const Repeater = require("../../QtQuick/Repeater")
const JQApplication = require("../../core/JQApplication")


class ListModel extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        count: {type:Real, value:0, },
        dynamicRoles: {type:Bool, value:false, },
        data: {type:List, },

        countChanged: {type:Signal, args:[]},
        dynamicRolesChanged: {type:Signal, args:[]},
        dataChanged: {type:Signal, args:['topLeft', 'bottomRight', 'roles']},
    })

    __views = []
    __changeSet = []

    SLOT_JQAbstractModelChanged(oldValue, newValue){
        // console.log('SLOT_JQAbstractModelChanged')
    }

    SLOT_JQAbstractModelDataChanged(oldValue, newValue){
        // console.log('SLOT_JQAbstractModelDataChanged')
    }

    __addViewListener(obj){
        if(this.__views.indexOf(obj) < 0) this.__views.push(obj)
    }

    __removeViewListener(obj){
        let index = this.__views.indexOf(obj)
        if(index >= 0) this.__views.splice(index, 1)
    }

    __beginUpdate(){
        // this.dataChanged.blockSignal(true)
        super.__beginUpdate()
    }

    __endUpdate(){
        if(this.signalsBlocked()) return
        // this.dataChanged.blockSignal(false)

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

        for(let obj of this.__views){
            if(!(obj instanceof Repeater)) 
                obj.__updateView(changeSet)
        } 

        this.__proxy.dataChanged()
        super.__endUpdate()
    }

    __updateRepeaters(changeSet){
        for(let obj of this.__views){
            if(obj instanceof Repeater) obj.__updateView(changeSet)
        } 
    }

    __removeChild(child){
        // let index = -1

        // index = this.resources.indexOf(child)
        // if(index >= 0) this.resources.__splice(index, 1)
    }

    __addChild(child){
        let index = -1

        if(child instanceof ListElement){
            index = this.data.indexOf(child)

            JQApplication.updateLater(this)
            if(index < 0) {
                this.__changeSet.push([this.data.length, this.data.length + 1, 'append'])
                this.data.__push(child)
            }
            
            
        }
    }

    append(dict){
        JQApplication.updateLater(this)

        let changeSet

        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

            changeSet = [this.data.length, this.data.length+dict.length, 'append']
            this.__changeSet.push(changeSet)
            for(let i = 0; i < dict.length; i++){
                this.data.__push(AbstractItemModel.create(this, this.data.length, dict[i]))
            }
            
		} else {
            changeSet = [this.data.length, this.data.length+1, 'append']
            this.__changeSet.push(changeSet)
            this.data.__push(AbstractItemModel.create(this, this.data.length, dict))
		}

        this.count = this.data.length

        this.__updateRepeaters([changeSet])
    }
    insert(index, dict){
        JQApplication.updateLater(this)

        let changeSet

        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

            changeSet = [index, index+dict.length, 'insert']
            this.__changeSet.push(changeSet)
            for(let i = 0; i < dict.length; i++){
                this.data.__splice(i+index, 0, AbstractItemModel.create(this, i+index, dict[i]))
            }
		} else {
            changeSet = [index, index+1, 'insert']
            this.__changeSet.push(changeSet)
            this.data.__splice(index, 0, AbstractItemModel.create(this, index, dict))
		}

        this.count = this.data.length

        this.__updateRepeaters([changeSet])
    }

    __recursiveRemoveLink(obj){
        if(typeof obj === 'object'){
            if(obj instanceof JQModules.QtQml.QObject){
                obj.__removeLink()
            } else {
                for(let key in obj){
                    this.__recursiveRemoveLink(obj[key])
                }
            }
        }
    }

    remove(index, count = 1){
        JQApplication.updateLater(this)

        let changeSet = [index, index+count, 'remove']

        this.__changeSet.push(changeSet)
        let removed = this.data.__splice(index, count)

        for(let r of removed){
            let model = r.__self
            this.__recursiveRemoveLink(model)
            // for(let key in model){
            //     if(model[key] instanceof JQModules.QtQml.QObject){
            //         model[key].__removeLink()
            //     }
            // }
        }

        this.count = this.data.length

        this.__updateRepeaters([changeSet])
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

    SLOT_dataChanged(...args){

    }

    __propogate(){
        
    }

    __destroy(){
        this.clear()
        super.__destroy()
    }
}



module.exports = ListModel