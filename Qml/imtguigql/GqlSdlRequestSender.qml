import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
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

    signal finished();

    function addInputParam(id, value){
        inputParams.InsertField(id, value);
    }

    function onResult(data){
        if (!sdlObjectComp){
            console.error("Unable to parse response. Please set SDL Component for creating response instance");
            return;
        }

        if (!data){
            console.error("Unable to parse response. Response data is invalid");
            return;
        }

        sdlObject = sdlObjectComp.createObject(root);
        sdlObject.fromObject(data);

        root.finished();
    }

    function onError(message, type){
        if (type == "Critical"){
            ModalDialogManager.showCriticalDialog(message);
        }
        else if (type == "Warning"){
            ModalDialogManager.showWarningDialog(message);
        }
        else if (type == "Info"){
            ModalDialogManager.showInfoDialog(message);
        }
        else if (type == "Error"){
            ModalDialogManager.showErrorDialog(message);
        }
    }

    function getHeaders(){
        return {};
    }

    function send(){
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

        if (inputObjectComp != null){
            let inputObject = inputObjectComp.createObject(root);

            let gqlObject = Gql.GqlObject("input")
            gqlObject.fromJson(inputObject.toJson())
            query.AddParam(gqlObject);

            root.setGqlQuery(query.GetQuery(), root.getHeaders());

            inputObject.destroy()
        }
        else{
            createQueryParams(query);
            root.setGqlQuery(query.GetQuery(), root.getHeaders());
        }
    }

    function createQueryParams(query){
        query.AddParam(inputParams);
    }

    onStateChanged: {
        if (state === "Error"){
            root.onError(qsTr("Network error"), "Critical");
        }
        else if (state === "Ready"){
            let responseObj = JSON.parse(this.json)
            if (!responseObj){
                root.onError("Unable convert json ", json, " to object", "Warning");
                return;
            }

            if ("errors" in responseObj){
                let errorsObject = responseObj["errors"];
                if (root.gqlCommandId in errorsObject){
                    errorsObject = errorsObject[root.gqlCommandId]
                }

                let message = ""
                if ("message" in errorsObject){
                    message = errorsObject["message"];
                }

                let type;
                if ("type" in errorsObject){
                    type = errorsObject["type"];
                }

                root.onError(message, type);

                return;
            }

            if ("data" in responseObj){
                let dataObject = responseObj["data"];
                let itemObject = dataObject[root.gqlCommandId];

                root.onResult(itemObject);
            }
        }
    }
}
