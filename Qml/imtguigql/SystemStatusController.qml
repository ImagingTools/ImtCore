import QtQuick 2.0
import Acf 1.0
import imtguigql 1.0

QtObject {
    id: systemStatusController;

    property string serverName;
    property string serverUrl;

    property string serverStatusGqlCommandId;
    property string databaseStatusGqlCommandId;

    property int systemStatus: -1; // 0 - NONE, 1 - TRY_CONNECTING, 2 - SERVER_CONNECTION_ERROR, 3 - DATABASE_CONNECTION_ERROR, 4 - NO_ERROR

    property alias checkIntervalMs: systemStatusController.timer.interval;

    property int serverStatus: -1; // -1 - NONE, 0 - CONNECTING, 1 - DISCONNECTED, 2 - CONNECTED
    property int databaseStatus: -1; // -1 - NONE, 0 - CONNECTING, 1 - DISCONNECTED, 2 - CONNECTED

    property var slaveSystemStatusController: null;

    property Connections slaveSystemStatusConnections: Connections {
        function onSystemStatusChanged(){
            if (systemStatusController.slaveSystemStatusController){
                let slaveSystemStatus = systemStatusController.slaveSystemStatusController.systemStatus;
                console.log("slave onSystemStatusChanged", slaveSystemStatus);

                if (systemStatusController.systemStatus == 0){
                    systemStatusController.systemStatus = -1;
                    systemStatusController.systemStatus = 0;
                }
            }
        }
    }

    onSlaveSystemStatusControllerChanged: {
        console.log("onSlaveSystemStatusControllerChanged", slaveSystemStatusController);

        if (slaveSystemStatusController){
            slaveSystemStatusConnections.target = slaveSystemStatusController;
        }
    }

    onSystemStatusChanged: {
        console.log("onStatusChanged", systemStatus, serverName);
    }

    onServerStatusChanged: {
        if (serverStatus === 2){
            if (databaseStatusGqlCommandId !== ""){
                databaseStatusGqlModel.updateModel();
            }
            else{
                let ok = true;
                if (slaveSystemStatusController){
                    if (slaveSystemStatusController.systemStatus !== 4){
                        ok = false;

                        systemStatusController.systemStatus = 0;
                    }
                }

                if (ok){
                    timer.stop();
                    systemStatusController.systemStatus = 4;
                }
            }
        }
        else if (serverStatus === 1){
            systemStatusController.systemStatus = 2;
        }
    }

    onDatabaseStatusChanged: {
        if (serverStatus !== 2){
            return;
        }

        if (databaseStatus == 2){
            let ok = true;
            if (slaveSystemStatusController){
                if (slaveSystemStatusController.systemStatus !== 4){
                    ok = false;

                    systemStatusController.systemStatus = 0;
                }
            }

            if (ok){
                timer.stop();
                systemStatusController.systemStatus = 4;
            }
        }
        else if (databaseStatus === 1){
            systemStatusController.systemStatus = 3;
        }
    }

    function getLastMessage(){
        console.log("getLastMessage", serverName, systemStatus);

        if (systemStatus == 1){
            return qsTr("Try connect to " + serverUrl + " ...")
        }
        else if (systemStatus == 2){
            return serverName + " " + qsTr("server connection error")
        }
        else if (systemStatus == 3){
            return serverName + " " + qsTr("database connection error")
        }
        else if (systemStatus == 0){

            if (slaveSystemStatusController){
                console.log("slaveSystemStatusController", slaveSystemStatusController.getLastMessage());

                return slaveSystemStatusController.getLastMessage();
            }
        }

        return "";
    }

    function checkStatus(){
        if (systemStatusController.systemStatus != 2 &&
            systemStatusController.systemStatus != 3 &&
            systemStatusController.systemStatus != 0){
            systemStatusController.systemStatus = 1;
        }

        console.log("checkStatus", serverName)

        serverStatusGqlModel.updateModel();
    }

    function updateSystemStatus(){
        console.log("updateSystemStatus", serverName)

        if (serverStatusGqlCommandId === ""){
            return;
        }

        systemStatus = -1;

        if (slaveSystemStatusController != null){
            slaveSystemStatusController.updateSystemStatus();
        }

        timer.start();
    }

    property Timer timer: Timer {
        interval: 7000;

        repeat: true;
//        triggeredOnStart: true;

        onTriggered: {
            console.log("onTriggered")
            if (systemStatusController.serverConnected && systemStatusController.databaseStatusGqlCommandId === "" ||
                systemStatusController.systemStatus === 4){
                stop();

                return;
            }

            if (systemStatusController.serverStatusGqlModel.state !== "Loading" &&
                systemStatusController.databaseStatusGqlModel.state !== "Loading"){
                systemStatusController.checkStatus();
            }
        }
    }

    property GqlModel serverStatusGqlModel: GqlModel {
        function updateModel() {
            var query = Gql.GqlRequest("query", systemStatusController.serverStatusGqlCommandId);
            var gqlData = query.GetQuery();

            systemStatusController.serverStatus = 0;
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready") {
                console.log("serverStatusGqlModel Ready", this.toJSON());

                var dataModelLocal;

                if (this.ContainsKey("errors")){
                    systemStatusController.serverStatus = 1;

                    return;
                }

                if (this.ContainsKey("data")){
                    systemStatusController.serverStatus = 2;
                }
            }
            else if (this.state === "Error"){
                systemStatusController.serverStatus = 1;
            }
        }
    }

    property GqlModel databaseStatusGqlModel: GqlModel {
        function updateModel() {
            var query = Gql.GqlRequest("query", systemStatusController.databaseStatusGqlCommandId);
            var gqlData = query.GetQuery();

            systemStatusController.databaseStatus = 0;

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready") {
                console.log("databaseStatusGqlModel Ready", this.toJSON());

                var dataModelLocal;

                if (this.ContainsKey("errors")){
                    systemStatusController.databaseStatus = 1;

                    return;
                }

                if (this.ContainsKey("data")){
                    systemStatusController.databaseStatus = 2;

                }
            }
            else if (this.state === "Error"){
                systemStatusController.databaseStatus = 1;
            }
        }
    }
}

