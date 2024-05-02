import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtcolgui 1.0

CollectionView {
    id: root;

    property string collectionId;

    property bool hasRemoteChanges: false;

    // Invisible fields that will be requested for collection
    property var additionalFieldIds: ["Id", "Name"]

    commandsControllerComp: Component {
        CommandsRepresentationProvider {
            commandId: root.collectionId;
            uuid: root.viewId;
        }
    }

    function getAdditionalInputParams(){
        return {};
    }

    dataControllerComp: Component {
        CollectionRepresentation {
            property bool isReady: false;

            Component.onCompleted: {
                Events.subscribeEvent("CommandsGuiReady", commandsIsReady);
            }

            Component.onDestruction: {
                Events.unSubscribeEvent("CommandsGuiReady", commandsIsReady);
            }

            function commandsIsReady(){
                isReady = true;

                Events.unSubscribeEvent("CommandsGuiReady", commandsIsReady);
            }

            collectionId: isReady ? root.collectionId : "";

            additionalFieldIds: root.additionalFieldIds;
        }
    }

    Component.onCompleted: {
        Events.subscribeEvent("UpdateAllModels", root.receiveRemoteChanges);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("UpdateAllModels", root.receiveRemoteChanges);
    }

    onHasRemoteChangesChanged: {
        if (root.visible && hasRemoteChanges){
            root.setAlertPanel(alertPanelComp)
        }
    }

    onSelectionChanged: {
        if (!root.visibleMetaInfo){
            return;
        }

        if (selection.length === 1){
            let index = selection[0];
            let elementsModel = root.table.elements;
            let elementId = elementsModel.GetData("Id", index);
            metaInfoProvider.getMetaInfo(elementId);
        }
    }

    onVisibleChanged: {
        if (hasRemoteChanges && visible){
            root.setAlertPanel(alertPanelComp)
        }
        else{
            root.setAlertPanel(undefined)
        }
    }

    function receiveRemoteChanges(){
        if (hasRemoteChanges){
            hasRemoteChanges = false;

            root.doUpdateGui();
            root.setAlertPanel(undefined);
        }
    }

    function setAlertPanel(alertPanel){
        let parameters = {"Id": collectionId, "AlertPanelComp": alertPanel};

        Events.sendEvent("SetAlertPanel", parameters)
    }


    MetaInfoProvider {
        id: metaInfoProvider;

        getMetaInfoGqlCommand: root.collectionId + "MetaInfo";

        onMetaInfoModelChanged: {
            root.metaInfoView.metaInfoModel = metaInfoProvider.metaInfoModel
        }

        onStateChanged: {
            if (state === "Loading"){
                root.metaInfoView.startLoading();
            }
            else{
                root.metaInfoView.stopLoading();
            }
        }
    }

    Component {
        id: alertPanelComp;

        Rectangle {
            anchors.fill: parent;

            color: Style.selectedColor;

            Image {
                id: icon;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 10;

                width: 20;
                height: 20;

                sourceSize.height: height;
                sourceSize.width: width;

                source: "../../../" + Style.getIconPath("Icons/Lamp", Icon.State.On, Icon.Mode.Normal);
            }

            BaseText {
                id: message;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: icon.right;
                anchors.leftMargin: 10;
                anchors.right: updateButton.left;

                text: qsTr("This table has been modified from another computer");
            }

            Button {
                id: updateButton;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                anchors.rightMargin: 10;

                width: 70;
                height: 30;

                text: qsTr("Update");

                onClicked: {
                    root.receiveRemoteChanges()
                }
            }
        }
    }

    function handleSubscription(dataModel){
        if (!dataModel){
            return;
        }

        if (dataModel.ContainsKey("token")){
            let accessToken = dataModel.GetData("token");
            Events.sendEvent("GetToken", function (token){
                if (String(token) == String(accessToken)){
                    root.doUpdateGui();
                }
                else{
                    root.hasRemoteChanges = true;
                }
            });
        }
    }

    SubscriptionClient {
        id: subscriptionClient;

        Component.onDestruction: {
            Events.sendEvent("UnRegisterSubscription", subscriptionClient);
        }

        property bool ok: root.collectionId !== "" && subscriptionClient.subscriptionId !== "";
        onOkChanged: {
            if (ok){
                let subscriptionRequestId = "On" + root.collectionId + "CollectionChanged"
                var query = Gql.GqlRequest("subscription", subscriptionRequestId);
                var inputParams = Gql.GqlObject("input");

                let additionInputParams = root.getAdditionalInputParams();
                if (Object.keys(additionInputParams).length > 0){
                    let additionParams = Gql.GqlObject("addition");
                    for (let key in additionInputParams){
                        additionParams.InsertField(key, additionInputParams[key]);
                    }
                    inputParams.InsertFieldObject(additionParams);
                }

                query.AddParam(inputParams);
                var queryFields = Gql.GqlObject("notification");
                queryFields.InsertField("Id");
                query.AddField(queryFields);
                console.log("RegisterSubscription", subscriptionRequestId);

                Events.sendEvent("RegisterSubscription", {"Query": query, "Client": subscriptionClient});
            }
        }

        onStateChanged: {
            console.log("SubscriptionClient onStateChanged", state);

            if (state === "Ready"){
                if (subscriptionClient.ContainsKey("data")){
                    let dataModelLocal = subscriptionClient.GetData("data")
                    root.handleSubscription(dataModelLocal);
                }
            }
        }
    }
}
