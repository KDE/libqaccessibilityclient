
#include <qcoreapplication.h>

#include <qdebug.h>

#include "kdeaccessibilityclient/registry.h"
#include "kdeaccessibilityclient/accessibleobject.h"

int main(int argc, char**args)
{
    QCoreApplication app(argc, args);

    KAccessibleClient::Registry *registry = KAccessibleClient::Registry::instance();
    QList<KAccessibleClient::AccessibleObject> apps = registry->applications();
    qDebug() << "Accessible applications:" << apps.count();
    foreach(const KAccessibleClient::AccessibleObject &obj, apps) {
        qDebug() << " " << obj.name();
    }

    registry->applications();

    return 0;
}
