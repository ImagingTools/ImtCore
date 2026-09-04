import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0


/**
	Usage example:
	
	Component.onCompleted: {
		requestSender.send();
	}
	
	GqlSdlRequestSender {
		id: requestSender;
		gqlCommandId: <GQL-command>;
		sdlObjectComp:
			Component {
			AuthorizationPayload {
				onFinished: {
					// processing ...
				}
			}
		}
	}
*/

GqlRequest {
	id: root;
	
	property string gqlCommandId;
	property int requestType: 0; // 0 - Query, 1 - Mutation, 2 - Subscription
	property string permissionPath
	
	/**
		SDL object created from sdlObjectComp
	*/
	property var sdlObject: null;
	
	/**
		SDL structure that will come from the server
	*/
	property Component sdlObjectComp: null;
	property Component inputObjectComp: null;
	
	property var inputParams: Gql.GqlObject("input");
	
	signal finished(int status); // If status -1 - error, 1 - success response
	
	function addInputParam(id, value){
		inputParams.InsertField(id, value);
	}
	
	function onResult(data){
		// A resolver that failed server-side (dead DB connection, unhandled exception)
		// answers 200 with `data: { <command>: null }`. Routing that through onError()
		// rather than returning silently is what keeps callers - which clear their
		// `loading` flag in onError()/finished() - from waiting forever.
		if (!data){
			console.warn("GraphQL request returned no data:", root.gqlCommandId);
			root.onError(qsTr("The server returned no data. Please try again."), "");
			return;
		}

		if (sdlObjectComp){
			sdlObject = sdlObjectComp.createObject(root)
		}
		
		if (sdlObject){
			sdlObject.fromObject(data)
		}
		
		if (!sdlObject){
			root.finished(-1)
			return
		}

		root.finished(1);
	}
	
	function onError(message, type){
		if (type == "Critical"){
			PopupManager.addErrorMessage(message, true);
		}
		else if (type == "Warning"){
			PopupManager.addWarningMessage(message, true);
		}
		else if (type == "Info"){
			PopupManager.addInfoMessage(message, true);
		}
		else if (type == "Error"){
			PopupManager.addErrorMessage(message, true);
		}
		
		root.finished(-1);
	}
	
	function getHeaders(){
		return {};
	}
	
	function send(sdlInputObject){
		if (gqlCommandId == "" || gqlCommandId === undefined || gqlCommandId == null ){
			console.error("Unable to send request. Error: GraphQL command-ID is invalid.");
			return;
		}
		
		let type = "";
		if (root.requestType == 0){
			type = "query";
		}
		else if (root.requestType == 1){
			type = "mutation";
		}
		else if (root.requestType == 2){
			type = "subscription";
		}
		else{
			console.error("Unable to send request", root.gqlCommandId ,". Error: Request type is unsupported.");
			return;
		}

		var query = Gql.GqlRequest(type, root.gqlCommandId);

		let gqlObject = Gql.GqlObject("input")
		if (sdlInputObject){
			// console.log("sdlInputObject", sdlInputObject.toJson())
			// gqlObject.fromJson(sdlInputObject.toJson())
			gqlObject.fromObject(sdlInputObject)
			query.AddParam(gqlObject);
		}
		else if (inputObjectComp != null){
			let inputObject = inputObjectComp.createObject(root);
			gqlObject.fromObject(inputObject)
			query.AddParam(gqlObject);
			
			inputObject.destroy()
		}
		else{
			createQueryParams(query);
		}

		let requestedFields = getRequestedFields();
		if(requestedFields !== null){
			query.AddField(requestedFields);
		}
		
		let headers = root.getHeaders()
		if (headers && root.permissionPath && root.permissionPath != "")
			headers["permissionPath"] = root.permissionPath

		root.setGqlQuery(query.GetQuery(), headers)
	}
	
	function createQueryParams(query){
		query.AddParam(inputParams);
	}
	
	function getRequestedFields(){
		return null;
	}

	onStateChanged: {
		if (state === "Error"){
			// Transport-level failure (server unreachable, 5xx, empty body). Passing
			// no type deliberately opens no modal: this state is now reached by every
			// failed request, including startup polling and background refreshes, and
			// a critical dialog per attempt would bury the UI. Senders that care
			// override onError(); everyone else still gets finished(-1).
			console.warn("GraphQL request failed:", root.gqlCommandId);
			root.onError(qsTr("Network error"), "");
		}
		else if (state === "Unauthorized" || state === "Forbidden"){
			// Auth recovery is owned by XmlHttpRequestProxy / AuthorizationController
			// (refresh + retry). Do not open a modal for every 401/403, but complete
			// the request so callers waiting on finished() are not stuck forever.
			root.finished(-1);
		}
		else if (state === "Ready"){
			let responseObj = null
			try{
				responseObj = JSON.parse(this.json)
			}
			catch(e){
				console.error("Unable convert json ", json, " to object", "Warning");
				console.error(e);
				root.onError("Unable convert json ", json, " to object", "Warning");
				return;
			}
			
			if (!responseObj){
				console.log("Unable convert json ", json, " to object", "Warning");
				root.onError("Unable convert json ", json, " to object", "Warning");
				return;
			}

			if ("errors" in responseObj){
				let errorsArray = responseObj["errors"];
				if (errorsArray && errorsArray.length > 0){
					let firstError = errorsArray[0];
					
					let message = "";
					if ("message" in firstError){
						message = firstError["message"];
					}
					
					let type = "";
					if ("extensions" in firstError && firstError["extensions"] && "type" in firstError["extensions"]){
						type = firstError["extensions"]["type"];
					}
					
					root.onError(message, type);
				}
				else{
					root.onError("Unknown error", "Error");
				}
				
				return;
			}
			
			if ("data" in responseObj){
				let dataObject = responseObj["data"];
				let itemObject = dataObject ? dataObject[root.gqlCommandId] : null;

				root.onResult(itemObject);
			}
			else{
				// 200 with neither "data" nor "errors" is still a terminal outcome.
				root.onResult(null);
			}
		}
	}
}
