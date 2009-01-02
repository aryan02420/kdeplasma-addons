/*
 *   Copyright (C) 2007 Tobias Koenig <tokoe@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QtCore/QDate>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>

#include <KUrl>
#include <KServiceTypeTrader>
#include <KStandardDirs>

#include <solid/networking.h>

#include "comic.h"

#include "cachedprovider.h"

ComicEngine::ComicEngine( QObject* parent, const QVariantList& args )
    : Plasma::DataEngine( parent, args ), mEmptySuffix( false )
{
    setPollingInterval( 0 );
    updateFactories();
}

ComicEngine::~ComicEngine()
{
}

void ComicEngine::init()
{
}

void ComicEngine::updateFactories()
{
    mFactories.clear();
    KService::List services = KServiceTypeTrader::self()->query( "Plasma/Comic" );
    Q_FOREACH ( const KService::Ptr &service, services ) {
        mFactories.insert( service->property( "X-KDE-PluginInfo-Name", QVariant::String ).toString(),
                           service );
    }
}

bool ComicEngine::updateSourceEvent( const QString &identifier )
{
    if ( identifier == "providers" ) {
        KService::List services = KServiceTypeTrader::self()->query( "Plasma/Comic" );
        foreach ( const KService::Ptr &service, services ) {
            QStringList data;
            data << service->name();
            QFileInfo file( service->icon() );
            if ( file.isRelative() ) {
                data << KStandardDirs::locate( "data", QString( "plasma-comic/%1.png" ).arg( service->icon() ) );
            } else {
                data << service->icon();
            }
            setData( identifier, service->property( "X-KDE-PluginInfo-Name", QVariant::String ).toString(), data );
        }
        return true;
    } else {
        // check whether it is cached already...
        if ( CachedProvider::isCached( identifier ) ) {
            QVariantList args;
            args << "String" << identifier;

            ComicProvider *provider = new CachedProvider( this, args );
            connect( provider, SIGNAL( finished( ComicProvider* ) ), this, SLOT( finished( ComicProvider* ) ) );
            connect( provider, SIGNAL( error( ComicProvider* ) ), this, SLOT( error( ComicProvider* ) ) );
            return true;
        }

        // ... start a new query otherwise
        const QStringList parts = identifier.split( ':', QString::KeepEmptyParts );

        //: are mandatory
        if ( parts.count() < 2 ) {
            setData( identifier, "Error", true );
            return false;
        }
        if ( !mFactories.contains( parts[ 0 ] ) ) {
            // User might have installed more from GHNS
            updateFactories();
            if ( !mFactories.contains( parts[ 0 ] ) ) {
                setData( identifier, "Error", true );
                return false;
            }
        }

        // check if there is a connection
        Solid::Networking::Status status = Solid::Networking::status();
        if ( status != Solid::Networking::Connected && status != Solid::Networking::Unknown ) {
            setData( identifier, "Error", true );
            setData( identifier, "Previous identifier suffix", lastCachedIdentifier( identifier ) );
            return false;
        }

        const KService::Ptr service = mFactories[ parts[ 0 ] ];

        bool isCurrentComic = parts[ 1 ].isEmpty();

        QVariantList args;
        ComicProvider *provider = 0;

        const QString type = service->property( "X-KDE-PlasmaComicProvider-SuffixType", QVariant::String ).toString();
        if ( type == "Date" ) {
            QDate date = QDate::fromString( parts[ 1 ], Qt::ISODate );
            if ( !date.isValid() )
                date = QDate::currentDate();

            args << "Date" << date;
        } else if ( type == "Number" ) {
            args << "Number" << parts[ 1 ].toInt();
        } else if ( type == "String" ) {
            args << "String" << parts[ 1 ];
        }
        args << service->storageId();

        provider = service->createInstance<ComicProvider>( this, args );
        if ( !provider ) {
            setData( identifier, "Error", true );
            return false;
        }
        provider->setIsCurrent( isCurrentComic );

        connect( provider, SIGNAL( finished( ComicProvider* ) ), this, SLOT( finished( ComicProvider* ) ) );
        connect( provider, SIGNAL( error( ComicProvider* ) ), this, SLOT( error( ComicProvider* ) ) );
        return true;
    }
}

bool ComicEngine::sourceRequestEvent( const QString &identifier )
{
    setData( identifier, DataEngine::Data() );

    return updateSourceEvent( identifier );
}

void ComicEngine::finished( ComicProvider *provider )
{
    // sets the data
    setComicData( provider );

    // store in cache if it's not the response of a CachedProvider,
    // if there is a valid image and if there is a next comic
    // (if we're on today's comic it could become stale)
    if ( dynamic_cast<CachedProvider*>( provider ) == 0 && !provider->image().isNull() &&
         !provider->nextIdentifier().isEmpty() ) {
        CachedProvider::Settings info;

        info[ "websiteUrl" ] = provider->websiteUrl().prettyUrl();
        info[ "nextIdentifier" ] = provider->nextIdentifier();
        info[ "previousIdentifier" ] = provider->previousIdentifier();
        info[ "title" ] = provider->name();
        info[ "suffixType" ] = provider->suffixType();
        info[ "lastCachedStripIdentifier" ] = provider->identifier().mid( provider->identifier().indexOf( ':' ) + 1 );

        //data that should be only written if available
        if ( !provider->comicAuthor().isEmpty() ) {
            info[ "comicAuthor" ] = provider->comicAuthor();
        }
        if ( !provider->firstStripIdentifier().isEmpty() ) {
            info[ "firstStripIdentifier" ] = provider->firstStripIdentifier();
        }
        if ( !provider->additionalText().isEmpty() ) {
            info[ "additionalText" ] = provider->additionalText();
        }
        if ( !provider->stripTitle().isEmpty() ) {
            info[ "stripTitle" ] = provider->stripTitle();
        }

        CachedProvider::storeInCache( provider->identifier(), provider->image(), info );
    }
    provider->deleteLater();
}

void ComicEngine::error( ComicProvider *provider )
{
    // sets the data
    setComicData( provider );

    QString identifier( provider->identifier() );

    /**
     * Requests for the current day have no suffix (date or id)
     * set initially, so we have to remove the 'faked' suffix
     * here again to not confuse the applet.
     */
    if ( provider->isCurrent() )
        identifier = identifier.left( identifier.indexOf( ':' ) + 1 );

    setData( identifier, "Error", true );
    // sets the previousIdentifier to the identifier of a strip that has been cached before
    setData( identifier, "Previous identifier suffix", lastCachedIdentifier( identifier ) );
    setData( identifier, "Next identifier suffix", QString() );

    provider->deleteLater();
}

