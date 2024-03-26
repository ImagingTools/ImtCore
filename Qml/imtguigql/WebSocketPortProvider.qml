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
            console.log("GetWebSocketUrl", webSocketUrlGqlModel.toJSON());
            var dataModelLocal;

            if (webSocketUrlGqlModel.ContainsKey("data")){
                dataModelLocal = webSocketUrlGqlModel.GetData("data")
                if (dataModelLocal.ContainsKey("GetWebSocketUrl")){
                    dataModelLocal = dataModelLocal.GetData("GetWebSocketUrl")

                    if (dataModelLocal.ContainsKey("Value")){
                        let value = dataModelLocal.GetData("Value")

                        try {
                            let url = new URL(value);
                            webSocketUrlGqlModel.port = url.port;
                        }
                        catch(error){
                            console.error("Web socket URL is invalid: ", value);
                        }
                    }
                }
            }
        }
        else if (this.state === "Error"){
            console.error("Web socket URL provider ERROR");
        }
    }
}
