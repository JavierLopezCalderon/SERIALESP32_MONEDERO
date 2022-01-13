#define ALTSOFTCOMM
#ifdef ALTSOFTCOMM
#include <AltSoftSerial.h>
#endif
#include "cctcom.h"
#include "headers.h"

unsigned int CoinValor, ValorError, ValorDesconexion;

// some garbage , without it my compiler gives a fatal error , you can skip this if the sketch compiles.
long weqwerqr; long xxxx[1]; long sdasdasd[1]; long sdasdasa[1]; long sdasdase[1];
///
milistimer somedelay;// to handle the break after a comm error
unsigned int coin_value[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // to store the coin value
void printASCIIdata() { // prints the ascii data field
  for (int i = 4 ; i < RX_buffer[1] + 4; i++) {
    Serial.write(RX_buffer[i]);
    //Serial.print(RX_buffer[i]);
  }
  Serial.println();
}
void printBINdata() { // prints the bin data field
  for (int i = 4 ; i < RX_buffer[1] + 4; i++) {
    Serial.print(RX_buffer[i]);
  }
  Serial.println();
}

void EnvioValor(){
  switch (CoinValor){
   case 10:
    Serial1.write("CU1");
    Serial.println("CU1");
   break;
   case 20:
    Serial1.write("CU2");
    Serial.println("CU2");
   break;
   case 50:
    Serial1.write("CU5");
    Serial.println("CU5");
   break;
   case 100:
    Serial1.write("CD1");
    Serial.println("CD1");
   break;
   case 200:
    Serial1.write("CD2");
    Serial.println("CD2");
   break;
   case 65386:
    Serial1.write("CC5");
    Serial.println("CC5");
   break;  
   case 65346:
    Serial1.write("CC1");
    Serial.println("CC5");
   break;
   default:
    Serial1.write("CC2");
    Serial.println("CC5");
   break;
  }
}

void EnvioError(){
  switch (ValorError){
    case 1:
      Serial1.write(0xE1);
      Serial.println("E1");
     break;
     case 14:
      Serial1.write(0xE2);
      Serial.println("E14");
     break;
     case 254:
      Serial1.write(0xE3);
      Serial.println("E254");
     break;
     case 8:
      Serial1.write(0xE4);
      Serial.println("E8");
     break;
  }
}

void EvioDesconexion(){
  switch (ValorDesconexion){
    case 1:
      Serial1.write(0xD1);
      Serial.println("D1");
      ValorDesconexion =0;
     break;
     case 2:
      Serial1.write(0xD2);
      Serial.println("D2");
      ValorDesconexion =0;
     break;
     case 3:
      Serial1.write(0xD3);
      Serial.println("D3");
      ValorDesconexion =0;
     break;
     case 4:
      Serial1.write(0xD4);
      Serial.println("D4");
      ValorDesconexion =0;
     break;
     case 5:
      Serial1.write(0xD5);
      Serial.println("D5");
      ValorDesconexion =0;
     break;
     case 6:
      Serial1.write(0xD6);
      Serial.println("D6");
      ValorDesconexion =0;
     break;
     case 7:
      Serial1.write(0xD7);
      Serial.println("D7");
      ValorDesconexion =0;
     break;
     case 8:
      Serial1.write(0xD8);
      Serial.println("D8");
      ValorDesconexion =0;
     break;
     case 9:
      Serial1.write(0xD9);
      Serial.println("D9");
      ValorDesconexion =0;
     break;
  }
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  ccTalk_comm_init();
  while ( RX_state != RXidle) {
    ccTalkReceive();
  }
}
void loop() {
  unsigned int coineventcounter;//used to follow the events from read credit or error codes
  unsigned int buffered_events; //how many events are queued

  coineventcounter = 0;// prepare for reading events
start:
  cctsend(simple_poll, 2, 0);
  while (RX_state < RXcomplete) {
    ccTalkReceive(); // wait for an answer or error
  }
  if (RX_state != RXcomplete) {
    Serial.println(RX_msg[RX_state]);// Show the error
    Serial.println("No answer to simple pool");
    ValorDesconexion = 1;
    EvioDesconexion();
    //while (1); // stop
  } else
    Serial.println("ccTalk device present");// simple pool was ok

  cctsend(request_manufacturer_id, 2, 0);
  while (RX_state < RXcomplete) {
    ccTalkReceive();
  }
  if (RX_state != RXcomplete) {
    Serial.println(RX_msg[RX_state]);// Show the error if any
    Serial.println("No answer to Request manufacturer ID");
    ValorDesconexion = 2;
    EvioDesconexion();
  } else
    printASCIIdata(); // Print the manufacturer ID

  cctsend(request_equipment_category_id, 2, 0);
  while (RX_state < RXcomplete) {
    ccTalkReceive();
  }
  if (RX_state != RXcomplete) {
    Serial.println(RX_msg[RX_state]);// Show the error if any
    Serial.println("No answer to Request equipment category id");
    ValorDesconexion = 3;
    EvioDesconexion();
  } else
    printASCIIdata(); // Print the equipment category ID

  cctsend(request_product_code, 2, 0);
  while (RX_state < RXcomplete) {
    ccTalkReceive();
  }
  if (RX_state != RXcomplete) {
    Serial.println(RX_msg[RX_state]);// Show the error if any
    Serial.println("No answer to request_product_code");
    ValorDesconexion = 4;
    EvioDesconexion();
  } else
    printASCIIdata(); // Print the product code

  cctsend(request_software_revision, 2, 0);
  while (RX_state < RXcomplete) {
    ccTalkReceive();
  }
  if (RX_state != RXcomplete) {
    Serial.println(RX_msg[RX_state]);// Show the error if any
    Serial.println("No answer to request_software_revision");
    ValorDesconexion = 5;
    EvioDesconexion();
  } else
    printASCIIdata(); // Print the Software revision

  for (int i = 0; i < 16 ; i++) coin_value[i] = 0; // Clean the coin value array

  // Get coin ID, filter garbage convert and store in coin_value as unsigned int
  for (unsigned char i = 1; i < 17; i++) {
    somedelay.startt(2);//for some unknown reason a little delay is needed between polls
    do {
    } while (!somedelay.isready());
    cctsend(request_coin_id, 2, 1, i);
    
    while (RX_state < RXcomplete) {
      ccTalkReceive();
    }
    
    if (RX_state != RXcomplete) {
      Serial.print(RX_msg[RX_state]);// Show the error if any
      Serial.print(" at coin channel ");
      Serial.println(i);
      break;
    }
   // if ((RX_buffer[4] == 32) && (RX_buffer[5] == 32) && (RX_buffer[6] == 32) && (RX_buffer[7] == 32) && (RX_buffer[8] == 32) && (RX_buffer[9] == 32)) continue;
   // if ((RX_buffer[4] == 46) && (RX_buffer[5] == 46) && (RX_buffer[6] == 46) && (RX_buffer[7] == 46) && (RX_buffer[8] == 46) && (RX_buffer[9] == 46)) continue;
   // if ((65 > RX_buffer[4]) || (RX_buffer[4] > 90) || (65 > RX_buffer[5]) || (RX_buffer[5] > 90) || (65 > RX_buffer[9]) || (RX_buffer[9] > 90)) break;
   // if ((48 > RX_buffer[6]) || (RX_buffer[6] > 57) || (48 > RX_buffer[7]) || (RX_buffer[7] > 57) || (48 > RX_buffer[8]) || (RX_buffer[8] > 57)) break;
    printASCIIdata();// Print ASCII coin ID
    coin_value[i - 1] = (RX_buffer[6] - 48) * 100 + (RX_buffer[7] - 48) * 10 + RX_buffer[8] - 48;
  }
  Serial.println("Coin values OK");
  Serial.println("Setting individual inhibits");
  cctsend(modify_inhibit_status, 2, 2, 255, 255); //just enable all channels
  while (RX_state < RXcomplete) {
    ccTalkReceive();
  }
  if (RX_state != RXcomplete) {
    Serial.println(RX_msg[RX_state]);// Show the error if any
    Serial.println("No answer to modify_inhibit_status");
    ValorDesconexion = 6;
    EvioDesconexion();
  }
  if (RX_buffer[3] == 0) Serial.println("OK");
  else {
    Serial.println("Error setting inhibits");
  }
  Serial.println("Setting master inhibit");
  cctsend(modify_master_inhibit_status, 2, 1, 1);
  while (RX_state < RXcomplete) {
    ccTalkReceive();
  }
  if (RX_state != RXcomplete) {
    Serial.println(RX_msg[RX_state]);// Show the error if any
    Serial.println("No answer to modify_master_inhibit_status");
    ValorDesconexion = 7;
    EvioDesconexion();
  }
  if (RX_buffer[3] == 0) Serial.println("OK");
  else {
    Serial.println("Error setting master inhibit");
    ValorDesconexion = 8;
    EvioDesconexion();
    //while(1);
  }
  // read buffer credit or error codes
  Serial.println("reading credits");
  while (1) {
    cctsend(read_buffered_credit_or_error_codes, 2, 0);
    while (RX_state < RXcomplete) {
      ccTalkReceive();
    }
    if (RX_state != RXcomplete) {
      Serial.println(RX_msg[RX_state]);// Show the error if any
      Serial.println("Stopped");
      ValorDesconexion = 9;
      EvioDesconexion();
      ccTalk_comm_init();
      while ( RX_state != RXidle) {
        ccTalkReceive();
      } goto start;// Some error handling needed here
    }

    somedelay.startt(2);//for some unknown reason a little delay is needed between polls
    do {
    } while (!somedelay.isready());
    // find how many new events are queued, RX_buffer[4] data contains the event counter
    if (RX_buffer[4] >= coineventcounter)  buffered_events = RX_buffer[4] - coineventcounter;
    else buffered_events = RX_buffer[4] + 255 - coineventcounter;

    if (buffered_events > 5) {// overflow,events lost, put some error handling here
      buffered_events = 0;
      coineventcounter = RX_buffer[4]; //Clear the queued events
    }

    while (buffered_events > 0) {// Read buffered events one by one
      buffered_events--;
      coineventcounter = (coineventcounter + 1) % 256;//increment event counter
      if (coineventcounter == 0) coineventcounter = 1; // skip 0
      if (RX_buffer[(buffered_events << 2) + 5] == 0) { // event A = 0 mans an error or coin rejected
        Serial.print("Some error or coin rejected, error code: ");
        Serial.println(RX_buffer[(buffered_events << 2) + 6]);// then event B is the error code
        ValorError= RX_buffer[(buffered_events << 2) + 6];
        EnvioError();
      } else {
        Serial.print("Coin value ");
        Serial.println(coin_value[RX_buffer[(buffered_events << 2) + 5] - 1]);//The corresponding coin value in the table
          CoinValor = coin_value[RX_buffer[(buffered_events << 2) + 5] - 1];
          //Serial.println(CoinValor);
          EnvioValor();              
        Serial.print("Sorter path ");
        Serial.println(RX_buffer[(buffered_events << 2) + 6]); // event B show the sorter path
      }
    }
  }
}
