/*
 * AmbientLight - convenience non-directional light. Qt Quick 3D itself has
 * no AmbientLight type (it favours image-based lighting via SceneEnvironment),
 * so we approximate with a PointLight that has zero attenuation positioned
 * very far away. Applications that want true IBL on desktop should set
 * SceneEnvironment.lightProbe instead.
 */
import QtQuick 2.15
import QtQuick3D 6.5 as QQ3D

QQ3D.PointLight {
    constantFade: 1.0
    linearFade: 0.0
    quadraticFade: 0.0
    brightness: 0.2
}
