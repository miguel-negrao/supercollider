/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "dialog.hpp"
#include "ui_settings_dialog.h"
#include "general_page.hpp"
#include "sclang_page.hpp"
#include "sclang_project_page.hpp"
#include "editor_page.hpp"
#include "shortcuts_page.hpp"
#include "../../core/session_manager.hpp"
#include "../../core/settings/manager.hpp"
#include "../../core/main.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QStackedWidget>
#include <QListWidget>
#include <QFile>
#include <QLineEdit>
#include <QPushButton>

namespace ScIDE { namespace Settings {

Dialog::Dialog( Manager *settings, Session* session, QWidget * parent ):
    QDialog(parent),
    mManager(settings),
    mSession(session),
    ui( new Ui::ConfigDialog )
{
    ui->setupUi(this);

    QWidget *w;

    generalPage = new GeneralPage;
    ui->configPageStack->addWidget(generalPage);
    ui->configPageList->addItem (
        new QListWidgetItem(QIcon::fromTheme("preferences-system"), tr("General")));
    connect(this, SIGNAL(storeRequest(Manager*, Session*, bool)), generalPage, SLOT(store(Manager*, Session*, bool)));
    connect(this, SIGNAL(loadRequest(Manager*, Session*)), generalPage, SLOT(load(Manager*, Session*)));
    connect(generalPage, SIGNAL(useLanguageConfigFromSessionChanged(bool)), this, SLOT(reAddSclangPage(bool)));

    bool isProject;
    if( settings->value("IDE/useLanguageConfigFromSession").toBool() && (mSession != nullptr) )
        isProject = mSession->value("IDE/interpreter/project").toBool();
    else
        isProject = mManager->value("IDE/interpreter/project").toBool();

    if( isProject )
    { w = new SclangProjectPage; } else
    { w = new SclangPage; }
    sclangPage = w;
    ui->configPageStack->addWidget(w);
    ui->configPageList->addItem (
        new QListWidgetItem(QIcon::fromTheme("applications-system"), tr("Interpreter")));
    connect(this, SIGNAL(storeRequest(Manager*, Session*, bool)), w, SLOT(store(Manager*, Session*, bool)));
    connect(this, SIGNAL(loadRequest(Manager*, Session*)), w, SLOT(load(Manager*, Session*)));

    w = new EditorPage;
    ui->configPageStack->addWidget(w);
    ui->configPageList->addItem (
        new QListWidgetItem(QIcon::fromTheme("accessories-text-editor"), tr("Editor")));
    connect(this, SIGNAL(storeRequest(Manager*, Session*, bool)), w, SLOT(store(Manager*, Session*, bool)));
    connect(this, SIGNAL(loadRequest(Manager*, Session*)), w, SLOT(load(Manager*, Session*)));

    w = new ShortcutsPage;
    ui->configPageStack->addWidget(w);
    ui->configPageList->addItem (
        new QListWidgetItem(QIcon::fromTheme("input-keyboard"), tr("Shortcuts")));
    connect(this, SIGNAL(storeRequest(Manager*, Session*, bool)), w, SLOT(store(Manager*, Session*, bool)));
    connect(this, SIGNAL(loadRequest(Manager*, Session*)), w, SLOT(load(Manager*, Session*)));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));


    ui->configPageList->setMinimumWidth( ui->configPageList->sizeHintForColumn(0) );

    reset();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::accept()
{
    Q_EMIT( storeRequest(mManager, mSession, generalPage->useLanguageConfigFromSessionChecked()) );

    QDialog::accept();
}

void Dialog::reject()
{
    QDialog::reject();
}

void Dialog::apply()
{
    Q_EMIT( storeRequest(mManager, mSession, generalPage->useLanguageConfigFromSessionChecked()) );
    Main::instance()->applySettings();
}

void Dialog::reset()
{
    Q_EMIT( loadRequest(mManager, mSession) );
}

void Dialog::reAddSclangPage(bool useLanguageConfigFromSession)
{
    ui->configPageStack->removeWidget(sclangPage);
    delete sclangPage;
    bool isProject;
    if( useLanguageConfigFromSession )
        isProject = mSession->value("IDE/interpreter/project").toBool();
    else
        isProject = mManager->value("IDE/interpreter/project").toBool();
    if( isProject )
    {
        SclangProjectPage *w = new SclangProjectPage;
        w->loadTemp(mManager, mSession, useLanguageConfigFromSession);
        sclangPage = w;
    } else {
        SclangPage *w = new SclangPage;
        w->loadTemp(mManager, mSession, useLanguageConfigFromSession);
        sclangPage = w;
    }
    ui->configPageStack->insertWidget(1, sclangPage);
    connect(this, SIGNAL(storeRequest(Manager*, Session*, bool)), sclangPage, SLOT(store(Manager*, Session*, bool)));
    connect(this, SIGNAL(loadRequest(Manager*, Session*)), sclangPage, SLOT(load(Manager*, Session*)));
}


}} // namespace ScIDE::Settings
