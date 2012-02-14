/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
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
****************************************************************************/

#include <QCoreApplication>
#include <QString>
#include <QtTest/QtTest>
#include <QtCore/qnumeric.h>
#include <QtGui/QImageReader>

#include <qgeoboundingbox.h>
#include <qgeoboundingcircle.h>
#include <qgeoserviceprovider.h>
#include <qplacemanager.h>
#include <qplacecategory.h>
#include <qplacecontentreply.h>
#include <qplacematchreply.h>
#include <qplacesearchsuggestionreply.h>
#include <qplacesearchrequest.h>
#include <qplacesearchresult.h>
#include <qplacesearchreply.h>
#include <qplacecontactdetail.h>
#include <qplaceimage.h>
#include <qplacesupplier.h>

#include "jsondbcleaner.h"

//Use until QTRY_VERIFY_WITH_TIMEOUT is available
#ifndef TRY_VERIFY_WITH_TIMEOUT
#define TRY_VERIFY_WITH_TIMEOUT(__expr, __timeout) \
         do { \
         const int __step = 50; \
         if (!(__expr)) { \
             QTest::qWait(0); \
         } \
         for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
             QTest::qWait(__step); \
         } \
         QVERIFY(__expr); \
     } while (0)
#endif

#ifndef WAIT_UNTIL
#define WAIT_UNTIL(__expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
    } while (0)
#endif

Q_DECLARE_METATYPE(QPlaceIdReply *);

QT_USE_NAMESPACE

class tst_QPlaceManagerJsonDb : public QObject
{
    Q_OBJECT
    enum UrlCorrectnes {
        ValidUrl,
        NoSchemeUrl
    };

    enum DestinationExists {
        Exists,
        DoesNotExist
    };

public:
    tst_QPlaceManagerJsonDb();

private Q_SLOTS:
    void initTestCase();
    void cleanup();

    void saveAndRemovePlace();
    void updatePlace();
    void simpleSaveAndRemoveCategory();
    void saveAndRemoveCategory();
    void updateCategory();
    void savePlaceWithCategory();

    void searchByName();
    void searchByBox();
    void searchByCircle();
    void searchWithLexicalPlaceNameHint();
    void searchWithDistanceHint();
    void searchByCategory();
    void unsupportedFunctions();
    void supportedFeatures();
    void categoryFunctions();
    void contactDetails();
    void contactDetails_data();
    void mulipleDetailTypes();
    void placeNotifications();
    void categoryNotifications();
    void compatiblePlace();
    void extendedAttribute();
    void matchingPlaces();
    void iconSourceDestination();
    void iconSourceDestination_data();
    void iconSourceOnly();
    void iconSourceOnly_data();
    void iconDestinationOnly();
    void iconDestinationOnly_data();
    void iconSavedFromDifferentManager();
    void iconFormats();
    void iconFormats_data();
    void iconUrls();
    void iconUrls_data();
    void constructIconUrl();

private:
    bool doSavePlace(const QPlace &place,
                QPlaceReply::Error expectedError = QPlaceReply::NoError,
                QString *placeId = 0);
    void doSavePlaces(QList<QPlace> &places);
    void doSavePlaces(const QList<QPlace *> &places);

    bool doRemovePlace(const QPlace &place,
                       QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool doSearch(const QPlaceSearchRequest &request,
                  QList<QPlaceSearchResult> *results,
             QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool doSearch(const QPlaceSearchRequest &request,
                  QList<QPlace> *results,
             QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool doFetchDetails(QString placeId,
                        QPlace *place,
                        QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool doSaveCategory(const QPlaceCategory &category,
                        QPlaceReply::Error expectedError = QPlaceReply::NoError,
                        QString *categoryId = 0);

    bool doSaveCategory(const QPlaceCategory &category,
                        const QString &parentId,
                        QPlaceReply::Error expectedError = QPlaceReply::NoError,
                        QString *categoryId = 0);

    bool doRemoveCategory(const QPlaceCategory &category,
                          QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool doMatch(const QPlaceMatchRequest &request,
                 QList<QPlace> *places,
                 QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool checkSignals(QPlaceReply *reply, QPlaceReply::Error expectedError);


    bool compareResultsByName(const QList<QPlaceSearchResult> &results, const QList<QPlace> &expectedResults);

    QImage dataUrlToImage(const QUrl &url);

    static const QString SmallSource;
    static const QString SmallDestination;
    static const QString SmallDestinationSize;
    static const QString MediumSource;
    static const QString MediumDestination;
    static const QString MediumDestinationSize;
    static const QString LargeSource;
    static const QString LargeDestination;
    static const QString LargeDestinationSize;
    static const QString FullscreenSource;
    static const QString FullscreenDestination;
    static const QString FullscreenDestinationSize;

    static const QSize SmallSize;
    static const QSize MediumSize;
    static const QSize LargeSize;
    static const QSize FullscreenSize;

    QGeoServiceProvider *provider;
    QPlaceManager *placeManager;
    QCoreApplication *coreApp;
    JsonDbCleaner *dbCleaner ;
};

//These constants are equivalent to those from the jsondb plugin icon class
const QString tst_QPlaceManagerJsonDb::SmallSource("smallSourceUrl");
const QString tst_QPlaceManagerJsonDb::SmallDestination("smallUrl");
const QString tst_QPlaceManagerJsonDb::SmallDestinationSize("smallSize");

const QString tst_QPlaceManagerJsonDb::MediumSource("mediumSourceUrl");
const QString tst_QPlaceManagerJsonDb::MediumDestination("mediumUrl");
const QString tst_QPlaceManagerJsonDb::MediumDestinationSize("mediumSize");

const QString tst_QPlaceManagerJsonDb::LargeSource("largeSourceUrl");
const QString tst_QPlaceManagerJsonDb::LargeDestination("largeUrl");
const QString tst_QPlaceManagerJsonDb::LargeDestinationSize("largeSize");

const QString tst_QPlaceManagerJsonDb::FullscreenSource("fullscreenSourceUrl");
const QString tst_QPlaceManagerJsonDb::FullscreenDestination("fullscreenUrl");
const QString tst_QPlaceManagerJsonDb::FullscreenDestinationSize("fullscreenSize");

const QSize tst_QPlaceManagerJsonDb::SmallSize = QSize(20,20);
const QSize tst_QPlaceManagerJsonDb::MediumSize = QSize(30,30);
const QSize tst_QPlaceManagerJsonDb::LargeSize = QSize(50, 50);
const QSize tst_QPlaceManagerJsonDb::FullscreenSize = QSize(320, 480);

tst_QPlaceManagerJsonDb::tst_QPlaceManagerJsonDb()
{
    //We need a QCoreApplication instance for
    //event dispatching
    int argc;
    char **argv=0;
    coreApp = new QCoreApplication(argc, argv);

    dbCleaner = new JsonDbCleaner(this);
}

void tst_QPlaceManagerJsonDb::initTestCase()
{
    qRegisterMetaType<QPlaceIdReply *>();

    QStringList providers = QGeoServiceProvider::availableServiceProviders();
    QVERIFY(providers.contains("nokia_places_jsondb"));

    provider = new QGeoServiceProvider("nokia_places_jsondb");
    placeManager = provider->placeManager();
    QVERIFY(placeManager);

    cleanup();
}

void tst_QPlaceManagerJsonDb::saveAndRemovePlace()
{
    QPlace place;
    place.setName("Char");
    QGeoLocation location;

    location.setCoordinate(QGeoCoordinate(10,20));

    QGeoAddress address;
    address.setStreet("1 test street");
    address.setCity("New york");
    address.setCountry("USA");
    location.setAddress(address);
    place.setLocation(location);

    QPlaceContactDetail phone;
    phone.setValue("555-5555");
    place.appendContactDetail(QPlaceContactDetail::Phone, phone);

    QPlaceContactDetail fax;
    fax.setValue("999-9999");
    place.appendContactDetail(QPlaceContactDetail::Fax, fax);

    QPlaceContactDetail email;
    email.setValue("email@adddresss.com");
    place.appendContactDetail(QPlaceContactDetail::Email, email);

    QPlaceContactDetail website;
    website.setValue("www.example.com");
    place.appendContactDetail(QPlaceContactDetail::Website, website);

    //Save a place
    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));

    //ensure we can retrieve it's details
    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    place.setPlaceId(placeId);
    QVERIFY(retrievedPlace == place);

    //try removing a place
    QVERIFY(doRemovePlace(place, QPlaceReply::NoError));

    //ensure it is actually deleted
    QVERIFY(doFetchDetails(placeId, &retrievedPlace, QPlaceReply::PlaceDoesNotExistError));
    QCOMPARE(retrievedPlace, QPlace());

    //try removing a place that does not exist;
    QVERIFY(doRemovePlace(place, QPlaceReply::PlaceDoesNotExistError));

    place.setVisibility(QtLocation::PublicVisibility);

    QVERIFY(doSavePlace(place, QPlaceReply::UnsupportedError, 0));
}

void tst_QPlaceManagerJsonDb::updatePlace()
{
    //save a place and check that we can retrieve it
    QPlace place;
    place.setName("Sydney");
    QGeoAddress address;
    address.setStreet("original street");
    QGeoLocation location;
    location.setAddress(address);
    place.setLocation(location);

    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &place));

    //update the place again with some changes
    place.setName(QLatin1String("Brisbane"));
    address.setStreet(QLatin1String("new street"));
    address.setCountry(QLatin1String("Australia"));
    location.setAddress(address);
    place.setLocation(location);

    QPlace retrievedPlace;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError));
    QVERIFY(doFetchDetails(place.placeId(), &retrievedPlace));
    QCOMPARE(retrievedPlace.name(), QLatin1String("Brisbane"));
    QCOMPARE(retrievedPlace.location().address().street(), QLatin1String("new street"));
    QCOMPARE(retrievedPlace.location().address().country(), QLatin1String("Australia"));

    //try updating a non-existent place
    place.setPlaceId("Non-existent id");
    QVERIFY(doSavePlace(place, QPlaceReply::PlaceDoesNotExistError));
}

void tst_QPlaceManagerJsonDb::simpleSaveAndRemoveCategory()
{
    QString categoryId;
    QPlaceCategory restaurant;
    restaurant.setName("Restaurant");

    //check saving of category
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);
    QPlaceReply * catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    QList<QPlaceCategory> categories = placeManager->childCategories();
    QCOMPARE(categories.count(), 1);

    QPlaceCategory retrievedCategory = placeManager->category(categoryId);
    QCOMPARE(retrievedCategory, restaurant);

    //check removal of category
    QVERIFY(doRemoveCategory(restaurant));
    catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    QCOMPARE(placeManager->category(categoryId), QPlaceCategory());
    categories = placeManager->childCategories();
    QCOMPARE(categories.count(), 0);
}

