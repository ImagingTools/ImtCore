import QtQuick 2.15
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: container;

    property string userMode;

    property alias userModeGqlModel: userModeModel;

    function updateModel(){
        userModeModel.getUserMode();
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

    Component {
        id: errorDialog;

        ErrorDialog {
            onFinished: {
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
//            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

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
        id: userModeModel;

        function getUserMode() {
            console.log("getUserMode UserMode");
            var query = Gql.GqlRequest("query", "UserMode");

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, userModeModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (userModeModel.ContainsKey("errors")){
                    return;
                }

                if (userModeModel.ContainsKey("data")){
                    dataModelLocal = userModeModel.GetData("data")

                    if (dataModelLocal.ContainsKey("UserMode")){
                        dataModelLocal = dataModelLocal.GetData("UserMode");

                        let databaseState = dataModelLocal.GetData("DatabaseConnectionState");
                        if (databaseState == false){
                            let message = dataModelLocal.GetData("Message");

                            message += '. ' + 'Please сheck the data to connect to the database'
                            modalDialogManager.openDialog(errorDialog, {"message": message});

                            return;
                        }

                        let parameters = dataModelLocal.GetData("Parameters");
                        if (parameters){
                            let value = dataModelLocal.GetData("Value");
                            if (value >= 0){
                                container.userMode = parameters.GetData("Id", value);
                            }
                        }

                        let superUserExists = dataModelLocal.GetData("SuperUserExists");
                        if (superUserExists == false){
                            modalDialogManager.openDialog(passwordInputDialog, {"message": qsTr("Please set the password for system administrator:")});
                        }
                    }
                }
            }
        }
    }
}
