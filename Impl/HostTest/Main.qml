// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
//
// HostTest — main window.
//
// All modules listed in the imports below are fully available because
// Main.cpp initialises their resources and registers "qrc:/qml" as a QML
// import path.  Add more imports or instantiate any exported type here to
// exercise individual components during development.
//

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

// ── ImtCore QML modules ──────────────────────────────────────────────────
import imtcontrols 1.0
import imtgui 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtlicgui 1.0
import imtcolgui 1.0
import imtdocgui 1.0
import imtchatgui 1.0
import imtdeskgui 1.0

// ── SDL-generated QML modules ────────────────────────────────────────────
import imtappApplicationSdl 1.0
import imtauthUsersSdl 1.0
import imtauthProfileSdl 1.0
import imtauthGroupsSdl 1.0
import imtauthRolesSdl 1.0
import imtbaseCommandsSdl 1.0
import imtbaseSearchSdl 1.0
import imtbaseSettingsSdl 1.0
import imtbaseImtBaseTypesSdl 1.0
import imtbaseImtCollectionSdl 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtbaseDocumentRevisionSdl 1.0
import imtbaseUndoManagerSdl 1.0
import imtbaseProgressManagerSdl 1.0
import imtcolorLabSdl 1.0
import imtcolorRgbColorHexSdl 1.0
import imt2dGeometrySdl 1.0
import imt2dPaintSystemSdl 1.0
import imtlicProductsSdl 1.0
import imtlicLicensesSdl 1.0
import imtlicFeaturesSdl 1.0
import imtchatImtChatSdl 1.0
import imtdeskImtDeskSdl 1.0
import imtpayImtPaySdl 1.0

