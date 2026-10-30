import QtQuick 2.12
import imtcontrols 1.0

BaseClass {
	id: canvasTableSdlDocument;

	property BaseModel m_headers: null;
	property BaseModel m_rows: null;

	Component {
		id: headerComponent;

		CanvasTableHeaderSdl {
		}
	}

	Component {
		id: rowComponent;

		CanvasTableRowSdl {
		}
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case "m_headers": return "headers";
			case "m_rows": return "rows";
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
			case "m_headers": return headerComponent;
			case "m_rows": return rowComponent;
		}
	}

	function createComponent(propertyId, typename){
		switch (propertyId){
			case "m_headers":
			case "m_rows":
				return Qt.createComponent("qrc:/qml/imtcontrols/Base/BaseModel.qml");
		}
	}
}