void tst_QPlaceManagerJsonDb::saveAndRemoveCategory()
{
    QString categoryId;
    QPlaceCategory restaurant;
    restaurant.setName("Restaurant");
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);

    QPlaceCategory fastFood;
    fastFood.setName("Fast Food");
    QVERIFY(doSaveCategory(fastFood, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    fastFood.setCategoryId(categoryId);

    QPlaceCategory fineDining;
    fineDining.setName("Fine dining");
    QVERIFY(doSaveCategory(fineDining, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    fineDining.setCategoryId(categoryId);

    QPlaceCategory pancakes;
    pancakes.setName("Pancakes");
    QVERIFY(doSaveCategory(pancakes, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    pancakes.setCategoryId(categoryId);

    QPlaceCategory pizza;
    pizza.setName("Pizza");
    QVERIFY(doSaveCategory(pizza, fastFood.categoryId(), QPlaceReply::NoError, &categoryId));
    pizza.setCategoryId(categoryId);

    QPlaceCategory burgers;
    burgers.setName("burgers");
    QVERIFY(doSaveCategory(burgers, fastFood.categoryId(), QPlaceReply::NoError, &categoryId));
    burgers.setCategoryId(categoryId);

    QPlaceCategory accommodation;
    accommodation.setName("Accommodation");
    QVERIFY(doSaveCategory(accommodation, QPlaceReply::NoError,&categoryId));
    accommodation.setCategoryId(categoryId);

    QPlaceCategory hotel;
    hotel.setName("Hotel");
    QVERIFY(doSaveCategory(hotel, accommodation.categoryId(), QPlaceReply::NoError, &categoryId));
    hotel.setCategoryId(categoryId);

    QPlaceCategory motel;
    motel.setName("Motel");
    QVERIFY(doSaveCategory(motel, accommodation.categoryId(), QPlaceReply::NoError, &categoryId));
    motel.setCategoryId(categoryId);
    QPlaceReply *catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    QList<QPlaceCategory> topLevelCategories = placeManager->childCategories();
    QVERIFY(topLevelCategories.contains(restaurant));
    QVERIFY(topLevelCategories.contains(accommodation));
    QVERIFY(topLevelCategories.count() == 2);

    QList<QPlaceCategory> categories = placeManager->childCategories(restaurant.categoryId());
    QVERIFY(categories.contains(fastFood));
    QVERIFY(categories.contains(fineDining));
    QVERIFY(categories.contains(pancakes));
    QCOMPARE(categories.count(), 3);

    categories = placeManager->childCategories(fastFood.categoryId());
    QVERIFY(categories.contains(pizza));
    QVERIFY(categories.contains(burgers));
    QCOMPARE(categories.count(), 2);

    categories = placeManager->childCategories(accommodation.categoryId());
    QVERIFY(categories.contains(hotel));
    QVERIFY(categories.contains(motel));
    QCOMPARE(categories.count(), 2);

    QVERIFY(doRemoveCategory(fastFood));
    catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    categories = placeManager->childCategories(restaurant.categoryId());
    QVERIFY(categories.contains(fineDining));
    QVERIFY(categories.contains(pancakes));
    QCOMPARE(categories.count(), 2);

    QVERIFY(placeManager->category(pizza.categoryId()) == QPlaceCategory());
    QVERIFY(placeManager->category(burgers.categoryId()) == QPlaceCategory());

    QVERIFY(doRemoveCategory(accommodation));
    catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    categories = placeManager->childCategories();
    QVERIFY(categories.contains(restaurant));
    QCOMPARE(categories.count(), 1);
}

void tst_QPlaceManagerJsonDb::updateCategory()
{
    //Test updating a category name
    QPlaceCategory category;
    category.setName("Foood");
    QString categoryId;
    QVERIFY(doSaveCategory(category, QPlaceReply::NoError, &categoryId));
    category.setCategoryId(categoryId);
    QPlaceReply *catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    QList<QPlaceCategory> categories;
    categories = placeManager->childCategories();
    QVERIFY(categories.contains(category));

    category.setName("Food");
    QVERIFY(doSaveCategory(category, QPlaceReply::NoError, &categoryId));
    catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    categories = placeManager->childCategories();
    QVERIFY(categories.contains(category));
    QCOMPARE(categories.count(), 1);

    //Test updating a category's parent
    QPlaceCategory restaurant;
    restaurant.setName("Restaurant");
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);

    QPlaceCategory fastFood;
    fastFood.setName("Fast Food");
    QVERIFY(doSaveCategory(fastFood, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    fastFood.setCategoryId(categoryId);

    QPlaceCategory fineDining;
    fineDining.setName("Fine dining");
    QVERIFY(doSaveCategory(fineDining, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    fineDining.setCategoryId(categoryId);

    QPlaceCategory pancakes;
    pancakes.setName("Pancakes");
    QVERIFY(doSaveCategory(pancakes, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    pancakes.setCategoryId(categoryId);

    QPlaceCategory pizza;
    pizza.setName("Pizza");
    QVERIFY(doSaveCategory(pizza, fastFood.categoryId(), QPlaceReply::NoError, &categoryId));
    pizza.setCategoryId(categoryId);

    QPlaceCategory burgers;
    burgers.setName("burgers");
    QVERIFY(doSaveCategory(burgers, fastFood.categoryId(), QPlaceReply::NoError, &categoryId));
    burgers.setCategoryId(categoryId);

    //resave pizza as a child of fine dining
    QVERIFY(doSaveCategory(pizza, fineDining.categoryId()));
    catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    //check that fast food no longer has  pizza as a child
    categories = placeManager->childCategories(fastFood.categoryId());
    QVERIFY(categories.contains(burgers));
    QCOMPARE(categories.count(), 1);

    //check that fine dining has pizza as a child
    categories = placeManager->childCategories(fineDining.categoryId());
    QVERIFY(categories.contains(pizza));
    QCOMPARE(categories.count(), 1);
}

void tst_QPlaceManagerJsonDb::savePlaceWithCategory()
{
    QString categoryId;
    QPlaceCategory restaurant;
    restaurant.setName("Restaurant");
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);

    QString placeId;
    QPlace monolithBurgers;
    monolithBurgers.setName("Monolith burgers");
    monolithBurgers.setCategory(restaurant);
    QVERIFY(doSavePlace(monolithBurgers,QPlaceReply::NoError, &placeId));
    monolithBurgers.setPlaceId(placeId);

    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.categories().count(), 1);
    QCOMPARE(retrievedPlace.categories().first(), restaurant);

    //update place with multiple categories
    QPlaceCategory facilities;
    facilities.setName("facilities");
    QVERIFY(doSaveCategory(facilities, QPlaceReply::NoError, &categoryId));
    facilities.setCategoryId(categoryId);

    QList<QPlaceCategory> categories;
    categories << facilities << restaurant;
    monolithBurgers.setCategories(categories);
    QVERIFY(doSavePlace(monolithBurgers,QPlaceReply::NoError, &placeId));

    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.categories().count(), 2);
    QSet<QPlaceCategory> catSet;

    QVERIFY(retrievedPlace.categories().contains(facilities));
    QVERIFY(retrievedPlace.categories().contains(restaurant));
}

void tst_QPlaceManagerJsonDb::searchByName()
{
    QPlace adelaide, adel, ad, brisbane, bradel;
    adelaide.setName("Adelaide");
    adel.setName("adel");
    ad.setName("ad");
    brisbane.setName("brisbane");
    bradel.setName("bradel");

    QList<QPlace> places;
    places << adelaide << adel << ad << brisbane << bradel;
    doSavePlaces(places);

    //test that search has exhibits substring behaviour
    //and is case insensitive
    QPlaceSearchRequest request;
    request.setSearchTerm("adel");
    QList<QPlaceSearchResult> results;
    QVERIFY(doSearch(request, &results));
    QList<QPlace> expectedPlaces;
    expectedPlaces << adelaide << adel << bradel;
    QVERIFY(compareResultsByName(results, expectedPlaces));

    //Search for a non-exisent place
    request.setSearchTerm("Nowhere");
    QVERIFY(doSearch(request, &results));
    QCOMPARE(results.count(), 0);
}

void tst_QPlaceManagerJsonDb::searchByBox()
{
    QList<QGeoCoordinate> outBox;
    QList<QGeoCoordinate> inBox1;

    inBox1 << QGeoCoordinate(-5.0, -5.0);
    inBox1 << QGeoCoordinate(-5.0, 0.0);
    inBox1 << QGeoCoordinate(-5.0, 5.0);
    inBox1 << QGeoCoordinate(0.0, -5.0);
    inBox1 << QGeoCoordinate(0.0, 0.0);
    inBox1 << QGeoCoordinate(0.0, 5.0);
    inBox1 << QGeoCoordinate(5.0, -5.0);
    inBox1 << QGeoCoordinate(5.0, 0.0);
    inBox1 << QGeoCoordinate(5.0, 5.0);

    outBox << QGeoCoordinate(-5.0, -6.0);
    outBox << QGeoCoordinate(5.0, -6.0);
    outBox << QGeoCoordinate(-6.0, -5.0);
    outBox << QGeoCoordinate(6.0, -5.0);
    outBox << QGeoCoordinate(-6.0, 5.0);
    outBox << QGeoCoordinate(6.0, 5.0);
    outBox << QGeoCoordinate(-5.0, 6.0);
    outBox << QGeoCoordinate(5.0, 6.0);

    QList<QGeoCoordinate> inBox2;

    inBox2 << QGeoCoordinate(-5.0, 10.0);
    inBox2 << QGeoCoordinate(-5.0, 15.0);
    inBox2 << QGeoCoordinate(-5.0, 20.0);
    inBox2 << QGeoCoordinate(0.0, 10.0);
    inBox2 << QGeoCoordinate(0.0, 15.0);
    inBox2 << QGeoCoordinate(0.0, 20.0);
    inBox2 << QGeoCoordinate(5.0, 10.0);
    inBox2 << QGeoCoordinate(5.0, 15.0);
    inBox2 << QGeoCoordinate(5.0, 20.0);

    outBox << QGeoCoordinate(-5.0, 9.0);
    outBox << QGeoCoordinate(5.0,  9.0);
    outBox << QGeoCoordinate(-6.0, 10.0);
    outBox << QGeoCoordinate(6.0,  10.0);
    outBox << QGeoCoordinate(-6.0, 20.0);
    outBox << QGeoCoordinate(6.0, 20.0);
    outBox << QGeoCoordinate(-5.0, 21.0);
    outBox << QGeoCoordinate(5.0, 21.0);

    QList<QGeoCoordinate> inBox3;

    inBox3 << QGeoCoordinate(10.0, -5.0);
    inBox3 << QGeoCoordinate(10.0, 0.0);
    inBox3 << QGeoCoordinate(10.0, 5.0);
    inBox3 << QGeoCoordinate(15.0, -5.0);
    inBox3 << QGeoCoordinate(15.0, 0.0);
    inBox3 << QGeoCoordinate(15.0, 5.0);
    inBox3 << QGeoCoordinate(20.0, -5.0);
    inBox3 << QGeoCoordinate(20.0, 0.0);
    inBox3 << QGeoCoordinate(20.0, 5.0);

    outBox << QGeoCoordinate(10.0, -6.0);
    outBox << QGeoCoordinate(20.0, -6.0);
    outBox << QGeoCoordinate(9.0, -5.0);
    outBox << QGeoCoordinate(21.0, -5.0);
    outBox << QGeoCoordinate(9.0, 5.0);
    outBox << QGeoCoordinate(21.0, 5.0);
    outBox << QGeoCoordinate(10.0, 6.0);
    outBox << QGeoCoordinate(20.0, 6.0);

    QList<QGeoCoordinate> inBox4;

    inBox4 << QGeoCoordinate(10.0, 10.0);
    inBox4 << QGeoCoordinate(10.0, 15.0);
    inBox4 << QGeoCoordinate(10.0, 20.0);
    inBox4 << QGeoCoordinate(15.0, 10.0);
    inBox4 << QGeoCoordinate(15.0, 15.0);
    inBox4 << QGeoCoordinate(15.0, 20.0);
    inBox4 << QGeoCoordinate(20.0, 10.0);
    inBox4 << QGeoCoordinate(20.0, 15.0);
    inBox4 << QGeoCoordinate(20.0, 20.0);

    outBox << QGeoCoordinate(10.0, 9.0);
    outBox << QGeoCoordinate(20.0, 9.0);
    outBox << QGeoCoordinate(9.0, 10.0);
    outBox << QGeoCoordinate(21.0, 10.0);
    outBox << QGeoCoordinate(9.0, 20.0);
    outBox << QGeoCoordinate(21.0, 20.0);
    outBox << QGeoCoordinate(10.0, 21.0);
    outBox << QGeoCoordinate(20.0, 21.0);

    QList<QGeoCoordinate> inBox5;

    inBox5 << QGeoCoordinate(-5.0, 175.0);
    inBox5 << QGeoCoordinate(-5.0, 180.0);
    inBox5 << QGeoCoordinate(-5.0, -175.0);
    inBox5 << QGeoCoordinate(0.0, 175.0);
    inBox5 << QGeoCoordinate(0.0, 180.0);
    inBox5 << QGeoCoordinate(0.0, -175.0);
    inBox5 << QGeoCoordinate(5.0, 175.0);
    inBox5 << QGeoCoordinate(5.0, 180.0);
    inBox5 << QGeoCoordinate(5.0, -175.0);

    outBox << QGeoCoordinate(-6.0, 175.0);
    outBox << QGeoCoordinate(-6.0, -175.0);
    outBox << QGeoCoordinate(-5.0, 174.0);
    outBox << QGeoCoordinate(-5.0, -174.0);
    outBox << QGeoCoordinate(5.0, 174.0);
    outBox << QGeoCoordinate(5.0, -174.0);
    outBox << QGeoCoordinate(6.0, 175.0);
    outBox << QGeoCoordinate(6.0, -175.0);

    QList<QGeoCoordinate> coords = outBox;
    coords.append(inBox1);
    coords.append(inBox2);
    coords.append(inBox3);
    coords.append(inBox4);
    coords.append(inBox5);

    QList<QPlace> places;
    for (int i = 0; i < coords.size(); ++i) {
        QPlace place;
        QGeoLocation location;
        location.setCoordinate(coords.at(i));
        place.setLocation(location);
        places << place;
    }
    doSavePlaces(places);

    QPlaceSearchRequest request;
    request.setSearchArea(QGeoBoundingBox(QGeoCoordinate(5.0, -5.0), QGeoCoordinate(-5.0, 5.0)));

    QList<QPlaceSearchResult> results1;
    doSearch(request, &results1);
    QList<QPlace> places1;
    foreach (const QPlaceSearchResult  &result, results1)
        places1.append(result.place());

    QCOMPARE(places1.size(), inBox1.size());

    QSet<QString> testSet1;
    for (int i = 0; i < places1.size(); ++i)
        testSet1.insert(places1.at(i).location().coordinate().toString());

    QSet<QString> inBoxSet1;
    for (int i = 0; i < inBox1.size(); ++i)
        inBoxSet1.insert(inBox1.at(i).toString());

    QCOMPARE(testSet1, inBoxSet1);

    request.setSearchArea(QGeoBoundingBox(QGeoCoordinate(5.0, 10.0), QGeoCoordinate(-5.0, 20.0)));
    QList<QPlace> places2;
    doSearch(request, &places2);
    QCOMPARE(places2.size(), inBox2.size());

    QSet<QString> testSet2;
    for (int i = 0; i < places2.size(); ++i)
        testSet2.insert(places2.at(i).location().coordinate().toString());

    QSet<QString> inBoxSet2;
    for (int i = 0; i < inBox2.size(); ++i)
        inBoxSet2.insert(inBox2.at(i).toString());

    QCOMPARE(testSet2, inBoxSet2);

    request.setSearchArea(QGeoBoundingBox(QGeoCoordinate(20.0, -5.0), QGeoCoordinate(10.0, 5.0)));
    QList<QPlace> places3;
    doSearch(request, &places3);

    QCOMPARE(places3.size(), inBox3.size());

    QSet<QString> testSet3;
    for (int i = 0; i < places3.size(); ++i)
        testSet3.insert(places3.at(i).location().coordinate().toString());

    QSet<QString> inBoxSet3;
    for (int i = 0; i < inBox3.size(); ++i)
        inBoxSet3.insert(inBox3.at(i).toString());

    QCOMPARE(testSet3, inBoxSet3);

    request.setSearchArea(QGeoBoundingBox(QGeoCoordinate(20.0, 10.0), QGeoCoordinate(10.0, 20.0)));
    QList<QPlace> places4;
    doSearch(request, &places4);
    QCOMPARE(places4.size(), inBox4.size());

    QSet<QString> testSet4;
    for (int i = 0; i < places4.size(); ++i)
        testSet4.insert(places4.at(i).location().coordinate().toString());

    QSet<QString> inBoxSet4;
    for (int i = 0; i < inBox4.size(); ++i)
        inBoxSet4.insert(inBox4.at(i).toString());

    QCOMPARE(testSet4, inBoxSet4);

    request.setSearchArea(QGeoBoundingBox(QGeoCoordinate(5.0, 175.0), QGeoCoordinate(-5.0, -175.0)));
    QList<QPlace> places5;
    doSearch(request, &places5);
    QCOMPARE(places5.size(), inBox5.size());

    QSet<QString> testSet5;
    for (int i = 0; i < places5.size(); ++i) {
        if (places5.at(i).location().coordinate().longitude() == -180.0) {
            QGeoLocation location;
            location.setCoordinate(QGeoCoordinate(places5.at(i).location().coordinate().latitude(), 180.0));
            places5[i].setLocation(location);
        }
        testSet5.insert(places5.at(i).location().coordinate().toString());
    }

    QSet<QString> inBoxSet5;
    for (int i = 0; i < inBox5.size(); ++i) {
        if (inBox5.at(i).longitude() == -180.0) {
            inBox5[i].setLongitude(180.0);
        }
        inBoxSet5.insert(inBox5.at(i).toString());
    }

    QCOMPARE(testSet5, inBoxSet5);

    //try a box that finds nothing
    request.setSearchArea(QGeoBoundingBox(QGeoCoordinate(-70,-70), QGeoCoordinate(-80,-60)));
    QVERIFY(doSearch(request, &places));
    QCOMPARE(places.count(), 0);

    //--- Test error conditions
    //bottom right latitude > top left latitude
    request.setSearchArea(QGeoBoundingBox(QGeoCoordinate(20,20), QGeoCoordinate(50,30)));
    QVERIFY(doSearch(request,&places, QPlaceReply::BadArgumentError));

    //try an invalid coordinate for one of the corners
    request.setSearchArea(QGeoBoundingBox(QGeoCoordinate(qQNaN(),20),QGeoCoordinate(10,30)));
    QVERIFY(doSearch(request,&places, QPlaceReply::BadArgumentError));
}

void tst_QPlaceManagerJsonDb::searchByCircle()
{
    QList<QGeoCoordinate> greenwhichFilterCoords;
    QList<QGeoCoordinate> datelineFilterCoords;
    QList<QGeoCoordinate> northPoleFilterCoords;
    QList<QGeoCoordinate> southPoleFilterCoords;
    QList<QGeoCoordinate> northFilterCoords;
    QList<QGeoCoordinate> eastFilterCoords;
    QList<QGeoCoordinate> northeastFilterCoords;

    QList<QGeoCoordinate> greenwhichLmCoords;
    QList<QGeoCoordinate> datelineLmCoords;
    QList<QGeoCoordinate> northPoleLmCoords;
    QList<QGeoCoordinate> southPoleLmCoords;
    QList<QGeoCoordinate> northLmCoords;
    QList<QGeoCoordinate> eastLmCoords;
    QList<QGeoCoordinate> northeastLmCoords;

    greenwhichFilterCoords << QGeoCoordinate(-0.1, -0.1);
    greenwhichFilterCoords << QGeoCoordinate(0.1, -0.1);
    greenwhichFilterCoords << QGeoCoordinate(-0.1, 0.1);
    greenwhichFilterCoords << QGeoCoordinate(0.1, 0.1);

    datelineFilterCoords << QGeoCoordinate(-0.1, -179.9);
    datelineFilterCoords << QGeoCoordinate(0.1, -179.9);
    datelineFilterCoords << QGeoCoordinate(-0.1, 179.9);
    datelineFilterCoords << QGeoCoordinate(0.1, 179.9);

    northPoleFilterCoords << QGeoCoordinate(89.9, -179.9);
    northPoleFilterCoords << QGeoCoordinate(89.9, -0.1);
    northPoleFilterCoords << QGeoCoordinate(89.9, 0.1);
    northPoleFilterCoords << QGeoCoordinate(89.9, 179.9);

    southPoleFilterCoords << QGeoCoordinate(-89.9, -179.9);
    southPoleFilterCoords << QGeoCoordinate(-89.9, -0.1);
    southPoleFilterCoords << QGeoCoordinate(-89.9, 0.1);
    southPoleFilterCoords << QGeoCoordinate(-89.9, 179.9);

    eastFilterCoords << QGeoCoordinate(-0.1, 10.0);
    eastFilterCoords << QGeoCoordinate(0.1, 10.0);
    northFilterCoords << QGeoCoordinate(10.0, -0.1);
    northFilterCoords << QGeoCoordinate(10.0, 0.1);
    northeastFilterCoords << QGeoCoordinate(10.0, 10.0);

    greenwhichLmCoords << QGeoCoordinate(-1.0, -1.0);
    greenwhichLmCoords << QGeoCoordinate(1.0, -1.0);
    greenwhichLmCoords << QGeoCoordinate(-1.0, 1.0);
    greenwhichLmCoords << QGeoCoordinate(1.0, 1.0);

    datelineLmCoords << QGeoCoordinate(-1.0, -179.0);
    datelineLmCoords << QGeoCoordinate(1.0, -179.0);
    datelineLmCoords << QGeoCoordinate(-1.0, 179.0);
    datelineLmCoords << QGeoCoordinate(1.0, 179.0);

    northPoleLmCoords << QGeoCoordinate(89.0, -179.0);
    northPoleLmCoords << QGeoCoordinate(89.0, -1.0);
    northPoleLmCoords << QGeoCoordinate(89.0, 1.0);
    northPoleLmCoords << QGeoCoordinate(89.0, 179.0);

    southPoleLmCoords << QGeoCoordinate(-89.0, -179.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, -1.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, 1.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, 179.0);

    eastLmCoords << QGeoCoordinate(-1.0, 11.0);
    eastLmCoords << QGeoCoordinate(1.0, 11.0);
    northLmCoords << QGeoCoordinate(11.0, -1.0);
    northLmCoords << QGeoCoordinate(11.0, 1.0);
    northeastLmCoords << QGeoCoordinate(11.0, 11.0);

    QList<QList<QGeoCoordinate> > coords;
    coords << greenwhichLmCoords;
    coords << datelineLmCoords;

    coords << northPoleLmCoords;
    coords << southPoleLmCoords;
    coords << eastLmCoords;
    coords << northLmCoords;
    coords << northeastLmCoords;

    for (int i = 0; i < coords.size(); ++i) {
        QList<QGeoCoordinate> c = coords.at(i);
        for (int j = 0; j < c.size(); ++j) {
            QPlace place;
            QGeoLocation location;
            location.setCoordinate(c.at(j));
            place.setLocation(location);
            doSavePlace(place);
        }
    }

    QList<QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> > > testSets;
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(greenwhichFilterCoords, greenwhichLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(datelineFilterCoords, datelineLmCoords);

    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northPoleFilterCoords, northPoleLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(southPoleFilterCoords, southPoleLmCoords);

    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northFilterCoords, northLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(eastFilterCoords, eastLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northeastFilterCoords, northeastLmCoords);

    qreal dist = QGeoCoordinate(0.0, 0.0).distanceTo(QGeoCoordinate(5.0, 5.0));

    QList<QPlaceSearchResult> results;
    QList<QPlace> places;
    for (int i = 0; i < testSets.size(); ++i) {
        QList<QGeoCoordinate> filterCoords = testSets.at(i).first;
        QList<QGeoCoordinate> plCoords = testSets.at(i).second;

        QPlaceSearchRequest request;
        for (int j = 0; j < filterCoords.size(); ++j) {
            request.setSearchArea(QGeoBoundingCircle(filterCoords.at(j),dist));

            if (i ==2 || i ==3) {
                //TODO: Testing poles, ignore fo rnow
                continue;
            } else {
                doSearch(request, &results);
                foreach (const QPlaceSearchResult &result, results)
                    places.append(result.place());
            }

            if (places.size() != plCoords.size()) {
                for (int k = 0; k < places.size(); ++k)
                    qWarning() << "pl" << places.at(k).location().coordinate().toString(QGeoCoordinate::Degrees);
                for (int k = 0; k < plCoords.size(); ++k)
                    qWarning() << "plCoords" << plCoords.at(k).toString(QGeoCoordinate::Degrees);
            }

            QCOMPARE(places.size(), plCoords.size());

            for (int k = 0; k < places.size(); ++k) {
                QVERIFY(plCoords.contains(places.at(k).location().coordinate()));
            }
            results.clear();
            places.clear();
        }
    }

    //--- Test error conditions and edge cases
    //try a circle that covers the north pole
    QPlaceSearchRequest request;
    request.setSearchArea(QGeoBoundingCircle(QGeoCoordinate(89.91,0),11000));
    QVERIFY(doSearch(request,&places, QPlaceReply::BadArgumentError));
    QCOMPARE(places.count(), 0);

    //try a circle that's close to but does not cover the north pole
    request.setSearchArea(QGeoBoundingCircle(QGeoCoordinate(89.91,0),9000));
    QVERIFY(doSearch(request,&places, QPlaceReply::NoError));

    //try a circle that covers the south pole
    request.setSearchArea(QGeoBoundingCircle(QGeoCoordinate(-89.91,180),11000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QCOMPARE(places.count(), 0);

    //try a circle that's close to but does not cover the south pole
    request.setSearchArea(QGeoBoundingCircle(QGeoCoordinate(-89.91,180),9000));
    QVERIFY(doSearch(request, &places, QPlaceReply::NoError));

    //try a radius of 0
    request.setSearchArea(QGeoBoundingCircle(QGeoCoordinate(11.0,11.0), 0));
    QVERIFY(doSearch(request, &places));
    QCOMPARE(places.count(), 1);
    QCOMPARE(places.at(0).location().coordinate().latitude(), 11.0);
    QCOMPARE(places.at(0).location().coordinate().longitude(), 11.0);

    //try an invalid center
    request.setSearchArea(QGeoBoundingCircle(QGeoCoordinate(), 5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    //try an invalid latitude for the center
    request.setSearchArea(QGeoBoundingCircle(QGeoCoordinate(qQNaN(),50), 5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    //try a proximity filter with an out of range latitude
    request.setSearchArea(QGeoBoundingCircle(QGeoCoordinate(90,10), 5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    request.setSearchArea(QGeoBoundingCircle(QGeoCoordinate(150,10), 5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    //try a proximity filter with an out of range longitude
    request.setSearchArea(QGeoBoundingCircle(QGeoCoordinate(-12,180.1),5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    request.setSearchArea(QGeoBoundingCircle(QGeoCoordinate(-12,-180.1),5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());
}

void tst_QPlaceManagerJsonDb::searchWithLexicalPlaceNameHint()
{
    QPlace melbourne, sydney, adelaide, brisbane;
    melbourne.setName(QLatin1String("Melbourne"));
    sydney.setName(QLatin1String("Sydney"));
    adelaide.setName(QLatin1String("Adelaide"));
    brisbane.setName(QLatin1String("Brisbane"));

    QList<QPlace *> places;
    places << &melbourne << &sydney << &adelaide << &brisbane;
    doSavePlaces(places);

    QPlaceSearchRequest searchRequest;
    searchRequest.setRelevanceHint(QPlaceSearchRequest::LexicalPlaceNameHint);
    QList<QPlace> results;
    QVERIFY(doSearch(searchRequest, &results));
    QCOMPARE(results.count(), places.count());
    QCOMPARE(results.at(0), adelaide);
    QCOMPARE(results.at(1), brisbane);
    QCOMPARE(results.at(2), melbourne);
    QCOMPARE(results.at(3), sydney);
}

void tst_QPlaceManagerJsonDb::searchWithDistanceHint()
{
    QList<QPlace *> places;

    QPlace place1;
    place1.setName(QLatin1String("LM1"));
    QGeoLocation location;
    location.setCoordinate(QGeoCoordinate(20,19));
    place1.setLocation(location);
    places << &place1;

    QPlace place2;
    place2.setName(QLatin1String("LM2"));
    location.setCoordinate(QGeoCoordinate(20,50));
    place2.setLocation(location);
    places << &place2;

    QPlace place3;
    place3.setName(QLatin1String("LM3"));
    location.setCoordinate(QGeoCoordinate(20, 30));
    place3.setLocation(location);
    places << &place3;

    QPlace place4;
    place4.setName(QLatin1String("LM4"));
    location.setCoordinate(QGeoCoordinate(5,20));
    place4.setLocation(location);
    places << &place4;

    QPlace place5;
    place5.setName(QLatin1String("LM5"));
    location.setCoordinate(QGeoCoordinate(80,20));
    place5.setLocation(location);
    places << &place5;

    QPlace place6;
    place6.setName(QLatin1String("LM6"));
    location.setCoordinate(QGeoCoordinate(60,20));
    place6.setLocation(location);
    places << &place6;

    doSavePlaces(places);

    QPlaceSearchRequest searchRequest;
    QGeoBoundingCircle circle(QGeoCoordinate(20,20));
    searchRequest.setSearchArea(circle);
    searchRequest.setRelevanceHint(QPlaceSearchRequest::DistanceHint);
    QList<QPlace> results;
    QVERIFY(doSearch(searchRequest, &results));
    QCOMPARE(results.count(), 6);
    QCOMPARE(results.at(0), place1);
    QCOMPARE(results.at(1), place3);
    QCOMPARE(results.at(2), place4);
    QCOMPARE(results.at(3), place2);
    QCOMPARE(results.at(4), place6);
    QCOMPARE(results.at(5), place5);

    qreal radius = QGeoCoordinate(20,20).distanceTo(QGeoCoordinate(20,50));
    circle.setRadius(radius);

    QVERIFY(doSearch(searchRequest, &results));
    QCOMPARE(results.count(),4);
    QCOMPARE(results.at(0), place1);
    QCOMPARE(results.at(1), place3);
    QCOMPARE(results.at(2), place4);
    QCOMPARE(results.at(3), place2);

    //try radius less than 1
    circle.setRadius(-5);
    QVERIFY(doSearch(searchRequest, &results));
    QCOMPARE(results.count(), 6);
    QCOMPARE(results.at(0), place1);
    QCOMPARE(results.at(1), place3);
    QCOMPARE(results.at(2), place4);
    QCOMPARE(results.at(3), place2);
    QCOMPARE(results.at(4), place6);
    QCOMPARE(results.at(5), place5);

}

void tst_QPlaceManagerJsonDb::searchByCategory()
{
    QString categoryId;
    QPlaceCategory restaurant;
    restaurant.setName("Restaurant");
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);

    QString placeId;
    QPlace park;
    park.setName("Park");
    QVERIFY(doSavePlace(park, QPlaceReply::NoError, &placeId));
    park.setPlaceId(placeId);

    QList<QPlaceCategory> categories;
    categories << restaurant;

    QPlace krustyBurger;
    krustyBurger.setName("Krusty burger");
    krustyBurger.setCategories(categories);
    QVERIFY(doSavePlace(krustyBurger, QPlaceReply::NoError, &placeId));
    krustyBurger.setPlaceId(placeId);

    QList<QPlace> places;
    QPlaceSearchRequest request;
    request.setCategory(restaurant);
    QVERIFY(doSearch(request, &places));
    QCOMPARE(places.count(), 1);
    QCOMPARE(places.at(0), krustyBurger);
}

void tst_QPlaceManagerJsonDb::unsupportedFunctions()
{
    QPlace place;
    place.setPlaceId("id");
    QPlaceContentRequest request;
    request.setContentType(QPlaceContent::ImageType);
    request.setLimit(5);
    request.setOffset(0);
    QPlaceContentReply *contentReply = placeManager->getPlaceContent(place.placeId(), request);
    QSignalSpy contentSpy(contentReply, SIGNAL(finished()));
    QTRY_VERIFY(contentSpy.count() == 1);
    QCOMPARE(contentReply->error(), QPlaceReply::UnsupportedError);

    QPlaceSearchRequest searchRequest;
    QPlaceSearchSuggestionReply *searchSuggestionReply = placeManager->searchSuggestions(searchRequest);
    QSignalSpy searchSuggestionSpy(searchSuggestionReply, SIGNAL(finished()));
    QTRY_VERIFY(searchSuggestionSpy.count() == 1 );
    QCOMPARE(searchSuggestionReply->error(), QPlaceReply::UnsupportedError);
}

void tst_QPlaceManagerJsonDb::supportedFeatures()
{
    QCOMPARE((placeManager->supportedFeatures() & QPlaceManager::SavePlaceFeature),
             QPlaceManager::SavePlaceFeature);
    QCOMPARE((placeManager->supportedFeatures() & QPlaceManager::RemovePlaceFeature),
            QPlaceManager::RemovePlaceFeature);
    QCOMPARE((placeManager->supportedFeatures() & QPlaceManager::SaveCategoryFeature),
             QPlaceManager::SaveCategoryFeature);
    QCOMPARE((placeManager->supportedFeatures() & QPlaceManager::RemoveCategoryFeature),
             QPlaceManager::RemoveCategoryFeature);

    QVERIFY((placeManager->supportedFeatures() & QPlaceManager::RecommendationsFeature) == 0);
    QVERIFY((placeManager->supportedFeatures() & QPlaceManager::SearchSuggestionsFeature) == 0);
    QVERIFY((placeManager->supportedFeatures() & QPlaceManager::CorrectionsFeature) == 0);
    QVERIFY((placeManager->supportedFeatures() & QPlaceManager::LocaleFeature) == 0);
}

void tst_QPlaceManagerJsonDb::categoryFunctions()
{
    QString categoryId;
    QPlaceCategory restaurant;
    restaurant.setName(QLatin1String("Restaurant"));
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);

    QPlaceCategory fastFood;
    fastFood.setName(QLatin1String("Fast Food"));
    QVERIFY(doSaveCategory(fastFood, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    fastFood.setCategoryId(categoryId);

    QPlaceCategory fineDining;
    fineDining.setName(QLatin1String("Fine dining"));
    QVERIFY(doSaveCategory(fineDining, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    fineDining.setCategoryId(categoryId);
    QPlaceReply *reply = placeManager->initializeCategories();
    QVERIFY(checkSignals(reply, QPlaceReply::NoError));

    //try find a parent id
    QCOMPARE(placeManager->parentCategoryId(fineDining.categoryId()), restaurant.categoryId());

    //try find the parent id of a top level category
    QCOMPARE(placeManager->parentCategoryId(restaurant.categoryId()), QString());

    //try find the parent id of a non-existent category.
    QCOMPARE(placeManager->parentCategoryId(QLatin1String("does-not-exist")), QString());

    //try find the child ids
    QStringList childIds = placeManager->childCategoryIds(restaurant.categoryId());
    QVERIFY(childIds.contains(fastFood.categoryId()));
    QVERIFY(childIds.contains(fineDining.categoryId()));
    QCOMPARE(childIds.count(), 2);

    //try find the child id of a category without a child
    QCOMPARE(placeManager->childCategoryIds(fineDining.categoryId()), QStringList());

    //try to find child ids of a non-existent category
    QCOMPARE(placeManager->childCategoryIds(QLatin1String("does-not-exist")), QStringList());

    //try to find a category by it's id
    QCOMPARE(placeManager->category(fastFood.categoryId()), fastFood);

    //try to find a category with a non-existent id
    QCOMPARE(placeManager->category(QLatin1String("does-not-exist")), QPlaceCategory());
}

void tst_QPlaceManagerJsonDb::contactDetails()
{
    QFETCH(QString, contactType);
    QPlace place;
    //create a place with a single contact detail of a given type
    place.setName(QLatin1String("place"));
    QPlaceContactDetail detail;
    detail.setLabel(QLatin1String("detailLabel"));
    detail.setValue(QLatin1String("detail"));
    place.appendContactDetail(contactType, detail);

    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    place.setPlaceId(placeId);

    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(retrievedPlace.contactTypes().contains(contactType));
    QCOMPARE(retrievedPlace.contactDetails(contactType).count(), 1);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(0), detail);

    //add multiple details of a given type;
    QPlaceContactDetail detail2;
    detail2.setLabel("detail2Label");
    detail2.setValue(QLatin1String("detail2"));
    QPlaceContactDetail detail3;
    detail3.setLabel("detail3Label");
    detail3.setValue(QLatin1String("detail3"));

    place.appendContactDetail(contactType, detail2);
    place.appendContactDetail(contactType, detail3);
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.contactDetails(contactType).count(), 3);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(0), detail);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(1), detail2);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(2), detail3);

    //try removing a detail
    QList<QPlaceContactDetail> details;
    details << detail <<detail3;
    place.setContactDetails(contactType, details);
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.contactDetails(contactType).count(), 2);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(0), detail);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(1), detail3);

    //try remove all details
    place.removeContactDetails(contactType);
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(retrievedPlace.contactTypes().contains(contactType) == false);
    QCOMPARE(retrievedPlace.contactDetails(contactType).count(), 0);

    //try creating a place with multiple contact details of a given type.
    QPlace place2;
    place2.setContactDetails(contactType, details);
    QVERIFY(doSavePlace(place2, QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(retrievedPlace.contactTypes().contains(contactType) == true);
    QCOMPARE(retrievedPlace.contactDetails(contactType).count(), 2);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(0), detail);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(1), detail3);
}

void tst_QPlaceManagerJsonDb::contactDetails_data()
{
    QTest::addColumn<QString>("contactType");

    QTest::newRow("phone") << QPlaceContactDetail::Phone;
    QTest::newRow("email") << QPlaceContactDetail::Email;
    QTest::newRow("website") << QPlaceContactDetail::Website;
    QTest::newRow("fax") << QPlaceContactDetail::Fax;
}

void tst_QPlaceManagerJsonDb::mulipleDetailTypes()
{
    //try saving a place with multiple detail types simultaneously.
    QPlace place;
    place.setName("Char");

    QPlaceContactDetail phone;
    phone.setLabel("phone1");
    phone.setValue("555-5555");
    QPlaceContactDetail phone2;
    phone2.setLabel("phone2");
    phone2.setValue("444-4444");
    QList<QPlaceContactDetail> phones;
    place.setContactDetails(QPlaceContactDetail::Phone, phones);

    QPlaceContactDetail fax;
    fax.setLabel("fax1");
    fax.setValue("999-9999");
    QPlaceContactDetail fax2;
    fax2.setLabel("fax2");
    fax2.setValue("999-9999");
    QList<QPlaceContactDetail> faxes;
    place.setContactDetails(QPlaceContactDetail::Fax, faxes);

    QPlaceContactDetail email;
    email.setValue("email@adddress.com");
    QPlaceContactDetail email2;
    email2.setValue("email2@adddress.com");
    place.appendContactDetail(QPlaceContactDetail::Email, email);
    place.appendContactDetail(QPlaceContactDetail::Email, email2);

    QPlaceContactDetail website;
    website.setLabel("website");
    website.setValue("www.example.com");
    QPlaceContactDetail website2;
    website2.setLabel("website2");
    website2.setValue("www.example2.com");
    place.appendContactDetail(QPlaceContactDetail::Website, website);
    place.appendContactDetail(QPlaceContactDetail::Website, website2);

    //Save a place
    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));

    //ensure we can retrieve it's details
    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    place.setPlaceId(placeId);
    QVERIFY(retrievedPlace == place);

    //try adding some more, changing and removing details of different types
    //when updating a place.
    phone2.setValue("222-2222");  //modify detail
    phones.clear();
    phones << phone << phone2;
    place.setContactDetails(QPlaceContactDetail::Phone, phones);

    //remove an entire set of details
    place.removeContactDetails(QPlaceContactDetail::Fax);

    //add remove one detail
    QList<QPlaceContactDetail> emails;
    emails << email2;
    place.setContactDetails(QPlaceContactDetail::Email, emails);

    //add more of a detail
    QPlaceContactDetail website3;
    website3.setLabel("website3");
    website3.setValue("www.example3.com");
    place.appendContactDetail(QPlaceContactDetail::Website, website3);

    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(retrievedPlace == place);
}

void tst_QPlaceManagerJsonDb::placeNotifications()
{
    QSignalSpy createSpy(placeManager, SIGNAL(placeAdded(QString)));
    QSignalSpy updateSpy(placeManager, SIGNAL(placeUpdated(QString)));
    QSignalSpy removeSpy(placeManager, SIGNAL(placeRemoved(QString)));

    //create place
    QPlace place;
    place.setName("Char");
    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    place.setPlaceId(placeId);

    QTRY_VERIFY(createSpy.count() == 1);
    createSpy.clear();
    QVERIFY(updateSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    //modify place
    QGeoLocation location;
    location.setCoordinate(QGeoCoordinate(10,10));
    place.setLocation(location);
    QVERIFY(doSavePlace(place, QPlaceReply::NoError));
    QTRY_VERIFY(updateSpy.count() == 1);
    updateSpy.clear();
    QVERIFY(createSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    //remove place
    QVERIFY(doRemovePlace(place, QPlaceReply::NoError));
    QTRY_VERIFY(removeSpy.count() == 1);
    removeSpy.clear();
    QVERIFY(createSpy.count() == 0);
    QVERIFY(updateSpy.count() == 0);
}

void tst_QPlaceManagerJsonDb::categoryNotifications()
{
    QSignalSpy createSpy(placeManager, SIGNAL(categoryAdded(QPlaceCategory,QString)));
    QSignalSpy updateSpy(placeManager, SIGNAL(categoryUpdated(QPlaceCategory,QString)));
    QSignalSpy removeSpy(placeManager, SIGNAL(categoryRemoved(QString,QString)));

    //create category
    QString restaurantId;
    QPlaceCategory restaurant;
    restaurant.setName("Restaurant");
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &restaurantId));
    restaurant.setCategoryId(restaurantId);
    QTRY_VERIFY(createSpy.count() == 1);
    createSpy.clear();
    QVERIFY(updateSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    //modify category
    restaurant.setName("RESTAURANT");
    QVERIFY(doSaveCategory(restaurant));
    QTRY_VERIFY(updateSpy.count() == 1);
    updateSpy.clear();
    QVERIFY(createSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    QVERIFY(doRemoveCategory(restaurant));
    QTRY_VERIFY(removeSpy.count() == 1);
    removeSpy.clear();
    QVERIFY(createSpy.count() == 0);
    QVERIFY(updateSpy.count() == 0);

    restaurant.setCategoryId(QString());
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &restaurantId));
    QTRY_VERIFY(createSpy.count() == 1);
    createSpy.clear();
    QVERIFY(updateSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    //save a category as a child
    QString steakId;
    QPlaceCategory steak;
    steak.setName("Steak");

    QVERIFY(doSaveCategory(steak, restaurantId, QPlaceReply::NoError, &steakId));
    steak.setCategoryId(steakId);
    QTRY_VERIFY(createSpy.count() == 1);
    QVERIFY(createSpy.at(0).at(0).value<QPlaceCategory>() == steak);
    createSpy.clear();
    QVERIFY(updateSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);
 }

void tst_QPlaceManagerJsonDb::compatiblePlace()
{
    QGeoServiceProvider geoTest("qmlgeo.test.plugin");
    QPlaceManager *geoTestManager = geoTest.placeManager();

    QPlace place;
    place.setPlaceId(QLatin1String("123"));
    place.setName(QLatin1String("Moe's Tavern"));

    QGeoAddress address;
    address.setStreet(QLatin1String("93 Brewing Ave"));
    address.setDistrict(QLatin1String("Maine"));
    address.setCity(QLatin1String("Springfield"));
    address.setCounty(QLatin1String("Jackson"));
    address.setState(QLatin1String("Minnesota"));
    address.setCountry("Unisted Statess");
    address.setCountryCode("USA");
    QGeoCoordinate coord(56,34,5);

    QGeoLocation location;
    location.setAddress(address);
    location.setCoordinate(coord);
    location.setBoundingBox(QGeoBoundingBox(QGeoCoordinate(20,20), QGeoCoordinate(10,30)));
    place.setLocation(location);

    QPlaceContactDetail phone;
    phone.setLabel("Phone");
    phone.setValue("555-1793");
    place.appendContactDetail(QPlaceContactDetail::Phone, phone);

    QPlaceSupplier supplier;
    supplier.setName("Springfield brewery");
    supplier.setSupplierId("ID");

    place.setSupplier(supplier);

    QPlaceImage image;
    image.setImageId("798");
    image.setUrl(QUrl("http://image.com/"));
    QPlaceContent::Collection imageCollection;
    imageCollection.insert(0,image);
    place.insertContent(QPlaceContent::ImageType, imageCollection);
    place.setTotalContentCount(QPlaceContent::ImageType, 1);

    QPlaceAttribute attribute;
    attribute.setLabel(QLatin1String("Smoking"));
    attribute.setText(QLatin1String("Yes"));

    QPlaceIcon icon;
    QVariantMap iconParams;
    iconParams.insert("s", "www.example.com/small.png");
    iconParams.insert("m", "www.example.com/medium.png");
    iconParams.insert("l", "www.example.com/large.png");
    icon.setParameters(iconParams);
    icon.setManager(geoTestManager);
    place.setIcon(icon);

    place.setExtendedAttribute(QLatin1String("Smoking"), attribute);

    place.setVisibility(QtLocation::PublicVisibility);

    QPlace compatPlace = placeManager->compatiblePlace(place);

    QVERIFY(compatPlace.placeId().isEmpty());
    QCOMPARE(compatPlace.name(),QLatin1String("Moe's Tavern"));
    QCOMPARE(compatPlace.location().address(), address);
    QVERIFY(compatPlace.location().coordinate() != location.coordinate());
    location.setCoordinate(QGeoCoordinate(56,34));
    QVERIFY(compatPlace.location().coordinate() == location.coordinate());
    QVERIFY(compatPlace.location().boundingBox().isEmpty());

    QCOMPARE(compatPlace.contactTypes().count(), 1);
    QVERIFY(compatPlace.contactTypes().contains(QPlaceContactDetail::Phone));
    QCOMPARE(compatPlace.contactDetails(QPlaceContactDetail::Phone).count(), 1);
    QCOMPARE(compatPlace.contactDetails(QPlaceContactDetail::Phone).at(0), phone);

    QCOMPARE(compatPlace.supplier(), QPlaceSupplier());
    QVERIFY(compatPlace.content(QPlaceContent::ImageType).isEmpty());
    QVERIFY(compatPlace.content(QPlaceContent::ReviewType).isEmpty());
    QVERIFY(compatPlace.content(QPlaceContent::EditorialType).isEmpty());
    QCOMPARE(compatPlace.totalContentCount(QPlaceContent::ImageType), 0);

    QCOMPARE(compatPlace.icon().parameters().value(SmallSource).toString(),  QLatin1String("www.example.com/small.png"));
    QCOMPARE(compatPlace.icon().parameters().value(MediumSource).toString(),  QLatin1String("www.example.com/medium.png"));
    QCOMPARE(compatPlace.icon().parameters().value(LargeSource).toString(),  QLatin1String("www.example.com/large.png"));

    QVERIFY(compatPlace.extendedAttributeTypes().isEmpty());
    QCOMPARE(compatPlace.extendedAttribute(QLatin1String("Smoking")), QPlaceAttribute());

    QCOMPARE(compatPlace.visibility(), QtLocation::UnspecifiedVisibility);
}

void tst_QPlaceManagerJsonDb::extendedAttribute()
{
    QPlaceAttribute attribute;
    attribute.setLabel(QLatin1String("x_id_nokia"));
    attribute.setText(QLatin1String("ae562"));
    QPlace place;
    place.setExtendedAttribute(QLatin1String("x_id_nokia"), attribute);

    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));

    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId,&retrievedPlace));
    QVERIFY(retrievedPlace.extendedAttributeTypes().contains(QLatin1String("x_id_nokia")));
}

void tst_QPlaceManagerJsonDb::matchingPlaces()
{
    QPlace place1;
    place1.setPlaceId(QLatin1String("abcd"));
    place1.setName("place1");

    QPlaceAttribute origin1;
    origin1.setText("nokia");
    place1.setExtendedAttribute(QLatin1String("x_provider"), origin1);

    QPlace place1Saved;
    place1Saved = placeManager->compatiblePlace(place1);

    QString placeId;
    QVERIFY(doSavePlace(place1Saved, QPlaceReply::NoError, &placeId));
    place1Saved.setPlaceId(placeId);

    QPlaceSearchResult result1;
    result1.setPlace(place1);

    QList<QPlaceSearchResult> results;
    results << result1;

    QPlaceMatchRequest matchRequest;
    QVariantMap parameters;
    parameters.insert(QPlaceMatchRequest::AlternativeId, QLatin1String("x_id_nokia"));
    matchRequest.setParameters(parameters);
    matchRequest.setResults(results);
    QList<QPlace> places;
    QVERIFY(doMatch(matchRequest,&places));
    QCOMPARE(places.count(), 1);
    QCOMPARE(place1Saved, places.at(0));

    //try matching multiple places
    QPlace nonMatchingPlace;
    nonMatchingPlace.setName("Non matching");
    nonMatchingPlace.setPlaceId(QLatin1String("1234"));
    QPlaceAttribute originNonMatch;
    originNonMatch.setText(QLatin1String("nokia"));
    nonMatchingPlace.setExtendedAttribute(QLatin1String("x_provider"),originNonMatch);
    QPlaceSearchResult nonMatchingResult;
    nonMatchingResult.setPlace(nonMatchingPlace);
    results.insert(1, nonMatchingResult);

    QPlace place2;
    place2.setName(QLatin1String("place2"));
    place2.setPlaceId(QLatin1String("efgh"));

    QPlaceAttribute origin2;
    origin2.setText(QLatin1String("nokia"));
    place2.setExtendedAttribute(QLatin1String("x_provider"), origin2);

    QPlace place2Saved = placeManager->compatiblePlace(place2);
    QVERIFY(doSavePlace(place2Saved, QPlaceReply::NoError, &placeId));
    place2Saved.setPlaceId(placeId);

    QPlaceSearchResult result2;
    result2.setPlace(place2);
    results.clear();
    results << result1 << nonMatchingResult << result2;

    matchRequest.setResults(results);
    QVERIFY(doMatch(matchRequest, &places));
    QCOMPARE(places.count(), 3);
    QCOMPARE(places.at(0), place1Saved);
    QCOMPARE(places.at(1), QPlace());
    QCOMPARE(places.at(2), place2Saved);
}

void tst_QPlaceManagerJsonDb::iconSourceDestination()
{
    //We test that an input source icon is correctly copied to it's specified destination
    //Source and Destination urls are provided
    QFETCH(QString, sourceIconResource);
    QFETCH(QSize, iconSize);
    QFETCH(QString, iconType);
    QFETCH(QString, source);
    QFETCH(QString, specifiedDestination);

    for (int destUrlCorrectness = ValidUrl; destUrlCorrectness != (NoSchemeUrl +1); ++destUrlCorrectness) {
        for (int destFileExists = Exists; destFileExists != (DoesNotExist + 1); ++destFileExists) {
            QPlace place;
            place.setName("place");
            QPlaceIcon icon;
            QVariantMap iconParams;

            QTemporaryFile sourceIconFile;

            if (iconType.startsWith(QLatin1String("file"))) {
                QImage sourceIconImage(sourceIconResource);
                sourceIconFile.open();
                sourceIconImage.save(sourceIconFile.fileName(), QImageReader::imageFormat(sourceIconResource));
                if (iconType == QLatin1String("file"))
                    iconParams.insert(source, QUrl::fromLocalFile(sourceIconFile.fileName()));
                else if (iconType == QLatin1String("file_improperUrl"))
                    iconParams.insert(source, QUrl(sourceIconFile.fileName()));
                else
                    qFatal("Unknown icon type");
            } else if (iconType == QLatin1String("dataUrl")) {
                QFile sourceIcon(sourceIconResource);
                sourceIcon.open(QIODevice::ReadOnly);
                QString mimeType;
                if (QImageReader::imageFormat(sourceIconResource) == "png")
                    mimeType = QLatin1String("image/png");
                QUrl dataUrl(QString::fromLatin1("data:") + mimeType + QLatin1String(";base64,") + sourceIcon.readAll().toBase64());
                iconParams.insert(source, dataUrl);
            }

            QTemporaryDir tempDir;
            QVERIFY(tempDir.isValid());
            QString destIconFileName = tempDir.path() + QLatin1String("/tempFile");
            if (destFileExists == Exists) {
                QFile destFile(destIconFileName);
                destFile.open(QIODevice::ReadWrite);
                destFile.close();
            }//else must be creating a new destination file

            if (destUrlCorrectness == ValidUrl)
                iconParams.insert(specifiedDestination, QUrl::fromLocalFile(destIconFileName));
            else  //must be no scheme url
                iconParams.insert(specifiedDestination, QUrl(destIconFileName));

            icon.setParameters(iconParams);
            place.setIcon(icon);

            QString placeId;
            QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));

            QPlace retrievedPlace;
            QVERIFY(doFetchDetails(placeId, &retrievedPlace));

            QVERIFY(retrievedPlace.icon().parameters().contains(specifiedDestination));

            QUrl retrievedUrl = retrievedPlace.icon().parameters().value(specifiedDestination).toUrl();

            QImage retrievedImage;

            QVERIFY(retrievedUrl.scheme().compare(QLatin1String("file"), Qt::CaseInsensitive) == 0);
            retrievedImage = QImage(retrievedUrl.toLocalFile());

            QCOMPARE(retrievedImage.size(), iconSize);
        }
    }
}

void tst_QPlaceManagerJsonDb::iconSourceDestination_data()
{
    QTest::addColumn<QString>("sourceIconResource");
    QTest::addColumn<QSize>("iconSize");
    QTest::addColumn<QString>("iconType");
    QTest::addColumn<QString>("source");
    QTest::addColumn<QString>("specifiedDestination");

    //Specify source and destionation, expect file to be copied into destination file
    QTest::newRow("source destination small file") << ":/resources/icon_small.png" << QSize(20,20) << "file" << SmallSource << SmallDestination;
    QTest::newRow("source destination medium file") << ":/resources/icon_medium.png" << QSize(30,30) << "file" << MediumSource << MediumDestination;
    QTest::newRow("source destination large file") << ":/resources/icon_large.png" << QSize(50,50) << "file" << LargeSource << LargeDestination;
    QTest::newRow("source destination fullscreen file") << ":/resources/icon_fullscreen.png" << QSize(320,480) << "file" << FullscreenSource << FullscreenDestination;

    //Specify source and destination, expect data url to be copied into destination url
    QTest::newRow("source destination small dataUrl") << ":/resources/icon_small.png" << QSize(20,20) << "dataUrl" << SmallSource << SmallDestination;
    QTest::newRow("source destination medium dataUrl") << ":/resources/icon_medium.png" << QSize(30,30) << "dataUrl" << MediumSource << MediumDestination;
    QTest::newRow("source destination large dataUrl") << ":/resources/icon_large.png" << QSize(50,50) << "dataUrl" << LargeSource << LargeDestination;
    QTest::newRow("source destination fullscreen dataUrl") << ":/resources/icon_fullscreen.png" << QSize(320,480) << "dataUrl" << FullscreenSource << FullscreenDestination;

    //try using improper source file urls, e.g. /some/path rather than file:///some/path
    QTest::newRow("source destination small file_improperUrl") << ":/resources/icon_small.png" << QSize(20,20) << "file_improperUrl" << SmallSource << SmallDestination;
    QTest::newRow("source destination medium file_improperUrl") << ":/resources/icon_medium.png" << QSize(30,30) << "file_improperUrl" << MediumSource << MediumDestination;
    QTest::newRow("source destination large file_improperUrl") << ":/resources/icon_large.png" << QSize(50,50) << "file_improperUrl" << LargeSource << LargeDestination;
    QTest::newRow("source destination fullscreen file_improperUrl") << ":/resources/icon_fullscreen.png" << QSize(320,480) << "file_improperUrl" << FullscreenSource << FullscreenDestination;
}

void tst_QPlaceManagerJsonDb::iconSourceOnly()
{
    //We test that a dataUrl is generated from the given input icon
    //Soucre urls are provided while destination urls
    QFETCH(QString, sourceIconResource);
    QFETCH(QSize, iconSize);
    QFETCH(QString, iconType);
    QFETCH(QString, source);
    QFETCH(QString, expectedDestination);

    QPlace place;
    place.setName("place");
    QPlaceIcon icon;
    QVariantMap iconParams;

    QTemporaryFile sourceIconFile;
    if (iconType.startsWith(QLatin1String("file"))) {
        QImage sourceIconImage(sourceIconResource);
        sourceIconFile.open();
        sourceIconImage.save(sourceIconFile.fileName(), QImageReader::imageFormat(sourceIconResource));
        if (iconType == QLatin1String("file"))
            iconParams.insert(source, QUrl::fromLocalFile(sourceIconFile.fileName()));
        else if (iconType == QLatin1String("file_improperUrl"))
            iconParams.insert(source, QUrl(sourceIconFile.fileName()));
        else
            qFatal("Unknown iconType");
    } else if (iconType == QLatin1String("dataUrl")) {
        QFile sourceIcon(sourceIconResource);
        sourceIcon.open(QIODevice::ReadOnly);
        QString mimeType;
        if (QImageReader::imageFormat(sourceIconResource) == "png")
            mimeType = QLatin1String("image/png");
        else
            qFatal("Unexpected image format");
        QUrl dataUrl(QString::fromLatin1("data:") + mimeType + QLatin1String(";base64,") + sourceIcon.readAll().toBase64());
        iconParams.insert(source, dataUrl);
    } else if (iconType == QLatin1String("unaccessible_webUrl")) {
        iconParams.insert(source, QUrl(sourceIconResource));
    }

    icon.setParameters(iconParams);
    place.setIcon(icon);
    QString placeId;

    if (iconType == QLatin1String("unaccessible_webUrl")) {
        QVERIFY(doSavePlace(place,QPlaceReply::BadArgumentError));
    } else {
        QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
        QPlace retrievedPlace;
        QVERIFY(doFetchDetails(placeId, &retrievedPlace));
        if (expectedDestination != QString()) {
            QVERIFY(retrievedPlace.icon().parameters().contains(expectedDestination));

            QUrl retrievedUrl = retrievedPlace.icon().parameters().value(expectedDestination).toUrl();
            QImage retrievedImage;
            if (expectedDestination != QString())
                QVERIFY(retrievedUrl.scheme().compare(QLatin1String("data"), Qt::CaseInsensitive) == 0);
            retrievedImage = dataUrlToImage(retrievedUrl);
            QCOMPARE(retrievedImage.size(), iconSize);
        }
    }
}

void tst_QPlaceManagerJsonDb::iconSourceOnly_data()
{
    QTest::addColumn<QString>("sourceIconResource");
    QTest::addColumn<QSize>("iconSize");
    QTest::addColumn<QString>("iconType");
    QTest::addColumn<QString>("source");
    QTest::addColumn<QString>("expectedDestination");

    //input icon is a file
    QTest::newRow("source small file") << ":/resources/icon_small.png" << SmallSize << "file" << SmallSource <<  SmallDestination;
    QTest::newRow("source medium file") << ":/resources/icon_medium.png" << MediumSize << "file" << MediumSource <<  MediumDestination;
    QTest::newRow("source large file") << ":/resources/icon_large.png" << LargeSize << "file" << LargeSource <<  LargeDestination;
    QTest::newRow("source fullscreen file") << ":/resources/icon_fullscreen.png" << FullscreenSize << "file" << FullscreenSource << ""; //fullscreen icons shouldn't be saved as data urls

    //input is a file but an improper url (e.g of the format /some/path instead of file:///some/path)
    QTest::newRow("source small file_improperUrl") << ":/resources/icon_small.png" << SmallSize << "file_improperUrl" << SmallSource <<  SmallDestination;
    QTest::newRow("source medium file_improperUrl") << ":/resources/icon_medium.png" << MediumSize << "file_improperUrl" << MediumSource <<  MediumDestination;
    QTest::newRow("source large file_improperUrl") << ":/resources/icon_large.png" << LargeSize << "file_improperUrl" << LargeSource <<  LargeDestination;
    QTest::newRow("source fullscreen file_improperUrl") << ":/resources/icon_fullscreen.png" << FullscreenSize << "file_improperUrl" << FullscreenSource << ""; //fullscreen icons shouldn't be saved as data urls

    //input icon is a data url
    QTest::newRow("source small dataUrl") << ":/resources/icon_small.png" << SmallSize << "dataUrl" << SmallSource << SmallDestination;
    QTest::newRow("source medium dataUrl") << ":/resources/icon_medium.png" << MediumSize << "dataUrl" << MediumSource << MediumDestination;
    QTest::newRow("source large dataUrl") << ":/resources/icon_large.png" << LargeSize << "dataUrl" << LargeSource << LargeDestination;
    QTest::newRow("source fullscreen dataUrl") << ":/resources/icon_fullscreen.png" << FullscreenSize << "dataUrl" << FullscreenSource << ""; //fullscreen icons shouldn't be saved as data urls

    //input icon is a non reachable url
    QTest::newRow("source small unaccessible_webUrl") << "www.example.com/icon_small.png" << SmallSize << "unaccessible_webUrl" << SmallSource << SmallDestination;
    QTest::newRow("source medium unaccessible_webUrl") << "www.example.com/icon_medium.png" << MediumSize << "unaccessible_webUrl" << MediumSource << MediumDestination;
    QTest::newRow("source large unaccessible_webUrl") << "www.example.com/icon_large.png" << LargeSize << "unaccessible_webUrl" << LargeSource << LargeDestination;
    QTest::newRow("source fullscreen unaccessible_webUrl") << "www.example.com/icon_fullscreen.png" << FullscreenSize << "unaccessible_webUrl" << FullscreenSource << ""; //fullscreen icons shouldn't be saved as data urls
}

void tst_QPlaceManagerJsonDb::iconDestinationOnly()
{
    //We test when a destination url is provided, that it is correctly saved
    //No soure urls are provided while destination urls are.
    QFETCH(QString, destination);
    QFETCH(QUrl, destinationUrl);
    QFETCH(QSize, specifiedSize);
    QFETCH(QString, iconResource); //empty reosurce indicates icon is not reachable/accessible

    QString destinationSize = destination;
    destinationSize.replace(QLatin1String("Url"), QLatin1String("Size"));

    QVariantMap iconParams;

    QTemporaryFile destinationFile;
    QSize size;
    if (destinationUrl.toString().startsWith("_autogenerated_file_")) {
        QImage iconImage(iconResource);
        destinationFile.open();
        iconImage.save(destinationFile.fileName(), QImageReader::imageFormat(iconResource));
        size = iconImage.size();

        if (destinationUrl == QUrl("_autogenerated_file_no_scheme_url_"))
            destinationUrl = QUrl::fromLocalFile(destinationFile.fileName());
        else
            destinationUrl = QUrl::fromLocalFile(destinationFile.fileName());
    } else if (destinationUrl == QUrl("_data_url_")) {
        QFile iconFile(iconResource);
        iconFile.open(QIODevice::ReadOnly);

        QString mimeType;
        if (QImageReader::imageFormat(iconResource) == "png")
            mimeType = QLatin1String("image/png");

        QImage iconImage(iconResource);
        size = iconImage.size();

        destinationUrl = QUrl(QString::fromLatin1("data:") + mimeType + QLatin1String(";base64,") + iconFile.readAll().toBase64());
    }

    iconParams.insert(destination, destinationUrl);

    if (!specifiedSize.isEmpty())
        iconParams.insert(destinationSize, specifiedSize);

    QPlace place;
    place.setName("Place");
    QPlaceIcon icon;
    icon.setParameters(iconParams);
    place.setIcon(icon);
    QString placeId;

    if (specifiedSize.isEmpty() && iconResource.isEmpty()) {
        QVERIFY(doSavePlace(place, QPlaceReply::BadArgumentError));
        return;
    } else {
        QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
    }

    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));

    QVERIFY(retrievedPlace.icon().parameters().keys().contains(destination));
    QCOMPARE(retrievedPlace.icon().parameters().value(destination).toUrl(), destinationUrl);

    if (!specifiedSize.isEmpty() && iconResource.isEmpty()) {
        QVERIFY(retrievedPlace.icon().parameters().contains(destinationSize));
        QCOMPARE(retrievedPlace.icon().parameters().value(destinationSize).toSize(), specifiedSize);
    } else {
        QCOMPARE(retrievedPlace.icon().parameters().value(destinationSize).toSize(), size);
    }
}

void tst_QPlaceManagerJsonDb::iconDestinationOnly_data()
{
    QTest::addColumn<QString>("destination");
    QTest::addColumn<QUrl>("destinationUrl");
    QTest::addColumn<QSize>("specifiedSize");
    QTest::addColumn<QString>("iconResource");

    //destination url is local file, no size is specified
    QTest::newRow("destination small file no_size_provided") << SmallDestination  << QUrl("_autogenerated_file_") << QSize() << ":/resources/icon_small.png";
    QTest::newRow("destination medium file no_size_provided") << MediumDestination  << QUrl("_autogenerated_file_") << QSize() << ":/resources/icon_medium.png";
    QTest::newRow("destination large file no_size_provided") << LargeDestination  << QUrl("_autogenerated_file_") << QSize() << ":/resources/icon_large.png";
    QTest::newRow("destination fullscreen file no_size_provided") << FullscreenDestination  << QUrl("_autogenerated_file_") << QSize() << ":/resources/icon_fullscreen.png";

    //destination url is local file, no size is specified, the url has no scheme
    QTest::newRow("destination small file no_size_provided") << SmallDestination  << QUrl("_autogenerated_file_no_scheme_url_") << QSize() << ":/resources/icon_small.png";
    QTest::newRow("destination medium file no_size_provided") << MediumDestination  << QUrl("_autogenerated_file_no_scheme_url_") << QSize() << ":/resources/icon_medium.png";
    QTest::newRow("destination large file no_size_provided") << LargeDestination  << QUrl("_autogenerated_file_no_scheme_url_") << QSize() << ":/resources/icon_large.png";
    QTest::newRow("destination fullscreen file no_size_provided") << FullscreenDestination  << QUrl("_autogenerated_file_no_scheme_url_") << QSize() << ":/resources/icon_fullscreen.png";

    //destination url is local file and an (inaccurate) size is specified, (the actual size of file should take precedence)
    QTest::newRow("destination small file size_provided") << SmallDestination  << QUrl("_autogenerated_file_") << QSize(86,99) << ":/resources/icon_small.png";
    QTest::newRow("destination medium file size_provided") << MediumDestination  << QUrl("_autogenerated_file_") << QSize(86, 99) << ":/resources/icon_medium.png";
    QTest::newRow("destination large file size_provided") << LargeDestination  << QUrl("_autogenerated_file_") << QSize(86, 99) << ":/resources/icon_large.png";
    QTest::newRow("destination fulscreen file size_provided") << FullscreenDestination  << QUrl("_autogenerated_file_") << QSize(86, 99) << ":/resources/icon_fullscreen.png";

    //destination url is a data url, no size is specified
    QTest::newRow("destination small dataurl no_size_provided") << SmallDestination  << QUrl("_data_url_") << QSize() << ":/resources/icon_small.png";
    QTest::newRow("destination medium dataurl no_size_provided") << MediumDestination  << QUrl("_data_url_") << QSize() << ":/resources/icon_medium.png";
    QTest::newRow("destination large dataurl no_size_provided") << LargeDestination  << QUrl("_data_url_") << QSize() << ":/resources/icon_large.png";
    QTest::newRow("destination fullscreen dataurl no_size_provided") << FullscreenDestination  << QUrl("_data_url_") << QSize() << ":/resources/icon_fullscreen.png";

    //destination url is a data url, and an (inaccurate) size is specified, (the actual size of the icon should take precedence)
    QTest::newRow("destination small dataurl size_provided") << SmallDestination  << QUrl("_data_url_") << QSize(86,99) << ":/resources/icon_small.png";
    QTest::newRow("destination medium dataurl size_provided") << MediumDestination  << QUrl("_data_url_") << QSize(86, 99) << ":/resources/icon_medium.png";
    QTest::newRow("destination large dataurl size_provided") << LargeDestination  << QUrl("_data_url_") << QSize(86, 99) << ":/resources/icon_large.png";
    QTest::newRow("destination fullscreen dataurl size_provided") << FullscreenDestination  << QUrl("_data_url_") << QSize(86, 99) << ":/resources/icon_fullscreen.png";

    //destination url is not accessible and size is specified
    QTest::newRow("destination small unaccessible_webUrl size_provided") << SmallDestination << QUrl("http://www.example.com/iconS.png") << SmallSize << "";
    QTest::newRow("destination medium unaccessible_webUrl size_provided") << MediumDestination << QUrl("http://www.example.com/iconM.png") << MediumSize << "" ;
    QTest::newRow("destination large unaccessible_webUrl size_provided") << LargeDestination << QUrl("http://www.example.com/iconL.png") << LargeSize << "";
    QTest::newRow("destination fullscreen unaccessible_webUrl size_provided") << FullscreenDestination << QUrl("http://www.example.com/iconF.png") << FullscreenSize << "";

    //destination url is accessible and no size is provided, expect failure to save
    QTest::newRow("destination small unaccessible_webUrl no_size_provided") << SmallDestination << QUrl("http://www.example.com/iconS.png") << QSize() << "";
    QTest::newRow("destination medium unaccessible_webUrl no_size_provided") << MediumDestination << QUrl("http://www.example.com/iconM.png") << QSize() << "" ;
    QTest::newRow("destination large unaccessible_webUrl no_size_provided") << LargeDestination << QUrl("http://www.example.com/iconL.png") << QSize() << "";
    QTest::newRow("destination fullscreen unaccessible_webUrl no_size_provided") << FullscreenDestination << QUrl("http://www.example.com/iconL.png") << QSize() << "";
}

void tst_QPlaceManagerJsonDb::iconSavedFromDifferentManager()
{
    QGeoServiceProvider geoTest("qmlgeo.test.plugin");
    QPlaceManager *geoTestManager = geoTest.placeManager();

    QPlaceIcon icon;
    icon.setManager(geoTestManager);
    QVariantMap iconParams;

    QTemporaryFile sourceIconFile;
    QImage sourceIconImage(":/resources/icon_small.png");
    sourceIconFile.open();
    sourceIconImage.save(sourceIconFile.fileName(), "png");

    //try an icon from another manager which different icons for each size variant
    //check that we get correctly generated data urls
    iconParams.insert("s", QUrl::fromLocalFile(":/resources/icon_small.png"));
    iconParams.insert("m", QUrl::fromLocalFile(":/resources/icon_medium.png"));
    iconParams.insert("l", QUrl::fromLocalFile(":/resources/icon_large.png"));
    iconParams.insert("f", QUrl::fromLocalFile(":/resources/icon_fullscreen.png"));
    icon.setParameters(iconParams);

    QPlace place;
    place.setName("Place");
    place.setIcon(icon);

    QPlace compatiblePlace = placeManager->compatiblePlace(place);
    QString placeId;
    QVERIFY(doSavePlace(compatiblePlace,QPlaceReply::NoError, &placeId));

    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));

    QVERIFY(retrievedPlace.icon().parameters().keys().contains(SmallDestination));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(SmallDestinationSize));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(MediumDestination));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(MediumDestinationSize));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(LargeDestination));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(LargeDestinationSize));

    //we don't expect fullscreen icon urls to be download into data urls
    //data urls are only meant for small icons
    QVERIFY(!retrievedPlace.icon().parameters().keys().contains(FullscreenDestination));

    QImage smallImage = dataUrlToImage(retrievedPlace.icon().parameters().value(SmallDestination).toUrl());
    QImage mediumImage = dataUrlToImage(retrievedPlace.icon().parameters().value(MediumDestination).toUrl());
    QImage largeImage = dataUrlToImage(retrievedPlace.icon().parameters().value(LargeDestination).toUrl());

    QCOMPARE(smallImage.size(), SmallSize);
    QCOMPARE(mediumImage.size(), MediumSize);
    QCOMPARE(largeImage.size(), LargeSize);

    QCOMPARE(retrievedPlace.icon().parameters().value(SmallDestinationSize).toSize(), SmallSize);
    QCOMPARE(retrievedPlace.icon().parameters().value(MediumDestinationSize).toSize(), MediumSize);
    QCOMPARE(retrievedPlace.icon().parameters().value(LargeDestinationSize).toSize(), LargeSize);

    //try an icon from another manaager which has only a single size variant
    //check that we get only one size variant
    iconParams.clear();
    iconParams.insert("s", QUrl::fromLocalFile(":/resources/icon_small.png"));
    icon.setParameters(iconParams);
    place.setIcon(icon);
    compatiblePlace = placeManager->compatiblePlace(place);
    QVERIFY(doSavePlace(compatiblePlace,QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));

    QCOMPARE(retrievedPlace.icon().parameters().keys().count(), 2);
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(SmallDestination));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(SmallDestinationSize));
    smallImage = dataUrlToImage(retrievedPlace.icon().parameters().value(SmallDestination).toUrl());
    QCOMPARE(smallImage.size(), SmallSize);
    QSize size = retrievedPlace.icon().parameters().value(SmallDestinationSize).toSize();
    QCOMPARE(size, SmallSize);

    //try an icon from a manager which has sizes which don't exactly match the standard expected icons
    //sizes, check that dataUrls are generated for the appropriate size variant
    iconParams.clear();
    iconParams.insert("m", QUrl::fromLocalFile(":/resources/icon_24x24.png"));
    iconParams.insert("l", QUrl::fromLocalFile(":/resources/icon_40x40.png"));
    icon.setParameters(iconParams);
    place.setIcon(icon);
    compatiblePlace = placeManager->compatiblePlace(place);
    QVERIFY(doSavePlace(compatiblePlace,QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.icon().parameters().keys().count(), 4);
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(SmallDestination));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(SmallDestinationSize));
    smallImage = dataUrlToImage(retrievedPlace.icon().parameters().value(SmallDestination).toUrl());
    QCOMPARE(smallImage.size(), QSize(24,24));
    size = retrievedPlace.icon().parameters().value(SmallDestinationSize).toSize();
    QCOMPARE(size, QSize(24,24));

    QVERIFY(retrievedPlace.icon().parameters().keys().contains(LargeDestination));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(LargeDestinationSize));
    smallImage = dataUrlToImage(retrievedPlace.icon().parameters().value(LargeDestination).toUrl());
    QCOMPARE(smallImage.size(), QSize(40,40));
    size = retrievedPlace.icon().parameters().value(LargeDestinationSize).toSize();
    QCOMPARE(size, QSize(40,40));
}

