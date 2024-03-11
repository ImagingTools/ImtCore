import QtQuick 2.0
import Acf 1.0
import imtguigql 1.0
import imtcontrols 1.0

StyleBase {
    id: styleContainer;

    function getDesignScheme(theme){
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
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready") {
                var dataModelLocal;

                if (styleQuery.ContainsKey("errors")){
                    return;
                }

                if (styleQuery.ContainsKey("data")){
                    dataModelLocal = styleQuery.GetData("data");

                    if(dataModelLocal.ContainsKey("GetStyle")) {
                        dataModelLocal = dataModelLocal.GetData("GetStyle");
                    }

                    if (dataModelLocal.ContainsKey("theme")){
                        styleContainer.theme = dataModelLocal.GetData("theme");
                    }

                    if(dataModelLocal.ContainsKey("source")){
                        styleContainer.parseStyleTheme(dataModelLocal);
                    }

                    Events.sendEvent("DesignSchemeChanged", styleContainer.theme);
                }
            }
        }
    }
}
