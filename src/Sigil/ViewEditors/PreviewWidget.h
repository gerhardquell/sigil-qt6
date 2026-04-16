#pragma once
#ifndef PREVIEW_WIDGET_H
#define PREVIEW_WIDGET_H

#include <QWidget>
#include <QString>
#include <QUrl>
#include <QWebEngineView>

class QTimer;

class PreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewWidget(QWidget *parent = nullptr);
    ~PreviewWidget();

    void setHtml(const QString &html, const QUrl &baseUrl = QUrl());
    void scheduleUpdate(const QString &html, const QUrl &baseUrl = QUrl());
    void scrollToElement(const QString &elementId);
    QWebEngineView *webView() const;

public slots:
    void updatePreview();

signals:
    void linkClicked(const QUrl &url);
    void loadFinished(bool ok);

private slots:
    void onLoadFinished(bool ok);

private:
    void setupUi();
    void connectSignals();

    QWebEngineView *m_webView;
    QTimer *m_updateTimer;
    QString m_pendingHtml;
    QUrl m_pendingBaseUrl;
    bool m_isLoading;
};

#endif
