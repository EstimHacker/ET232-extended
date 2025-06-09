#include "mqttconnector.h"
#include "QMessageBox"

#include <QJsonDocument>
#include <QJsonObject>

MqttConnector::MqttConnector(QObject* parent, QString host, QString user, QString password) : QMqttClient(parent)
{
    setUsername(user);
    setPassword(password);
    setPort(1883);
    setHostname(host);
    connectToHost();

    connect(this, &QMqttClient::stateChanged, this, &MqttConnector::updateLogStateChange);
    connect(this, &QMqttClient::messageReceived, this, &MqttConnector::messageReceived);
}

void MqttConnector::messageReceived(const QByteArray &message, const QMqttTopicName &topic [[maybe_unused]])
{
    QJsonDocument doc = QJsonDocument::fromJson(message);

    // Check if the document is valid
    if (doc.isObject()) {
        auto jsonObj = doc.object();

        // Unpack the values
        auto idVal = jsonObj.value("id");
        auto channelVal = jsonObj.value("channel");
        auto intensityVal = jsonObj.value("vibrate");
        if (idVal != QJsonValue::Undefined && intensityVal != QJsonValue::Undefined) {
            auto id = idVal.toString();
            auto channel = (channelVal != QJsonValue::Undefined) ? channelVal.toInt() : 0;
            auto intensity = intensityVal.toInt();

            if (id.endsWith("intensity") && channel >= 1 && channel <= 2 && intensity >= 0 && intensity <= 100) {
                emit valueReceived({ (channel==1) ? ReceivedValue::Vaiable::A : ReceivedValue::Vaiable::B, intensity});
                return;
            }
            if (id.endsWith("parameter") && intensity >= 0 && intensity <= 100) {
                emit valueReceived({ ReceivedValue::Vaiable::MA, intensity});
                return;
            }
        }
    }}


void MqttConnector::updateLogStateChange()
{
    switch (state()) {
    case ClientState::Connected:
        subscribe(QMqttTopicFilter("et232/#"));
        emit mqttConnected(true);
        break;

    case ClientState::Disconnected:
        emit mqttConnected(false);
        break;

    default:
        break;
    }
}
