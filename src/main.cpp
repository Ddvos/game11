#include <Arduino.h>
#include <EscapeLogic.hpp>

#define DEVICE_NAME "puzzle11-stationconnection"

byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED};
IPAddress server(192,168,11,101);

#define RELAY_PIN 49
int cablepins[] = { 22, 23, 24, 25, 28, 29, 32,33,36,37,40,41,44,45,48,49,50};

const int solution[9][2] = {{0,8},{1,9},{2,10},{3,11},{4,12},{5,13},{6,14},{7,15},{16,16}}; // all wright connection
int connections[9] = {false,false,false,false,false,false,false,false,false}; // keep track of good connections
int totalGoodConnections=0;
int saveWrongConnections[9][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}; //all wrong connection

int openPort = false; // do something when the sollution is made 

const String countries[8] = {"frankrijk","duitsland","nederland","belgie","engeland","italie","spanje","zweden"};

void puzzleStart()
{
    pinMode(A2, INPUT_PULLUP);
    Serial.print("Start game");
}


int i = 0;
void puzzleLoop() {


    for (int checkingPin = 0; checkingPin < 17; checkingPin++){ // loop through all inputs and set checkingPin low 
      pinMode(cablepins[checkingPin], OUTPUT); 
      // Serial.println(cablepins[checkingPin]);
      digitalWrite(cablepins[checkingPin], LOW);
  
      for (int p = checkingPin; p < 17; p++){ //check conection checkingPin with other pins
        
        if(digitalRead(cablepins[p])== LOW && p != checkingPin){ // connection is made with a pin
          
            for(int s =0; s<9; s++ ){// check if connection is in sollution  array
        
              if(solution[s][0] == checkingPin && solution[s][1] ==  p && connections[s] == false){ // wright connection is made
               
                     Serial.print("Combination ");
                     Serial.print(solution[s][0]);
                     Serial.print(" and  ");
                     Serial.print(solution[s][1]);
                     Serial.println(" Is a soluttion ");
                     EscapeLogicClient::sendProgress(countries[s]);

                     totalGoodConnections++;

                     if(totalGoodConnections == 3){
                      EscapeLogicClient::sendPuzzleFinished();
                      Serial.println("!!! Game is won !!!");
                     }

                     connections[s]= true;                                           
              }
              if(((solution[s][0] == p || solution[s][1] ==  p)&&( saveWrongConnections[s][0]== 0 && saveWrongConnections[s][1]== 0))&& connections[s] == false ){
   
                     Serial.print( "wrong connection: ");
                     Serial.print( checkingPin); 
                     Serial.print( " and ");
                     Serial.println( p); 
                     EscapeLogicClient::sendProgress("wrongConnection");
                     saveWrongConnections[s][0]={checkingPin};
                     saveWrongConnections[s][1]={p};
              }
            }
        }
        if(digitalRead(cablepins[p])== HIGH && p != checkingPin){ //wright connection is diconnected
           for(int s =0; s<9; s++ ){// check if connection is in sollution  array
             if(solution[s][0] == checkingPin && solution[s][1] ==  p && connections[s] == true){
                  Serial.println("Good connection lost");
                  EscapeLogicClient::sendProgressBack(countries[s]);
                   totalGoodConnections--;
                   connections[s]= false;    
             } 
               if(saveWrongConnections[s][0] == checkingPin && saveWrongConnections[s][1] == p  ){
                  Serial.print( "[END] wrong connection: ");
                  Serial.print( checkingPin);  
                  Serial.print( " and ");
                  Serial.println( p); 
                  EscapeLogicClient::sendProgressBack("wrongConnection");
                  saveWrongConnections[s][0]= {0};
                  saveWrongConnections[s][1]= {0}; 
             }
           }   
        }
 
      }
    
    pinMode(cablepins[checkingPin], INPUT_PULLUP); // set checking pin back to input pullup    
     
 }

}

void puzzleSolved()
{
  Serial.println("Puzzle Doneeee");
}

  void setup()
  {
    // put your setup code here, to run once:
    pinMode(RELAY_PIN, OUTPUT);

    for(int i = 0; i < 17; i++) { 
      pinMode(cablepins[i], INPUT_PULLUP);
    }

    Serial.begin(115200);
    EscapeLogicClient::start(mac, server, "pottenpuzzel");
    EscapeLogicClient::setOnPuzzleStart(puzzleStart);
    EscapeLogicClient::setPuzzleLoop(puzzleLoop);
    EscapeLogicClient::setOnPuzzleSolved(puzzleSolved);

   


  }

  void loop()
  {
    EscapeLogicClient::loop();
  }
