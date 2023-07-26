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

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, container.authorizationGqlModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (container.authorizationGqlModel.ContainsKey("errors")){
                    dataModelLocal = container.authorizationGqlModel.GetData("errors")
                    dataModelLocal = dataModelLocal.GetData("UserToken")

                    let message = dataModelLocal.GetData("message");
                    container.failed(message);

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

                            container.accepted();
                        }
                    }
                }
            }
        }
    }
}