void tst_QPlaceManagerJsonDb::iconFormats()
{
    QFETCH(QByteArray, imageFormat);
    QFETCH(QString, mimeType);

    QTemporaryFile sourceIconFile;
    sourceIconFile.open();
    QImage sourceIconImage;

    if (imageFormat == "svg") {
        QFile file(":/resources/icon_small.svg");
        file.open(QIODevice::ReadOnly);
        sourceIconFile.write(file.readAll());
        sourceIconFile.close();
    } else {
        QVERIFY(sourceIconImage.load(":/resources/icon_small.png"));
        QVERIFY(sourceIconImage.save(&sourceIconFile, imageFormat));
    }

    QVariantMap iconParams;
    iconParams.insert(SmallSource, QUrl::fromLocalFile(sourceIconFile.fileName()));

    QPlace place;
    place.setName("place");
    QPlaceIcon icon;
    icon.setParameters(iconParams);
    place.setIcon(icon);

    QString placeId;

    if (imageFormat == "tiff")
        QEXPECT_FAIL("", "tiff format known to fail as documented in QTBUG-23898", Abort);

    if (imageFormat == "svg" && !QImageReader::supportedImageFormats().contains("svg")) {
        //svg format is not supported therefore saving of the icon should fail
        //in this instance we are creating a data url and therefore need to be
        //able to detect the image format and mime type.
        QVERIFY(doSavePlace(place,QPlaceReply::BadArgumentError));
    } else {
        QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));

        QPlace retrievedPlace;
        QVERIFY(doFetchDetails(placeId, &retrievedPlace));

        QRegExp regExp("^data:(.*);.*$");
        regExp.indexIn(retrievedPlace.icon().parameters().value(SmallDestination).toUrl().toString());
        QCOMPARE(regExp.cap(1), mimeType);
        QCOMPARE(dataUrlToImage(retrievedPlace.icon().parameters().value(SmallDestination).toUrl()).size(), SmallSize);
    }
}

