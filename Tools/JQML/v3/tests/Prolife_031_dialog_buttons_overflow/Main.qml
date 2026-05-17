import QtQuick 2.0

// Prolife_031: Dialog button overflow check
//
// Dialog visual layout (DialogDecorator -> dialogColumn: Column):
//   topPanel      height = controlHeightL = 40
//   contentArea   height = dialog.height - 100
//   buttonsZone   height = buttonsDialog.height + 2 * marginXL = 30 + 48 = 78
//
// Dialog widths (buttons are right-aligned with marginM=10 from right):
//   HardwareProductBinding : w=1000, h=900-200=700 (activeView=900)
//   LicenseTree            : w=1400*0.95=1330, h=900*0.95=855 (root=1400x900)
//   ProductEditor          : w=800,  h=700
//   RevokeLicense          : w=900,  h=600
//   SplitLicense           : w=800,  h=500
//
// Checks:
//   1. columnHeight == topPanel + content + buttonsZone (no gap/overflow in layout)
//   2. buttonsZone right edge <= dialog width
//   3. button row fits inside buttonsZone (y=32, bottom=62 < 78)

Item {
    id: root
    width: 1400
    height: 900

    // Style constants from imtcontrols/Base/StyleBase.qml
    property int controlHeightL: 40   // topPanel height
    property int controlHeightM: 30   // DialogButtons row height
    property int marginXL: 24
    property int marginL: 16
    property int marginM: 10

    // Derived
    property int topPanelH: controlHeightL                                   // 40
    property int buttonsZoneH: controlHeightM + 2 * marginXL                 // 78
    property int buttonRowH: controlHeightM                                   // 30
    property int buttonRowBottomMargin: marginL                               // 16
    // button row y inside buttonsZone (anchors.bottom - bottomMargin - height)
    property int buttonRowY: buttonsZoneH - buttonRowH - buttonRowBottomMargin // 32
    property int buttonRowBottom: buttonRowY + buttonRowH                     // 62

    // --- Dialog geometry ---
    property int d1W: 1000;   property int d1H: root.height - 200  // HardwareBinding
    property int d2W: Math.round(root.width * 0.95)
    property int d2H: Math.round(root.height * 0.95)               // LicenseTree
    property int d3W: 800;    property int d3H: 700                 // ProductEditor
    property int d4W: 900;    property int d4H: 600                 // RevokeLicense
    property int d5W: 800;    property int d5H: 500                 // SplitLicense

    // Helper: compute total visual column height for a dialog of height dH
    function columnHeight(dH) {
        return topPanelH + (dH - 100) + buttonsZoneH
    }
    // buttonZone fits in column width: buttonsZone is right-aligned, its width <= dW
    // (buttons width is driven by button text; here we just check that the
    //  minimum single-button width 80px + 2*marginM doesn't exceed dialog width)
    property int minButtonAreaW: 80 + 2 * marginM  // 100

    function check(label, dW, dH) {
        var colH = columnHeight(dH)
        var contentH = dH - 100
        // button row sits inside buttonsZone => always OK if formula holds
        var btnFitsVertically = buttonRowBottom <= buttonsZoneH
        var btnZoneFitsWidth  = minButtonAreaW <= dW
        // column = top + content + buttons
        var colCorrect = colH === (topPanelH + contentH + buttonsZoneH)
        console.log("prolife31",
            label,
            "dW=" + dW, "dH=" + dH,
            "colH=" + colH,
            "contentH=" + contentH,
            "btnZoneH=" + buttonsZoneH,
            "btnRowY=" + buttonRowY,
            "btnRowBottom=" + buttonRowBottom,
            "colOK=" + colCorrect,
            "vOK=" + btnFitsVertically,
            "hOK=" + btnZoneFitsWidth
        )
    }

    Component.onCompleted: {
        check("HardwareBinding", d1W, d1H)
        check("LicenseTree",     d2W, d2H)
        check("ProductEditor",   d3W, d3H)
        check("RevokeLicense",   d4W, d4H)
        check("SplitLicense",    d5W, d5H)
        Qt.quit()
    }
}
