#ifndef MQTTCONNECTOR_H
#define MQTTCONNECTOR_H

#include <QMqttClient>
#include <QObject>

struct ReceivedValue {
    enum class Vaiable { A, B, MA } variable;
    int value;
};

class MqttConnector : public QMqttClient
{
    Q_OBJECT
public:
    MqttConnector(QObject* partent, QString host, QString user, QString password);

signals:
    void valueReceived(const ReceivedValue& value);
    void mqttConnected(const bool connected);

private slots:
    void updateLogStateChange();
    void messageReceived(const QByteArray &message, const QMqttTopicName &topic);
};

#endif // MQTTCONNECTOR_H
