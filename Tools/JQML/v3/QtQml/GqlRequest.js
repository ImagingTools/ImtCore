const QtObject = require("./QtObject")
const String = require("./String")
const Signal = require("./Signal")

class GqlRequest extends QtObject {
    static requestTimeout = 120000

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
        xhr.gqlRequest = this

        if(headers && typeof headers === 'object'){
            for(let name in headers){
                xhr.setRequestHeader(name, headers[name])
            }
        }

        // A request that never answers ends as a transport error instead of loading forever.
        xhr.timeout = GqlRequest.requestTimeout

        xhr.onreadystatechange = ()=>{
            if (!this.__destroyed && xhr.readyState === XMLHttpRequest.DONE){
				if (xhr.status === 401){
					this.state = "Unauthorized"
				}
				else if (xhr.status === 403){
					this.state = "Forbidden"
				}
				else if (xhr.status === 200){
					this.json = xhr.responseText
					if (this.state === "Ready"){
						this.state = "Loading"
					}

					this.state = "Ready"
				}
				else{
					// 5xx, a dropped connection (status 0) or a timeout: without this the request stayed in
					// "Loading", finished() never came and the caller kept its loading indicator.
					this.state = "Error"
				}
			}

        }

        xhr.send(gqlData)
    }
}



module.exports = GqlRequest