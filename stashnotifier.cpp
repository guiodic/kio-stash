/***************************************************************************
*   Copyright (C) 2016 by Arnav Dhamija <arnav.dhamija@gmail.com>         *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
***************************************************************************/

#include "stashnotifier.h"
#include "stash_adaptor.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMetaType>
#include <QDBusMetaType>

#include <KDirWatch>
#include <KPluginFactory>
#include <KPluginLoader>
#include <kdirnotify.h>

K_PLUGIN_FACTORY_WITH_JSON(StashNotifierFactory, "stashnotifier.json", registerPlugin<StashNotifier>();)

StashNotifier::StashNotifier(QObject *parent, const QList<QVariant> &var) : KDEDModule(parent)
{
    dirWatch = new KDirWatch(this);
    qDebug() << "Launching STASH NOTIFIER DAEMON";

    new StashNotifierAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/StashNotifier", this);
    dbus.registerService("org.kde.kio.StashNotifier");

    fileSystem = new StashFileSystem(parent);

    connect(dirWatch, &KDirWatch::dirty, this, &StashNotifier::dirty);
    connect(dirWatch, &KDirWatch::created, this, &StashNotifier::created);
    connect(dirWatch, &KDirWatch::deleted, this, &StashNotifier::removePath);
}

StashNotifier::~StashNotifier()
{
}

QStringList StashNotifier::fileList(const QString &path) //forwards list over QDBus to the KIO slave
{
    QStringList contents;
    return contents;
}

void StashNotifier::addPath(const QString &source, const QString &stashPath, const int &fileType)
{
    QString processedPath = processString(stashPath);
    if (fileSystem->findNode(stashPath).type == StashFileSystem::NodeType::InvalidNode) { //only folders not already exisiting are added
        if (fileType == StashFileSystem::NodeType::DirectoryNode) {
            dirWatch->addDir(processedPath);
            fileSystem->addFolder(processedPath);
        } else if (fileType == StashFileSystem::NodeType::FileNode) {
            dirWatch->addFile(processedPath);
            fileSystem->addFile(processString(source), stashPath);
        } else if (fileType == StashFileSystem::NodeType::SymlinkNode) {
            dirWatch->addFile(processedPath);
            fileSystem->addSymlink(processString(source), stashPath);
        }
        emit listChanged();
    }
}

QString StashNotifier::processString(const QString &path) //removes trailing slash and strips newline character
{
    QString processedPath = path.simplified();
    if (processedPath.at(processedPath.size() - 1) == '/') {
        processedPath.chop(1);
    }
    return processedPath;
}

void StashNotifier::removePath(const QString &path)
{
    QString processedPath = processString(path);
    if (QFileInfo(processedPath).isDir()) { //would this even work?
        dirWatch->removeDir(processedPath);
    } else if (QFileInfo(processedPath).isFile()) { //change file logic, remove folder logic
        dirWatch->removeFile(processedPath);
    }
    fileSystem->delEntry(path);
    emit listChanged();
}

void StashNotifier::dirty(const QString &path)
{
    //what is supposed to happen here?
    qDebug() << "SOMETHING HAS CHANGED:" << path;
}

void StashNotifier::created(const QString &path)
{
    qDebug() << "CREATED:" << path;
}

#include "stashnotifier.moc"
