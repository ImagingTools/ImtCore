import QtQuick 2.15
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property string token: "";
    property string login: "";

    signal accepted();

    function updateModel(){
        userModeGqlModel.getUserMode();
    }

    function authorization(login, password){
        authorizationGqlModel.authorization(login, password);
    }

    GqlModel{
        id: authorizationGqlModel;

        function authorization(login, password) {
            var query = Gql.GqlRequest("query", "UserAuthorization");

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
                     dataModelLocal = dataModelLocal.GetData("UserAuthorization")

                    let message = dataModelLocal.GetData("message");
                    errorMessage.text = message;

                    return;
                }

                if (authorizationGqlModel.ContainsKey("data")){
                    dataModelLocal = authorizationGqlModel.GetData("data")

                    if (dataModelLocal.ContainsKey("UserAuthorization")){
                        dataModelLocal = dataModelLocal.GetData("UserAuthorization")

                        if (dataModelLocal.ContainsKey("Token")){
                            let token = dataModelLocal.GetData("Token")
                            container.token = token;

                            this.SetGlobalAccessToken(token);

                            let login = dataModelLocal.GetData("Login")
                            container.login = login;

                            accepted();
                        }
                    }
                }
            }
        }
    }
}
