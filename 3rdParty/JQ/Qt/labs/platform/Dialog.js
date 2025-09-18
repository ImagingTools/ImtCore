const QtObject = require("../../../QtQml/QtObject")
const String = require("../../../QtQml/String")
const Bool = require("../../../QtQml/Bool")
const Var = require("../../../QtQml/Var")
const Int = require("../../../QtQml/Int")
const Real = require("../../../QtQml/Real")
const List = require("../../../QtQml/List")
const Signal = require("../../../QtQml/Signal")

class Dialog extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        data: { type: List},
        flags: { type: Real, value:0},
        modality: { type: Real, value:0},
        parentWindow: { type: Var, value:undefined},
        result: { type: Int, value:0},
        title: { type: String, value:''},
        visible: { type: Bool, value:false},

        dataChanged: {type:Signal, args:[]},
        flagsChanged: {type:Signal, args:[]},
        modalityChanged: {type:Signal, args:[]},
        parentWindowChanged: {type:Signal, args:[]},
        resultChanged: {type:Signal, args:[]},
        titleChanged: {type:Signal, args:[]},
        visibleChanged: {type:Signal, args:[]},

        accepted: {type:Signal, args:[]},
        rejected: {type:Signal, args:[]},
    })

    accept(){}
    close(){}
    done(result){}
    open(){}
    reject(){}
}



module.exports = Dialog