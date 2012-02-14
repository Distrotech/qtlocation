/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qplacerestmanager.h"
#include "qplacecategoriesrepository.h"

#include <QtNetwork>
#include <QHash>

#include <qgeoboundingcircle.h>
#include <qgeoboundingbox.h>
#include "qplacerestreply.h"
#include <QtLocation/QPlaceContentRequest>

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

QT_USE_NAMESPACE

const char *placesServerUrl = "http://places.maps.ovi.com/rest/v1/places/";
const char *searchServerUrl = "http://where.desktop.mos.svc.ovi.com/NOSe/json";
const char *recomendation = "/recommendations/nearby";
const char *reviews = "/reviews?";
const char *images = "/images?";
const char *categoriesTree = "categories/find-places/grouped";

const char *const_query = "&q=";
const char *const_catQuery = "&ta=";
const char *const_lat = "&lat=";
const char *const_lon = "&lon=";
const char *const_top = "&vpn=";
const char *const_bottom = "&vps=";
const char *const_left = "&vpw=";
const char *const_right = "&vpe=";
const char *const_limit = "&to=";
const char *const_offset = "&of=";
const char *const_dym = "&dym=";
const char *const_views = "?vi=where"; // address, poi or where (both)
const char *const_deviceproductid = "&dv=oviMaps"; // oviMaps, ml, rv

const char *const_review_start = "&start=";
const char *const_review_limit = "&limit=";

static QString searchServer;
static QString placeServer;

QPlaceRestManager *QPlaceRestManager::mInstance = NULL;

/*!
    Constructor.
*/
QPlaceRestManager::QPlaceRestManager(QObject *parent) :
    QObject(parent)
{
    mLocales << QLocale();
    mManager = new QNetworkAccessManager(this);

    if (searchServer.isNull()) {
        QSettings settings("Nokia");
        // no app name, they are in Nokia/OrganizationDefaults
        searchServer = settings.value("searchURI").toString();
        if (searchServer.isEmpty()) {
            searchServer = searchServerUrl;
        }
        placeServer = settings.value("placesURI").toString();
        if (placeServer.isEmpty()) {
            placeServer = placesServerUrl;
        }
    }
}

/*!
    Method creating instance of rest provider.
*/
QPlaceRestManager *QPlaceRestManager::instance()
{
    if (!mInstance) {
        mInstance = new QPlaceRestManager();
    }
    return mInstance;
}

/*!
    Sets the proxy used for requests.
*/
void QPlaceRestManager::setProxy(const QNetworkProxy &proxy)
{
    mManager->setProxy(proxy);
}

/*!
    Predefines a places request and executes sendGeneralRequest().
*/
QPlaceRestReply *QPlaceRestManager::sendPlaceRequest(const QString &placeId)
{
    return sendGeneralRequest(placeServer + placeId);
}

QPlaceRestReply *QPlaceRestManager::sendPlaceImagesRequest(const QString &placeId, const QPlaceContentRequest &params)
{
    QString query = placeServer + placeId + images;
    if (params.offset() > -1) {
        query += const_review_start + QString::number(params.offset());
    }
    if (params.limit() > 0) {
        query += const_review_limit + QString::number(params.limit());
    }
    return sendGeneralRequest(query);
}

/*!
    Predefines a review request and executes sendGeneralRequest().
*/
QPlaceRestReply *QPlaceRestManager::sendPlaceReviewRequest(const QString &placeId, const QPlaceContentRequest &params)
{
    QString query = placeServer + placeId + reviews;
    if (params.offset() > -1) {
        query += const_review_start + QString::number(params.offset());
    }
    if (params.limit() > 0) {
        query += const_review_limit + QString::number(params.limit());
    }

    return sendGeneralRequest(query);
}

/*!
    Predefines a recomendation request and executes sendGeneralRequest().
*/
QPlaceRestReply *QPlaceRestManager::sendRecommendationRequest(const QPlaceSearchRequest &query, const QString &userId)
{
    Q_UNUSED(userId);
    return sendGeneralRequest(placeServer + query.searchTerm() + recomendation);
}

/*!
    Predefines a categories tree request and executes sendGeneralRequest().
*/
QPlaceRestReply *QPlaceRestManager::sendCategoriesTreeRequest()
{
    return sendGeneralRequest(placeServer + categoriesTree);
}

