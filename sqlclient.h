#ifndef SQLCLIENT_H
#define SQLCLIENT_H
#include "taos.h"
#include <QVariant>
using namespace std;

class sqlClient
{
private:
    QString clientName;
    QString ip;
    int port;
    QString username;
    QString password;
    TAOS *taos = nullptr;

public:
    sqlClient();
    sqlClient(const QString &clientName,
              const QString &ip,
              int port,
              const QString &username,
              const QString &password);
    QString getClientName() const;
    void setClientName(const QString &newClientName);
    QString getIp() const;
    void setIp(const QString &newIp);
    int getPort() const;
    void setPort(int newPort);
    QString getUsername() const;
    void setUsername(const QString &newUsername);
    QString getPassword() const;
    void setPassword(const QString &newPassword);
    TAOS *getTaos() const;
    void setTaos(TAOS *newTaos);
    QVariant toVariant() const;
    static sqlClient fromVariant(const QVariant &variant);
};

#endif // SQLCLIENT_H
