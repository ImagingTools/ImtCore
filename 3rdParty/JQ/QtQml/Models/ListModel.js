const QtObject = require("../../QtQml/QtObject")
const Var = require("../../QtQml/Var")
const Real = require("../../QtQml/Real")
const Signal = require("../../QtQml/Signal")

class ListModel extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        count: {type:Real, value:0, signalName:'countChanged'},
        data: {type:Var, signalName:'dataChanged'},

        countChanged: {type:Signal, slotName:'onCountChanged', args:[]},
        dataChanged: {type:Signal, slotName:'onDataChanged', args:[]},
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