void tst_QPlaceManagerJsonDb::iconFormats_data()
{
    QTest::addColumn<QByteArray>("imageFormat");
    QTest::addColumn<QString>("mimeType");

    QTest::newRow("bmp") << QByteArray("bmp") << "image/bmp";
    QTest::newRow("jpg") << QByteArray("jpg") << "image/jpeg";
    QTest::newRow("jpeg") << QByteArray("jpeg") << "image/jpeg";
    QTest::newRow("png") << QByteArray("png") << "image/png";
    QTest::newRow("pbm") << QByteArray("pbm") << "image/x-portable-bitmap";
    QTest::newRow("pgm") << QByteArray("pgm") << "image/x-portable-graymap";
    QTest::newRow("ppm") << QByteArray("ppm") << "image/x-portable-pixmap";
    QTest::newRow("tiff") << QByteArray("tiff") << "image/tiff";
    QTest::newRow("xbm") << QByteArray("xbm") << "image/x-xbitmap";
    QTest::newRow("xpm") << QByteArray("xpm") << "image/x-xpixmap";
    QTest::newRow("svg") << QByteArray("svg") << "image/svg+xml";
}

void tst_QPlaceManagerJsonDb::iconUrls()
{
    QFETCH(QString, sizeType);
    QFETCH(QSize, size);

    QString source = sizeType + QLatin1String("SourceUrl");
    QString destination = sizeType + QLatin1String("Url");
    QString destinationSize = sizeType + QLatin1String("Size");

    QPlace place;
    place.setName("place");
    QPlaceIcon icon;
    QVariantMap iconParams;

    //test conversion to valid url, in this care using a valid file scheme
    iconParams.insert(destination, QUrl("/home/user/icon.png"));
    iconParams.insert(destinationSize, size);
    icon.setParameters(iconParams);
    place.setIcon(icon);

    QString placeId;
    QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));

    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.icon().parameters().value(destination).toUrl(), QUrl("file:///home/user/icon.png"));

    //test conversion to valid url,
    iconParams.insert(destination, QUrl("qrc:/home/user/icon.png"));
    icon.setParameters(iconParams);
    place.setIcon(icon);
    QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.icon().parameters().value(destination).toUrl(), QUrl("qrc:///home/user/icon.png"));

    iconParams.insert(destination, QUrl("qrc:///home/user/icon.png"));
    icon.setParameters(iconParams);
    place.setIcon(icon);
    QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.icon().parameters().value(destination).toUrl(), QUrl("qrc:///home/user/icon.png"));

    //test urls that are non-encoded and encoded
    iconParams.insert(destination, QUrl("qrc:///home/user/i con.png"));
    icon.setParameters(iconParams);
    place.setIcon(icon);
    QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.icon().parameters().value(destination).toUrl(), QUrl("qrc:///home/user/i con.png"));

    iconParams.insert(destination, QUrl("qrc:///home/user/ico%20n.png"));
    icon.setParameters(iconParams);
    place.setIcon(icon);
    QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.icon().parameters().value(destination).toUrl(), QUrl("qrc:///home/user/ico n.png"));

    //try using a QString instead of a url in the parameters
    iconParams.insert(destination, "www.example.com");
    icon.setParameters(iconParams);
    place.setIcon(icon);
    QVERIFY(doSavePlace(place,QPlaceReply::BadArgumentError, &placeId));

    //try using a QString instead of a url in the parameters
    iconParams.insert(source, "www.example.com");
    icon.setParameters(iconParams);
    place.setIcon(icon);
    QVERIFY(doSavePlace(place,QPlaceReply::BadArgumentError, &placeId));
}

