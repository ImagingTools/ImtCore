import QtQuick 2.15
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property string userMode;

    function updateModel(){
        userModeGqlModel.getUserMode();
    }

    GqlModel{
        id: userModeGqlModel;

        function getUserMode() {
            var query = Gql.GqlRequest("query", "GetUserMode");

            console.log("getUserMode GetUserMode");

            var inputParams = Gql.GqlObject("input");
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("UserMode");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, userModeGqlModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (userModeGqlModel.ContainsKey("errors")){
                    return;
                }

                if (userModeGqlModel.ContainsKey("data")){
                    dataModelLocal = userModeGqlModel.GetData("data")

                    if (dataModelLocal.ContainsKey("GetUserMode")){
                        dataModelLocal = dataModelLocal.GetData("GetUserMode");

                        if (dataModelLocal.ContainsKey("items")){
                            dataModelLocal = dataModelLocal.GetData("items");

                            let value = dataModelLocal.GetData("Value");
                            let parameters = dataModelLocal.GetData("Parameters");

                            if (parameters){
                                let userMode = parameters.GetData("Id", value);

                                container.userMode = userMode;
                            }
                        }
                    }
                }
            }
        }
    }
}
