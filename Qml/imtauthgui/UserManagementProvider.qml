import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0

QtObject {
    id: container;

    property string userMode;
    property bool superuserExists: false;

    signal updated();

    onUserModeChanged: {
        Events.sendEvent("UserModeChanged", container.userMode);
    }

    property alias userModeGqlModel: userModeModel;

    function isNoUserManagement(){
        return userMode === "NO_USER_MANAGEMENT";
    }

    function updateModel(){
        userMode = "";
        userModeModel.getUserMode();
    }

    property Component errorDialog: Component {
        ErrorDialog {
            onFinished: {}
        }
    }

    property GqlModel saveQuery: GqlModel {
        id: saveQuery;

        function updateModel(password){
            var query = Gql.GqlRequest("query", "UserAdd");

            var queryFields = Gql.GqlObject('addedNotification');

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", "su");

            let obj = {"Username": "su", "UserId": "su", "Password": password, "Name": "superuser"}

            var jsonString = JSON.stringify(obj);

            inputParams.InsertField ("Item", jsonString);

            query.AddParam(inputParams);

            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }
    }

    property GqlModel userModeModel: GqlModel{
        id: userModeModel;

        function getUserMode() {
            var query = Gql.GqlRequest("query", "UserMode");

            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready"){
                var dataModelLocal;

                if (userModeModel.containsKey("errors")){
                    dataModelLocal = userModeModel.getData("errors");

                    if (dataModelLocal.containsKey("UserMode")){
                        dataModelLocal = dataModelLocal.getData("UserMode");
                    }

                    let message = ""
                    if (dataModelLocal.containsKey("message")){
                        message = dataModelLocal.getData("message");
                    }

                    ModalDialogManager.showWarningDialog(message)

                    return;
                }

                if (userModeModel.containsKey("data")){
                    dataModelLocal = userModeModel.getData("data")
                    if (dataModelLocal.containsKey("UserMode")){
                        dataModelLocal = dataModelLocal.getData("UserMode");
                    }

                    if (dataModelLocal.containsKey("Parameters")){
                        let parameters = dataModelLocal.getData("Parameters");
                        let value = dataModelLocal.getData("Value");
                        if (value >= 0){
                            container.userMode = parameters.getData("Id", value);
                        }
                    }
                }
                container.updated();
            }
        }
    }
}
