import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0


/* Usage example:

    ApplicationMain {
        id: window;

        authorizationServerConnected: pumaConnectionChecker.status === 1;

        PumaConnectionChecker {
            id: pumaConnectionChecker;
            subscriptionManager: window.subscriptionManager;
        }
    }
*/

SubscriptionClient {
    id: pumaSub;

    property int status: -1; // -1 - Unknown, 0 - Connecting, 1 - Connected, 2 - Disconnected
    property string subscriptionRequestId: "PumaWsConnection";
    property var subscriptionManager: null;

    property bool ok: subscriptionManager ? subscriptionManager.status === 1 : false;
    onOkChanged: {
        if (subscriptionManager){
            var query = Gql.GqlRequest("subscription", pumaSub.subscriptionRequestId);
            var queryFields = Gql.GqlObject("notification");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            subscriptionManager.registerSubscription(query, pumaSub)

            status = 0;
        }
    }

    onStateChanged: {
        if (state === "Ready"){
            let ok = false;
            if (pumaSub.containsKey("data")){
                let localModel = pumaSub.getData("data")
                if (localModel.containsKey(pumaSub.subscriptionRequestId)){
                    localModel = localModel.getData(pumaSub.subscriptionRequestId)

                    if (localModel.containsKey("status")){
                        let status = localModel.getData("status")
                        if (status === "Disconnected"){
                            ok = true;
                            pumaSub.status = 2;
                        }
                        else if (status === "Connected"){
                            ok = true;
                            pumaSub.status = 1;
                        }
                    }
                }
            }

            if (!ok){
                pumaSub.status = -1;
            }
        }
    }
}

