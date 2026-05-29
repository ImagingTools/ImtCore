import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
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
	documentCollectionFilter: null
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
		table.setSortingInfo(FeatureItemTypeMetaInfo.s_timeStamp, "DESC");
	}
	
	Component {
		id: featureDataComp;
		FeatureData {}
	}
	
	Component {
		id: featureDocumentComp;
		
		FeatureEditor {
			id: featureEditor;
			
			commandsControllerComp: Component {GqlBasedCommandsController {
					typeId: "Feature";
				}
			}
			
			commandsDelegateComp: Component {ViewCommandsDelegateBase {
					view: featureEditor;
					onCommandActivated: {
						let selectedIndex = featureEditor.tableView.currentIndex;
						
						if (commandId === "InsertFeature"){
							if (!featureEditor.featureData.hasSubFeatures()){
								featureEditor.featureData.emplaceSubFeatures()
							}

							let childModel = featureEditor.featureData.m_subFeatures;
							if (selectedIndex != null && selectedIndex.item){
								let selectedItemData = selectedIndex.item.itemData;
								if (selectedItemData) {
									if (!selectedItemData.hasSubFeatures()){
										selectedItemData.emplaceSubFeatures()
									}
									childModel = selectedItemData.m_subFeatures;
								}
							}
							let newFeatureData = featureDataComp.createObject(childModel)
							newFeatureData.m_isPermission = true;
							newFeatureData.m_featureName = qsTr("Feature Name");
							childModel.addElement(newFeatureData)
							featureEditor.model.modelChanged([])
						}
						else if (commandId === "RemoveFeature"){
							if (selectedIndex != null && selectedIndex.item){
								let parentModel = featureEditor.featureData.m_subFeatures;
								if (selectedIndex.parentKey && selectedIndex.parentKey !== "") {
									let parentNode = featureEditor.tableView.nodeForKey(selectedIndex.parentKey);
									if (parentNode && parentNode.sourceItem && parentNode.sourceItem.itemData) {
										parentModel = parentNode.sourceItem.itemData.m_subFeatures;
									}
								}
								
								parentModel.remove(selectedIndex.item.sourceRow);
								
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
		}
	}
}


