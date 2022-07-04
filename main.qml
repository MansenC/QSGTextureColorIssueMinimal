// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.2
import ExampleCustomMaterial 1.0

Item {
    id: root

    width: 640
    height: 480


//! [1]
    CustomItem {
        anchors.fill: parent
    }
//! [1]

    Rectangle {
        id: labelFrame
        anchors.margins: -10
        radius: 10
        color: "white"
        border.color: "black"
        opacity: 0.8
        anchors.fill: description
    }

    Text {
        id: description
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        wrapMode: Text.WordWrap
        text: "This example shows how to create a custom material in C++ and use it in QML.\n"
        + "The custom material uses a fragment shader that calculates the Mandelbrot set,"
        + " and exposes the shader uniforms as QML properties."
    }
}
