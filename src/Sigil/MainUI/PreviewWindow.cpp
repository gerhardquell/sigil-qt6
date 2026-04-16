/************************************************************************
**
**  Copyright (C) 2012 Dave Heiland, John Schember
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

#include <QSplitter>
#include <QStackedWidget>
#include <QWebEngineView>
#include <QVBoxLayout>

#include "MainUI/PreviewWindow.h"
#include "Misc/SleepFunctions.h"
#include "ResourceObjects/HTMLResource.h"

static const QString SETTINGS_GROUP = "previewwindow";

PreviewWindow::PreviewWindow(QWidget *parent)
    :
    QDockWidget(tr("Preview"), parent),
    m_MainWidget(*new QWidget(this)),
    m_Layout(*new QVBoxLayout(&m_MainWidget)),
    m_Preview(new QWebEngineView(this)),
    m_Splitter(new QSplitter(this)),
    m_StackedViews(new QStackedWidget(this))
{
    SetupView();
    LoadSettings();
    ConnectSignalsToSlots();
}

PreviewWindow::~PreviewWindow()
{
    if (m_Preview) {
        delete m_Preview;
        m_Preview = nullptr;
    }
}

void PreviewWindow::SetupView()
{
    m_Layout.setContentsMargins(0, 0, 0, 0);
    m_Layout.addWidget(m_Splitter);
    m_Splitter->addWidget(m_Preview);
    setWidget(&m_MainWidget);
}

void PreviewWindow::LoadSettings()
{
    // Settings loading stub
}

void PreviewWindow::ConnectSignalsToSlots()
{
    // Signals connection stub
}

QList<ViewEditor::ElementIndex> PreviewWindow::GetCaretLocation()
{
    // Stub implementation
    return QList<ViewEditor::ElementIndex>();
}

bool PreviewWindow::IsVisible()
{
    return isVisible();
}

bool PreviewWindow::HasFocus()
{
    return m_Preview->hasFocus();
}

float PreviewWindow::GetZoomFactor()
{
    // QWebEngineView doesn't have simple zoom factor getter
    // This would need to be implemented with JavaScript or settings
    return 1.0f;
}

void PreviewWindow::UpdatePage(QString filename, QString text, QList< ViewEditor::ElementIndex > location)
{
    Q_UNUSED(filename)
    Q_UNUSED(location)

    if (!text.isEmpty()) {
        m_Preview->setHtml(text);
    }
}

void PreviewWindow::SetZoomFactor(float factor)
{
    m_Preview->setZoomFactor(factor);
}

void PreviewWindow::SplitterMoved(int pos, int index)
{
    Q_UNUSED(pos)
    Q_UNUSED(index)
}

void PreviewWindow::showEvent(QShowEvent *event)
{
    QDockWidget::showEvent(event);
    emit Shown();
}
