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

        xhr.onreadystatechange = ()=>{
            if (!this.__destroyed && xhr.readyState === XMLHttpRequest.DONE){
				if (xhr.status === 401){
					this.state = "Unauthorized"
				}
				
				if (xhr.status === 403){
					this.state = "Forbidden"
				}
				
				if (xhr.status === 200){
					this.json = xhr.responseText
					if (this.state === "Ready"){
						this.state = "Loading"
					}
					
					this.state = "Ready"
				}
			}

        }

        xhr.send(gqlData)
    }
}



module.exports = GqlRequest