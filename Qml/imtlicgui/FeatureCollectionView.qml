import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0

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
        collectionFilter.setSortingInfoId("LastModified");

        let documentManager = MainDocumentManager.getDocumentManager(featureCollectionViewContainer.collectionId);
        if (documentManager){
            featureCollectionViewContainer.commandsDelegate.documentManager = documentManager;

            documentManager.registerDocumentView("Feature", "FeatureEditor", featureDocumentComp);
            documentManager.registerDocumentDataController("Feature", dataControllerComp);
        }
    }

    Component {
        id: featureDocumentComp;

        FeatureEditor {
            id: featureEditor;

            commandsControllerComp: Component {CommandsRepresentationProvider {
                    commandId: "Feature";
                    uuid: featureEditor.viewId;
                }
            }

            commandsDelegateComp: Component {ViewCommandsDelegateBase {
                    view: featureEditor;
                    onCommandActivated: {
                        let selectedIndex = null;
                        if (featureEditor.tableView.tableSelection.items.length > 0){
                            selectedIndex = featureEditor.tableView.tableSelection.items[0];
                        }

                        if (commandId === "New"){
                            if (selectedIndex != null){
                                featureEditor.tableView.addChildItem(selectedIndex, {"FeatureId":"", "FeatureName":"Feature Name", "FeatureDescription":"", "Dependencies":"", "Optional":false, "ChildModel":0})

                                featureEditor.model.dataChanged(null, null);
                            }
                        }
                        else if (commandId === "Remove"){
                            if (selectedIndex != null){
                                featureEditor.tableView.removeChildItem(selectedIndex);
                                featureEditor.model.dataChanged(null, null);
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: dataControllerComp;

        GqlDocumentDataController {
            gqlGetCommandId: "FeatureItem";
            gqlUpdateCommandId: "FeatureUpdate";
            gqlAddCommandId: "FeatureAdd";
        }
    }
}


