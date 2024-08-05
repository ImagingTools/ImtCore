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

        this.count = this.data.length

        for(let obj of this.__views){
            obj.__updateView()
        }
    }

    append(dict){
        JQApplication.updateLater(this)

        if (Array.isArray(dict)) {
			if (dict.length === 0)
				return
            for(let i = 0; i < dict.length; i++){
                this.data.push(dict[i])
            }
		} else {
            this.data.push(dict)
		}
    }
    insert(){

    }
    remove(){

    }
    get(){

    }
    set(){
        
    }
    clear(){

    }
}

module.exports = ListModel