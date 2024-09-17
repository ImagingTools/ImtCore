pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

QtObject {
    id: root;

    property string productId: "";

    signal logoutSignal();
    signal userModeChanged(string userMode);
    signal superuserExistResult(bool exists);
    signal loginSuccessful();
    signal loginFailed();

    Component.onCompleted: {
        Events.subscribeEvent("Login", root.login)
        Events.subscribeEvent("Logout", root.userLogout)
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("Login", root.login)
        Events.unSubscribeEvent("Logout", root.userLogout)
    }

    property UserManagementProvider userManagementProvider: UserManagementProvider {
        onUserModeChanged: {
            root.userModeChanged(userMode);
        }
    }

    property SuperuserProvider superuserProvider: SuperuserProvider {
        onResult: {
            root.superuserExistResult(exists)
        }
    }

    property UserTokenProvider userTokenProvider: UserTokenProvider {
        productId: root.productId;
        onAccepted: {
            root.loginSuccessful();
        }

        onFailed: {
            root.loginFailed();
        }
    }

    function updateSuperuserModel(){
        superuserProvider.superuserExists();
    }

    function updateUserManagementModel(){
        userManagementProvider.updateModel();
    }

    function loggedUserIsSuperuser(){
        return userTokenProvider.login === "su";
    }

    function getLoggedUserId(){
        return userTokenProvider.login;
    }

    function getPermissions(){
        return userTokenProvider.permissions;
    }

    function getSystemId(){
        return userTokenProvider.systemId;
    }

    function userLogout(param){
        userTokenProvider.login = ""
        userTokenProvider.userId = ""
        userTokenProvider.token = ""
        userTokenProvider.permissions = []

        userTokenProvider.authorizationGqlModel.SetGlobalAccessToken("");

        root.logoutSignal();
    }

    function login(param){
        if (!param){
            return;
        }

        let login = param["Login"];
        let password = param["Password"];

        userTokenProvider.authorization(login, password)
    }

    function getToken(callback){
        if (callback){
            callback(userTokenProvider.token);
        }
    }

    function getLogin(callback){
        if (callback){
            callback(userTokenProvider.login);
        }
    }

    function getUserMode(){
        return userManagementProvider.userMode;
    }

    function isStrongUserManagement(){
        return userManagementProvider.userMode === "STRONG_USER_MANAGEMENT";
    }

    function isSimpleUserManagement(){
        return userManagementProvider.userMode === "NO_USER_MANAGEMENT" || userManagementProvider.userMode === "OPTIONAL_USER_MANAGEMENT";
    }
}
