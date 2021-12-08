#include "Arduino.h"
#include "TFT_eSPI.h"
#include "DHT.h"
#include "Adafruit_BME280.h"
#include <WiFi.h>
#include <ESP_Mail_Client.h>

#define MAX_TEMPERATURE            25
#define NORMAL_TEMPERATURE         20

#define RESEND_TIME                2*60*60*1000

#define ERROR_MARGIN               4
#define ERROR_DISABLE              3

#define LETTER_LENGTH              6
#define LETTER_HEIGHT              8

#define DHTPIN 16
#define DHTTYPE DHT22

#define WIFI_SSID "ESP32"
#define WIFI_PASSWORD "12345678"

#define SMTP_HOST "smtp.gmail.com"

/** The smtp port e.g. 
 * 25  or esp_mail_smtp_port_25
 * 465 or esp_mail_smtp_port_465
 * 587 or esp_mail_smtp_port_587
*/
#define SMTP_PORT 465//esp_mail_smtp_port_587 //port 465 is not available for Outlook.com

/* The log in credentials */
// #define AUTHOR_EMAIL "ea.jcbs@gmail.com"
// #define AUTHOR_PASSWORD "5'wV.lLJ~W&p;0|i^LK9"

#define AUTHOR_EMAIL "isajah.tappe@gmail.com"
#define AUTHOR_PASSWORD "KoenigsKind777"

#define RECEIVER_EMAIL "isajah.tappe@gmail.com"
#define RECEIVER_WARNING "isajah.tappe@gmail.com"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

const char rootCACert[] PROGMEM = "-----BEGIN CERTIFICATE-----\n"
                                  "-----END CERTIFICATE-----\n";

unsigned long lastAlert = 0;
bool alert = false;
bool error = false;

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BME280 bme(18, 19);

void dPrint(String text, int x, int y, int size, int color, int datum = 0, int backgroundColor = -1, String oldText = "", int oldTextSize = -1, boolean redraw = false, int padding = 0) {
  display.setTextPadding(padding);
  display.setTextDatum(datum);
  if(backgroundColor == 0)
    backgroundColor = TFT_BLACK;
  if(oldText.length() != 0) {
    if(oldTextSize < 0)
      oldTextSize = size;
    int xold = x;
    if(datum % 3 == 1) {
      x -= size*LETTER_LENGTH*text.length()/2;
      xold -= oldTextSize*LETTER_LENGTH*oldText.length()/2;
      display.setTextDatum(datum-1);
    } else if(datum % 3 == 2) {
      x -= LETTER_LENGTH*size*text.length();
      xold -= LETTER_LENGTH*oldTextSize*oldText.length();
      display.setTextDatum(datum-2);
    }
    display.setTextColor(backgroundColor);
    display.setTextSize(oldTextSize);
    if(redraw) {
      display.drawString(oldText, x, y);
      display.setTextColor(color);
      display.setTextSize(size);
      display.drawString(text, x, y);
    } else {
      for(int i = 0; i < oldText.length(); i++) {
        if(text.charAt(i) != oldText.charAt(i) || text.length() != oldText.length()) {
          display.drawString((String) oldText.charAt(i), (xold+LETTER_LENGTH*oldTextSize*i), y);
        }
      }
      display.setTextColor(color);
      display.setTextSize(size);
      for(int i = 0; i < text.length(); i++) {
        if(text.charAt(i) != oldText.charAt(i) || text.length() != oldText.length()) {
          display.drawString((String) text.charAt(i), x+LETTER_LENGTH*size*i, y);
        }
      }
    }
  } else {
    display.setTextSize(size);
    display.setCursor(x, y);
    if(backgroundColor > -1)
      display.setTextColor(color, backgroundColor);
    else
      display.setTextColor(color);
    display.drawString(text);
  }
}

//Verkürzung: Writing mit Integern
void dPrint(int text, int x, int y, int size, int color, int datum = 0, int backgroundColor = -1, int oldText = -1, int oldTextSize = -1, boolean redraw = false, int padding = 0) {
  dPrint(String(text), x, y, size, color, datum, backgroundColor, (oldText == -1) ? "" : String(oldText), oldTextSize, redraw, padding);
}


