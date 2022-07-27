#include "EscapeLogic.hpp"


Callback EscapeLogicClient::standbyLoop;
Callback EscapeLogicClient::puzzleStart;
Callback EscapeLogicClient::puzzleLoop;
Callback EscapeLogicClient::puzzleSolved;
Callback EscapeLogicClient::solvedLoop;
Callback EscapeLogicClient::puzzleReset;
Callback EscapeLogicClient::solveOnNextTry;
EthernetClient EscapeLogicClient::ethClient;
MQTTClient EscapeLogicClient::client;
String EscapeLogicClient::deviceId;
String EscapeLogicClient::topic;
EscapeLogicClient::State EscapeLogicClient::state;

void (*EscapeLogicClient::cb)(String action, String payload);

void EscapeLogicClient::setOnReceived(void (*cb)(String action, String payload))
{
    EscapeLogicClient::cb = cb;
}

void EscapeLogicClient::start(byte mac[], IPAddress server, String deviceId)
{
    EscapeLogicClient::deviceId = deviceId;

    startEthernet(mac);

    topic = deviceId + "/#";

    // client.setServer(server, 1883);
    // client.setCallback(clientReceivedCallback);

    client.begin(server, ethClient);
    
    client.onMessage(EscapeLogicClient::clientReceivedCallback);

    standbyLoop = noop;
    puzzleStart = noop;
    puzzleLoop = noop;
    puzzleSolved = noop;
    solvedLoop = noop;
    puzzleReset = noop;
    solveOnNextTry = noop;

    EscapeLogicClient::cb = [](String action, String payload){};

    state = State::Standby;
}


void EscapeLogicClient::loop()
{

    client.loop();

    if (!client.connected())
    {
        connectMQTT();
    }

    switch (state)
    {
    case Standby:
        standbyLoop();
        break;
    case Started:
        puzzleLoop();
        break;
    case Solved:
        solvedLoop();
        break;
    }
}

void EscapeLogicClient::clientReceivedCallback(String &receivedTopic, String &payload)
{
    Serial.println("incoming: ");
    Serial.print(receivedTopic.c_str());
    Serial.println(payload.c_str());

    // Note: Do not use the client in the callback to publish, subscribe or
    // unsubscribe as it may cause deadlocks when other things arrive while
    // sending and receiving acknowledgments. Instead, change a global variable,
    // or push to a queue and handle it in the loop after calling `client.loop()`.
    Serial.print("Message arrived [");

    int index = receivedTopic.indexOf("/");
    if(index == -1) { 
        return;
    }
    String start = receivedTopic.substring(0, index);
    String end = receivedTopic.substring(index + 1);
    Serial.print("start: "); Serial.println(start);
    Serial.print("end: "); Serial.println(end);
    if(start.equals(deviceId) ) {
        EscapeLogicClient::cb(end, payload);

        if (end.equals("activate"))
        {
            puzzleStart();
            state = State::Started;
        }

        if (end.equals("skip"))
        {
            puzzleSolved();
            state = State::Solved;
        }

        if (end.equals("reset"))
        {
            puzzleReset();
            state = State::Standby;
        }

        if (end.equals("solve-on-next-try"))
        {
            solveOnNextTry();
        }
    }
}

void EscapeLogicClient::startEthernet(byte macAddress[])
{
    if (Ethernet.begin(macAddress) == 0)
    {
        Serial.println("Failed to configure Ethernet using DHCP");
        // Check for Ethernet hardware present
        if (Ethernet.hardwareStatus() == EthernetNoHardware)
        {
            Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
            while (true)
            {
                delay(1); // do nothing, no point running without Ethernet hardware
            }
        }
        if (Ethernet.linkStatus() == LinkOFF)
        {
            Serial.println("Ethernet cable is not connected.");
        }
        // try to congifure using IP address instead of DHCP:
        // Ethernet.begin(macAddress, ip, myDns);
        Serial.println("DHCP failed...");
        while(true)
        {
            delay(1); // do nothing.
        }
    }
    else
    {
        Serial.print("  DHCP assigned IP ");
        Serial.println(Ethernet.localIP());
    }
}

void EscapeLogicClient::connectMQTT()
{
    // Loop until we're reconnected
    auto name = deviceId.c_str();
    Serial.print("Attempting MQTT connection using ");

    Serial.print(name);

    while (!client.connect(name,"public", "public"))
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nconnected!");
    client.subscribe(topic);
}

void EscapeLogicClient::sendPuzzleFinished()
{
    client.publish(deviceId + "/finished", "hello world");
    state = Solved;
    puzzleSolved();
}

void EscapeLogicClient::sendProgress(String key)
{
    client.publish(deviceId + "/progress", key);
}

void EscapeLogicClient::sendProgressBack(String key)
{
    client.publish(deviceId + "/progress-back", key);
}
void EscapeLogicClient::sendLiveFeedback(String key)
{
    client.publish(deviceId + "/live-feedback", key);
}

void EscapeLogicClient::setStandbyLoop(Callback cb)
{
    standbyLoop = cb;
}
void EscapeLogicClient::setOnPuzzleStart(Callback cb){
    puzzleStart = cb;
}
void EscapeLogicClient::setPuzzleLoop(Callback cb)
{
    puzzleLoop = cb;
}
void EscapeLogicClient::setOnPuzzleSolved(Callback cb){
    puzzleSolved = cb;
}
void EscapeLogicClient::setSolvedLoop(Callback cb){
    solvedLoop = cb;
}
void EscapeLogicClient::setOnReset(Callback cb){
    puzzleReset = cb;
}
void EscapeLogicClient::setOnSolveOnNextTry(Callback cb)
{
    solveOnNextTry = cb;
}
