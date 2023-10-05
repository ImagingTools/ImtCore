import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

QtObject {
    id: root;

    property TreeItemModel clientApplicationInfo: null;
    property TreeItemModel serverApplicationInfo: null;

    signal updated();

    onClientApplicationInfoChanged: {
        console.log("onClientApplicationInfoChanged", clientApplicationInfo.toJSON());

        root.updated();
    }

    onServerApplicationInfoChanged: {
        console.log("onServerApplicationInfoChanged", serverApplicationInfo.toJSON());

        root.updated();
    }

    function updateModel(){
        console.log("ApplicationInfo updateModel");

        root.applicationInfoQuery.updateModel();
    }

    property GqlModel applicationInfoQuery : GqlModel {
        function updateModel() {
            var query = Gql.GqlRequest("query", "GetApplicationInfo");

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, root.applicationInfoQuery);

            if (this.state === "Ready") {
                var dataModelLocal;

                if (root.applicationInfoQuery.ContainsKey("errors")){
                    dataModelLocal = root.applicationInfoQuery.GetData("errors");

                    return;
                }

                if (root.applicationInfoQuery.ContainsKey("data")){
                    dataModelLocal = root.applicationInfoQuery.GetData("data");

                    if (dataModelLocal.ContainsKey("GetApplicationInfo")){
                        dataModelLocal = dataModelLocal.GetData("GetApplicationInfo");

                        root.serverApplicationInfo = dataModelLocal;
                    }
                }
            }
        }
    }//GetSettings
}
