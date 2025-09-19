#include "openetworkaccessmanagerfactory.h"


QNetworkAccessManager *OPENetworkAccessManagerFactory::create(QObject *parent)
{
    //qDebug() << "------> OPENAM Factory Create Called";
    return new OPENetworkAccessManager(parent); // QNetworkAccessManager(parent);
}

OPENetworkAccessManager::OPENetworkAccessManager(QObject* /*parent*/)
{
    //qDebug() << "----> NEW OPENAM";
    //QObject::connect(this, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
    //                 this, SLOT(ignoreSSLErrors(QNetworkReply*,QList<QSslError>)));

    // Bump timeout up so we don't loose connection during uploads
    //configuration().setConnectTimeout(30);
    setTransferTimeout(this->transferTimeout());
    //TODO - Set keepalive properties? uploads are timing out

    // Deprecated in qt6
    //qDebug() << "====> Network Accessible " << networkAccessible();
    // Set access to enabled - force it as it detects incorrectly at times
    // on an offline network
    //setNetworkAccessible(QNetworkAccessManager::Accessible);

}

QNetworkReply *OPENetworkAccessManager::get(const QNetworkRequest &request)
{
    qDebug() << "OPENetworkAccessManger::get";
    return QNetworkAccessManager::get(request);
}

QNetworkReply *OPENetworkAccessManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    qDebug() << "OPENetworkAccessManager::createRequest";
    return QNetworkAccessManager::createRequest(op, request, outgoingData);
}

void OPENetworkAccessManager::ignoreSSLErrors(QNetworkReply *reply, QList<QSslError> errors)
{
    qDebug() << "------> OPENAM - Ignoring SSL Errors";
    reply->ignoreSslErrors(errors);
}
