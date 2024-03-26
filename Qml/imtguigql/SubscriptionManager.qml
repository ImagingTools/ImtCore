import QtQuick 2.0
import Acf 1.0
import QtWebSockets 1.2
import imtcontrols 1.0

WebSocket {
    id: container;

    active: true

    property TreeItemModel socketModel: TreeItemModel {}

    property var subscriptionModel: []

    Component.onCompleted: {
        Events.subscribeEvent("RegisterSubscription", container.registerSubscriptionEvent);
        Events.subscribeEvent("UnRegisterSubscription", container.unRegisterSubscription);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("RegisterSubscription", container.registerSubscriptionEvent);
        Events.unSubscribeEvent("UnRegisterSubscription", container.unRegisterSubscription);
    }

    onActiveChanged: {
        console.log("SubscriptionManager onActiveChanged", active)
    }

    onStatusChanged: {
        console.log("SubscriptionManager onStatusChanged", status)

        if (status == WebSocket.Error){
            console.error("SubscriptionManager ERROR", errorString)
        }
        else if (status == WebSocket.Open){
            sendTextMessage("{ \"type\": \"connection_init\" }")

            registerSubscriptionToServer();
        }
        else if (status == WebSocket.Closed){
            for (let index = 0; index < subscriptionModel.length; index++){
                subscriptionModel[index]["status"] = "unregistered"
            }
        }
    }

    function reconnect(){
        console.log("reconnect to", url)

        active = false;
        active = true;
    }

    property Timer timer: Timer{
        id: timer;

        interval: 4000;

        repeat: true;
        running: true;

        onTriggered: {
            if (container.status == WebSocket.Closed ||
                container.status == WebSocket.Error){
                container.reconnect()
            }
        }
    }

    onUrlChanged: {
        console.log("SubscriptionManager onUrlChanged", url)
    }

    onTextMessageReceived:{
        let ok = socketModel.CreateFromJson(message)

        if (socketModel.GetData("type") === "connection_ask"){
            console.log("SubscriptionManager onTextMessageReceived", message)

            registerSubscriptionToServer()
        }
        else if (socketModel.GetData("type") === "start_ask"){
            console.log("SubscriptionManager onTextMessageReceived", message)

            for (let index = 0; index < subscriptionModel.length; index++){
                if (subscriptionModel[index]["subscriptionId"] === socketModel.GetData("id")){
                    let subscription = subscriptionModel[index]["subscription"]
                    subscriptionModel[index]["status"] = "registered"
                    subscription.state = "Registered"
                }
            }
        }
        else if (socketModel.GetData("type") === "data"){
            console.log("SubscriptionManager onTextMessageReceived", message)
            console.log("subscriptionModel", subscriptionModel)
            for (let index = 0; index < subscriptionModel.length; index++){
                if (subscriptionModel[index]["subscriptionId"] == socketModel.GetData("id")){
                    let subscription = subscriptionModel[index]["subscription"]
                    console.log("subscription", subscription);
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

    function registerSubscriptionEvent(parameters){
        let query = parameters["Query"];
        let client = parameters["Client"];

        console.log("client", client.subscriptionId);

        registerSubscription(query, client);
    }

    function clear(){
        container.subscriptionModel = []
    }

    function registerSubscriptionToServer(){
        console.log("registerSubscriptionToServer",container.status);
        if (container.status != WebSocket.Open){
            return;
        }

        for (let index = 0; index < subscriptionModel.length; index++){
            if (subscriptionModel[index]["status"] === "unregistered"){
                let request = {}
                request["id"] = subscriptionModel[index]["subscriptionId"]
                request["type"] = "start"
                let payload = {}
                let query = subscriptionModel[index]["query"]
                payload["data"] = query.GetQuery()
                request["payload"] = payload

                console.log("Id", JSON.stringify(request));

                container.sendTextMessage(JSON.stringify(request))
            }
        }
    }

    function registerSubscription(query, subscriptionClient){
        console.log("registerSubscription", query, subscriptionClient);

        for (let index = 0; index < subscriptionModel.length; index++){
            if (subscriptionModel[index]["subscription"] === subscriptionClient){
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
            if (subscriptionModel[index]["subscription"] === subscriptionClient){
                subscriptionModel.splice(index, 1)
                return;
            }
        }
    }
}
