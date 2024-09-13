#include "sqlclient.h"

QString sqlClient::getClientName() const
{
    return clientName;
}

void sqlClient::setClientName(const QString &newClientName)
{
    clientName = newClientName;
}

QString sqlClient::getIp() const
{
    return ip;
}

void sqlClient::setIp(const QString &newIp)
{
    ip = newIp;
}

int sqlClient::getPort() const
{
    return port;
}

void sqlClient::setPort(int newPort)
{
    port = newPort;
}

QString sqlClient::getUsername() const
{
    return username;
}

void sqlClient::setUsername(const QString &newUsername)
{
    username = newUsername;
}

QString sqlClient::getPassword() const
{
    return password;
}

void sqlClient::setPassword(const QString &newPassword)
{
    password = newPassword;
}

TAOS *sqlClient::getTaos() const
{
    return taos;
}

void sqlClient::setTaos(TAOS *newTaos)
{
    taos = newTaos;
}

QVariant sqlClient::toVariant() const
{
    QVariantMap map;
    map["clientName"] = clientName;
    map["ip"] = ip;
    map["port"] = port;
    map["username"] = username;
    map["password"] = password;
    return QVariant::fromValue(map);
}

sqlClient sqlClient::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return sqlClient(map["clientName"].toString(), map["ip"].toString(),
                     map["port"].toInt(), map["username"].toString(), map["password"].toString());
}

sqlClient::sqlClient() {}

sqlClient::sqlClient(const QString &clientName,
                     const QString &ip,
                     int port,
                     const QString &username,
                     const QString &password)
    : clientName(clientName)
    , ip(ip)
    , port(port)
    , username(username)
    , password(password)
{}
