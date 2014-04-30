/*
    Copyright 2012 Frederik Gladhorn <gladhorn@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QACCESSIBILITYCLIENT_REGISTRY_P_H
#define QACCESSIBILITYCLIENT_REGISTRY_P_H

#include <atspi/atspi-constants.h>

#include <qobject.h>
#include <qmap.h>
#include <qdbuscontext.h>
#include <qdbusargument.h>
#include <qsignalmapper.h>
#include <qsharedpointer.h>

#include "atspi/dbusconnection.h"
#include "qaccessibilityclient/registry.h"
#include "qaccessibilityclient/accessibleobject.h"
#include "qaccessibilityclient/accessibleobject_p.h"
#include "atspi/qt-atspi.h"
#include "cachestrategy_p.h"

class QDBusPendingCallWatcher;

namespace QAccessibleClient {

class DBusConnection;
class AtSpiDBus;

class RegistryPrivate :public QObject, public QDBusContext
{
    Q_OBJECT
public:
    RegistryPrivate(Registry *qq);

    void init();

    bool isEnabled() const;
    void setEnabled(bool enable);
    bool isScreenReaderEnabled() const;
    void setScreenReaderEnabled(bool enable);

    QUrl url(const AccessibleObject &object) const;
    AccessibleObject fromUrl(const QUrl &url) const;

    void subscribeEventListeners(const Registry::EventListeners & listeners);
    Registry::EventListeners eventListeners() const;

    QString name(const AccessibleObject &object) const;
    QString description(const AccessibleObject &object) const;
    AccessibleObject::Role role(const AccessibleObject &object) const;
    QString roleName(const AccessibleObject &object) const;
    QString localizedRoleName(const AccessibleObject &object) const;
    quint64 state(const AccessibleObject &object) const;
    int layer(const AccessibleObject &object) const;
    int mdiZOrder(const AccessibleObject &object) const;
    double alpha(const AccessibleObject &object) const;
    QRect boundingRect(const AccessibleObject &object) const;
    QRect characterRect(const AccessibleObject &object, int offset) const;
    AccessibleObject::Interfaces supportedInterfaces(const AccessibleObject &object) const;

    int caretOffset(const AccessibleObject &object) const;
    int characterCount(const AccessibleObject &object) const;
    QList< QPair<int,int> > textSelections(const AccessibleObject &object) const;
    void setTextSelections(const AccessibleObject &object, const QList< QPair<int,int> > &selections);
    QString text(const AccessibleObject &object, int startOffset = 0, int endOffset = -1) const;
    QString textWithBoundary(const AccessibleObject &object, int offset, AccessibleObject::TextBoundary boundary, int *startOffset, int *endOffset) const;

    bool setText(const AccessibleObject &object, const QString &text);
    bool insertText(const AccessibleObject &object, const QString &text, int position, int length = -1);
    bool copyText(const AccessibleObject &object, int startPos, int endPos);
    bool cutText(const AccessibleObject &object, int startPos, int endPos);
    bool deleteText(const AccessibleObject &object, int startPos, int endPos);
    bool pasteText(const AccessibleObject &object, int position);

    AccessibleObject application(const AccessibleObject &object) const;
    QString appToolkitName(const AccessibleObject &object) const;
    QString appVersion(const AccessibleObject &object) const;
    int appId(const AccessibleObject &object) const;
    QString appLocale(const AccessibleObject &object, uint lctype) const;
    QString appBusAddress(const AccessibleObject &object) const;

    double minimumValue(const AccessibleObject &object) const;
    double maximumValue(const AccessibleObject &object) const;
    double minimumValueIncrement(const AccessibleObject &object) const;
    double currentValue(const AccessibleObject &object) const;

    QList<AccessibleObject> selection(const AccessibleObject &object) const;

    QString imageDescription(const AccessibleObject &object) const;
    QString imageLocale(const AccessibleObject &object) const;
    QRect imageRect(const AccessibleObject &object) const;

    QVector< QSharedPointer<QAction> > actions(const AccessibleObject &object);

    QList<AccessibleObject> topLevelAccessibles() const;
    AccessibleObject parentAccessible(const AccessibleObject &object) const;

    int childCount(const AccessibleObject &object) const;
    int indexInParent(const AccessibleObject &object) const;
    AccessibleObject child(const AccessibleObject &object, int index) const;
    QList<AccessibleObject> children(const AccessibleObject &object) const;

    static QString ACCESSIBLE_OBJECT_SCHEME_STRING;

private Q_SLOTS:
    AccessibleObject accessibleFromPath(const QString &service, const QString &path) const;
    AccessibleObject accessibleFromReference(const QSpiObjectReference &reference) const;
    AccessibleObject accessibleFromContext() const;

    void connectionFetched();
    void slotSubscribeEventListenerFinished(QDBusPendingCallWatcher *call);
    void a11yConnectionChanged(const QString &interface,const QVariantMap &changedProperties, const QStringList &invalidatedProperties);

    void slotPropertyChange(const QString &property, int detail1, int detail2, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowCreate(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowDestroy(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowClose(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowReparent(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowMinimize(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowMaximize(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowRestore(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowActivate(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowDeactivate(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowDesktopCreate(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowDesktopDestroy(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowRaise(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowLower(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowMove(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowResize(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowShade(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotWindowUnshade(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);

    void slotStateChanged(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    //void slotPropertyChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference);
    //void slotBoundsChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference);
    //void slotLinkSelected(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference);

    void slotChildrenChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference);
    void slotVisibleDataChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference);
    void slotSelectionChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference);
    void slotModelChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference);

    void slotTextCaretMoved(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);
    void slotTextSelectionChanged(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference);

    //void slotTextBoundsChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference);
    void slotTextChanged(const QString &state, int start, int end, const QDBusVariant &text, const QAccessibleClient::QSpiObjectReference &reference);
    //void slotTextAttributesChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference);
    //void slotAttributesChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference);

    void actionTriggered(const QString &action);

private:
    QVariant getProperty ( const QString &service, const QString &path, const QString &interface, const QString &name ) const;
    bool subscribeEvent(const QLatin1String &iface, const QLatin1String &signal);
    static AccessibleObject::Role atspiRoleToRole(AtspiRole role);

    DBusConnection conn;
    QSignalMapper m_actionMapper;
    Registry *q;
    Registry::EventListeners m_subscriptions;
    Registry::EventListeners m_pendingSubscriptions;
    QHash<QString, AccessibleObject::Interface> interfaceHash;
    QSignalMapper m_eventMapper;
    ObjectCache *m_cache;
//     typedef QMap<QString, QSharedPointer<AccessibleObjectPrivate> >::Iterator AccessibleObjectsHashIterator;
//     typedef QMap<QString, QSharedPointer<AccessibleObjectPrivate> >::ConstIterator AccessibleObjectsHashConstIterator;
//     QMap<QString, QSharedPointer<AccessibleObjectPrivate> > accessibleObjectsHash;
    bool removeAccessibleObject(const QAccessibleClient::AccessibleObject &accessible);
    bool removeAccessibleObject(const QAccessibleClient::QSpiObjectReference &reference);

    friend class Registry;
    friend class AccessibleObject;
    friend class AccessibleObjectPrivate;
};

}

#endif
