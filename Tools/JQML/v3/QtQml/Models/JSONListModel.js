const ListModel = require("./ListModel")
const String = require("../../QtQml/String")
const Signal = require("../../QtQml/Signal")

class JSONListModel extends ListModel {
    static meta = Object.assign({}, ListModel.meta, {
        source: {type:String, value:'', },
        inquiry: {type:String, value:'', },
        json: {type:String, value:'', },
        target: {type:String, value:'', },
        state: {type:String, value:'', },

        sourceChanged: {type:Signal, args:[]},
        inquiryChanged: {type:Signal, args:[]},
        jsonChanged: {type:Signal, args:[]},
        targetChanged: {type:Signal, args:[]},
        stateChanged: {type:Signal, args:[]},
    })

    'Component.onCompleted'(){
        this.updateJSONModel()
    }

    SLOT_sourceChanged(oldValue, newValue){
        this.state = "Loading"
        if(this.source == "")
            return
        let xhr = new XMLHttpRequest()
        xhr.open("GET", this.source)
        xhr.onreadystatechange = ()=>{
            if(xhr.readyState === XMLHttpRequest.DONE){
                this.json = xhr.responseText
                this.updateJSONModel()
                this.state = "Ready"
            }
        }
        xhr.send()
    }

    SLOT_inquiryChanged(oldValue, newValue){
        this.updateJSONModel()
    }
    
    makeJson(){

    }

    sendJson(method){
        let xhr = new XMLHttpRequest()
		xhr.open(method, this.target)
		xhr.send(this.json)
    }

    updateJSONModel(){
        if( this.json === "" ) return

        this.clear()
		let d1 = new Date()
        let objectArray = this.parseJSONString(this.json, this.inquiry)

        this.append(objectArray)
		let d2 = new Date()
    }

    parseJSONString(jsonString, jsonPathQuery){
        let objectArray = JSON.parse(jsonString)
		if(jsonPathQuery !== "") objectArray = jsonPath(objectArray, jsonPathQuery)

		return objectArray
    }
}



module.exports = JSONListModel