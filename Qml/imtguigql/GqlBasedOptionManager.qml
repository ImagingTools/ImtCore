import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0
import imtbaseImtCollectionSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

OptionManager {
	id: dataProvider

	property alias inputModel: dataModelProvider.inputModel
	property alias responseModel: dataModelProvider.responseModel
	property alias getCommandId: dataModelProvider.getCommandId

	optionModel: dataModelProvider.dataModel

	function requestDataModel(params){
		dataModelProvider.inputModel = params
		dataModelProvider.requestDataModel()
	}

	GqlBasedDataModelProvider {
		id: dataModelProvider

		dataModel: optionsListModel.m_options

		responseModel: OptionsList { id: optionsListModel }

		onDataModelReady: {
			dataProvider.dataModelReady(params)
		}

		onDataModelLoadFailed: {
			dataProvider.dataModelLoadFailed(errorMessage)
		}
	}
}


