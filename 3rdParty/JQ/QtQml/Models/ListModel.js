const QtObject = require("../../QtQml/QtObject")
const Real = require("../../QtQml/Real")
const List = require("../../QtQml/List")
const Signal = require("../../QtQml/Signal")

class ListModel extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        count: {type:Real, value:0, signalName:'countChanged'},
        data: {type:List, signalName:'dataChanged'},

        countChanged: {type:Signal, slotName:'onCountChanged', args:[]},
        dataChanged: {type:Signal, slotName:'onDataChanged', args:['topLeft', 'bottomRight', 'roles']},
    })

    append(){

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