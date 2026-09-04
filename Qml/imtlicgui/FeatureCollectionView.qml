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
			// commandsDelegateComp is now defined inside FeatureEditor itself.
		}
	}
	
	Component {
		id: dataControllerComp;
		
		GqlRequestDocumentDataController {
			id: requestDocumentDataController
			
			property FeatureData featureData: documentModel
			
			gqlGetCommandId: ImtlicFeaturesSdlCommandIds.s_getFeatureItem;
			getPermissionPath: "/FeatureManagement/ViewFeatures";
			gqlUpdateCommandId: ImtlicFeaturesSdlCommandIds.s_updateFeature;
			updatePermissionPath: "/FeatureManagement/EditFeature/ChangeFeature";
			gqlAddCommandId: ImtlicFeaturesSdlCommandIds.s_addFeature;
			addPermissionPath: "/FeatureManagement/EditFeature/AddFeature";
			
			typeId: "Feature";
			documentName: featureData ? featureData.m_featureName: "";
			documentDescription: featureData ? featureData.m_description: "";
			
			documentModelComp: Component {
				FeatureData {}
			}
		}
	}
}


