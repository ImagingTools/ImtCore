import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0

Item {
    id: container;

    property string getMetaInfoGqlCommand;

    property TreeItemModel metaInfoModel: TreeItemModel {}

    function getMetaInfo(objectId){
        metaInfo.getMetaInfo(objectId);
    }

    function getAdditionalInputParams(){
        return {};
    }

    GqlModel {
        id: metaInfo;

        function getMetaInfo(objectId){
            console.log( "CollectionView metaInfo getMetaInfo");
            var query = Gql.GqlRequest("query", container.getMetaInfoGqlCommand);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", objectId);

            let additionInputParams = container.getAdditionalInputParams();
            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("metaInfo");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready"){
                var dataModelLocal;

                if (metaInfo.ContainsKey("errors")){
                    dataModelLocal = metaInfo.GetData("errors");
                    let message = ""
                    if (dataModelLocal.ContainsKey("message")){
                        message = dataModelLocal.GetData("message");
                    }

                    let type;
                    if (dataModelLocal.ContainsKey("type")){
                        type = dataModelLocal.GetData("type");
                    }

                    Events.sendEvent("SendError", {"Message": message, "ErrorType": type})

                    return;
                }

                if (metaInfo.ContainsKey("data")){
                    dataModelLocal = metaInfo.GetData("data");

                    if (dataModelLocal.ContainsKey(container.getMetaInfoGqlCommand)){
                        dataModelLocal = dataModelLocal.GetData(container.getMetaInfoGqlCommand);
                        console.log( "dataModelLocal", dataModelLocal.toJSON());

                        container.metaInfoModel = dataModelLocal;
                    }
                }
            }
        }
    }//MetaInfo
}

