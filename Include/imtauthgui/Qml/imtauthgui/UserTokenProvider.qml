import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

QtObject {
    id: container;

    property string token: "";
    property string login: "";
    property string userId: "";
    property string passwordHash: "";

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

            console.log("UserToken", gqlData);

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("User Token State:", this.state, container.authorizationGqlModel.toJSON());

            if (this.state === "NetworkError"){
                let message = qsTr("There was a problem when trying to access the server. Check your network connection or contact your system administrator.");
                Events.sendEvent("SendError", {"Message": message, "ErrorType": "Warning"})
            }
            else if (this.state === "Ready"){
                var dataModelLocal;

                if (container.authorizationGqlModel.ContainsKey("errors")){
                    dataModelLocal = container.authorizationGqlModel.GetData("errors")
                    dataModelLocal = dataModelLocal.GetData("UserToken")

                    let message = dataModelLocal.GetData("message");

                    let type;
                    if (dataModelLocal.ContainsKey("type")){
                        type = dataModelLocal.GetData("type");
                    }

                    container.failed(message);

                    Events.sendEvent("SendError", {"Message": message, "ErrorType": type})

                    return;
                }

                if (container.authorizationGqlModel.ContainsKey("data")){
                    dataModelLocal = container.authorizationGqlModel.GetData("data");

                    if (dataModelLocal.ContainsKey("UserToken")){
                        dataModelLocal = dataModelLocal.GetData("UserToken");

                        if (dataModelLocal.ContainsKey("Token")){
                            let token = dataModelLocal.GetData("Token");
                            container.token = token;

                            this.SetGlobalAccessToken(token);

                            let login = dataModelLocal.GetData("Login");
                            container.login = login;

                            let userId = dataModelLocal.GetData("UserId");
                            container.userId = userId;

                            let userPasswordHash = dataModelLocal.GetData("PasswordHash");
                            container.passwordHash = userPasswordHash;

                            if (dataModelLocal.ContainsKey("Permissions")){
                                let permissions = dataModelLocal.GetData("Permissions");

                                Events.sendEvent("SetPermissions", permissions.split(';'))
                            }

                            container.accepted();
                        }
                    }
                }
            }
        }
    }
}
