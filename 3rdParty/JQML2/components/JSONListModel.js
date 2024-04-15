const { ListModel } = require('./ListModel')
const { QString } = require('../utils/properties')

class JSONListModel extends ListModel {
    static defaultProperties = {
        source: { type: QString, value: '', changed: '$sourceChanged' },
        inquiry: { type: QString, value: '', changed: '$inquiryChanged' },
        json: { type: QString, value: '' },
        target: { type: QString, value: '' },
        state: { type: QString, value: '' },

    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
    }

    $complete(){
        this.updateJSONModel()
        super.$complete()
    }

    $sourceChanged(){
        this.state = "Loading"
        if(this.source == "")
            return
        let xhr = new XMLHttpRequest()
        xhr.open("GET", this.source)
        xhr.onreadystatechange = function(){
            if(xhr.readyState === XMLHttpRequest.DONE){
                this.json = xhr.responseText
                this.updateJSONModel()
                this.state = "Ready"
            }
        }.bind(this)
        xhr.send()
    }

    $inquiryChanged(){
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

module.exports.JSONListModel = JSONListModel