void tst_QPlaceManagerJsonDb::iconUrls_data()
{
    QTest::addColumn<QString>("sizeType");
    QTest::addColumn<QSize>("size");

    QTest::newRow("small") << "small" << SmallSize;
    QTest::newRow("medium") << "medium" << MediumSize;
    QTest::newRow("large") << "large" << LargeSize;
    QTest::newRow("fullscreen") << "fullscreen" << FullscreenSize;
}

void tst_QPlaceManagerJsonDb::constructIconUrl()
{
    QPlaceIcon icon;
    QVariantMap iconParams;

    //try with all possible size buckets set
    iconParams.insert(SmallDestination, QUrl("http://www.example.com/icon_small.png"));
    iconParams.insert(SmallDestinationSize, SmallSize);
    iconParams.insert(MediumDestination, QUrl("http://www.example.com/icon_medium.png"));
    iconParams.insert(MediumDestinationSize, MediumSize);
    iconParams.insert(LargeDestination, QUrl("http://www.example.com/icon_large.png"));
    iconParams.insert(LargeDestinationSize, LargeSize);
    iconParams.insert(FullscreenDestination, QUrl("http://www.example.com/icon_fullscreen.png"));
    iconParams.insert(FullscreenDestinationSize, FullscreenSize);

    icon.setParameters(iconParams);
    icon.setManager(placeManager);

    QCOMPARE(icon.url(SmallSize), QUrl("http://www.example.com/icon_small.png"));
    QCOMPARE(icon.url(MediumSize), QUrl("http://www.example.com/icon_medium.png"));
    QCOMPARE(icon.url(LargeSize), QUrl("http://www.example.com/icon_large.png"));
    QCOMPARE(icon.url(FullscreenSize), QUrl("http://www.example.com/icon_fullscreen.png"));

    //try with only a single destination icon set
    iconParams.clear();
    iconParams.insert(LargeDestination, QUrl("http://www.example.com/icon_large.png"));
    iconParams.insert(LargeDestinationSize, LargeSize);
    icon.setParameters(iconParams);

    QCOMPARE(icon.url(SmallSize), QUrl("http://www.example.com/icon_large.png"));
    QCOMPARE(icon.url(MediumSize), QUrl("http://www.example.com/icon_large.png"));
    QCOMPARE(icon.url(LargeSize), QUrl("http://www.example.com/icon_large.png"));
    QCOMPARE(icon.url(FullscreenSize), QUrl("http://www.example.com/icon_large.png"));

    //try requesting non-standard sizes and don't populate all size buckets
    iconParams.clear();
    iconParams.insert(SmallDestination, QUrl("http://www.example.com/icon_small.png"));
    iconParams.insert(SmallDestinationSize, SmallSize);
    iconParams.insert(LargeDestination, QUrl("http://www.example.com/icon_large.png"));
    iconParams.insert(LargeDestinationSize, LargeSize);
    icon.setParameters(iconParams);

    QCOMPARE(icon.url(QSize(10,10)), QUrl("http://www.example.com/icon_small.png"));
    QCOMPARE(icon.url(QSize(34,34)), QUrl("http://www.example.com/icon_small.png"));
    QCOMPARE(icon.url(QSize(35,35)), QUrl("http://www.example.com/icon_large.png"));
    QCOMPARE(icon.url(QSize(60,60)), QUrl("http://www.example.com/icon_large.png"));

    //try the icons themselves haveing non standard sizes
    iconParams.clear();
    iconParams.insert(SmallDestination, QUrl("http://www.example.com/icon_small.png"));
    iconParams.insert(SmallDestinationSize, QSize(25,25));
    iconParams.insert(LargeDestination, QUrl("http://www.example.com/icon_large.png"));
    iconParams.insert(LargeDestinationSize, QSize(35,35));
    icon.setParameters(iconParams);

    QCOMPARE(icon.url(QSize(24,24)), QUrl("http://www.example.com/icon_small.png"));
    QCOMPARE(icon.url(QSize(29,29)), QUrl("http://www.example.com/icon_small.png"));
    QCOMPARE(icon.url(QSize(30,30)), QUrl("http://www.example.com/icon_large.png"));
    QCOMPARE(icon.url(QSize(60,60)), QUrl("http://www.example.com/icon_large.png"));

    //TODO: edge case testing for all combinations
}

