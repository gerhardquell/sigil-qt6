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

#include "SigilUrlSchemeHandler.h"

#include <QFile>
#include <QFileInfo>
#include <QBuffer>
#include <QMimeDatabase>
#include <QMimeType>
#include <QWebEngineProfile>
#include <QWebEngineUrlScheme>
#include <QWebEngineUrlRequestJob>
#include <cstdio>

#include "ResourceObjects/Resource.h"
#include "Misc/Utility.h"

// Debug prefix for stderr logging - grep with: 2>&1 | grep SIGIL-PREVIEW
#define DBG_PREFIX "[SIGIL-PREVIEW] "

const QString SigilUrlSchemeHandler::SIGIL_SCHEME = "sigil";
const QString SigilUrlSchemeHandler::SIGIL_HOST = "book";

// EPUB3 Core Media Types - mapped from file extensions
static const QHash<QString, QString> EPUB3_MIME_TYPES = {
    // Images
    { "jpg", "image/jpeg" },
    { "jpeg", "image/jpeg" },
    { "png", "image/png" },
    { "gif", "image/gif" },
    { "svg", "image/svg+xml" },

    // Audio (EPUB3)
    { "mp3", "audio/mpeg" },
    { "mp4", "audio/mp4" },
    { "m4a", "audio/mp4" },
    { "aac", "audio/mp4" },
    { "ogg", "audio/ogg" },
    { "oga", "audio/ogg" },
    { "opus", "audio/ogg" },
    { "wav", "audio/wav" },
    { "wave", "audio/wav" },
    { "webm", "audio/webm" },

    // Video (EPUB3)
    { "mp4v", "video/mp4" },
    { "m4v", "video/mp4" },
    { "ogv", "video/ogg" },
    { "webmv", "video/webm" },

    // Fonts (EPUB3)
    { "woff", "font/woff" },
    { "woff2", "font/woff2" },
    { "ttf", "font/ttf" },
    { "otf", "font/otf" },

    // Documents
    { "css", "text/css" },
    { "js", "application/javascript" },
    { "xhtml", "application/xhtml+xml" },
    { "html", "text/html" },
    { "htm", "text/html" },
    { "ncx", "application/x-dtbncx+xml" },
    { "opf", "application/oebps-package+xml" },

    // Other
    { "xml", "application/xml" },
    { "smil", "application/smil+xml" },
    { "pls", "application/pls+xml" },
};

SigilUrlSchemeHandler::SigilUrlSchemeHandler(QObject *parent)
    : QWebEngineUrlSchemeHandler(parent),
      m_Resources(nullptr),
      m_MimeDatabase(new QMimeDatabase())
{
}

SigilUrlSchemeHandler::~SigilUrlSchemeHandler()
{
    delete m_MimeDatabase;
}

void SigilUrlSchemeHandler::registerScheme()
{
    QWebEngineUrlScheme scheme(SIGIL_SCHEME.toUtf8());
    scheme.setSyntax(QWebEngineUrlScheme::Syntax::HostAndPort);
    scheme.setDefaultPort(0);
    scheme.setFlags(QWebEngineUrlScheme::SecureScheme |
                    QWebEngineUrlScheme::LocalScheme |
                    QWebEngineUrlScheme::LocalAccessAllowed);
    QWebEngineUrlScheme::registerScheme(scheme);
}

void SigilUrlSchemeHandler::setResourceHash(const QHash<QString, Resource*> *resources)
{
    m_Resources = resources;
}

void SigilUrlSchemeHandler::setBasePath(const QString &basePath)
{
    m_BasePath = basePath;
}

void SigilUrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    QUrl url = job->requestUrl();

    // Verify scheme
    if (url.scheme() != SIGIL_SCHEME) {
        job->fail(QWebEngineUrlRequestJob::RequestFailed);
        return;
    }

    // Extract path from URL
    QString path = url.path();
    if (path.isEmpty()) {
        fprintf(stderr, DBG_PREFIX "schemeHandler: empty path for url='%s'\n", url.toString().toUtf8().constData());
        job->fail(QWebEngineUrlRequestJob::UrlNotFound);
        return;
    }

    // Remove leading slash
    if (path.startsWith("/")) {
        path = path.mid(1);
    }

    // Resolve to absolute path
    QString absolutePath = resolvePath(path);
    if (absolutePath.isEmpty()) {
        fprintf(stderr, DBG_PREFIX "schemeHandler: NOT FOUND path='%s'\n", path.toUtf8().constData());
        job->fail(QWebEngineUrlRequestJob::UrlNotFound);
        return;
    }

    // Read file content
    QFile file(absolutePath);
    if (!file.open(QIODevice::ReadOnly)) {
        fprintf(stderr, DBG_PREFIX "schemeHandler: CANNOT READ '%s'\n", absolutePath.toUtf8().constData());
        job->fail(QWebEngineUrlRequestJob::UrlNotFound);
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    // Determine MIME type
    QString mimeType = getMimeType(absolutePath);

    fprintf(stderr, DBG_PREFIX "schemeHandler: OK '%s' → %s (%d bytes)\n",
            path.toUtf8().constData(), mimeType.toUtf8().constData(), data.size());

    // Create reply
    QBuffer *buffer = new QBuffer(job);
    buffer->setData(data);
    job->reply(mimeType.toUtf8(), buffer);
}

QString SigilUrlSchemeHandler::resolvePath(const QString &urlPath) const
{
    // First, try to find the resource in the hash
    Resource *resource = findResource(urlPath);
    if (resource) {
        return resource->GetFullPath();
    }

    // Fallback: construct absolute path from base path
    if (!m_BasePath.isEmpty()) {
        QString absolutePath = m_BasePath + "/" + urlPath;
        if (QFile::exists(absolutePath)) {
            return absolutePath;
        }
    }

    // Try direct file path
    if (QFile::exists(urlPath)) {
        return urlPath;
    }

    return QString();
}

QString SigilUrlSchemeHandler::getMimeType(const QString &filePath) const
{
    QString ext = QFileInfo(filePath).suffix().toLower();

    // Check our EPUB3 mapping first
    if (EPUB3_MIME_TYPES.contains(ext)) {
        return EPUB3_MIME_TYPES.value(ext);
    }

    // Fallback to system MIME database
    QMimeType mime = m_MimeDatabase->mimeTypeForFile(filePath);
    if (mime.isValid()) {
        return mime.name();
    }

    // Default fallback
    return "application/octet-stream";
}

Resource* SigilUrlSchemeHandler::findResource(const QString &relativePath) const
{
    if (!m_Resources) {
        return nullptr;
    }

    // Try direct match
    if (m_Resources->contains(relativePath)) {
        return m_Resources->value(relativePath);
    }

    // Try with different path separators
    QString normalizedPath = relativePath;
    normalizedPath.replace("\\", "/");

    if (m_Resources->contains(normalizedPath)) {
        return m_Resources->value(normalizedPath);
    }

    // Search by filename (last resort - handles paths like "../Images/cover.jpg")
    QString filename = QFileInfo(normalizedPath).fileName();

    Q_FOREACH(Resource *resource, m_Resources->values()) {
        if (resource->Filename() == filename) {
            return resource;
        }
    }

    return nullptr;
}
