import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Item {
	id: datePicker;

	height: 20;
	width: content.width;

	// aliases
	property alias selectedYear: yearField.text;
	property alias selectedMonth: monthField.text;
	property alias selectedDay: dayField.text;
	property alias currentDayButtonVisible: currentDate.visible;
	property alias defaultWidth: mainRow.width;
	property alias moveToEndYear: yearComboObj.moveToEnd;
	property alias moveToIndexYear: yearComboObj.moveToIndex;
	property alias monthCombo: monthComboObj;
	property alias yearCombo: yearComboObj;
	property alias dayCombo: dayComboObj;
	property alias contentWidth: content.width;

	property var monthNames:
		[
		qsTr("January"),
		qsTr("February"),
		qsTr("March"),
		qsTr("April"),
		qsTr("May"),
		qsTr("June"),
		qsTr("July"),
		qsTr("August"),
		qsTr("September"),
		qsTr("October"),
		qsTr("November"),
		qsTr("December")
	];

	property int startYear: 2010;
	property int lastYear: 2040;
	property int todayYear;
	property int todayMonth;
	property int selectedIndexMonth;
	property int shownItemsCountCombo: 12;

	property bool completed: false;
	property bool startWithCurrentDay: true;
	property bool startWithFirstDay: false;
	property bool hasDay: true;
	property bool hasDayCombo: true;
	property bool hasMonthCombo: true;
	property bool hasYearCombo: true;
	property bool hasButtons: true;
	property bool openST: monthComboObj.openST || yearComboObj.openST || dayComboObj.openST;
	property bool endComboOnCurrentMonth: false;
	property bool readOnly: false;
	property bool textInputBan: false;
	// icons
	property int iconWidth: 12;
	property int iconHeight: 10;
	property string iconUpSource: readOnly ? "../../../" + Style.getIconPath("Icons/Up", Icon.State.Off, Icon.Mode.Disabled)
										   : "../../../" + Style.getIconPath("Icons/Up", Icon.State.On, Icon.Mode.Normal);
	property string iconDownSource: readOnly ? "../../../" + Style.getIconPath("Icons/Down", Icon.State.Off, Icon.Mode.Disabled)
											 : "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);
	// margins
	property int mainMargin: 0;
	// CustomTextField
	property int textFieldHeight: 20;
	property int textFieldWidthYear: 42;
	property int textFieldWidthMonth: 85;
	property int textFieldWidthDay: 25;
	property int textFieldRadius:0;
	property int textSize: Style.fontSizeNormal;
	property string textFieldColor: Style.baseColor;
	property string fontColor: Style.textColor;
	property string textFieldBorderColor: ""; // textField.acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;
	property int textFieldMargin: 5;
	property int horizontalAlignment: TextInput.AlignHCenter;
	// buttons
	property int buttonWidth: 15;
	property int buttonHeight: 10;
	property string buttonColor:"";
	property int buttonHighlighted: -1;
	property int spinBoxSpacing: 0;
	// combo models
	property TreeItemModel yearTreeModel: TreeItemModel{}
	property TreeItemModel monthTreeModel: TreeItemModel{}
	property TreeItemModel dayTreeModel: TreeItemModel{}

	property var params;

	// signals
	signal dateChanged();

	Component.onCompleted: {
		var date_ = new Date();

		datePicker.selectedIndexMonth = date_.getMonth();
		datePicker.todayMonth = date_.getMonth();
		datePicker.todayYear = date_.getFullYear();

		if (datePicker.startWithCurrentDay || datePicker.startWithFirstDay){
			yearField.text = datePicker.todayYear;
			datePicker.selectedIndexMonth = datePicker.todayMonth;
			dayField.text = datePicker.startWithCurrentDay ? date_.getDate() : "1";
		}

		if(datePicker.hasMonthCombo){
			let monthCount =  datePicker.endComboOnCurrentMonth ? date_.getMonth() + 1
																: datePicker.monthNames.length;
			for(var i = 0; i < monthCount; i++){
				var index = datePicker.monthTreeModel.insertNewItem();
				datePicker.monthTreeModel.setData("id", String(index), index);
				datePicker.monthTreeModel.setData("name",qsTr(datePicker.monthNames[i]),index);
			}
		}

		if(datePicker.hasYearCombo && !yearComboObj.model.getItemsCount()){
			let invalidRange = (datePicker.startYear < 1 || datePicker.lastYear < 1)
							|| (datePicker.startYear > datePicker.todayYear || datePicker.lastYear < datePicker.todayYear)
			if(invalidRange){
				datePicker.startYear = 1900;
				datePicker.lastYear = 2100;
			}

			if(datePicker.startYear > datePicker.lastYear){
				let temp = datePicker.startYear;
				datePicker.startYear = datePicker.lastYear;
				datePicker.lastYear = temp;
			}
			for(let i = datePicker.startYear; i <= datePicker.lastYear;i++){
				let index = datePicker.yearTreeModel.insertNewItem();
				datePicker.yearTreeModel.setData("id", String(index), index);
				datePicker.yearTreeModel.setData("name", String(i),index);
			}
		}

		if(datePicker.hasDayCombo && datePicker.selectedYear !== ""){
			datePicker.fillDayModel();
		}

		datePicker.completed = true;
	}

	onSelectedYearChanged: {
		// console.log("onSelectedYearChanged");
		if(datePicker.hasMonthCombo && datePicker.endComboOnCurrentMonth && datePicker.completed){
			datePicker.fillMonthModel();
		}
		if(datePicker.hasDayCombo && datePicker.completed){
			datePicker.fillDayModel();
		}
		if (datePicker.completed){
			datePicker.dateChanged();
		}
	}

	onSelectedDayChanged: {
		if (datePicker.completed){
			datePicker.dateChanged();
		}
	}

	onSelectedIndexMonthChanged: {
		if (datePicker.completed){
			datePicker.dateChanged();
		}
		if(datePicker.hasDayCombo && datePicker.completed){
			datePicker.fillDayModel();
		}
	}

	onParamsChanged: {
		if(params){
			for (let key in params) {
				if(datePicker[key] !== undefined){
					datePicker[key] = params[key];
				}
			}
		}
	}

	function getDate(){
		let year	= Number(datePicker.selectedYear)
		let month	= datePicker.selectedIndexMonth
		let day		= Number(datePicker.selectedDay)

		let date = new Date(year, month,day)
		let isValid = date.getFullYear() === year &&
						date.getMonth() === month &&
						date.getDate() === day

		return isValid ? date : null
	}

	function getDateAsString(){
		let monthStr = String(datePicker.selectedIndexMonth + 1).padStart(2, '0')
		let dayStr = String(datePicker.selectedDay).padStart(2, '0')

		return datePicker.selectedYear + "-" + monthStr + "-" + dayStr;
	}

	function setCurrentDay(){
		datePicker.setDateFromDateObject(new Date())
	}

	function checkDate(year, month, day){
		let date = new Date(year, month, day);
		let isValid = date.getFullYear() === year &&
						date.getMonth() === month &&
						date.getDate() === day;

		return isValid
	}

	function findIndexByValue(treeModel, fieldId, targetValue){
		if(treeModel === undefined || typeof treeModel.getItemsCount !== 'function'){
			return
		}

		let modelCount = treeModel.getItemsCount()
		for(let i = 0; i < modelCount; i++){
			let gottenValue = treeModel.getData(fieldId, i)
			if(Number(gottenValue) === Number(targetValue)){
				return i
			}
		}

		return -1
	}

	function setDateFromDateObject(dateObj){
		if(!dateObj){
			return
		}

		let isValid = dateObj instanceof Date && !isNaN(dateObj.getTime())
		if(!isValid){
			console.error("Object is not Date typed or is invalid")

			return
		}

		let year	= dateObj.getFullYear()
		let month	= dateObj.getMonth()
		let day		= dateObj.getDate()

		yearField.text = year
		datePicker.selectedIndexMonth = month
		dayField.text = day

		if(datePicker.hasMonthCombo){
			monthComboObj.currentIndex = month
		}

		if(datePicker.hasYearCombo){
			let modelIndex = findIndexByValue(datePicker.yearTreeModel, "name", year)
			if(modelIndex >= 0){
				let id = datePicker.yearTreeModel.getData("id", modelIndex)
				yearComboObj.currentIndex = id
			}
		}

		if(datePicker.hasDayCombo){
			let modelIndex_ = findIndexByValue(datePicker.dayTreeModel, "name", day)
			if(modelIndex_ >= 0){
				let id = datePicker.dayTreeModel.getData("id", modelIndex_)
				dayComboObj.currentIndex = id
			}
		}
	}

	function setDate(year, month, day){
		if (!datePicker.checkDate(year, month, day)){
			return
		}

		yearField.text = year;
		datePicker.selectedIndexMonth = month;
		dayField.text = day;

		if(datePicker.hasMonthCombo){
			monthComboObj.currentIndex = month;
		}
		if(datePicker.hasYearCombo){
			let modelIndex = findIndexByValue(datePicker.yearTreeModel, "name", year)
			if(modelIndex >= 0){
				let id = datePicker.yearTreeModel.getData("id", modelIndex);
				yearComboObj.currentIndex = id;
			}
		}
		if(datePicker.hasDayCombo){
			let modelIndex_ = findIndexByValue(datePicker.dayTreeModel, "name", day)
			if(modelIndex_ >= 0){
				let id = datePicker.dayTreeModel.getData("id", modelIndex_);
				dayComboObj.currentIndex = id;
			}
		}
	}

	function setDateFromString(stringDate){ // ISO-string
		let year	= stringDate.slice(0,4);
		let month	= stringDate.slice(5,7);
		let day		= stringDate.slice(8);
		if(day[0] === "0"){
			day = day.slice(1);
		}
		if(month[0] === "0"){
			month = month.slice(1);
		}

		month = Number(month) - 1;
		setDate(year, month, day);
	}

	function fillDayModel(){
		datePicker.dayTreeModel.clear();
		let lastDay = datePicker.getLastDayOfMonth(datePicker.selectedIndexMonth + 1);
		for(let i = 1; i <= lastDay; i++){
			let index = datePicker.dayTreeModel.insertNewItem();
			datePicker.dayTreeModel.setData("id", String(i - 1), index);
			datePicker.dayTreeModel.setData("name",i,index);
		}
	}

	function fillMonthModel(){
		datePicker.monthTreeModel.clear();

		let monthCount = datePicker.monthNames.length;
		let monthComboIndexPrev = datePicker.selectedIndexMonth;

		if(Number(datePicker.selectedYear) === datePicker.todayYear){
			monthCount = Number(datePicker.todayMonth) + 1;
		}

		for(let i = 0; i < monthCount; i++){
			let index = datePicker.monthTreeModel.insertNewItem();
			datePicker.monthTreeModel.setData("id", String(index), index);
			datePicker.monthTreeModel.setData("name",qsTr(datePicker.monthNames[i]),index);
		}

		if(monthComboIndexPrev > datePicker.monthTreeModel.getItemsCount() - 1){
			monthComboIndexPrev = 0;
		}

		monthComboObj.currentIndex = monthComboIndexPrev;
	}

	function getLastDayOfMonth(month){
		if (month < 1 || month > 12) {
			console.log("getLastDayOfMonth(): invalid parameters (month < 1 || month > 12)");
			return "";
		}

		// new Date(2025, 4, 0) -> day 0 of May -> last day of April
		// day count change supported, leap year supported
		var year = Number(datePicker.selectedYear);
		return new Date(year, month, 0).getDate();
	}

	function isLeapYearFun(year) {
		return (year % 4 === 0 && year % 100 !== 0) || (year % 400 === 0);
	}


	Row {
		id: content;

		height: datePicker.height;
		spacing: Style.sizeMainMargin;

		RegularExpressionValidator {
			id: yearValid;

			regularExpression: /^(19|20)\d{2}$/;
		}

		Row {
			id: mainRow;
			height: parent.height;
			spacing: datePicker.mainMargin;

			Item{
				id: yearItem;

				anchors.verticalCenter: parent.verticalCenter;

				width: yearField.width + yearButtons.width + datePicker.spinBoxSpacing;
				height: datePicker.textFieldHeight;

				MouseArea {
					id: yearMouseArea;

					anchors.fill: yearField;

					enabled: !datePicker.readOnly;
					propagateComposedEvents: true;

					onWheel: {
						if (datePicker.readOnly){
							return;
						}

						if(wheel.angleDelta.y > 0){
							yearUpButton.clicked();
						}
						else {
							yearDownButton.clicked();
						}
					}
				}

				CustomTextField {
					id: yearField;

					anchors.verticalCenter: parent.verticalCenter;

					width: datePicker.textFieldWidthYear;
					height: datePicker.textFieldHeight;
					visible: !yearComboObj.openST;

					radius: datePicker.textFieldRadius;
					textSize: datePicker.textSize;
					fontColor: datePicker.fontColor;
					color: datePicker.textFieldColor;
					// borderColorConst: datePicker.textFieldBorderColor !== "" ? datePicker.textFieldBorderColor : acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;
					margin: datePicker.textFieldMargin;

					leftPadding: horizontalAlignment == TextInput.AlignLeft ? 4 : 0;
					horizontalAlignment: datePicker.horizontalAlignment;

					textInputValidator: yearValid;

					Keys.onUpPressed: {
						yearUpButton.clicked();
					}

					Keys.onDownPressed: {
						yearDownButton.clicked();
					}

					readOnly: true // datePicker.readOnly || datePicker.textInputBan;
				}

				Column {
					id: yearButtons;

					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: yearField.right;
					anchors.leftMargin: visible ? datePicker.spinBoxSpacing : 0;

					width: visible ? datePicker.buttonWidth : 1;
					visible: datePicker.hasButtons;

					Button {
						id: yearUpButton;

						width: datePicker.buttonWidth;
						height: datePicker.buttonHeight;

						iconSource: datePicker.iconUpSource;
						enabled: !datePicker.readOnly;
						decorator: Component { ButtonDecorator {
								color: yearUpButton.hovered ? Style.buttonHoverColor : "transparent";
								border.width: 0;
							}}

						onClicked: {
							if (!enabled){
								return;
							}

							let selectedYear = Number(yearField.text);
							let newYear = String(selectedYear + 1);

							if (datePicker.checkDate(Number(newYear), datePicker.selectedIndexMonth, Number(dayField.text))){
								yearField.text = newYear;
							}
						}
					}

					Button {
						id: yearDownButton;

						width: datePicker.buttonWidth;
						height: datePicker.buttonHeight;

						iconSource: datePicker.iconDownSource;
						enabled: !datePicker.readOnly;
						decorator: Component { ButtonDecorator {
								color: yearDownButton.hovered ? Style.buttonHoverColor : "transparent";
								border.width: 0;
							}}

						onClicked: {
							if (!enabled || yearField.text == ""){
								return;
							}

							let selectedYear = Number(yearField.text);
							let newYear = String(selectedYear - 1);

							if (datePicker.checkDate(Number(newYear), datePicker.selectedIndexMonth, Number(dayField.text))){
								yearField.text = newYear;
							}
						}
					}
				}

				ComboBox {
					id: yearComboObj;

					anchors.fill: yearField;

					visible: datePicker.hasYearCombo;
					enabled: visible;
					model: datePicker.yearTreeModel;

					textSize: datePicker.textSize;
					fontColor: yearField.fontColor;
					borderColor: "transparent";
					backgroundColor: "transparent";
					fontColorTitle: "transparent";
					textCentered: true;
					shownItemsCount: datePicker.shownItemsCountCombo;
					moveToIndex : datePicker.todayYear - datePicker.startYear - datePicker.shownItemsCountCombo +1;
					visibleScrollBar: false;
					isColor: true;

					changeable: !datePicker.readOnly;
					// z: yearField.readOnly ? 1 : 0;

					onCurrentIndexChanged:{
						if(yearComboObj.currentIndex >=0){
							let selectedYear = yearComboObj.currentText;

							if (datePicker.checkDate(Number(selectedYear), datePicker.selectedIndexMonth, Number(dayField.text))){
								yearField.text = selectedYear;
							}
							else{
								yearComboObj.currentIndex = -1;
								monthComboObj.currentIndex = -1;
								dayComboObj.currentIndex = -1;
							}
						}

					}
				} // yearComboObj
			} // yearItem

			Item{
				id: monthItem;
				anchors.verticalCenter: parent.verticalCenter;

				width: monthField.width + monthButtons.width + datePicker.spinBoxSpacing;
				height: datePicker.textFieldHeight;

				MouseArea {
					id: monthMouseArea;

					anchors.fill: monthField;
					enabled: !datePicker.readOnly;
					propagateComposedEvents: true;

					onWheel: {
						if (datePicker.readOnly){
							return;
						}

						if(wheel.angleDelta.y > 0){
							monthUpButton.clicked();
						}
						else {
							monthDownButton.clicked();
						}
					}
				}

				CustomTextField {
					id: monthField;

					anchors.verticalCenter: parent.verticalCenter;

					width: datePicker.textFieldWidthMonth;
					height: parent.height;

					visible: !monthComboObj.openST;

					radius: datePicker.textFieldRadius;
					textSize: datePicker.textSize;
					fontColor: datePicker.fontColor;
					color: datePicker.textFieldColor;
					margin: datePicker.textFieldMargin;
					readOnly: true // datePicker.readOnly || datePicker.textInputBan;

					leftPadding: horizontalAlignment == TextInput.AlignLeft ? 4 : 0;
					horizontalAlignment: datePicker.horizontalAlignment;
					text: datePicker.monthNames[datePicker.selectedIndexMonth];

					Keys.onUpPressed: {
						monthUpButton.clicked();
					}

					Keys.onDownPressed: {
						monthDownButton.clicked();
					}
				}

				Column {
					id: monthButtons;

					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: monthField.right;
					anchors.leftMargin: visible ? datePicker.spinBoxSpacing : 0;

					width: visible ? datePicker.buttonWidth : 1;
					visible: datePicker.hasButtons;

					Button {
						id: monthUpButton;

						width: datePicker.buttonWidth;
						height: datePicker.buttonHeight;

						iconSource: datePicker.iconUpSource;
						enabled: !datePicker.readOnly;
						decorator: Component { ButtonDecorator {
								color: monthUpButton.hovered ? Style.buttonHoverColor : "transparent";
								border.width: 0;
							}}

						onClicked: {
							if (!enabled){
								return;
							}
							let index = 0;
							if (datePicker.selectedIndexMonth < 11){
								index = datePicker.selectedIndexMonth + 1;
							}

							if (datePicker.checkDate(Number(yearField.text), index, Number(dayField.text))){
								datePicker.selectedIndexMonth = index;
							}
						}
					}

					Button {
						id: monthDownButton;

						width: datePicker.buttonWidth;
						height: datePicker.buttonHeight;

						iconSource: datePicker.iconDownSource;

						enabled: !datePicker.readOnly;

						decorator: Component { ButtonDecorator {
								color: monthDownButton.hovered ? Style.buttonHoverColor : "transparent";
								border.width: 0;
							}}

						onClicked: {
							if (!enabled){
								return;
							}

							let index = 11;
							if (datePicker.selectedIndexMonth > 0){
								index = datePicker.selectedIndexMonth - 1;
							}

							if (datePicker.checkDate(Number(yearField.text), index, Number(dayField.text))){
								datePicker.selectedIndexMonth = index;
							}
						}
					}
				}

				ComboBox {
					id: monthComboObj;

					anchors.fill: monthField;

					visible: datePicker.hasMonthCombo;
					enabled: visible;
					model: datePicker.monthTreeModel;

					textSize: datePicker.textSize;//Style.fontSizeNormal;
					fontColor: monthField.fontColor;
					borderColor: "transparent";
					backgroundColor: "transparent";
					fontColorTitle: "transparent";
					textCentered: true;
					shownItemsCount: 12;
					visibleScrollBar: false;
					isColor: true;
					changeable: !datePicker.readOnly;

					onCurrentIndexChanged:{
						if(monthComboObj.currentIndex >= 0){
							if (datePicker.checkDate(Number(yearField.text), monthComboObj.currentIndex, Number(dayField.text))){
								datePicker.selectedIndexMonth = monthComboObj.currentIndex;
							}
							else{
								yearComboObj.currentIndex = -1;
								monthComboObj.currentIndex = -1;
								dayComboObj.currentIndex = -1;
							}
						}
					}
				} // monthComboObj

			} // monthItem

			RegularExpressionValidator {
				id: dayValid;

				regularExpression: /\b([1-9]|[12][0-9]|3[01])\b/;
			}

			Item {
				id: dayItem;

				anchors.verticalCenter: parent.verticalCenter;

				visible: datePicker.hasDay;

				width: dayField.width + dayButtons.width + datePicker.spinBoxSpacing;
				height: datePicker.textFieldHeight;

				MouseArea {
					id: daysMouseArea;

					anchors.fill: dayField;

					enabled: !datePicker.readOnly;

					propagateComposedEvents: true;

					onWheel: {
						if(wheel.angleDelta.y > 0){
							dayUpButton.clicked();
						}
						else {
							dayDownButton.clicked();
						}
					}
				}

				CustomTextField {
					id: dayField;

					anchors.verticalCenter: parent.verticalCenter;

					visible: datePicker.hasDay && !dayComboObj.openST;

					width: datePicker.textFieldWidthDay;
					height: parent.height;

					radius: datePicker.textFieldRadius;
					textSize: datePicker.textSize;
					fontColor: datePicker.fontColor;
					color: datePicker.textFieldColor;
					margin: datePicker.textFieldMargin;
					readOnly: true;
					leftPadding: horizontalAlignment == TextInput.AlignLeft ? 4 : 0;
					horizontalAlignment: datePicker.horizontalAlignment;
					textInputValidator: dayValid;

					Keys.onUpPressed: {
						if (datePicker.readOnly){
							return;
						}

						dayUpButton.clicked();
					}

					Keys.onDownPressed: {
						if (datePicker.readOnly){
							return;
						}

						dayDownButton.clicked();
					}
				}

				Column {
					id: dayButtons;

					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: dayField.right;
					anchors.leftMargin: visible ? datePicker.spinBoxSpacing : 0;

					width: visible ? datePicker.buttonWidth : 1;

					visible: datePicker.hasDay && datePicker.hasButtons;

					Button {
						id: dayUpButton;

						width: datePicker.buttonWidth;
						height: datePicker.buttonHeight;

						iconSource: datePicker.iconUpSource;
						enabled: !datePicker.readOnly;
						decorator: Component { ButtonDecorator {
								color: dayUpButton.hovered ? Style.buttonHoverColor : "transparent";
								border.width: 0;
							}}

						onClicked: {
							if (!enabled){
								return;
							}

							let selectedDay = Number(dayField.text);
							if (selectedDay < 31){
								let newDay = String(selectedDay + 1)

								if (datePicker.checkDate(Number(yearField.text), datePicker.selectedIndexMonth, Number(newDay))){
									dayField.text = newDay;
								}
							}
						}
					}

					Button {
						id: dayDownButton;

						width: datePicker.buttonWidth;
						height: datePicker.buttonHeight;

						iconSource: datePicker.iconDownSource;
						enabled: !datePicker.readOnly;
						decorator: Component { ButtonDecorator {
								color: dayDownButton.hovered ? Style.buttonHoverColor : "transparent";
								border.width: 0;
							}}

						onClicked: {
							if (!enabled){
								return;
							}

							let selectedDay = Number(dayField.text);
							if (selectedDay > 1){
								let newDay = String(selectedDay - 1);

								if (datePicker.checkDate(Number(yearField.text), datePicker.selectedIndexMonth, Number(newDay))){
									dayField.text = newDay;
								}
							}
						}
					}
				}

				ComboBox {
					id: dayComboObj;

					anchors.fill: dayField;

					visible: datePicker.hasDayCombo;
					enabled: visible;

					model: datePicker.dayTreeModel;

					textSize: datePicker.textSize;
					fontColor: dayField.fontColor;
					borderColor: "transparent";
					backgroundColor: "transparent";
					fontColorTitle: "transparent";
					textCentered: true;
					shownItemsCount: datePicker.shownItemsCountCombo;
					visibleScrollBar: false;
					isColor: true;
					changeable: !datePicker.readOnly;

					onCurrentIndexChanged:{
						if(dayComboObj.currentIndex >=0){
							let selectedDay = dayComboObj.currentText;

							if (datePicker.checkDate(Number(datePicker.selectedYear), datePicker.selectedIndexMonth, Number(selectedDay))){
								dayField.text = selectedDay;
							}
							else{
								yearComboObj.currentIndex = -1;
								monthComboObj.currentIndex = -1;
								dayComboObj.currentIndex = -1;
							}
						}
					}
				} // dayComboObj
			} // dayItem
		}

		Rectangle {
			id: currentDate;

			anchors.verticalCenter: parent.verticalCenter;

			width: 7;
			height: width;
			radius: width;
			color: Style.textColor;

			MouseArea {
				anchors.fill: parent;

				cursorShape: Qt.PointingHandCursor;

				onClicked: {
					datePicker.setCurrentDay();
				}
			}
		}
	}
}
