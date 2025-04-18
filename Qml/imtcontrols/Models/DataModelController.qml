import QtQuick 2.12

/*!
	\qmltype DataController
	\inqmlmodule Custom
	\brief Base class for loading model data.

	The DataController is an abstract base class that defines an interface for
	loading data. It provides signals to indicate successful or failed data retrieval.
	Subclasses should override the \l saveDataModel() method to implement specific
	data-fetching logic.
*/
QtObject {

	/*!
		\qmlproperty var DataController::resultModel
		\brief Holds the model data used in QML components.

		This property stores the data retrieved by the provider. It should be
		updated by subclasses when new data is loaded.
	*/
	property var resultModel

	/*!
		\qmlsignal DataController::resultModelReady(var params)
		\brief Emitted when data is successfully loaded.

		The \a params argument contains any parameters passed when requesting the data.
	*/
	signal resultModelReady(var resultModel)

	/*!
		\qmlsignal DataController::dataModelLoadFailed(string errorMessage)
		\brief Emitted when data loading fails.

		The \a errorMessage argument provides details about the failure.
	*/
	signal dataModelSaveFailed(string errorMessage)

	/*!
		\qmlmethod void DataController::saveDataModel(var params)
		\brief Loads model data.

		This method should be overridden in subclasses to fetch data from a static
		source or a remote server.

		\a params contains any parameters required for the data request.

		The base implementation does not perform an actual data retrieval and
		should not be used directly.
	*/
	function saveDataModel(params){
		console.warn("saveDataModel() should be implemented in a subclass")

		resultModelReady(resultModel)
	}


}

