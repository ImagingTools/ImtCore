import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Rectangle {
    id: authPageContainer;

    color: Style.dialogBackgroundColor;

    MouseArea {
        anchors.fill: parent;

        onClicked: {
            authPageContainer.visible = false;
        }
    }

    Rectangle {
        id: loginContainer;

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;

        width: 450;
        height: 380;

        radius: 3;

        clip: true;

        color: Style.backgroundColor;

        property Item root;

        property string userToken;

        Rectangle{
            id: headerRec;

            width: parent.width;
            height: 100;

            color: Style.backgroundColor;

            Text{
                id: welcomeText;

                anchors.top: parent.top;
                anchors.topMargin: 30;
                anchors.left: parent.left;
                anchors.leftMargin: (parent.width-welcomeText.width)/2;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_title;
                text: qsTr("Welcome to Lisa");
            }

            Text{
                id: loginText;

                anchors.top: welcomeText.bottom;
                anchors.topMargin: 20;
                anchors.left: parent.left;
                anchors.leftMargin: (parent.width-loginText.width)/2;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                text: qsTr("Please login");
            }
        }

        CustomTextField {
            id: loginTextInput;

            anchors.left: parent.left;
            anchors.top: headerRec.bottom;
            anchors.topMargin: 50;
            anchors.leftMargin: 75;

            width: loginContainer.width - 150;
            height: 30;

            placeHolderText: qsTr("Enter the login");

            onTextChanged: {
                console.log("LoginTextInput onInputTextChanged");
            }
        }

        CustomTextField {
            id: passwordTextInput;

            anchors.left: parent.left;
            anchors.top: loginTextInput.bottom;
            anchors.topMargin: 30;
            anchors.leftMargin: 75;

            width: loginContainer.width - 150;
            height: 30;

            placeHolderText: qsTr("Enter the password");
            echoMode: TextInput.Password

            onTextChanged: {
                console.log("PasswordTextInput onInputTextChanged");
            }
        }

        Rectangle{
            id: footerRec;

            width: passwordTextInput.width;

            anchors.left: parent.left;
            anchors.top: passwordTextInput.bottom;
            anchors.topMargin: 10;
            anchors.leftMargin: 75;

            CheckBox{
                id: checkRemember;

                anchors.left: parent.left;
                anchors.top: parent.top;
                anchors.topMargin: 10;
                anchors.leftMargin: 10;

                checkState: Qt.Unchecked;

                onClicked: {
                    if (checkRemember.checkState == Qt.Unchecked){
                        checkRemember.checkState = Qt.Checked
                    }
                    else{
                       checkRemember.checkState = Qt.Unchecked
                    }
                }
            }

            Text{
                id: rememberText;

                anchors.top: parent.top;
                anchors.topMargin: 7;
                anchors.left: checkRemember.right;
                anchors.leftMargin: 7;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_small;
                text: qsTr("Remember me");
            }

            Text{
                id: forgotText;

                anchors.top: parent.top;
                anchors.topMargin: 7;
                anchors.left: rememberText.right;
                anchors.leftMargin: 120;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_small;
    //            text: '<html><style type="text/css"></style><a href="http://google.com">Forgot password?</a></html>'
                onLinkActivated: {
                    Qt.openUrlExternally(link)
                }
            }
        }

        AuxButton {
            id: loginButton;

            anchors.top: footerRec.bottom;
            anchors.left: parent.left;
            anchors.topMargin: 50;
            anchors.leftMargin: (parent.width - width)/2;

            width: 100;
            height: 30;

            hasText: true;
            textButton: qsTr("Login");
            color: Style.buttonColor;
            onClicked: {
                console.log("LoginDialog loginButton onClicked");

                authorizationGqlModel.authorization();
            }
        }
    }

    GqlModel{
        id: authorizationGqlModel;

        function authorization() {
            var query = Gql.GqlRequest("query", "UserAuthorization");

            var inputParams = Gql.GqlObject("input");

            inputParams.InsertField ("Login", loginTextInput.text);
            inputParams.InsertField ("Password", passwordTextInput.text);

            query.AddParam(inputParams);

//            var queryFields = Gql.GqlObject("items");
//            queryFields.InsertField("UserMode");
//            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("TopPanel GqlModel getUserMode query ", gqlData);

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, authorizationGqlModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (authorizationGqlModel.ContainsKey("errors")){
                    return;
                }

                if (authorizationGqlModel.ContainsKey("data")){
                    dataModelLocal = modelUserMode.GetData("data")
//                    if (dataModelLocal.ContainsKey("GetUserMode")){
//                        dataModelLocal = dataModelLocal.GetData("GetUserMode");
//                        if(dataModelLocal.ContainsKey("items")){
//                            dataModelLocal = dataModelLocal.GetData("items");
//                            if(dataModelLocal.ContainsKey("Value")){
//                                dataModelLocal = dataModelLocal.GetData("Value");
//                                if (dataModelLocal == 0){
//                                    loginDialog = false
//                                }
//                                else{
//                                    loginDialog = true
//                                }
//                                if (dataModelLocal == 2){
//                                    var source = "AuxComponents/LoginDialog.qml";
//                                    var parameters = {};
//                                    parameters["localSettings"] = thumbnailDecoratorContainer.localSettings;
//                                    thumbnailDecoratorContainer.openDialog(source, parameters);
//                                }
//                            }
//                        }
//                    }
                }
            }
        }
    }
}

