import QtQuick 2.15
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: container;

    property string userMode;

    function updateModel(){
        userModeGqlModel.getUserMode();
    }

    Component {
        id: passwordInputDialog;

        SuperUserPasswordDialog {
            title: qsTr("Set a password");

            onFinished: {
                if (buttonId == "Ok"){
                    console.log("onFinished", inputValue);

                    saveQuery.updateModel(inputValue);
                }
            }
        }
    }

    GqlModel {
        id: saveQuery;

        function updateModel(password){
            console.log( "updateModel saveModel");
            var query = Gql.GqlRequest("query", "UserAdd");

            var queryFields = Gql.GqlObject('addedNotification');

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", "su");

            let obj = {"Username": "su", "UserId": "su", "Password": password, "Name": "superuser"}

            var jsonString = JSON.stringify(obj);
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

            inputParams.InsertField ("Item", jsonString);

            query.AddParam(inputParams);

            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }
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

                        let value = dataModelLocal.GetData("Value");
                        let superUserExists = dataModelLocal.GetData("SuperUserExists");

                        container.userMode = value;

                        if (superUserExists == false){
                            modalDialogManager.openDialog(passwordInputDialog, {"message": qsTr("Please set the password for system administrator:")});
                        }

//                        modalDialogManager.openDialog(passwordInputDialog, {"message": qsTr("Please set the password for superuser") + ":"});
                    }
                }
            }
        }
    }
}
