
#include <qcoreapplication.h>

#include <qdebug.h>

#include "kdeaccessibilityclient/registry.h"
#include "kdeaccessibilityclient/accessibleobject.h"

int main(int argc, char**args)
{
    QCoreApplication app(argc, args);

    KAccessibleClient::Registry registry;
    QList<KAccessibleClient::AccessibleObject> apps = registry.applications();
    qDebug() << "Accessible applications:" << apps.count();
    foreach(const KAccessibleClient::AccessibleObject &obj, apps) {
        qDebug() << "App:" << obj.name() << " (parent: " << obj.parent().name() << ")";
        foreach(const KAccessibleClient::AccessibleObject &child, obj.children()) {
            qDebug() << " Window:" << child.name() << " (parent: " << child.parent().name() << ")";
        }
    }

    return 0;
}
