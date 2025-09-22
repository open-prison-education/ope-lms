#ifndef OPENETWORKACCESSMANAGERFACTORY_H
#define OPENETWORKACCESSMANAGERFACTORY_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QQmlNetworkAccessManagerFactory>
// Deprecated in QT6
//#include <QNetworkConfiguration>
#include <QNetworkReply>
#include <QSslError>

class OPENetworkAccessManager: public QNetworkAccessManager
{
    Q_OBJECT
public:
    OPENetworkAccessManager(QObject *parent);
    virtual QNetworkReply *get(const QNetworkRequest &request);
    virtual QNetworkReply *createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);

    // If no bytes sent by this time, timeout
    int transfer_timeout = 60;
protected:

public slots:
    void ignoreSSLErrors(QNetworkReply *reply, QList<QSslError> errors);

};



/**
 * @brief The OPENetworkAccessManagerFactory class
 * A class to deal with network manager settings within QML
 *
 */
class OPENetworkAccessManagerFactory : public QObject, public QQmlNetworkAccessManagerFactory
{
    Q_OBJECT
public:
    virtual QNetworkAccessManager *create(QObject *parent);
};

#endif // OPENETWORKACCESSMANAGERFACTORY_H
