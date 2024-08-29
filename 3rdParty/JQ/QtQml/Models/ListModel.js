const QtObject = require("../../QtQml/QtObject")
const Real = require("../../QtQml/Real")
const List = require("../../QtQml/List")
const Signal = require("../../QtQml/Signal")
const JQApplication = require("../../core/JQApplication")

class ModelData {

}

class ListModel extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        count: {type:Real, value:0, signalName:'countChanged'},
        data: {type:List, signalName:'dataChanged'},

        countChanged: {type:Signal, slotName:'onCountChanged', args:[]},
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
        this.blockSignals(true)
    }

    __endUpdate(){
        this.blockSignals(false)

        // this.count = this.data.length

        for(let obj of this.__views){
            obj.__updateView(this.__changeSet)
        }

        this.__changeSet = []
    }

    append(dict){
        JQApplication.updateLater(this)

        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return

            this.__changeSet.push([this.data.length, this.data.length+dict.length, 'append'])
            for(let i = 0; i < dict.length; i++){
                this.data.push(dict[i])
            }
		} else {
            this.__changeSet.push([this.data.length, this.data.length+1, 'append'])
            this.data.push(dict)
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
                this.data.splice(i+index, 0, dict[i])
            }
		} else {
            this.__changeSet.push([index, index+1, 'insert'])
            this.data.splice(index, 0, dict)
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

        this.__changeSet.push([0, this.data.length, 'remove'])
        this.data = []

        this.count = this.data.length
    }

    __destroy(){
        super.__destroy()
    }
}

module.exports = ListModel