void tst_QPlaceManagerJsonDb::cleanup()
{
    QSignalSpy cleanSpy(dbCleaner, SIGNAL(dbCleaned()));
    dbCleaner->cleanDb();
    QTRY_VERIFY(cleanSpy.count() == 1);
}

bool tst_QPlaceManagerJsonDb::doSavePlace(const QPlace &place,
                                          QPlaceReply::Error expectedError,
                                          QString *placeId)
{
    QPlaceIdReply *saveReply = placeManager->savePlace(place);
    bool isSuccessful = checkSignals(saveReply, expectedError);
    if (placeId != 0) {
        *placeId = saveReply->id();
    }

    if (saveReply->id().isEmpty() && expectedError == QPlaceReply::NoError) {
        qWarning("ID is empty in reply for save operation");
        qWarning() << "Error string = " << saveReply->errorString();
        isSuccessful = false;
    }

    return isSuccessful;
}

void tst_QPlaceManagerJsonDb::doSavePlaces(QList<QPlace> &places)
{
    QPlaceIdReply *saveReply;

    foreach (QPlace place, places) {
        saveReply = placeManager->savePlace(place);
        QSignalSpy saveSpy(saveReply, SIGNAL(finished()));
        TRY_VERIFY_WITH_TIMEOUT(saveSpy.count() == 1, 10000);
        QCOMPARE(saveReply->error(), QPlaceReply::NoError);
        saveSpy.clear();
    }
}

