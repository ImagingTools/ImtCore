import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0

RemoteCollectionView {
     id: root;

     visibleMetaInfo: false;

     collectionId: "Licenses";

     commandsDelegate: DocumentCollectionViewDelegate {
         collectionView: root;

         documentTypeId: "License";
         viewTypeId: "LicenseEditor";
     }

     Component.onCompleted: {
         collectionFilter.setSortingOrder("DESC");
         collectionFilter.setSortingInfoId("LastModified");

         let documentManager = MainDocumentManager.getDocumentManager(root.collectionId);
         if (documentManager){
             root.commandsDelegate.documentManager = documentManager;

             documentManager.registerDocumentView("License", "LicenseEditor", featureDocumentComp);
             documentManager.registerDocumentDataController("License", dataControllerComp);
         }
     }

     Component {
         id: featureDocumentComp;

         LicenseEditor {
             id: licenceEditor;

             commandsController: CommandsRepresentationProvider {
                 commandId: "License";
                 uuid: licenceEditor.viewId;
             }
         }
     }

     Component {
         id: dataControllerComp;

         GqlDocumentDataController {
             gqlGetCommandId: "LicenseItem";
             gqlUpdateCommandId: "LicenseUpdate";
             gqlAddCommandId: "LicenseAdd";
         }
     }
 }
