
#include <qcoreapplication.h>

#include <qdebug.h>

#include "libkdeaccessibilityclient/registry.h"
#include "libkdeaccessibilityclient/accessibleobject.h"

int main(int argc, char**args)
{
    QCoreApplication app(argc, args);

    KAccessibleClient::Registry *registry = KAccessibleClient::Registry::instance();
    qDebug() << "Accessible applications:";
    qDebug() << registry->applications().count();

    return app.exec();
}
