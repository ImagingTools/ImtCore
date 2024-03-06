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

    dataController: CollectionRepresentation {
        collectionId: root.collectionId;

        additionalFieldIds: root.additionalFieldIds;
    }

    commandsController: CommandsRepresentationProvider {
        commandId: root.collectionId;
        uuid: root.viewId;
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

    function receiveRemoteChanges(){
        if (hasRemoteChanges){
            hasRemoteChanges = false;

            root.doUpdateGui();
            root.setAlertPanel(undefined);
        }
    }

    function setAlertPanel(alertPanel){
    }

    MetaInfoProvider {
        id: metaInfoProvider;

        getMetaInfoGqlCommand: root.collectionId + "MetaInfo";

        onMetaInfoModelChanged: {
            root.setMetaInfoModel(metaInfoProvider.metaInfoModel);
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
                    Events.sendEvent("UpdateAllModels");
                }
            }
        }
    }

    SubscriptionClient {
        id: subscriptionClient;

        property bool ok: root.collectionId !== "" && subscriptionClient.subscriptionId !== "";
        onOkChanged: {
            if (ok){
                let subscriptionRequestId = "On" + root.collectionId + "CollectionChanged"
                var query = Gql.GqlRequest("subscription", subscriptionRequestId);
                var queryFields = Gql.GqlObject("notification");
                queryFields.InsertField("Id");
                query.AddField(queryFields);

                Events.sendEvent("RegisterSubscription", {"Query": query, "Client": subscriptionClient});
            }
        }

        onStateChanged: {
            console.log("SubscriptionClient onStateChanged", state);

            if (state === "Ready"){
                if (subscriptionClient.ContainsKey("data")){
                    root.doUpdateGui();

//                    let dataModelLocal = subscriptionClient.GetData("data")
//                    if (dataModelLocal.ContainsKey("token")){
//                        let accessToken = dataModelLocal.GetData("token");
//                        Events.sendEvent("GetToken", function (token){
//                            if (String(token) == String(accessToken)){
//                                root.doUpdateGui();
//                            }
//                            else{
//                                root.hasRemoteChanges = true;
//                            }
//                        });
//                    }
                }
            }
        }
    }
}