void ComicEngine::setComicData( ComicProvider *provider )
{
    QString identifier( provider->identifier() );

    /**
     * Requests for the current day have no suffix (date or id)
     * set initially, so we have to remove the 'faked' suffix
     * here again to not confuse the applet.
     */
    if ( provider->isCurrent() )
        identifier = identifier.left( identifier.indexOf( ':' ) + 1 );

    setData( identifier, "Image", provider->image() );
    setData( identifier, "Website Url", provider->websiteUrl() );
    setData( identifier, "Next identifier suffix", provider->nextIdentifier() );
    setData( identifier, "Previous identifier suffix", provider->previousIdentifier() );
    setData( identifier, "Comic Author", provider->comicAuthor() );
    setData( identifier, "Additional text", provider->additionalText() );
    setData( identifier, "Strip title", provider->stripTitle() );
    setData( identifier, "First strip identifier suffix", provider->firstStripIdentifier() );
    setData( identifier, "Identifier", provider->identifier() );
    setData( identifier, "Title", provider->name() );
    setData( identifier, "SuffixType", provider->suffixType() );
}

QString ComicEngine::lastCachedIdentifier( const QString &identifier ) const
{
        QString id = identifier.left( identifier.indexOf( ':' ) );
        QString data = KStandardDirs::locateLocal( "data", "plasma_engine_comic/" );
        data += QUrl::toPercentEncoding( id );
        QSettings settings( data + ".conf", QSettings::IniFormat );
        QString previousIdentifier = settings.value( "lastCachedStripIdentifier", QString() ).toString();

        return previousIdentifier;
}

#include "comic.moc"
