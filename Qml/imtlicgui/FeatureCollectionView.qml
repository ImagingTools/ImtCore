import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtlicFeaturesSdl 1.0
import Qt.labs.platform 1.0

RemoteCollectionView {
	id: featureCollectionViewContainer;
	
	visibleMetaInfo: false;
	
	collectionId: "Features";
	
	commandsDelegateComp: Component {DocumentCollectionViewDelegate {
			collectionView: featureCollectionViewContainer;
			documentTypeIds: ["Feature"]
			documentViewTypeIds: ["FeatureEditor"]
			documentViewsComp: [featureDocumentComp]
			documentDataControllersComp: [dataControllerComp]
		}
	}
	
	Component {
		id: messageDialog;
		
		ErrorDialog {}
	}
	
	Component.onCompleted: {
		collectionFilter.setSortingOrder("DESC");
		collectionFilter.setSortingInfoId(FeatureItemTypeMetaInfo.s_lastModified);
	}
	
	Component {
		id: featureDataComp;
		FeatureData {}
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
						let selectedIndex = null;
						if (featureEditor.tableView.tableSelection.items.length > 0){
							selectedIndex = featureEditor.tableView.tableSelection.items[0];
						}
						
						if (commandId === "InsertFeature"){
							let childModel = featureEditor.featureData.m_subFeatures;
							if (selectedIndex != null){
								childModel = selectedIndex.itemData.m_subFeatures;
							}
							
							let newFeatureData = featureDataComp.createObject(childModel)
							newFeatureData.m_isPermission = true;
							newFeatureData.m_featureName = qsTr("Feature Name");
							childModel.append({item: newFeatureData})
							featureEditor.model.modelChanged([])
						}
						else if (commandId === "RemoveFeature"){
							if (selectedIndex != null){
								let parentModel = featureEditor.featureData.m_subFeatures;
								if (selectedIndex.parentIndex != null){
									parentModel = selectedIndex.parentIndex.itemData.m_subFeatures;
								}
								
								parentModel.remove(selectedIndex.index);
								
								featureEditor.model.modelChanged([])
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
			
			property FeatureData featureData: documentModel
			
			gqlGetCommandId: ImtlicFeaturesSdlCommandIds.s_getFeatureItem;
			gqlUpdateCommandId: ImtlicFeaturesSdlCommandIds.s_updateFeature;
			gqlAddCommandId: ImtlicFeaturesSdlCommandIds.s_addFeature;
			
			typeId: "Feature";
			documentName: featureData ? featureData.m_featureName: "";
			documentDescription: featureData ? featureData.m_description: "";
			
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


