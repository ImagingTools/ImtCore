const QtObject = require("./QtObject")
const String = require("./String")
const Signal = require("./Signal")

class GqlRequest extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        json: { type: String, value: ''},
        state: { type: String, value: ''},

        jsonChanged: {type:Signal, args:[]},
        stateChanged: {type:Signal, args:[]},
    })

    setGqlQuery(gqlData, headers){
        this.state = "Loading"

        let xhr = new XMLHttpRequest()
        xhr.open("POST", "../../graphql")

        if(headers && typeof headers === 'object'){
            for(let name in headers){
                xhr.setRequestHeader(name, headers[name])
            }
        }

        xhr.send(gqlData)

        xhr.onreadystatechange = ()=>{
            if(this.__destroyed) return

            if (xhr.readyState === XMLHttpRequest.DONE){
                this.json = xhr.responseText;

                this.state = "Ready";
            }
        }
    }
}



module.exports = GqlRequest