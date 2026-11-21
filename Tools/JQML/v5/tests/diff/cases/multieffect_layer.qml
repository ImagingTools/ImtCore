import QtQuick
import QtQuick.Effects

// Item.layer with a MultiEffect (the API: defaults, the source of the effect, a change), ShaderEffectSource
Item {
    width: 200; height: 100
    Image { id: img; width: 32; height: 32
        layer.enabled: true
        layer.effect: MultiEffect { id: fx; brightness: 1.0; colorization: 1.0; colorizationColor: "#2196f3" } }
    Rectangle { id: r; x: 50; width: 60; height: 30; radius: 10
        layer.enabled: true
        layer.effect: MultiEffect { maskEnabled: true; maskSource: ShaderEffectSource { sourceItem: Rectangle { width: 60; height: 30; radius: 10 } } } }
    MultiEffect { id: plain }
    ShaderEffectSource { id: ses }
    Component.onCompleted: {
        console.log(plain.blurMax, plain.shadowOpacity, plain.shadowBlur, plain.shadowColor, plain.colorizationColor, plain.shadowScale, plain.autoPaddingEnabled, plain.maskThresholdMax, plain.brightness)
        console.log(ses.live, ses.hideSource, ses.recursive, ses.sourceItem, img.layer.enabled, img.layer.samplerName, img.layer.smooth, r.layer.enabled)
        Qt.quit()
    }
}
