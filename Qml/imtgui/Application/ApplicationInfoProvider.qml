import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0

QtObject {
    id: root;

    property TreeItemModel clientApplicationInfo: null;
    property TreeItemModel serverApplicationInfo: null;

    signal updated();

    onClientApplicationInfoChanged: {
        root.updated();
    }

    onServerApplicationInfoChanged: {
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

                if (root.applicationInfoQuery.containsKey("errors")){
                    dataModelLocal = root.applicationInfoQuery.getData("errors");

                    return;
                }

                if (root.applicationInfoQuery.containsKey("data")){
                    dataModelLocal = root.applicationInfoQuery.getData("data");

                    if (dataModelLocal.containsKey("GetApplicationInfo")){
                        dataModelLocal = dataModelLocal.getData("GetApplicationInfo");

                        root.serverApplicationInfo = dataModelLocal;
                    }
                }
            }
        }
    }//GetSettings
}
