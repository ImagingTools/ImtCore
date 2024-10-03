import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtauthUsersSdl 1.0

QtObject {
    id: root;

    signal result(bool exists, string error);

    signal modelStateChanged(string state);

    function superuserExists(){
        request.addInputParam("Id", "su");
        request.send();
    }

    property GqlSdlRequestSender request : GqlSdlRequestSender {
        gqlCommandId: ImtauthUsersSdlCommandIds.s_userItem;

        function createQueryParams(query){
            var queryFields = Gql.GqlObject("item");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            query.AddParam(inputParams);
        }

        function onResult(){
            root.result(true, "");
        }

        function onError(){
            root.result(false, "");
        }
    }
}


