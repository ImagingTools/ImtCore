const QtObject = require("../../QtQml/QtObject")
const String = require("../../QtQml/String")
const Bool = require("../../QtQml/Bool")
const Var = require("../../QtQml/Var")
const Int = require("../../QtQml/Int")
const Real = require("../../QtQml/Real")
const List = require("../../QtQml/List")
const Signal = require("../../QtQml/Signal")

class Dialog extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        data: { type: List, signalName: 'dataChanged' },
        flags: { type: Real, value:0, signalName: 'flagsChanged' },
        modality: { type: Real, value:0, signalName: 'modalityChanged' },
        parentWindow: { type: Var, value:undefined, signalName: 'parentWindowChanged' },
        result: { type: Int, value:0, signalName: 'resultChanged' },
        title: { type: String, value:'', signalName: 'titleChanged' },
        visible: { type: Bool, value:false, signalName: 'visibleChanged' },

        dataChanged: {type:Signal, slotName:'onDataChanged', args:[]},
        flagsChanged: {type:Signal, slotName:'onFlagsChanged', args:[]},
        modalityChanged: {type:Signal, slotName:'onModalityChanged', args:[]},
        parentWindowChanged: {type:Signal, slotName:'onParentWindowChanged', args:[]},
        resultChanged: {type:Signal, slotName:'onResultChanged', args:[]},
        titleChanged: {type:Signal, slotName:'onTitleChanged', args:[]},
        visibleChanged: {type:Signal, slotName:'onVisibleChanged', args:[]},

        accepted: {type:Signal, slotName:'onAccepted', args:[]},
        rejected: {type:Signal, slotName:'onRejected', args:[]},
    })

    accept(){}
    close(){}
    done(result){}
    open(){}
    reject(){}
}

module.exports = Dialog