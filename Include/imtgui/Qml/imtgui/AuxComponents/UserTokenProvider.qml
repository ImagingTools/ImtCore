import QtQuick 2.15
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property string token: "";
    property string login: "";

    signal accepted();
    signal errorSignal(string message);
    onErrorSignal: {
        console.log("ERROR: ",message);
    }



    function authorization(loginF, passwordF){
        console.log('DEBUG::authorization', loginF, passwordF);
        authorizationGqlModel.authorization(loginF, passwordF);
    }

    GqlModel{
        id: authorizationGqlModel;

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
            console.log("State:", this.state, authorizationGqlModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (authorizationGqlModel.ContainsKey("errors")){
                    dataModelLocal = authorizationGqlModel.GetData("errors")
                    dataModelLocal = dataModelLocal.GetData("UserToken")

                    let message = dataModelLocal.GetData("message");
                    container.errorSignal(message);

                    return;
                }

                if (authorizationGqlModel.ContainsKey("data")){
                    dataModelLocal = authorizationGqlModel.GetData("data");

                    if (dataModelLocal.ContainsKey("UserToken")){
                        dataModelLocal = dataModelLocal.GetData("UserToken");

                        if (dataModelLocal.ContainsKey("Token")){
                            let token = dataModelLocal.GetData("Token");
                            container.token = token;

                            this.SetGlobalAccessToken(token);

                            let login = dataModelLocal.GetData("Login");
                            container.login = login;

                            container.accepted();
                        }
                    }
                }
            }
        }
    }
}
