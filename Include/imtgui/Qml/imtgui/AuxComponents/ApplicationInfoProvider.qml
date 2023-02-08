import QtQuick 2.15
import Acf 1.0
import imtqml 1.0

Item {
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

        applicationInfoQuery.updateModel();
    }

    GqlModel {
        id: applicationInfoQuery;

        function updateModel() {
            var query = Gql.GqlRequest("query", "GetApplicationInfo");

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, applicationInfoQuery);

            if (this.state === "Ready") {
                var dataModelLocal;

                if (applicationInfoQuery.ContainsKey("errors")){
                    dataModelLocal = applicationInfoQuery.GetData("errors");

                    return;
                }

                if (applicationInfoQuery.ContainsKey("data")){
                    dataModelLocal = applicationInfoQuery.GetData("data");

                    if (dataModelLocal.ContainsKey("GetApplicationInfo")){
                        dataModelLocal = dataModelLocal.GetData("GetApplicationInfo");

                        root.serverApplicationInfo = dataModelLocal;
                    }
                }
            }
        }
    }//GetSettings
}
