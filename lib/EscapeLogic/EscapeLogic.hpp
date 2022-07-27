#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <MQTT.h>
#ifndef ESCAPELOGIC_HPP
#define ESCAPELOGIC_HPP

typedef void (*Callback)(void);

class EscapeLogicClient
{
public:
    enum State
    {
        Standby,
        Started,
        Solved
    };

    static State state;
    static void start(byte mac[], IPAddress server, String deviceId);

    // ~EscapeLogicClient();
    static void setOnReceived(void (*cb)(String action, String payload));

    static void setStandbyLoop(Callback cb);
    static void setOnPuzzleStart(Callback cb);
    static void setPuzzleLoop(Callback cb);
    static void setOnPuzzleSolved(Callback cb);
    static void setSolvedLoop(Callback cb);
    static void setOnReset(Callback cb);
    static void setOnSolveOnNextTry(Callback cb);

    static void sendPuzzleFinished();
    static void sendProgress(String key);
    static void sendProgressBack(String key);
    static void sendLiveFeedback(String key);

    static void sendCommand(String command);

    static void loop();

private:
    static void noop() {}
    
    static Callback standbyLoop;
    static Callback puzzleStart;
    static Callback puzzleLoop;
    static Callback puzzleSolved;
    static Callback solvedLoop;
    static Callback puzzleReset;
    static Callback solveOnNextTry;

    static void (*cb)(String action, String payload);

    static void startEthernet(byte macAddress[]);
    static void connectMQTT();
    static void clientReceivedCallback(String &receivedTopic, String &payload);

    static EthernetClient ethClient;
    static MQTTClient client;
    static String deviceId;
    static String topic;

    

    // byte macAddress[4] = {0,0,0,0};
};

#endif