/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status)
{
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success())
  {
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");

    //You need to clear sending result as the memory usage will grow up as it keeps the status, timstamp and
    //pointer to const char of recipients and subject that user assigned to the SMTP_Message object.

    //Because of pointer to const char that stores instead of dynamic string, the subject and recipients value can be
    //a garbage string (pointer points to undefind location) as SMTP_Message was declared as local variable or the value changed.

    //smtp.sendingResult.clear();
  }
}

  /* Declare the message class */
  SMTP_Message message;
  byte font_ = 1;

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));

  if (bme.begin(0x76)) {
    Serial.println(F("BME-Sensor initialized"));
  } else {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
  }

  display.init(INITR_GREENTAB128);
  display.setRotation(3);
  display.writecommand(ST7735_DISPON);
  display.fillScreen(TFT_BLACK);

  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.reconnect();
  while (WiFi.status() != WL_CONNECTED)
  {
    // return;
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);
}

bool sendMessage(float temperature, bool type = 0, float otherTemperature = 0) {
  Serial.println();

  Serial.print("Connecting to AP");
  if(WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    WiFi.reconnect();
    while (WiFi.status() != WL_CONNECTED)
    {
      // return;
      Serial.print(".");
      delay(200);
  
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  }


    /** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
   * 
   * Debug port can be changed via ESP_MAIL_DEFAULT_DEBUG_PORT in ESP_Mail_FS.h
  */


  /* Declare the session config data */
  ESP_Mail_Session session;

  /** ########################################################
   * Some properties of SMTPSession data and parameters pass to 
   * SMTP_Message class accept the pointer to constant char
   * i.e. const char*. 
   * 
   * You may assign a string literal to that properties or function 
   * like below example.
   *   
   * session.login.user_domain = "mydomain.net";
   * session.login.user_domain = String("mydomain.net").c_str();
   * 
   * or
   * 
   * String doman = "mydomain.net";
   * session.login.user_domain = domain.c_str();
   * 
   * And
   * 
   * String name = "Jack " + String("dawson");
   * String email = "jack_dawson" + String(123) + "@mail.com";
   * 
   * message.addRecipient(name.c_str(), email.c_str());
   * 
   * message.addHeader(String("Message-ID: <abcde.fghij@gmail.com>").c_str());
   * 
   * or
   * 
   * String header = "Message-ID: <abcde.fghij@gmail.com>";
   * message.addHeader(header.c_str());
   * 
   * ###########################################################
  */

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  // session.login.user_domain = "mydomain.net";
  // session.certificate.cert_data = rootCACert;

  /* Set the NTP config time */
  session.time.ntp_server = "pool.ntp.org,time.nist.gov";
  session.time.gmt_offset = 3;
  session.time.day_light_offset = 0;

  /* Set the message headers */
  message.sender.name = "Server Raum Temperatur Überwachung";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Server Temperatur Alarm";
  message.addRecipient("We", RECEIVER_WARNING);
  if(type == 0)
  message.addRecipient("Hausmeister", RECEIVER_EMAIL);

  String textMsg = "Server Raum Temperatur: ";
  textMsg+=String(temperature, 2);
  textMsg+="°c";
  if(type == 1) {
    textMsg+="\nother Sensor: ";
    textMsg+=String(otherTemperature, 2);
    textMsg+="°c";
  }
  message.text.content = textMsg.c_str();

  /** The Plain text message character set e.g.
   * us-ascii
   * utf-8
   * utf-7
   * The default value is utf-8
  */
  message.text.charSet = "us-ascii";

  /** The content transfer encoding e.g.
   * enc_7bit or "7bit" (not encoded)
   * enc_qp or "quoted-printable" (encoded)
   * enc_base64 or "base64" (encoded)
   * enc_binary or "binary" (not encoded)
   * enc_8bit or "8bit" (not encoded)
   * The default value is "7bit"
  */
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;


  //If this is a reply message
  //message.in_reply_to = "<parent message id>";
  //message.references = "<parent references> <parent message id>";

  /** The message priority
   * esp_mail_smtp_priority_high or 1
   * esp_mail_smtp_priority_normal or 3
   * esp_mail_smtp_priority_low or 5
   * The default value is esp_mail_smtp_priority_low
  */
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;



  //message.response.reply_to = "someone@somemail.com";
  //message.response.return_path = "someone@somemail.com";

  /** The Delivery Status Notifications e.g.
   * esp_mail_smtp_notify_never
   * esp_mail_smtp_notify_success
   * esp_mail_smtp_notify_failure
   * esp_mail_smtp_notify_delay
   * The default value is esp_mail_smtp_notify_never
  */
  //message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

  /* Set the custom message header */
  message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

  //For Root CA certificate verification (ESP8266 and ESP32 only)
  //session.certificate.cert_data = rootCACert;
  //or
  //session.certificate.cert_file = "/path/to/der/file";
  //session.certificate.cert_file_storage_type = esp_mail_file_storage_type_flash; // esp_mail_file_storage_type_sd
  //session.certificate.verify = true;

  //The WiFiNINA firmware the Root CA certification can be added via the option in Firmware update tool in Arduino IDE

  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return 0;

  /* Start sending Email and close the session */
  bool out = MailClient.sendMail(&smtp, &message);
  
    // Serial.println("Error sending Email, " + smtp.errorReason());

  //to clear sending result log
  //smtp.sendingResult.clear();

  ESP_MAIL_PRINTF("Free Heap: %d\n", MailClient.getFreeHeap());
  return out;
}

float oldTemperature = 0;
float oldHumidity = 0;
float oldPressure = 0;

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    // return;


  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure()/100;
  Serial.print(F("other Sensor: "));
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
  Serial.print(F("BME: "));
  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.print(F("°C Pressure: "));
  Serial.print(pressure);
  Serial.println(F("mbar"));
  display.setTextSize(1);
  //temperature
  display.setTextFont(7);
  display.setCursor(TFT_HEIGHT/2-23, 32);
  display.setTextColor(TFT_BLACK);
  display.setTextDatum(CC_DATUM);
  display.drawString(String(oldTemperature, 1));
  display.setTextColor(temperature>MAX_TEMPERATURE?TFT_RED:TFT_WHITE);
  display.setCursor(TFT_HEIGHT/2-23, 32);
  display.drawString(String(temperature, 1));
  display.setFreeFont(&FreeSansBold24pt7b);
  display.drawString("             c");
  display.fillCircle(122, 22, 7, temperature>MAX_TEMPERATURE?TFT_RED:TFT_WHITE);
  display.fillCircle(122, 22, 3, TFT_BLACK);
  //humidity
  display.setTextFont(7);
  display.setTextColor(TFT_BLACK);
  display.setCursor(TFT_HEIGHT/2-23, 94);
  display.drawString(String(oldHumidity, 1));
  display.setTextColor(TFT_WHITE);
  display.drawString(String(humidity, 1));
  display.setFreeFont(&FreeSansBold24pt7b);
  display.drawString("            %");
  // dPrint(String(temperature, 1)+String(char(167))+"c", TFT_HEIGHT/2, 30, 1, temperature>=MAX_TEMPERATURE?TFT_RED:TFT_WHITE, CC_DATUM, TFT_BLACK, String(oldTemperature, 1)+String(char(167))+"c", -1, temperature>=MAX_TEMPERATURE&&oldTemperature<MAX_TEMPERATURE||temperature<MAX_TEMPERATURE&&oldTemperature>=MAX_TEMPERATURE);
  // dPrint(String(humidity, 1)+"%", TFT_HEIGHT/2, 100, 1, TFT_WHITE, CC_DATUM, TFT_BLACK, String(oldHumidity, 1)+"%");
  // dPrint(String(pressure, 0)+" mBar", TFT_HEIGHT/2, 110, 2, TFT_WHITE, CC_DATUM, TFT_BLACK, String(oldPressure, 0)+" mBar");
  oldTemperature = temperature;
  oldHumidity = humidity;
  oldPressure = pressure;
  t = 1;
  if(abs(t-temperature)>ERROR_MARGIN) {
    if(!error)
      sendMessage(temperature, 1, t);
    error = true;
  } else if(abs(t-temperature)<ERROR_DISABLE && error)
    error = false;
  if(temperature > MAX_TEMPERATURE) {
    alert = true;
    if(lastAlert!=0&&millis()-lastAlert<RESEND_TIME)
      return;
    if (!sendMessage(temperature))
      Serial.println("Error sending Email, " + smtp.errorReason());
    else
      lastAlert = millis();
  }
  if(temperature < NORMAL_TEMPERATURE && alert) {
    alert = false;
    lastAlert = 0;
  }
  }
}