import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtdocgui 1.0

DocumentDataController {
	id: root

	function updateRepresentationFromDocument(params){
		dataModelProvider.requestDataModel(params)
	}

	function updateDocumentFromRepresentation(params){
		dataModelController.saveDataModel(params)
	}

	property GqlBasedDataModelProvider dataModelProvider: GqlBasedDataModelProvider {
        dataModel: root.representationModel
        onDataModelReady: {
            root.representationUpdated(dataModel)
        }
	}

	property GqlBasedDataModelController dataModelController: GqlBasedDataModelController {
        resultModel: root.representationModel
        onResultModelReady: {
            root.documentUpdated()
        }
	}
}


