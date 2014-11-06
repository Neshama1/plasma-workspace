/*
 *   Copyright 2012 Marco Martin <notmart@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

import org.kde.plasma.private.notifications 1.0

Column {
    id: jobsRoot
    anchors {
        left: parent.left
        right: parent.right
    }

    property alias count: jobsRepeater.count
    //height: 192 // FIXME: should be dynamic, once childrenRect works again

    property var cancelledJobs: []

    PlasmaCore.DataSource {
        id: jobsSource

        property variant runningJobs

        engine: "applicationjobs"
        interval: 0

        onSourceAdded: {
            connectSource(source);
        }

        onSourceRemoved: {
            if (!notifications) {
                return
            }

            var cancelledJobPos = cancelledJobs.indexOf(source)
            if (cancelledJobPos > -1) {
                cancelledJobs.splice(cancelledJobPos, 1)
                return
            }

            var message = runningJobs[source]["label1"] ? runningJobs[source]["label1"] : runningJobs[source]["label0"]
            var infoMessage = runningJobs[source]["infoMessage"]
            notifications.addNotification({
                source: source,
                appIcon: runningJobs[source]["appIconName"],
                appName: runningJobs[source]["appName"],
                summary: infoMessage ? i18n("Job Finished") : i18nc("the job, which can be anything, has finished", "%1: Finished", infoMessage),
                body: message,
                isPersistent: true,
                expireTimeout: 6000,
                urgency: 0,
                configurable: false,
                actions: UrlHelper.isUrlValid(message) ? [{"id": message, "text": i18n("Open...")}] : [] // If the source contains "Job", it tries to open the "id" value (which is "message")
            })

            delete runningJobs[source]
        }

        onNewData: {
            var jobs = runningJobs
            jobs[sourceName] = data
            runningJobs = jobs
        }

        onDataChanged: {
            var total = 0
            for (var i = 0; i < sources.length; ++i) {
                if (jobsSource.data[sources[i]]["percentage"]) {
                    total += jobsSource.data[sources[i]]["percentage"]
                }
            }

            total /= sources.length
            notificationsApplet.globalProgress = total/100
        }

        Component.onCompleted: {
            jobsSource.runningJobs = new Object
            connectedSources = sources
        }
    }

    Item {
        visible: jobsRepeater.count > 3

        PlasmaComponents.ProgressBar {
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                right: parent.right
            }

            minimumValue: 0
            maximumValue: 100
            value: notificationsApplet.globalProgress * 100
        }
    }

    Repeater {
        id: jobsRepeater

        model: jobsSource.sources
        delegate: JobDelegate {}
    }
}
