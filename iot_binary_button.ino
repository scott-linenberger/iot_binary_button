/* bring in Adafruit IO */
#include "AdafruitIO_WiFi.h"
/* bring in EEPROM */
#include <EEPROM.h>

/* pin configurations */
#define PIN_LED_RED 4
#define PIN_LED_BLUE 5
#define PIN_BUTTON 12

/* button debounce variables */
#define TIMEOUT_DEBOUNCE 500
#define TIMEOUT_PRESSED 50 
unsigned long time_last_pressed = 0;

/* connection constants */
#define IO_USERNAME "scottlinenberger"
#define IO_KEY "3058994edee143b2a7712f02881567a6"
#define WIFI_SSID "fortiguest"
#define WIFI_PASS "3levatecultur3"

/* init Adafruit IO */
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

/* setup the feed */
AdafruitIO_Feed *feed = io.feed("binary-button-alpha");
AdafruitIO_Feed *response = io.feed("binary-button-alpha-response");

/* button state variable */
boolean isOn = false;

void setup() {
  /* fire up the Serial connection */
  Serial.begin(9600);

  /* wait for Serial */
  while (!Serial) {}

  /* start EEPROM onboard memory */
  initEEProm(); 

  /* setup led pins */
  initPins();

  /* connect to Adafruit IO */
  connectToIO(PIN_LED_BLUE);

  /* flash the blue LED */
  flashLed(PIN_LED_BLUE, true);
}

void initPins() {
  /* set the LED pins as outputs */
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);

  /* setup button pin */
  pinMode(PIN_BUTTON, INPUT);

  /* turn on the red led */
  digitalWrite(PIN_LED_RED, HIGH);

  /* turn the blue led off */
  digitalWrite(PIN_LED_BLUE, LOW);
}

void initEEProm() {
  /* start EEPROM */
  EEPROM.begin(512);

  /* read the last update value from EEPROM */
  isOn = EEPROM.read(0);  
  
  Serial.print("Stored value: ");
  Serial.println(isOn);
}

void connectToIO(int pinLed) {
  Serial.println("Connecting to Adafruit IO...");
  io.connect();

  /* handle incoming messages */
  response->onMessage(handleMessage);

  /* set a flag for toggling the LED */
  boolean isOn = true;

  /* wait for the connection */
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    isOn = !isOn;
    digitalWrite(pinLed, isOn);
    delay(500);
  }

  /* connected! */
  Serial.println();
  Serial.println(io.statusText());
}

/* handle incoming messages from the IO feed */
void handleMessage(AdafruitIO_Data *data) {
  Serial.println("Response received!");
  flashLed(PIN_LED_RED, false); /* flash the blue LED on incoming messages */
}

boolean isButtonPressed() {
  /* read the button value */
  boolean buttonValue = digitalRead(PIN_BUTTON);

  /* if the button isn't pressed, return false*/
  if (buttonValue == LOW) {
    return false;
  }

  /* if the debounce timeout hasn't expired, return false*/
  if (millis() - time_last_pressed < TIMEOUT_DEBOUNCE) {
    return false;
  }

  /* if the debounce timeout has passed */

  /* record when the button was pressed */
  unsigned long timeout_pressed = millis();

  /* verify button was pressed long enough */
  while (digitalRead(PIN_BUTTON) == HIGH) {
    /* wait while the button is pressed */
  }

  /* check if the button was pressed long enough */
  if (millis() - timeout_pressed < TIMEOUT_PRESSED) {
    return false; /* button wasn't pressed long enough, return false*/
  }

  /* the debounce timeout has expired */
  /* the button was pressed long enough */
  /* update the last pressed time */
  time_last_pressed = millis();

  /* flash the blue LED */
  flashLed(PIN_LED_BLUE, true);

  /* button press was valid! return true */
  return true;
}

void flashLed(int pinLed, boolean isOffWhenDone) {
  boolean isOn = true;

  for(uint8_t timesFlashed = 0; timesFlashed < 7; timesFlashed++) {
    digitalWrite(pinLed, isOn); /* write the value */
    isOn = !isOn; /* toggle the value */
    delay(100); /* short delay */
  }

  if (isOffWhenDone) {
    /* turn the LED off */
    digitalWrite(pinLed, LOW);
  } else {
    /* turn the LED on */
    digitalWrite(pinLed, HIGH);
  }
}

void handleButtonPress() {
  /* toggle the isOn value */
  isOn = !isOn;

  /* write the state to EEPROM */
  EEPROM.write(0, isOn);
  EEPROM.commit();

  /* write it to the Adafruit IO feed */
  feed->save(isOn);
}

void loop() {
  /* run io */
  io.run();

  if(isButtonPressed()) {
    Serial.println("Pressed!");
    handleButtonPress();
  }

} 
