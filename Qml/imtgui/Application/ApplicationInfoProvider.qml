import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtappApplicationSdl 1.0

QtObject {
    id: root;

    property ApplicationInfo serverApplicationInfo: null;

    // property TreeItemModel clientApplicationInfo: null; // ???
    // property TreeItemModel serverApplicationInfo: null;

    signal updated();

    // onClientApplicationInfoChanged: {
    //     root.updated();
    // }

    onServerApplicationInfoChanged: {
        root.updated();
    }

    function updateModel(){
        // root.applicationInfoQuery.updateModel();

        request.send();
    }

    property GqlSdlRequestSender request : GqlSdlRequestSender {
        gqlCommandId: ImtappApplicationSdlCommandIds.s_getApplicationInfo;
        sdlObjectComp: Component { ApplicationInfo{} }
        onSdlObjectChanged: {
            root.serverApplicationInfo = sdlObject;
        }
    }

    // property GqlModel applicationInfoQuery : GqlModel {
    //     function updateModel() {
    //         var query = Gql.GqlRequest("query", "GetApplicationInfo");

    //         var gqlData = query.GetQuery();

    //         this.setGqlQuery(gqlData);
    //     }

    //     onStateChanged: {
    //         if (this.state === "Ready") {
    //             var dataModelLocal;

    //             if (root.applicationInfoQuery.containsKey("errors")){
    //                 dataModelLocal = root.applicationInfoQuery.getData("errors");

    //                 return;
    //             }

    //             if (root.applicationInfoQuery.containsKey("data")){
    //                 dataModelLocal = root.applicationInfoQuery.getData("data");

    //                 if (dataModelLocal.containsKey("GetApplicationInfo")){
    //                     dataModelLocal = dataModelLocal.getData("GetApplicationInfo");

    //                     root.serverApplicationInfo = dataModelLocal;
    //                 }
    //             }
    //         }
    //     }
    // }//GetSettings
}