Window {
	id: root

	width: 900
	height: 640
	minimumWidth: 640
	minimumHeight: 480
	visible: true
	title: "HostTest — ImtCore QML Component Host"
	color: "#1e1e2e"

	// ── SDL model instances ───────────────────────────────────────────────
	// Instantiate a few SDL-generated data models to verify they load.

	ApplicationInfo { id: appInfo }
	SearchInput     { id: searchInput }

	// ── Root layout ───────────────────────────────────────────────────────
	ColumnLayout {
		anchors.fill: parent
		anchors.margins: 12
		spacing: 8

		// Header bar
		Rectangle {
			Layout.fillWidth: true
			height: 44
			color: "#313244"
			radius: 6

			Text {
				anchors.centerIn: parent
				text: "HostTest — ImtCore QML Component Host"
				color: "#cdd6f4"
				font.pixelSize: 16
				font.bold: true
			}
		}

		// Tab bar
		TabBar {
			id: tabBar
			Layout.fillWidth: true
			background: Rectangle { color: "#181825" }

			TabButton {
				text: "imtcontrols"
				width: implicitWidth + 16
				contentItem: Text { text: parent.text; color: tabBar.currentIndex === 0 ? "#cba6f7" : "#a6adc8"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
				background: Rectangle { color: tabBar.currentIndex === 0 ? "#313244" : "transparent" }
			}
			TabButton {
				text: "imtgui / imtguigql"
				width: implicitWidth + 16
				contentItem: Text { text: parent.text; color: tabBar.currentIndex === 1 ? "#cba6f7" : "#a6adc8"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
				background: Rectangle { color: tabBar.currentIndex === 1 ? "#313244" : "transparent" }
			}
			TabButton {
				text: "SDL modules"
				width: implicitWidth + 16
				contentItem: Text { text: parent.text; color: tabBar.currentIndex === 2 ? "#cba6f7" : "#a6adc8"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
				background: Rectangle { color: tabBar.currentIndex === 2 ? "#313244" : "transparent" }
			}
		}

		// Tab content
		StackLayout {
			Layout.fillWidth: true
			Layout.fillHeight: true
			currentIndex: tabBar.currentIndex

			// ── Tab 1 : imtcontrols ───────────────────────────────────────
			ScrollView {
				clip: true
				contentWidth: availableWidth

				ColumnLayout {
					width: parent.width
					spacing: 16

					SectionTitle { text: "imtcontrols — base types" }

					// BaseClass (root of SDL models)
					InfoRow {
						label: "BaseClass"
						value: "QtObject-based root for all SDL-generated models"
					}

					// ControlBase — pure-QML FocusScope used by all controls
					InfoRow {
						label: "ControlBase"
						value: "FocusScope root for all imtcontrols visual widgets"
					}

					SectionTitle { text: "imtcontrols — available widgets (sample)" }

					Text {
						color: "#a6e3a1"
						text: "Button · TextField · CheckBox · ComboBox · Slider\n" +
							  "SpinBox · DatePicker · CalendarView · Table · TreeView\n" +
							  "BarChart · Piechart · SplitView · StackView · SwipeView\n" +
							  "… (see ImtCore/Qml/imtcontrols/ for the full list)"
						font.pixelSize: 13
						wrapMode: Text.WordWrap
						Layout.fillWidth: true
					}

					SectionTitle { text: "imtgui — application components (sample)" }

					Text {
						color: "#a6e3a1"
						text: "ApplicationMain · Configurator · LanguageProvider\n" +
							  "PagesManager · PagesProvider · AlertMessage\n" +
							  "TabPanel · MenuPanel · ButtonPanel · TopCenterPanel\n" +
							  "… (see ImtCore/Qml/imtgui/ for the full list)"
						font.pixelSize: 13
						wrapMode: Text.WordWrap
						Layout.fillWidth: true
					}
				}
			}

			// ── Tab 2 : imtgui / imtguigql ───────────────────────────────
			ScrollView {
				clip: true
				contentWidth: availableWidth

				ColumnLayout {
					width: parent.width
					spacing: 16

					SectionTitle { text: "imtguigql — GraphQL-aware components" }

					Text {
						color: "#a6e3a1"
						text: "ClientStyle · GqlBasedDataModelController\n" +
							  "SubscriptionManager · SubscriptionClient\n" +
							  "ComboBoxGql · GqlRequestSender · GqlSdlRequestSender\n" +
							  "GqlBasedPermissionsProvider · GqlBasedCommandsController\n" +
							  "… (see ImtCore/Qml/imtguigql/ for the full list)"
						font.pixelSize: 13
						wrapMode: Text.WordWrap
						Layout.fillWidth: true
					}

					SectionTitle { text: "imtauthgui · imtlicgui · imtcolgui · imtdocgui" }

					Text {
						color: "#a6e3a1"
						text: "Authorization and session UI · License management UI\n" +
							  "Colour picker UI · Document viewer / editor UI\n" +
							  "… (see ImtCore/Qml/imtauthgui/, imtlicgui/, etc.)"
						font.pixelSize: 13
						wrapMode: Text.WordWrap
						Layout.fillWidth: true
					}

					SectionTitle { text: "imtchatgui · imtdeskgui" }

					Text {
						color: "#a6e3a1"
						text: "Chat/conversation UI · Desk / ticket management UI"
						font.pixelSize: 13
						wrapMode: Text.WordWrap
						Layout.fillWidth: true
					}
				}
			}

			// ── Tab 3 : SDL modules ───────────────────────────────────────
			ScrollView {
				clip: true
				contentWidth: availableWidth

				ColumnLayout {
					width: parent.width
					spacing: 16

					SectionTitle { text: "imtappApplicationSdl — ApplicationInfo model" }

					InfoRow { label: "m_version";         value: appInfo.m_version         || "<empty>" }
					InfoRow { label: "m_productName";     value: appInfo.m_productName     || "<empty>" }
					InfoRow { label: "m_companyName";     value: appInfo.m_companyName     || "<empty>" }
					InfoRow { label: "m_applicationId";   value: appInfo.m_applicationId   || "<empty>" }
					InfoRow { label: "m_applicationType"; value: appInfo.m_applicationType || "<empty>" }

					SectionTitle { text: "All loaded SDL QML modules" }

					Text {
						color: "#89b4fa"
						font.pixelSize: 13
						wrapMode: Text.WordWrap
						Layout.fillWidth: true
						text:
							"imtbasesdl\n" +
							"  imtbaseCommandsSdl · imtbaseSearchSdl · imtbaseSettingsSdl\n" +
							"  imtbaseImtBaseTypesSdl · imtbaseImtCollectionSdl\n" +
							"  imtbaseCollectionDocumentServiceSdl · imtbaseCollectionDocumentManagerSdl\n" +
							"  imtbaseDocumentRevisionSdl · imtbaseUndoManagerSdl\n" +
							"  imtbaseProgressManagerSdl · imtbaseMimeTypeSdl\n" +
							"  imtbaseDocumentCollectionFilterSdl · imtbaseComplexCollectionFilterSdl\n" +
							"  imtbaseDocumentIdFilterSdl · imtbaseCollectionImportSdl\n" +
							"  imtbaseFilterableSelectSdl\n\n" +
							"imtappsdl\n" +
							"  imtappApplicationSdl\n\n" +
							"imtauthsdl\n" +
							"  imtauthUsersSdl · imtauthGroupsSdl · imtauthRolesSdl\n" +
							"  imtauthProfileSdl · imtauthSessionsSdl\n" +
							"  imtauthAuthorizationSdl · imtauthAuthorizationOptionsSdl\n" +
							"  imtauthPersonalAccessTokensSdl · imtauthTenantsSdl\n" +
							"  imtauthTenantMembershipsSdl · imtauthUserActionsSdl\n" +
							"  imtauthUserCollectionDocumentServiceSdl\n" +
							"  imtauthGroupCollectionDocumentServiceSdl\n" +
							"  imtauthRoleCollectionDocumentServiceSdl\n" +
							"  imtauthRelationshipCollectionDocumentServiceSdl\n" +
							"  imtauthCrossOrgGrantCollectionDocumentServiceSdl\n" +
							"  imtauthTenantCollectionDocumentServiceSdl\n" +
							"  imtauthTenantCollectionDocumentManagerSdl\n\n" +
							"imtcolorsdl\n" +
							"  imtcolorLabSdl · imtcolorRgbColorHexSdl\n\n" +
							"imt2dsdl\n" +
							"  imt2dGeometrySdl · imt2dPaintSystemSdl\n\n" +
							"imtlicsdl\n" +
							"  imtlicProductsSdl · imtlicLicensesSdl · imtlicFeaturesSdl\n\n" +
							"imtchatsdl\n" +
							"  imtchatImtChatSdl · imtchatConversationCollectionDocumentServiceSdl\n" +
							"  imtchatConversationCollectionDocumentManagerSdl\n\n" +
							"imtdesksdl\n" +
							"  imtdeskImtDeskSdl · imtdeskTicketCollectionDocumentServiceSdl\n" +
							"  imtdeskTicketCollectionDocumentManagerSdl\n\n" +
							"imtpaysdl\n" +
							"  imtpayImtPaySdl"
					}
				}
			}
		}
	}

	// ── Inline helper components ─────────────────────────────────────────

	component SectionTitle: Text {
		required property string text
		Layout.fillWidth: true
		color: "#cba6f7"
		font.pixelSize: 14
		font.bold: true
		topPadding: 8
		bottomPadding: 2
	}

	component InfoRow: RowLayout {
		required property string label
		required property string value
		Layout.fillWidth: true
		spacing: 8

		Text {
			text: label + ":"
			color: "#89b4fa"
			font.pixelSize: 13
			Layout.minimumWidth: 180
		}
		Text {
			text: value
			color: "#cdd6f4"
			font.pixelSize: 13
			Layout.fillWidth: true
			wrapMode: Text.WordWrap
		}
	}
}