void tst_QPlaceManagerJsonDb::doSavePlaces(const QList<QPlace *> &places)
{
    QPlaceIdReply *saveReply;

    static int count= 0;
    foreach (QPlace *place, places) {
        count++;
        saveReply = placeManager->savePlace(*place);
        QSignalSpy saveSpy(saveReply, SIGNAL(finished()));
        TRY_VERIFY_WITH_TIMEOUT(saveSpy.count() == 1, 10000);
        QCOMPARE(saveReply->error(), QPlaceReply::NoError);
        place->setPlaceId(saveReply->id());
        saveSpy.clear();
    }
}

bool tst_QPlaceManagerJsonDb::doRemovePlace(const QPlace &place,
                                            QPlaceReply::Error expectedError)
{
    QPlaceIdReply *removeReply = placeManager->removePlace(place.placeId());
    bool isSuccessful = false;
    isSuccessful = checkSignals(removeReply, expectedError)
                    && (removeReply->id() == place.placeId());
    return isSuccessful;
}

bool tst_QPlaceManagerJsonDb::doSearch(const QPlaceSearchRequest &request,
                                       QList<QPlaceSearchResult> *results, QPlaceReply::Error expectedError)
{
    QPlaceSearchReply *searchReply= placeManager->search(request);
    bool success = checkSignals(searchReply, expectedError);
    *results = searchReply->results();
    return success;
}

