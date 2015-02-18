/*
 *   Copyright (C) 2009 Petri Damstén <damu@iki.fi>
 *                  - Original Implementation.
 *                 2009 Andrew Coles  <andrew.coles@yahoo.co.uk>
 *                  - Extension to iplocationtools engine.
 *                 2015 Martin Gräßlin <mgraesslin@kde.org>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "location_ip.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <KJob>
#include <KIO/Job>
#include <KIO/TransferJob>

class Ip::Private : public QObject {
    Q_OBJECT
public:
    Private(Ip *q) : q(q) {}

    void readGeoLocation(KJob *job)
    {
        m_geoLocationResolved = true;
        if (job && job->error()) {
            qDebug() << "error" << job->errorString();
            m_geoLocationPayload.clear();
            checkUpdateData();
            return;
        }
        const QJsonObject json = QJsonDocument::fromJson(m_geoLocationPayload).object();
        m_geoLocationPayload.clear();

        auto accuracyIt = json.find(QStringLiteral("accuracy"));
        if (accuracyIt != json.end()) {
            m_data["accuracy"] = (*accuracyIt).toDouble();
        } else {
            m_data["accuracy"] = 40000;
        }

        auto locationIt = json.find(QStringLiteral("location"));
        if (locationIt != json.end()) {
            QJsonObject location = (*locationIt).toObject();
            m_data["latitude"] = location.value(QStringLiteral("lat")).toDouble();
            m_data["longitude"] = location.value(QStringLiteral("lng")).toDouble();
        }
        checkUpdateData();
    }

    void clear() {
        m_geoLocationPayload.clear();
        m_countryPayload.clear();
        m_countryResolved = false;
        m_geoLocationResolved = false;
        m_data.clear();
    }

    void geoLocationData(KIO::Job *job, const QByteArray &data) {
        Q_UNUSED(job)

        if (data.isEmpty()) {
            return;
        }
        m_geoLocationPayload.append(data);
    }

    void countryData(KIO::Job *job, const QByteArray &data) {
        Q_UNUSED(job)

        if (data.isEmpty()) {
            return;
        }
        m_countryPayload.append(data);
    }

    void readCountry(KJob *job) {
        m_countryResolved = true;
        if (job && job->error()) {
            qDebug() << "error" << job->errorString();
            m_countryPayload.clear();
            checkUpdateData();
            return;
        }

        const QJsonObject json = QJsonDocument::fromJson(m_countryPayload).object();
        m_countryPayload.clear();

        m_data["country"] = json.value(QStringLiteral("country_name")).toString();
        m_data["country code"] = json.value(QStringLiteral("country_code")).toString();
        checkUpdateData();
    }

private:
    void checkUpdateData() {
        if (!m_countryResolved || !m_geoLocationResolved) {
            return;
        }
        q->setData(m_data);
    }

    Ip *q;
    QByteArray m_geoLocationPayload;
    QByteArray m_countryPayload;
    bool m_countryResolved = false;
    bool m_geoLocationResolved = false;
    Plasma::DataEngine::Data m_data;
};

Ip::Ip(QObject* parent, const QVariantList& args)
    : GeolocationProvider(parent, args), d(new Private(this))
{
    setUpdateTriggers(SourceEvent | NetworkConnected);
}

Ip::~Ip()
{
    delete d;
}

void Ip::update()
{
    d->clear();
    // TODO: add wifi data if available
    const QByteArray postData = QByteArrayLiteral("{}");
    const QString apiKey = QStringLiteral("60e8eae6-3988-4ada-ad48-2cfddddf216b");
    KIO::TransferJob *datajob = KIO::http_post(QUrl(QStringLiteral("https://location.services.mozilla.com/v1/geolocate?key=%1").arg(apiKey)),
                                               postData,
                                               KIO::HideProgressInfo);
    datajob->addMetaData(QStringLiteral("content-type"), QStringLiteral("application/json"));

    qDebug() << "Fetching https://location.services.mozilla.com/v1/geolocate";
    connect(datajob, &KIO::TransferJob::data, d, &Ip::Private::geoLocationData);
    connect(datajob, &KIO::TransferJob::result, d, &Ip::Private::readGeoLocation);

    datajob = KIO::http_post(QUrl(QStringLiteral("https://location.services.mozilla.com/v1/country?key=%1").arg(apiKey)),
                             postData,
                             KIO::HideProgressInfo);
    datajob->addMetaData(QStringLiteral("content-type"), QStringLiteral("application/json"));
    connect(datajob, &KIO::TransferJob::data, d, &Ip::Private::countryData);
    connect(datajob, &KIO::TransferJob::result, d, &Ip::Private::readCountry);
}

K_EXPORT_PLASMA_GEOLOCATIONPROVIDER(ip, Ip)

#include "location_ip.moc"
