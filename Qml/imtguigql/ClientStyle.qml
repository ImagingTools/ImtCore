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
            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready") {
                var dataModelLocal;

                if (styleQuery.containsKey("errors")){
                    return;
                }

                if (styleQuery.containsKey("data")){
                    dataModelLocal = styleQuery.getData("data");

                    if(dataModelLocal.containsKey("GetStyle")) {
                        dataModelLocal = dataModelLocal.getData("GetStyle");
                    }

                    if (dataModelLocal.containsKey("theme")){
                        styleContainer.theme = dataModelLocal.getData("theme");
                    }

                    if(dataModelLocal.containsKey("source")){
                        styleContainer.parseStyleTheme(dataModelLocal);
                    }

                    Events.sendEvent("DesignSchemeChanged", styleContainer.theme);
                }
            }
        }
    }
}
