import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0

Item {
    id: container;

    property string getMetaInfoGqlCommand;

    property TreeItemModel metaInfoModel: TreeItemModel {}

    property alias state: metaInfo.state;

    function getMetaInfo(objectId){
        if (getMetaInfoGqlCommand == ""){
            return;
        }

        metaInfo.getMetaInfo(objectId);
    }

    function getHeaders(){
        return {};
    }

    GqlModel {
        id: metaInfo;

        function getMetaInfo(objectId){
            var query = Gql.GqlRequest("query", container.getMetaInfoGqlCommand);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("id", objectId);

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("metaInfo");
            queryFields.InsertField("id");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            this.setGqlQuery(gqlData, container.getHeaders());
        }

        onStateChanged: {
            if (this.state === "Ready"){
                var dataModelLocal;

                if (metaInfo.containsKey("errors")){
                    dataModelLocal = metaInfo.getData("errors");
                    let message = ""
                    if (dataModelLocal.containsKey("message")){
                        message = dataModelLocal.getData("message");
                    }

                    let type;
                    if (dataModelLocal.containsKey("type")){
                        type = dataModelLocal.getData("type");
                    }

                    ModalDialogManager.showWarningDialog(message)

                    return;
                }

                if (metaInfo.containsKey("data")){
                    dataModelLocal = metaInfo.getData("data");

                    if (dataModelLocal.containsKey(container.getMetaInfoGqlCommand)){
                        dataModelLocal = dataModelLocal.getData(container.getMetaInfoGqlCommand);
                        container.metaInfoModel = dataModelLocal;
                    }
                }
            }
        }
    }//MetaInfo
}

