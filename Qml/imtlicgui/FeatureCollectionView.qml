import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtlicFeaturesSdl 1.0

RemoteCollectionView {
    id: featureCollectionViewContainer;

    visibleMetaInfo: false;

    collectionId: "Features";

    commandsDelegateComp: Component {DocumentCollectionViewDelegate {
            collectionView: featureCollectionViewContainer;

            documentTypeId: "Feature";
            viewTypeId: "FeatureEditor";
        }
    }

    Component.onCompleted: {
        collectionFilter.setSortingOrder("DESC");
        collectionFilter.setSortingInfoId(FeatureItemTypeMetaInfo.s_lastModified);

        let documentManager = MainDocumentManager.getDocumentManager(featureCollectionViewContainer.collectionId);
        if (documentManager){
            featureCollectionViewContainer.commandsDelegate.documentManager = documentManager;

            documentManager.registerDocumentView("Feature", "FeatureEditor", featureDocumentComp);
            documentManager.registerDocumentDataController("Feature", dataControllerComp);
        }
    }

    Component {
        id: treeItemModelComp;

        TreeItemModel {}
    }

    Component {
        id: featureDocumentComp;

        FeatureEditor {
            id: featureEditor;

            commandsControllerComp: Component {CommandsPanelController {
                    commandId: "Feature";
                    uuid: featureEditor.viewId;
                    commandsView: featureEditor.commandsView;
                }
            }

            commandsDelegateComp: Component {ViewCommandsDelegateBase {
                    view: featureEditor;
                    onCommandActivated: {
                        console.log("onCommandActivated", commandId);
                        let selectedIndex = null;
                        if (featureEditor.tableView.tableSelection.items.length > 0){
                            selectedIndex = featureEditor.tableView.tableSelection.items[0];
                        }

                        if (commandId === "InsertFeature"){
                            if (selectedIndex != null){
                                let childModel = selectedIndex.getData("ChildModel");

                                let emptyModel = treeItemModelComp.createObject(childModel)
                                childModel.insertNewItemWithParameters(0, {"FeatureId":"", "FeatureName":"Feature Name", "FeatureDescription":"", "Dependencies":"", "Optional":false, "ChildModel": emptyModel});
                            }
                        }
                        else if (commandId === "RemoveFeature"){
                            if (selectedIndex != null){
                                let parentModel = selectedIndex.getParentModel();

                                parentModel.removeItem(selectedIndex.index);
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: dataControllerComp;

        GqlRequestDocumentDataController {
            id: requestDocumentDataController

            gqlGetCommandId: ImtlicFeaturesSdlCommandIds.s_featureItem;
            gqlUpdateCommandId: ImtlicFeaturesSdlCommandIds.s_featureUpdate;
            gqlAddCommandId: ImtlicFeaturesSdlCommandIds.s_featureAdd;

            documentModelComp: Component {
                FeatureData {}
            }

            payloadModel: FeatureDataPayload {
                onFinished: {
                    requestDocumentDataController.documentModel = m_featureData;
                }
            }
        }
    }
}


