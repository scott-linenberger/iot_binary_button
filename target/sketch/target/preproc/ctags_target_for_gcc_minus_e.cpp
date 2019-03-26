# 1 "c:\\src\\iot_binary_button\\iot_binary_button.ino"
# 1 "c:\\src\\iot_binary_button\\iot_binary_button.ino"
/* bring in Adafruit IO */
# 3 "c:\\src\\iot_binary_button\\iot_binary_button.ino" 2
/* bring in EEPROM */
# 5 "c:\\src\\iot_binary_button\\iot_binary_button.ino" 2

/* pin configurations */




/* button debounce variables */


unsigned long time_last_pressed = 0;

/* connection constants */





/* init Adafruit IO */
AdafruitIO_WiFi io("scottlinenberger", "3058994edee143b2a7712f02881567a6", "fortiguest", "3levvatecultur3");

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
  connectToIO(5);

  /* flash the blue LED */
  flashLed(5, true);
}

void initPins() {
  /* set the LED pins as outputs */
  pinMode(4, 0x01);
  pinMode(5, 0x01);

  /* setup button pin */
  pinMode(12, 0x00);

  /* turn on the red led */
  digitalWrite(4, 0x1);

  /* turn the blue led off */
  digitalWrite(5, 0x0);
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
  flashLed(4, false); /* flash the blue LED on incoming messages */
}

boolean isButtonPressed() {
  /* read the button value */
  boolean buttonValue = digitalRead(12);

  /* if the button isn't pressed, return false*/
  if (buttonValue == 0x0) {
    return false;
  }

  /* if the debounce timeout hasn't expired, return false*/
  if (millis() - time_last_pressed < 500) {
    return false;
  }

  /* if the debounce timeout has passed */

  /* record when the button was pressed */
  unsigned long timeout_pressed = millis();

  /* verify button was pressed long enough */
  while (digitalRead(12) == 0x1) {
    /* wait while the button is pressed */
  }

  /* check if the button was pressed long enough */
  if (millis() - timeout_pressed < 50) {
    return false; /* button wasn't pressed long enough, return false*/
  }

  /* the debounce timeout has expired */
  /* the button was pressed long enough */
  /* update the last pressed time */
  time_last_pressed = millis();

  /* flash the blue LED */
  flashLed(5, true);

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
    digitalWrite(pinLed, 0x0);
  } else {
    /* turn the LED on */
    digitalWrite(pinLed, 0x1);
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
