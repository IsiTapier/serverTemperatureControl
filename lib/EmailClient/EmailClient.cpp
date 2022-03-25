#include "EmailClient.h"

#define ERROR_MARGIN               4
#define ERROR_DISABLE              3

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT esp_mail_smtp_port_465//esp_mail_smtp_port_587 //port 465 is not available for Outlook.com

// #define AUTHOR_EMAIL "Isajah.Tappe@gmail.com"
// #define AUTHOR_PASSWORD "KoenigsKind777"

void smtpCallback(SMTP_Status status);

const char rootCACert[] PROGMEM = "-----BEGIN CERTIFICATE-----\n"
                                  "-----END CERTIFICATE-----\n";
unsigned long lastAlert = 0;
bool alert = false;
bool error = false;

SMTPSession smtp;
/* Declare the message class */
byte font_ = 1;

void EmailClient::setup() {
    smtp.debug(0);
    smtp.callback(smtpCallback);
}

void EmailClient::loop() {
  int temperature = DataCollection::getTempBme();
  int humidity = DataCollection::getHumidityBme();
  // if(abs(t-temperature)>ERROR_MARGIN) {
  //   if(!error)
  //     sendMessage(temperature, 1, t);
  //   error = true;
  // } else if(abs(t-temperature)<ERROR_DISABLE && error)
  //   error = false;
  if(temperature >= MAX_TEMPERATURE) {
    alert = true;
    if(lastAlert!=0&&millis()-lastAlert<RESEND_TIME)
      return;
    if (!sendMessage(temperature, humidity))
      Serial.println("Error sending Email, " + smtp.errorReason());
    else
      lastAlert = millis();
  }
  if(temperature <= NORMAL_TEMPERATURE && alert) {
    if(SEND_ENTWARNUNG)
      if(!sendMessage(temperature, humidity, 1))
        Serial.println("Error sending Email, " + smtp.errorReason());
    alert = false;
    lastAlert = 0;
  }
}

bool EmailClient::sendMessage(float temperature, float humidity, bool entwarnung, bool type) {
  if(!cEthernet::getConnected())
    return false;
  SMTP_Message message;
  ESP_Mail_Session session;

  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  /* Set the NTP config time */
  session.time.ntp_server = "pool.ntp.org,time.nist.gov";
  session.time.gmt_offset = 3;
  session.time.day_light_offset = 0;

  /* Set the message headers */
  message.sender.name = entwarnung?"Entwarnung":"Alarm";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Server Raum Temperatur Überwachung";
  message.addRecipient("We", RECEIVER_WARNING);
  if(type)
    message.addRecipient("Hausmeister", RECEIVER_EMAIL);

  String textMsg = "Temperatur: ";
  textMsg+=String(round(temperature));
  textMsg+="°c\n";
  textMsg+="Luftfeuchtigkeit: ";
  textMsg+=String(round(humidity));
  textMsg+="%";

  message.text.content = textMsg.c_str();
  // message.text.charSet = "us-ascii";
  // message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_high;
  message.addHeader("Message-ID: <abcde.fghij@gmail.com>");
  if(!smtp.connect(&session))
    return 0;

  bool out = MailClient.sendMail(&smtp, &message);

  ESP_MAIL_PRINTF("Free Heap: %d\n", MailClient.getFreeHeap());
  return out;
}

void EmailClient::smtpCallback(SMTP_Status status)
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
  }
}

 

