import QtQuick 2.0
import imtqml 1.0
import Acf 1.0
import QtWebSockets 1.15

WebSocket {
    id: container;

    active: true

    property var subscriptionModel: []

    signal error(string message);

    onStatusChanged: {
        console.log("SubscriptionManager onStatusChanged", status)

        if (status == WebSocket.Error){
            console.error("SubscriptionManager ERROR", errorString)
            error(errorString);
        }
        else if (status == WebSocket.Open){
            sendTextMessage("{ \"type\": \"connection_init\" }")
        }
        else if (status == WebSocket.Closed){
            for (let index = 0; index < subscriptionModel.length; index++){
                subscriptionModel[index]["status"] = "unregistered"
            }
        }
    }

    function registerSubscription(query, subscriptionClient){
        let index
        for (index = 0; index < subscriptionModel.length; index++){
            if (subscriptionModel[index]["subscription"] == subscriptionClient){
                return;
            }
        }
        let commandId = query.GetCommandId()
        subscriptionModel.push(
        {
            "commandId": commandId,
            "subscriptionId": subscriptionClient.subscriptionId,
            "query": query,
            "status": "unregistered",
            "subscription": subscriptionClient
        }
       )

        registerSubscriptionToServer()
    }


    function unRegisterSubscription(subscriptionClient){
        let index
        for (index = 0; index < subscriptionModel.length; index++){
            if (subscriptionModel[index]["subscription"] == subscriptionClient){
                subscriptionModel.splice(index, 1)
                return;
            }
        }
    }

//    TreeItemModel {
//        id: subscriptionModel
//    }

    property TreeItemModel socketModel: TreeItemModel {}

    onTextMessageReceived:{
        console.log("SubscriptionManager onTextMessageReceived", message)
        let ok = socketModel.CreateFromJson(message)

        if (socketModel.GetData("type") == "connection_ack"){
            registerSubscriptionToServer()
        }
        else if (socketModel.GetData("type") == "start_ack"){
            for (let index = 0; index < subscriptionModel.length; index++){
                if (subscriptionModel[index]["subscriptionId"] == socketModel.GetData("id")){
                    let subscription = subscriptionModel[index]["subscription"]
                    subscriptionModel[index]["status"] = "registered"
                    subscription.state = "Registered"
                }
            }
        }
        else if (socketModel.GetData("type") == "data"){
            for (let index = 0; index < subscriptionModel.length; index++){
                if (subscriptionModel[index]["subscriptionId"] == socketModel.GetData("id")){
                    let subscription = subscriptionModel[index]["subscription"]
                    subscription.state = "Processing"
                    let dataModelLocal = socketModel.GetData("payload");
                    console.log("dataModelLocal", dataModelLocal.toJSON());
                    // dataModelLocal = dataModelLocal.GetData("data");
                    subscription.Copy(dataModelLocal)
                    subscription.state = "Ready"

                    return;
                }
            }
        }
    }

    function registerSubscriptionToServer(){
        console.log("SubscriptionManager registerSubscriptionToServer")
        if (container.status != WebSocket.Open){
            return;
        }

        for (let index = 0; index < subscriptionModel.length; index++){
            if (subscriptionModel[index]["status"] == "unregistered"){
               // let queryTxt = subscriptionModel.GetData("query", index).GetQuery();
                let request = {}
                request["id"] = subscriptionModel[index]["subscriptionId"]
                request["type"] = "start"
                let payload = {}
                let query = subscriptionModel[index]["query"]
                payload["data"] = query.GetQuery()
                request["payload"] = payload
                console.log("registerSubscription", JSON.stringify(request))
                container.sendTextMessage(JSON.stringify(request))
            }
        }
    }

//    function SetGlobalAccessToken(token){
//        console.log("SetGlobalAccessToken",token)
//        XMLHttpRequest.QMLAuthToken = token
//    }
}
