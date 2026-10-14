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

	GqlPureRequestSender {
		id: requestSender;
		gqlCommandId: <GQL-command>;

		onFinished: {
			if (status === 1){
				// result is a plain object
			}
		}
	}
*/

GqlRequest {
	id: root;

	property string gqlCommandId;
	property int requestType: 0; // 0 - Query, 1 - Mutation, 2 - Subscription
	property string permissionPath

	property var result: null;

	property var inputParams: Gql.GqlObject("input");

	signal finished(int status); // If status -1 - error, 1 - success response

	property bool traceTiming: false
	property real traceResponseAt: 0

	function traceNow(){
		if (typeof performance !== "undefined" && performance.now){
			return performance.now()
		}

		return Date.now()
	}

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

		let parseStarted = 0
		if (root.traceTiming){
			parseStarted = root.traceNow()
			root.traceResponseAt = parseStarted
		}

		let responseObj = null
		try {
			responseObj = JSON.parse(data)
			if (root.traceTiming){
				console.log("GqlPure", root.gqlCommandId, "JSON.parse", Math.round(root.traceNow() - parseStarted), "мс")
			}
		}
		catch (e){
			console.error("Unable convert json ", data, " to object", "Warning");
			console.error(e);
			root.onError(qsTr("Unable to read the server response"), "");
			return;
		}

		if (!responseObj){
			console.log("Unable convert json ", data, " to object", "Warning");
			root.onError(qsTr("Unable to read the server response"), "");
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
			root.result = dataObject ? dataObject[root.gqlCommandId] : null;
			if (!root.result){
				console.warn("GraphQL request returned no data:", root.gqlCommandId);
				root.onError(qsTr("The server returned no data. Please try again."), "");
				return;
			}

			root.finished(1);
			return;
		}

		// 200 with neither "data" nor "errors" is still a terminal outcome.
		root.onError(qsTr("Unable to read the server response"), "");
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

	function send(){
		if (gqlCommandId == "" || gqlCommandId === undefined || gqlCommandId == null ){
			console.error("Unable to send request. Error: GraphQL command-ID is invalid.");
			root.onError("GraphQL command-ID is invalid", "");
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
			root.onError("Request type is unsupported", "");
			return;
		}

		var query = Gql.GqlRequest(type, root.gqlCommandId);
		let gqlQuery = "";
		try {
			createQueryParams(query);

			let requestedFields = getRequestedFields();
			if (requestedFields !== null){
				let fieldIds = requestedFields.GetFieldIds();
				for (let i = 0; i < fieldIds.length; i++){
					let fieldId = fieldIds[i];
					if (requestedFields.IsObject(fieldId)){
						query.AddField(requestedFields.GetFieldArgumentObjectPtr(fieldId));
					}
					else{
						query.AddField(Gql.GqlObject(fieldId));
					}
				}
			}

			gqlQuery = query.GetQuery();
		}
		catch (error){
			console.error("Unable to create request", root.gqlCommandId, ". Error:", error.message);
			root.onError(error.message, "Error");
			return;
		}

		let headers = root.getHeaders()
		if (headers && root.context && root.context != "")
			headers["context"] = root.context

		root.setGqlQuery(gqlQuery, headers)
	}

	function createQueryParams(query){
		query.AddParam(inputParams);
	}

	// Fields of the returned GqlObject are the selection set of the command result, the object itself is not selected
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
			root.onResult(this.json);
		}
	}
}
