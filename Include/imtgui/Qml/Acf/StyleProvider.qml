pragma Singleton

import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: styleProvider;

    property TreeItemModel styleModel: TreeItemModel {}

    function updateModel(theme){
        console.log("PreferenceDialog callGetStyleQuery", theme);
        styleQuery.getStyle(theme);
    }

    GqlModel {
        id: styleQuery;

        function getStyle(theme){
            var query = Gql.GqlRequest("query", "GetStyle");
            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("theme", theme);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("style");
            queryFields.InsertField("theme", theme);
            queryFields.InsertField("source", "source");
            queryFields.InsertField("test", "test");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("styleQuery gqlData", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, styleQuery);
            if (this.state === "Ready") {
                var dataModelLocal;

                if (styleQuery.ContainsKey("errors")){
                    return;
                }

                if (styleQuery.ContainsKey("data")){
                    dataModelLocal = styleQuery.GetData("data");

                    if(dataModelLocal.ContainsKey("GetStyle")) {
                        dataModelLocal = dataModelLocal.GetData("GetStyle");

                        styleModel = dataModelLocal;
                    }
                }
            }
        }
    }
}