bool tst_QPlaceManagerJsonDb::doSearch(const QPlaceSearchRequest &request,
                                       QList<QPlace> *results, QPlaceReply::Error expectedError)
{
    bool success = false;
    results->clear();
    QList<QPlaceSearchResult> searchResults;
    success = doSearch(request, &searchResults, expectedError);
    foreach (const QPlaceSearchResult &searchResult, searchResults)
        results->append(searchResult.place());
    return success;
}

bool tst_QPlaceManagerJsonDb::doFetchDetails(QString placeId, QPlace *place, QPlaceReply::Error expectedError)
{
    QPlaceDetailsReply *detailsReply = placeManager->getPlaceDetails(placeId);
    bool success = checkSignals(detailsReply, expectedError);
    *place = detailsReply->place();
    return success;
}

bool tst_QPlaceManagerJsonDb::doSaveCategory(const QPlaceCategory &category,
                                          QPlaceReply::Error expectedError,
                                          QString *categoryId)
{
    QPlaceIdReply *saveReply = placeManager->saveCategory(category);
    bool isSuccessful = false;
    isSuccessful = checkSignals(saveReply, expectedError);
    if (categoryId != 0)
        *categoryId = saveReply->id();
    return isSuccessful;
}

bool tst_QPlaceManagerJsonDb::doSaveCategory(const QPlaceCategory &category,
                                             const QString &parentId,
                                          QPlaceReply::Error expectedError,
                                          QString *categoryId)
{
    QPlaceIdReply *idReply = placeManager->saveCategory(category, parentId);
    bool isSuccessful = checkSignals(idReply, expectedError)
                        && (idReply->error() == expectedError);

    if (categoryId != 0)
        *categoryId = idReply->id();
    return isSuccessful;
}

bool tst_QPlaceManagerJsonDb::doRemoveCategory(const QPlaceCategory &category,
                                          QPlaceReply::Error expectedError)
{
    QPlaceIdReply *idReply = placeManager->removeCategory(category.categoryId());

    bool isSuccessful = checkSignals(idReply, expectedError) &&
                        (idReply->error() == expectedError);
    return isSuccessful;
}

bool tst_QPlaceManagerJsonDb::checkSignals(QPlaceReply *reply, QPlaceReply::Error expectedError)
{
    QSignalSpy finishedSpy(reply, SIGNAL(finished()));
    QSignalSpy errorSpy(reply, SIGNAL(error(QPlaceReply::Error,QString)));
    QSignalSpy managerFinishedSpy(placeManager, SIGNAL(finished(QPlaceReply*)));
    QSignalSpy managerErrorSpy(placeManager,SIGNAL(error(QPlaceReply*,QPlaceReply::Error,QString)));

    if (expectedError != QPlaceReply::NoError) {
        //check that we get an error signal from the reply
        WAIT_UNTIL(errorSpy.count() == 1);
        if (errorSpy.count() != 1) {
            qWarning() << "Error signal for search operation not received";
            return false;
        }

        //check that we get the correct error from the reply's signal
        QPlaceReply::Error actualError = qvariant_cast<QPlaceReply::Error>(errorSpy.at(0).at(0));
        if (actualError != expectedError) {
            qWarning() << "Actual error code in reply signal does not match expected error code";
            qWarning() << "Actual error code = " << actualError;
            qWarning() << "Expected error coe =" << expectedError;
            return false;
        }

        //check that we get an error  signal from the manager
        WAIT_UNTIL(managerErrorSpy.count() == 1);
        if (managerErrorSpy.count() !=1) {
           qWarning() << "Error signal from manager for search operation not received";
           return false;
        }

        //check that we get the correct reply instance in the error signal from the manager
        if (qvariant_cast<QPlaceReply*>(managerErrorSpy.at(0).at(0)) != reply)  {
            qWarning() << "Reply instance in error signal from manager is incorrect";
            return false;
        }

        //check that we get the correct error from the signal of the manager
        actualError = qvariant_cast<QPlaceReply::Error>(managerErrorSpy.at(0).at(1));
        if (actualError != expectedError) {
            qWarning() << "Actual error code from manager signal does not match expected error code";
            qWarning() << "Actual error code =" << actualError;
            qWarning() << "Expected error code = " << expectedError;
            return false;
        }
    }

    //check that we get a finished signal
    WAIT_UNTIL(finishedSpy.count() == 1);
    if (finishedSpy.count() !=1) {
        qWarning() << "Finished signal from reply not received";
        return false;
    }

    if (reply->error() != expectedError) {
        qWarning() << "Actual error code does not match expected error code";
        qWarning() << "Actual error code: " << reply->error();
        qWarning() << "Expected error code" << expectedError;
        return false;
    }

    if (expectedError == QPlaceReply::NoError && !reply->errorString().isEmpty()) {
        qWarning() << "Expected error was no error but error string was not empty";
        qWarning() << "Error string=" << reply->errorString();
        return false;
    }

    //check that we get the finished signal from the manager
    WAIT_UNTIL(managerFinishedSpy.count() == 1);
    if (managerFinishedSpy.count() != 1) {
        qWarning() << "Finished signal from manager not received";
        return false;
    }

    //check that the reply instance in the finished signal from the manager is correct
    if (qvariant_cast<QPlaceReply *>(managerFinishedSpy.at(0).at(0)) != reply) {
        qWarning() << "Reply instance in finished signal from manager is incorrect";
        return false;
    }

    return true;
}

bool tst_QPlaceManagerJsonDb::doMatch(const QPlaceMatchRequest &request,
                                      QList<QPlace> *places,
                                      QPlaceReply::Error expectedError)
{
    QPlaceMatchReply *reply = placeManager->matchingPlaces(request);
    bool isSuccessful = checkSignals(reply, expectedError) &&
                        (reply->error() == expectedError);
    *places = reply->places();
    return isSuccessful;
}

//Assumes all place names are unique
bool tst_QPlaceManagerJsonDb::compareResultsByName(const QList<QPlaceSearchResult> &results,
                                             const QList<QPlace> &expectedResults)
{
    QSet<QString> resultNames;
    foreach (const QPlaceSearchResult &result, results)
        resultNames.insert(result.place().name());

    QSet<QString> expectedNames;
    foreach (const QPlace &place, expectedResults)
        expectedNames.insert(place.name());

    bool isMatch = (resultNames == expectedNames);
    if (results.count() != expectedResults.count() || !isMatch) {
        qWarning() << "comparison of results by name does not match";
        qWarning() << "result names: " << resultNames;
        qWarning() << "expected names: " << expectedNames;
        return false;
    }

    return isMatch;
}

QImage tst_QPlaceManagerJsonDb::dataUrlToImage(const QUrl &url)
{
    QByteArray data = QByteArray::fromPercentEncoding(url.toEncoded());
    data.remove(0,5);
    int pos = data.indexOf(',');
    if (pos != -1) {
        QByteArray payload = QByteArray::fromBase64(data.mid(pos + 1));
        data.truncate(pos);

        if (!data.endsWith(";base64")) {
            qWarning() << "Data url payload not base64 encoded";
            return QImage();
        }

         return QImage::fromData(payload);
    }

    return QImage();
}

QTEST_APPLESS_MAIN(tst_QPlaceManagerJsonDb)

#include "tst_qplacemanager_jsondb.moc"
