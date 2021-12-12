#include "EmailClient.h"

#define ERROR_MARGIN               4
#define ERROR_DISABLE              3


#define RESEND_TIME                2*60*60*1000

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465//esp_mail_smtp_port_587 //port 465 is not available for Outlook.com

#define AUTHOR_EMAIL "isajah.tappe@gmail.com"
#define AUTHOR_PASSWORD "KoenigsKind777"

#define RECEIVER_EMAIL "isajah.tappe@gmail.com"
#define RECEIVER_WARNING "isajah.tappe@gmail.com"

void smtpCallback(SMTP_Status status);

const char rootCACert[] PROGMEM = "-----BEGIN CERTIFICATE-----\n"
                                  "-----END CERTIFICATE-----\n";
unsigned long lastAlert = 0;
bool alert = false;
bool error = false;

SMTPSession smtp;
/* Declare the message class */
SMTP_Message message;
byte font_ = 1;

void EmailClient::setup() {
    smtp.debug(1);
    smtp.callback(smtpCallback);
}

void EmailClient::loop() {
  int t = 1;
  int temperature = DataCollection::getTempBme();
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

bool EmailClient::sendMessage(float temperature, bool type, float otherTemperature) {
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
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.addHeader("Message-ID: <abcde.fghij@gmail.com>");
  if (!smtp.connect(&session))
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

 