/*!
    Predefines a suggestion request and executes sendGeneralRequest().
*/
QPlaceRestReply *QPlaceRestManager::sendSuggestionRequest(const QPlaceSearchRequest &query)
{
    return sendGeneralRequest(prepareSearchRequest(query) + QLatin1String("&lh=1"));
}

/*!
    Predefines a search request and executes sendGeneralRequest().
*/
QPlaceRestReply *QPlaceRestManager::sendSearchRequest(const QPlaceSearchRequest &query)
{
    return sendGeneralRequest(prepareSearchRequest(query));
}

QPlaceRestReply *QPlaceRestManager::postRatingRequest(const QString &placeId, const QString &userId, int value)
{
    QNetworkRequest request;

    QString url = placesServerUrl + placeId + "/ugc/ratings";
    request.setUrl(url);

    request.setRawHeader("Accept-Language", createLanguageString());
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("X-Ovi-NcimUser", userId.toUtf8());

#if defined(QPLACES_LOGGING)
    qDebug() << "QRestDataProvider::sendGeneralRequest: " + url;
#endif
    QByteArray data = "{ratings:{rating:[{value:" + QString::number(value).toAscii() + ",type:OVERALL}]}}";
    return new QPlaceRestReply(mManager->post(request, data));
}

QList<QLocale> QPlaceRestManager::locales() const
{
    return mLocales;
}

void QPlaceRestManager::setLocales(const QList<QLocale> &locales)
{
    mLocales = locales;
}

/*!
    Sends a general predefined request. Is private.
*/
QPlaceRestReply *QPlaceRestManager::sendGeneralRequest(const QUrl &url)
{
    QNetworkRequest request;
    request.setUrl(url);

#if defined(QPLACES_LOGGING)
    qDebug() << "QRestDataProvider::sendGeneralRequest: Language - " + createLocaleString();
#endif

    request.setRawHeader("Accept-Language", createLanguageString());

#if defined(QPLACES_LOGGING)
    qDebug() << "QRestDataProvider::sendGeneralRequest: " + url.toString();
#endif

    return new QPlaceRestReply(mManager->get(request));
}

QByteArray QPlaceRestManager::createLanguageString() const
{
    QByteArray language;
    QList<QLocale> locales = mLocales;
    if (locales.isEmpty())
        locales << QLocale();

    foreach (const QLocale &loc, locales) {
        language.append(loc.name().replace(2, 1, '-'));
        language.append(", ");
    }
    language.chop(2);

    return language;
}

/*!
    Returns prepared search string.
*/
QString QPlaceRestManager::prepareSearchRequest(const QPlaceSearchRequest &query)
{
    QString searchString(searchServer);
    // add view and device parameters
    searchString += const_views;
    searchString += const_deviceproductid;
    // process search center
    if (query.searchArea() != NULL) {
        if (query.searchArea()->type() == QGeoBoundingArea::CircleType) {
            QGeoBoundingCircle * circle = static_cast<QGeoBoundingCircle *>(query.searchArea());
            searchString += const_lat + QString::number(circle->center().latitude());
            searchString += const_lon + QString::number(circle->center().longitude());
        } else if (query.searchArea()->type() == QGeoBoundingArea::BoxType) {
            QGeoBoundingBox *box = static_cast<QGeoBoundingBox *> (query.searchArea());
            searchString += const_top + QString::number(box->topLeft().latitude());
            searchString += const_left + QString::number(box->topLeft().longitude());
            searchString += const_bottom + QString::number(box->bottomRight().latitude());
            searchString += const_right + QString::number(box->bottomRight().longitude());
        }
    }

    if (!query.categories().isEmpty()) {
        QStringList ids;
        foreach (const QPlaceCategory &category, query.categories())
            ids.append(QPlaceCategoriesRepository::instance()->getCategoryTagId(category));

        searchString += const_catQuery + ids.join(QLatin1String(","));
    }

    if (!query.searchTerm().isEmpty())
        searchString += const_query + query.searchTerm();

    // processing limit
    if (query.limit() > 0){
        searchString += const_limit + QString::number(query.limit());
    }
    // processing offset
    if (query.offset() > -1){
        searchString += const_offset + QString::number(query.offset());
    }
    // process DYM
    if (query.maximumCorrections() > 0){
        searchString += const_dym + QString::number(query.maximumCorrections());
    }

#if defined(QPLACES_LOGGING)
    qDebug() << "QRestDataProvider::prepareSearchRequest: " + searchString;
#endif

    return searchString;
}