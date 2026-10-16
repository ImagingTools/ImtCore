import QtQuick 2.12
import imtcontrols 1.0


BaseClass {
	id: node

	property int Level__: 0
	property bool Visible__: true
	property bool IsOpen__: false
	property int OpenState__: -1
	property bool HasBranch__: false
	property string BranchIds__: ""
	property string InnerId__: ""
	property int ChildrenCount__: -1
	property int flatIndex: -1
	property var childrenModel: null
}
