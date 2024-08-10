import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

QtObject {
    id: container;

    property string token: "";
    property string login: "";
    property string userId: "";
    property string passwordHash: "";
    property string systemId: "";
    property var permissions: [];

    signal accepted();
    signal failed(string message);

    property string modelState: container.authorizationGqlModel.state;

    Component.onCompleted: {
        Events.subscribeEvent("GetToken", container.getToken);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("GetToken", container.getToken);
    }

    function getToken(callback){
        if (callback){
            callback(token);
        }
    }

    function authorization(loginF, passwordF){
        container.authorizationGqlModel.authorization(loginF, passwordF);
    }

    property GqlModel authorizationGqlModel : GqlModel{
        function authorization(login, password) {
            var query = Gql.GqlRequest("query", "UserToken");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField ("Login", login);
            inputParams.InsertField ("Password", password);
            query.AddParam(inputParams);

            var gqlData = query.GetQuery();

            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready"){
                var dataModelLocal;

                if (container.authorizationGqlModel.containsKey("errors")){
                    dataModelLocal = container.authorizationGqlModel.getData("errors")
                    dataModelLocal = dataModelLocal.getData("UserToken")

                    let message = dataModelLocal.getData("message");

                    let type;
                    if (dataModelLocal.containsKey("type")){
                        type = dataModelLocal.getData("type");
                    }

                    container.failed(message);

                    ModalDialogManager.showWarningDialog(message)

                    return;
                }

                if (container.authorizationGqlModel.containsKey("data")){
                    dataModelLocal = container.authorizationGqlModel.getData("data");

                    if (dataModelLocal.containsKey("UserToken")){
                        dataModelLocal = dataModelLocal.getData("UserToken");

                        if (dataModelLocal.containsKey("Token")){
                            let token = dataModelLocal.getData("Token");
                            container.token = token;
                            container.authorizationGqlModel.SetGlobalAccessToken(token);
                        }

                        if (dataModelLocal.containsKey("Login")){
                            let login = dataModelLocal.getData("Login");
                            container.login = login;
                        }

                        if (dataModelLocal.containsKey("UserId")){
                            let userId = dataModelLocal.getData("UserId");
                            container.userId = userId;
                        }

                        if (dataModelLocal.containsKey("PasswordHash")){
                            let userPasswordHash = dataModelLocal.getData("PasswordHash");
                            container.passwordHash = userPasswordHash;
                        }

                        if (dataModelLocal.containsKey("SystemId")){
                            let systemId = dataModelLocal.getData("SystemId");
                            container.systemId = systemId;
                        }

                        if (dataModelLocal.containsKey("Permissions")){
                            let permissions = dataModelLocal.getData("Permissions");
                            container.permissions = permissions.split(';')
                        }

                        container.accepted();
                    }
                }
            }
        }
    }
}
