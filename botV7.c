
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
// Replace with your network credentials
const char *ssid = "LAPTOP-S";
const char *password = "0987654321";
int islogin = 0;
int ispass = 0;
int haspass = 0;
int iswithdraw = 0;
int isdeposit = 0;
int changePIN = 0;
int changeUsername = 0;
int transaction = 0;
String username = "";
String pin = "";
String amount = "";
String attempts = "3";
// Initialize Telegram BOT
#define BOTtoken "5642819473:AAExTuAXh4uzyiOspls5LgHfTihr9MJAwZk" // your Bot Token (Get from Botfather)
String GOOGLE_SCRIPT_IDupdate = "AKfycbxEC1BD_CN5OcuqRgLx2ZupQqU5hIgn4fcJNw0lQgQTSOOWwfO00q5r_mIiiSuqoSib";
String GOOGLE_SCRIPT_IDcheckUsr = "AKfycbyLWy7ZlCGvv-PdbWzSi-RAsDysMKTbh2lo8ioraGA49QkEfikRqI0_dpXA0eJ_NoEU";
String GOOGLE_SCRIPT_IDcheckPIN = "AKfycbxgSd8CItnL4zNDToGhROtE25KfKPIldmtWERTqvNlWGiuI9csVQc_tm7kfZlmhp1SH";
String GOOGLE_SCRIPT_IDcheckBal = "AKfycbyLSYgTXeOHAStq5QFQfghLFmmqJWEH4Mj7lbxef6CHbDCqG9sAO8KE4qPOA__QB40K";
String GOOGLE_SCRIPT_IDcheckAttempts = "AKfycbz7_lm8SIM4y-yx7owIZM4N5DB1Gnp1O2pU6QQDV21o7EX3fuBiRR6Ma8hmhh8aLgeF";
String GOOGLE_SCRIPT_IDcheckTrans = "AKfycbz2fV7nClcKyvqEfYhKiuQynbTnWJKr1PfZUxTVHqkybVBxDgOUDOkGECgCkqDiOPGX";
// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "1234490714"
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
const int ledPin = 2;
bool ledState = LOW;
// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages)
{
    Serial.println("handleNewMessages");
    Serial.println(String(numNewMessages));
    for (int i = 0; i < numNewMessages; i++)
    {
        // Chat id of the requester
        String chat_id = String(bot.messages[i].chat_id);
        if (chat_id != CHAT_ID)
        {
            bot.sendMessage(chat_id, "Unauthorized user", "");
            continue;
        }
        // Print the received message
        String text = bot.messages[i].text;
        Serial.println(text);
        String from_name = bot.messages[i].from_name;
        if (text == "/start")
        {
            String welcome = "Welcome, " + from_name + ".\n";
            welcome += "Use the following commands to control your outputs.\n\n";
            welcome += "/led_on to turn GPIO ON \n";
            welcome += "/led_off to turn GPIO OFF \n";
            welcome += "/state to request current GPIO state \n";
            welcome += "/login to login \n";
            bot.sendMessage(chat_id, welcome, "");
        }
        else if (text == "/led_on")
        {
            bot.sendMessage(chat_id, "LED state set to ON", "");
            ledState = HIGH;
            digitalWrite(ledPin, ledState);
        }


        else if (text == "/led_off")
        {
            bot.sendMessage(chat_id, "LED state set to OFF", "");
            ledState = LOW;
            digitalWrite(ledPin, ledState);
        }

        else if (text == "/state")
        {
            if (digitalRead(ledPin))
            {
                bot.sendMessage(chat_id, "LED is ON", "");
            }
            else
            {
                bot.sendMessage(chat_id, "LED is OFF", "");
            }
        }

        else if (text == "/login")
        {
            islogin = 1;
            bot.sendMessage(chat_id, "ENTER USERNAME", "");
        }
        
        else
        {
            if (islogin == 1)
            {
                HTTPClient http;
                String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_IDcheckUsr + "/exec?read";
                Serial.println("Making a request");
                http.begin(url.c_str()); // Specify the URL and certificate
                http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
                int httpCode = http.GET();
                String dataFromSheet;
                if (httpCode > 0)
                { // Check for the returning code
                    dataFromSheet = http.getString();
                    Serial.println(httpCode);
                    Serial.println(dataFromSheet);
                    if (dataFromSheet == text)
                    {
                        username = text;
                        bot.sendMessage(chat_id, "ENTER PASSWORD", "");
                        ispass = 1;
                    }
                    else
                    {
                        bot.sendMessage(chat_id, "NO SUCH USER", "");
                    }
                }
                else
                {
                    bot.sendMessage(chat_id, "Couldn't process login request, log in again", "");
                }
                islogin = 0;
            }
            else if (ispass == 1)
            {
                HTTPClient http;
                String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_IDcheckPIN + "/exec?read";
                Serial.println("Making a request");
                http.begin(url.c_str()); // Specify the URL and certificate
                http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
                int httpCode = http.GET();
                String dataFromSheet;
                if (httpCode > 0)
                { // Check for the returning code
                    dataFromSheet = http.getString();
                    Serial.println(httpCode);
                    Serial.println(dataFromSheet);
                    if (dataFromSheet == text )
                    {
                        pin = text;
                        bot.sendMessage(chat_id, "LOGIN SUCCESSFULL !!", "");
                        haspass = 1;
                        attempts = "3";
                    }
                    else
                    {
                        attempts = String(attempts.toInt() - 1);
                        transaction+=1;
                        String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_IDupdate +
                                          "/exec?" + "data1=" + username + "&data2=" + dataFromSheet + "&data3=" + String(amount) + "&data4=0&data5=0&data6=" + attempts+"&data7=WrongPinEntered&data8="+String(transaction);
                        Serial.println(urlFinal);
                        HTTPClient http;
                        http.begin(urlFinal.c_str());
                        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
                        int httpCode = http.GET();
                        Serial.print("HTTP Status Code: ");
                        Serial.println(httpCode);
                        http.end();
                        bot.sendMessage(chat_id, "Wrong PIN !!! \n You have "+attempts + "attempts remaining.", "");
                    }
                }
                else
                {
                    bot.sendMessage(chat_id, "Couldn't process pin request ,please login again", "");
                }
                ispass = 0;
            }
            if (haspass == 1)
            {   
               
               
                if (text == "/logout")
                {
                    if (haspass == 1)
                    {
                        haspass = 0;
                        bot.sendMessage(chat_id, "LOGGED OUT", "");
                    }
                    else
                    {
                        bot.sendMessage(chat_id, "Nobody has logged in , you noob", "");
                    }
                }
                if (text == "/CheckBalance")
                {
                    HTTPClient http;
                    String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_IDcheckBal + "/exec?read";
                    Serial.println("Making a request");
                    http.begin(url.c_str()); // Specify the URL and certificate
                    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
                    int httpCode = http.GET();
                    String dataFromSheet;
                    if (httpCode > 0)
                    { // Check for the returning code
                        dataFromSheet = http.getString();
                        Serial.println(httpCode);
                        Serial.println(dataFromSheet);
                        bot.sendMessage(chat_id,"Your balance is : "+ dataFromSheet, "");
                        amount = dataFromSheet;
                    }
                    else
                    {
                        bot.sendMessage(chat_id, "Couldn't process balance request.", "");
                    }
                }
                if (text == "/CheckAttempts")
                {
                    HTTPClient http;
                    String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_IDcheckAttempts + "/exec?read";
                    Serial.println("Making a request");
                    http.begin(url.c_str()); // Specify the URL and certificate
                    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
                    int httpCode = http.GET();
                    String dataFromSheet;
                    if (httpCode > 0)
                    { // Check for the returning code
                        dataFromSheet = http.getString();
                        Serial.println(httpCode);
                        Serial.println(dataFromSheet);
                        bot.sendMessage(chat_id, "You have "+ dataFromSheet+" incorrect attempts remaining.", "");
                        attempts = dataFromSheet;
                    }
                    else
                    {
                        bot.sendMessage(chat_id, "Couldn't process attempts request", "");
                    }
                }
                if (text == "/CheckTransactions")
                {
                    HTTPClient http;
                    String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_IDcheckTrans + "/exec?read";
                    Serial.println("Making a request");
                    http.begin(url.c_str()); // Specify the URL and certificate
                    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
                    int httpCode = http.GET();
                    String dataFromSheet;
                    if (httpCode > 0)
                    { // Check for the returning code
                        dataFromSheet = http.getString();
                        Serial.println(httpCode);
                        Serial.println(dataFromSheet);
                        bot.sendMessage(chat_id,"You have made "+ dataFromSheet+ " transactions.", "");
                    }
                    else
                    {
                        bot.sendMessage(chat_id, "Couldn't process transactions request", "");
                    }
                }
                if (text == "/Withdraw")
                {
                    bot.sendMessage(chat_id, "ENTER AMOUNT TO WITHDRAW", "");
                    iswithdraw = 1;
                }
                else if (iswithdraw == 1)
                {
                    HTTPClient http;
                    String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_IDcheckBal + "/exec?read";
                    Serial.println("Making a request");
                    http.begin(url.c_str()); // Specify the URL and certificate
                    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
                    int httpCode = http.GET();
                    String dataFromSheet;
                    if (httpCode > 0)
                    { // Check for the returning code
                        dataFromSheet = http.getString();
                        Serial.println(httpCode);
                        Serial.println(dataFromSheet);
                        if (dataFromSheet.toInt() >= text.toInt())
                        {
                            transaction+=1;
                            amount = ((dataFromSheet.toInt()) - (text.toInt()));
                            bot.sendMessage(chat_id, "WITHDRAW SUCCESSFULL !!", "");
                            String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_IDupdate +
                                              "/exec?" + "data1=" + username + "&data2=" + pin + "&data3=" + String(amount) + "&data4=" + text + "&data5=0&data6=" + attempts+"&data7=MoneyWithdrawn&data8="+String(transaction);
                            Serial.print("POST data to spreadsheet:");
                            Serial.println(urlFinal);
                            HTTPClient http;
                            http.begin(urlFinal.c_str());
                            http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
                            int httpCode = http.GET();
                            Serial.print("HTTP Status Code: ");
                            Serial.println(httpCode);
                            http.end();
                        }
                        else
                        {
                            bot.sendMessage(chat_id, "INSUFFICIENT BALANCE", "");
                        }
                    }
                    else
                    {
                        bot.sendMessage(chat_id, "Couldn't process withdraw request", "");
                    }
                    iswithdraw = 0;
                }
                if (text == "/Deposit")
                {
                    bot.sendMessage(chat_id, "ENTER AMOUNT TO DEPOSIT", "");
                    isdeposit = 1;
                }
                else if (isdeposit == 1)
                {
                    HTTPClient http;
                    String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_IDcheckBal + "/exec?read";
                    Serial.println("Making a request");
                    http.begin(url.c_str()); // Specify the URL and certificate
                    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
                    int httpCode = http.GET();
                    String dataFromSheet;
                    if (httpCode > 0)
                    { // Check for the returning code
                        transaction+=1;
                        dataFromSheet = http.getString();
                        Serial.println(httpCode);
                        Serial.println(dataFromSheet);
                        amount = ((dataFromSheet.toInt()) + (text.toInt()));
                        bot.sendMessage(chat_id, "DEPOSIT SUCCESSFULL !!", "");
                        String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_IDupdate +
                                          "/exec?" + "data1=" + username + "&data2=" + pin + "&data3=" + String(amount) + "&data4=0&data5=" + text + "&data6=" + attempts+"&data7=MoneyDeposited&data8="+String(transaction);
                        Serial.print("POST data to spreadsheet:");
                        Serial.println(urlFinal);
                        HTTPClient http;
                        http.begin(urlFinal.c_str());
                        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
                        int httpCode = http.GET();
                        Serial.print("HTTP Status Code: ");
                        Serial.println(httpCode);
                        http.end();
                    }
                    else
                    {
                        bot.sendMessage(chat_id, "Couldn't process deposit request", "");
                    }
                    isdeposit = 0;
                }
                if (text == "/ChangePIN")
                {
                    bot.sendMessage(chat_id, "ENTER NEW PIN", "");
                    changePIN = 1;
                }
                else if (changePIN == 1)
                {
                    transaction+=1;
                    pin=text;
                    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_IDupdate +
                                      "/exec?" + "data1=" + username + "&data2=" + text + "&data3=" + String(amount) +
                                      "&data4=0&data5=0&data6=" + attempts+"&data7=PINChanged&data8="+String(transaction);
                    Serial.print("POST data to spreadsheet:");
                    Serial.println(urlFinal);
                    HTTPClient http;
                    http.begin(urlFinal.c_str());
                    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
                    int httpCode = http.GET();
                    Serial.print("HTTP Status Code: ");
                    Serial.println(httpCode);
                    http.end();
                    bot.sendMessage(chat_id, "PIN CHANGED SUCCESSFULLY !!", "");
                    changePIN = 0;
                }
                if (text == "/changeUsername")
                {
                    bot.sendMessage(chat_id, "ENTER NEW USERNAME", "");
                    changeUsername = 1;
                }
                else if (changeUsername == 1)
                {
                    transaction+=1;
                    username=text;
                    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_IDupdate +
                                      "/exec?" + "data1=" + text + "&data2=" + pin + "&data3=" + String(amount) +
                                      "&data4=0&data5=0&data6=" + attempts+"&data7=UsernameChanged&data8="+String(transaction);
                    Serial.print("POST data to spreadsheet:");
                    Serial.println(urlFinal);
                    HTTPClient http;
                    http.begin(urlFinal.c_str());
                    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
                    int httpCode = http.GET();
                    Serial.print("HTTP Status Code: ");
                    Serial.println(httpCode);
                    http.end();
                    bot.sendMessage(chat_id, "USERNAME CHANGED SUCCESSFULLY !!", "");
                    changeUsername = 0;
                }
                if (haspass == 1 && islogin == 0 && ispass == 0 && iswithdraw == 0 && isdeposit == 0 && changePIN == 0 && changeUsername == 0)
            
                {
                    String welcome = "Welcome.\n";
                    welcome += "Use the following commands to control your outputs.\n\n";
                    welcome += "/CheckBalance to Check your balance \n";
                    welcome += "/CheckTransactions to Check your number of transactions \n";
                    welcome += "/Withdraw to take away some money  \n";
                    welcome += "/Deposit to deposit money  \n";
                    welcome += "/ChangePIN to change your PIN \n";
                    welcome += "/changeUsername to change your Username (We DO NOT recommend doing this)\n";
                    welcome += "/CheckAttempts to check your number of incorrect attempts you have left\n";
                    welcome += "/logout to logout  \n";
                    bot.sendMessage(chat_id, welcome, "");
                }
            }
        }
    }
}
void setup()
{
    Serial.begin(115200);

#ifdef ESP8266
configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
client.setTrustAnchors(&cert);    // Add root certificate for api.telegram.org
#endif

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, ledState);

    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    #ifdef ESP32
client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
#endif

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    // Print ESP32 Local IP Address
    Serial.println(WiFi.localIP());
}

void loop()
{
    if (millis() > lastTimeBotRan + botRequestDelay)
    {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        while (numNewMessages)
        {
            Serial.println("got response");
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        lastTimeBotRan = millis();
    }
}