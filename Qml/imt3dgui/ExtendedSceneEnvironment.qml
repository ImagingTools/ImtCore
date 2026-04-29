import QtQuick 2.15

SceneEnvironment {
    property bool aoEnabled: false
    property real aoStrength: 0
    property real aoDistance: 5
    property real aoSoftness: 50
    property int aoSampleRate: 2
    property real aoBias: 0

    property bool glowEnabled: false
    property real glowStrength: 1
    property real glowIntensity: 0.8
    property real glowBloom: 0
    property int glowBlendMode: 0
    property int glowQualityHigh: 0

    property bool depthOfFieldEnabled: false
    property real depthOfFieldFocusDistance: 600
    property real depthOfFieldFocusRange: 100
    property real depthOfFieldBlurAmount: 4

    property bool vignetteEnabled: false
    property real vignetteStrength: 15
    property color vignetteColor: "#000000ff"
    property real vignetteRadius: 0.35

    property bool lensFlareEnabled: false
    property real lensFlareBloomScale: 10
    property real lensFlareBloomBias: 0
    property real lensFlareDistortion: 5

    property bool colorAdjustmentsEnabled: false
    property real colorAdjustmentsBrightness: 0
    property real colorAdjustmentsContrast: 1
    property real colorAdjustmentsSaturation: 1
}
