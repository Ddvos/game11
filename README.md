# Flight 815 - Puzzle 11 stationconnection

# Algemene info spel
Het doel van dit spel is de juiste 8 combinaties te maken door door met kabels 2 stationen met elkaar te verbinden.
Zodra deze 8 combinaties zijn gemaakt schakeld het relay de deur open om naar de volgende ruimte te gaan. Totaal zijn er 200 opties,
waarvan er 16 zijn aangesloten op de Arduino Mega met Ethernet Shield.

Bij een goede of foutieve connectie stuurt de Arduino een signaal naar het Dasboard.
Als een connectie weg gaat stuurt hij ook een signaal.

Foutieve verbindingen zijn alleen zichtbaar binnen de 16 aangesloten pinnen. 

## oplossing spel
Dit zijn de juiste 8 combinaties met binnen op de Arduino:
* Pin22 Flame 2 - Pin36 Swan 16
* Pin32 Flame 18 - Pin48 Rabbit 14
* Pin23 Tempest 12 - Pin37 Swan 24
* Pin24 Tempest 21 - Pin40 Orchid 5
* Pin25 Hydra 6 - Pin41 Arrow 11
* Pin28 Hydra 23 -Pin44 Rabbit 4
* Pin29 Lampost 7 -Pin45 Orchid 17
* Pin33 Lampost 22 -Pin49 Arrow 19

* Pin 7 zit aan het relay 
* Pin 50 is een overige pin waaraan alle andere inputs gekoppeld kunnen worden.(Zie Upgrades)
  

# Hardware
* Arduino Mega met EthernetShield
* Relay wat de deur kan open schakelen

# Software
Het programma maakt gebruik van de Escape Logic Library met MQTT om data te sturen.
Verder zijn er geen extra library's nodig. 

Zodra het programma is gestart looped hij door 2 for loops.

in de eerste loop gaat hij door alle ```cablepins[]``` deze zet hij om de beurt op output ```LOW``` en aan het eind weer op ```INPUT_PULLUP```, hij switched dus continu tussen input and output.

in de tweede loop leest hij alle ```INPUT_PULLUP``` pinnen uit. Als deze laag is, zijn de pin uit de eerste loop en tweede loop op het zelfde moment laag en dus verbonden. Op dat moment controleerd hij beide pinnen in de array: ```solution[9][2]``` als deze daar bijde invoorkomen is dit een oplossing en slaat hij deze op in de array: ```connections[9]```. en stuurt hij een signaal naar EscapeLogic.

Komen beide pinnen daar niet in voor dan is er een verkeerde verbinding. Deze slaat hij op in de array:  ```saveWrongConnections[9][2]```. 

Vervolgens kijkt hij zodra een pin ```HIGH``` is of deze in één van de opgeslagen array's voorkomt. Zo ja dan betekent het dat een conectie verloren is gegaan. Dit doet hij voorzowel de goede als de verkeerde verbindingen.

Als er in totaal 8 goede connecties zijn is het spel gewonnen. 



# Upgrades
Het spel stuurt nu alle goede verbindingen en zodra er een verkeerde verbinding is.
Maar het is ook een mogelijkheid om het aantal verkeerde verbindingen te sturen en welke pinnen dit zijn.  