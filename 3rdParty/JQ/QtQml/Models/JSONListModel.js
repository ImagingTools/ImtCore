const ListModel = require("./ListModel")
const String = require("../../QtQml/String")
const Signal = require("../../QtQml/Signal")

class JSONListModel extends ListModel {
    static meta = Object.assign({}, ListModel.meta, {
        inquiry: {type:String, value:'', signalName:'inquiryChanged'},
        json: {type:String, value:'', signalName:'jsonChanged'},
        target: {type:String, value:'', signalName:'targetChanged'},
        state: {type:String, value:'', signalName:'stateChanged'},

        inquiryChanged: {type:Signal, slotName:'onInquiryChanged', args:[]},
        jsonChanged: {type:Signal, slotName:'onJsonChanged', args:[]},
        targetChanged: {type:Signal, slotName:'onTargetChanged', args:[]},
        stateChanged: {type:Signal, slotName:'onStateChanged', args:[]},
    })

    makeJson(){

    }

    sendJson(method){

    }

    updateJSONModel(){

    }

    parseJSONString(jsonString, jsonPathQuery){

    }
}

module.exports = JSONListModel