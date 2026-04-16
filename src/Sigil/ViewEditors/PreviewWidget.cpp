#include "PreviewWidget.h"

#include <QVBoxLayout>
#include <QTimer>
#include <QWebEnginePage>
#include <QWebEngineSettings>

PreviewWidget::PreviewWidget(QWidget *parent)
    : QWidget(parent),
      m_webView(nullptr),
      m_updateTimer(nullptr),
      m_isLoading(false)
{
    setupUi();
    connectSignals();
}

PreviewWidget::~PreviewWidget()
{
}

void PreviewWidget::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_webView = new QWebEngineView(this);
    m_webView->setContextMenuPolicy(Qt::NoContextMenu);

    // Configure settings for read-only preview
    QWebEngineSettings *settings = m_webView->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, false);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);

    layout->addWidget(m_webView);
    setLayout(layout);

    // Setup debounce timer (400ms single shot)
    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(400);
}

void PreviewWidget::connectSignals()
{
    connect(m_updateTimer, &QTimer::timeout, this, &PreviewWidget::updatePreview);
    connect(m_webView, &QWebEngineView::loadFinished, this, &PreviewWidget::onLoadFinished);

    // Handle link clicks
    connect(m_webView->page(), &QWebEnginePage::linkHovered, this, [this](const QString &url) {
        if (!url.isEmpty()) {
            emit linkClicked(QUrl(url));
        }
    });
}

void PreviewWidget::setHtml(const QString &html, const QUrl &baseUrl)
{
    m_pendingHtml = html;
    m_pendingBaseUrl = baseUrl;
    updatePreview();
}

void PreviewWidget::scheduleUpdate(const QString &html, const QUrl &baseUrl)
{
    m_pendingHtml = html;
    m_pendingBaseUrl = baseUrl;
    m_updateTimer->start();
}

void PreviewWidget::updatePreview()
{
    if (m_isLoading) {
        // If already loading, schedule another update
        m_updateTimer->start();
        return;
    }

    m_isLoading = true;
    m_webView->setHtml(m_pendingHtml, m_pendingBaseUrl);
}

void PreviewWidget::scrollToElement(const QString &elementId)
{
    if (elementId.isEmpty()) {
        return;
    }

    QString js = QString(
        "var element = document.getElementById('%1');"
        "if (element) {"
        "    element.scrollIntoView({behavior: 'smooth', block: 'center'});"
        "}"
    ).arg(elementId);

    m_webView->page()->runJavaScript(js);
}

QWebEngineView *PreviewWidget::webView() const
{
    return m_webView;
}

void PreviewWidget::onLoadFinished(bool ok)
{
    m_isLoading = false;
    emit loadFinished(ok);
}
