import QtQuick 2.12
import imtcontrols 1.0

QtObject {
	id: root;
	
	property string json;
	property string state;
	
	function setGqlQuery(gqlData, headers){
		root.state = "Loading"

		let xhr = new XMLHttpRequest

		xhr.gqlRequest = root;

		// Installed before send(): XmlHttpRequestProxy wraps send() and captures the
		// handler that exists at that moment, so assigning ours afterwards replaced
		// the proxy's 401/403 hook (it survived only through the proxy's separate
		// "loadend" listener). It also closes the race where DONE is reached before
		// a handler exists at all.
		xhr.onreadystatechange = function(){
			if (xhr.readyState !== XMLHttpRequest.DONE){
				return;
			}

			// Every terminal outcome must leave "Loading". A request stuck in
			// "Loading" forever blocks single-flight guards (ApplicationInfoProvider)
			// and leaves callers waiting on state / finished() indefinitely — which
			// is what used to happen for 5xx and transport failures, since only
			// 200/401/403 were handled here.
			if (xhr.status === 200){
				root.json = xhr.responseText;
				if (root.state === "Ready"){
					root.state = "Loading"
				}

				root.state = "Ready";
			}
			else if (xhr.status === 401){
				root.state = "Unauthorized";
			}
			else if (xhr.status === 403){
				root.state = "Forbidden";
			}
			else{
				root.state = "Error";
			}
		}

		xhr.open("POST", "../../graphql")

		if(headers){
			for(let name in headers){
				xhr.setRequestHeader(name, headers[name])
			}
		}

		xhr.send(gqlData)
	}
}
