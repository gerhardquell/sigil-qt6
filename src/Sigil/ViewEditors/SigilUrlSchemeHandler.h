/************************************************************************
**
**  Copyright (C) 2026 Sigil Contributors
**
**  This file is part of Sigil.
**
**  Sigil is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  Sigil is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Sigil.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#pragma once
#ifndef SIGIL_URL_SCHEME_HANDLER_H
#define SIGIL_URL_SCHEME_HANDLER_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QBuffer>
#include <QWebEngineUrlSchemeHandler>

class QWebEngineUrlRequestJob;
class QMimeDatabase;
class Resource;

/**
 * Custom URL scheme handler for Sigil's WebEngine preview.
 * Handles requests for sigil:// URLs to serve EPUB resources
 * including CSS, images, fonts, audio, and video files.
 *
 * EPUB3 Core Media Types supported:
 * - Images: image/jpeg, image/png, image/gif, image/svg+xml
 * - Audio: audio/mpeg (MP3), audio/mp4 (AAC), audio/ogg
 * - Video: video/mp4 (H.264), video/webm
 * - Fonts: font/woff, font/woff2, application/font-ttf
 * - Documents: text/css, application/javascript
 */
class SigilUrlSchemeHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT

public:
    /**
     * The URL scheme used for Sigil resources.
     * URLs will be formatted as: sigil://book/<resource-path>
     */
    static const QString SIGIL_SCHEME;
    static const QString SIGIL_HOST;

    /**
     * Constructor.
     * @param parent The parent object.
     */
    explicit SigilUrlSchemeHandler(QObject *parent = nullptr);

    /**
     * Destructor.
     */
    ~SigilUrlSchemeHandler();

    /**
     * Register this handler with the QWebEngineProfile.
     * Must be called before any WebEngine views are created.
     */
    static void registerScheme();

    /**
     * Set the resource hash for resolving resource paths.
     * This maps relative paths (e.g., "Styles/stylesheet.css") to Resource objects.
     * @param resources Hash of path -> Resource mapping
     */
    void setResourceHash(const QHash<QString, Resource*> *resources);

    /**
     * Set the base path of the book (the OEBPS folder path).
     * Used for resolving relative paths.
     * @param basePath The absolute path to the OEBPS folder
     */
    void setBasePath(const QString &basePath);

    /**
     * Main request handler called by the WebEngine.
     * @param job The URL request job to handle
     */
    void requestStarted(QWebEngineUrlRequestJob *job) override;

private:
    /**
     * Resolve a URL path to an absolute file path.
     * @param urlPath The path from the URL (e.g., "Text/chapter.xhtml")
     * @return The absolute filesystem path, or empty if not found
     */
    QString resolvePath(const QString &urlPath) const;

    /**
     * Get the MIME type for a file based on its extension.
     * Handles EPUB3 core media types.
     * @param filePath The file path
     * @return The MIME type string
     */
    QString getMimeType(const QString &filePath) const;

    /**
     * Find a resource by its relative path.
     * @param relativePath Path relative to OEBPS (e.g., "Images/cover.jpg")
     * @return The Resource object, or nullptr if not found
     */
    Resource* findResource(const QString &relativePath) const;

    ///////////////////////////////
    // PRIVATE MEMBER VARIABLES
    ///////////////////////////////

    const QHash<QString, Resource*> *m_Resources;
    QString m_BasePath;
    QMimeDatabase *m_MimeDatabase;
};

#endif // SIGIL_URL_SCHEME_HANDLER_H
