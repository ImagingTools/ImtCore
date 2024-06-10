import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

GqlModel {
    id: webSocketUrlGqlModel;

    property int port: -1;

    function updateModel() {
        var query = Gql.GqlRequest("query", "GetWebSocketUrl");
        var gqlData = query.GetQuery();
        this.SetGqlQuery(gqlData);
    }

    onStateChanged: {
        if (this.state === "Ready") {
            console.log("GetWebSocketUrl", webSocketUrlGqlModel.toJson());
            var dataModelLocal;

            if (webSocketUrlGqlModel.containsKey("data")){
                dataModelLocal = webSocketUrlGqlModel.getData("data")
                if (dataModelLocal.containsKey("GetWebSocketUrl")){
                    dataModelLocal = dataModelLocal.getData("GetWebSocketUrl")

                    if (dataModelLocal.containsKey("Value")){
                        let value = dataModelLocal.getData("Value")

                        try {
                            let url = new URL(value);
                            webSocketUrlGqlModel.port = url.port;
                        }
                        catch(error){
                            webSocketUrlGqlModel.port = -1;
                            console.error("Web socket URL is invalid: ", value);
                        }
                    }
                }
            }
        }
        else if (this.state === "Error"){
            webSocketUrlGqlModel.port = -1;
            console.error("Web socket URL provider ERROR");
        }
    }
}
