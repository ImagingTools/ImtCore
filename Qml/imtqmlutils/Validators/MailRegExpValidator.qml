import QtQuick 2.15

RegularExpressionValidator {
	id: mailValid;

	regularExpression: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
}


