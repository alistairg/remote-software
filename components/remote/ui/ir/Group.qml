/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

import QtQuick 2.11

Rectangle {
    id: group
    width: parent.width
    height: 500
    color: colorMedium
    radius: cornerRadius
    y: 0

    property int offset: 0


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    state: "closed"

    states: [
        State {
            name: "closed"
            PropertyChanges {target: group; y: 560 - offset }
            PropertyChanges {target: group; color: colorDark }
            PropertyChanges {target: edge; color: colorMedium }
        },
        State {
            name: "open"
            PropertyChanges {target: group; y: parent.height - group.height }
            PropertyChanges {target: group; color: colorMedium }
            PropertyChanges {target: edge; color: colorDark }
        },
        State {
            name: "move"
            PropertyChanges {target: group; y: parent.height - group.height }
            PropertyChanges {target: group; color: colorDark }
            PropertyChanges {target: edge; color: colorMedium }
        }
    ]

    transitions: [
        Transition {
            to: "closed"
            PropertyAnimation { target: group; properties: "y"; easing.type: Easing.OutExpo; duration: 300 }
            ColorAnimation { easing.type: Easing.OutExpo; duration: 300 }
        },
        Transition {
            to: "open"
            PropertyAnimation { target: group; properties: "y"; easing.type: Easing.OutBack; easing.overshoot: 0.8; duration: 300 }
            ColorAnimation { easing.type: Easing.OutExpo; duration: 300 }
        },
        Transition {
            to: "move"
            PropertyAnimation { target: group; properties: "y"; easing.type: Easing.OutBack; easing.overshoot: 0.8; duration: 300 }
            ColorAnimation { easing.type: Easing.OutExpo; duration: 300 }
        }
    ]

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // TOP EDGE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Rectangle {
        id: edge
        width: parent.width
        height: parent.height
        color: colorDark
        radius: cornerRadius
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -4
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MOUSEAREA
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MouseArea {
        width: parent.width
        height: 80
        anchors.top: parent.top

        drag.target: group
        drag.axis: Drag.YAxis
        drag.minimumY: group.parent.height - group.height
        drag.maximumY: 560 - offset

        onReleased: {
            if (group.state == "closed" && y < 560 - offset - 20) {
                group.state = "open";
            } else {
                group.state = "closed";
            }
        }

    }

    Rectangle {
        width: 40
        height: 4
        radius: 4
        color: colorText
        opacity: 0.2
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
