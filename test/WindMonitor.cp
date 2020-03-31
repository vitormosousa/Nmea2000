// Demo: NMEA2000 library. Send main wind data to the bus.

#include <Arduino.h>
#include <N2kMessages.h>


//#define USE_N2K_CAN 1
#define USE_MCP_CAN_CLOCK_SET 8
// Pin for SPI Can Select
#define N2k_SPI_CS_PIN 53
// Use interrupt  and it is connected to pin 21
#define N2k_CAN_INT_PIN 20
// This will automatically choose right CAN library and create suitable NMEA2000 object

#include <NMEA2000_CAN.h>
// List here messages your device will transmit.
const unsigned long TransmitMessages[] PROGMEM = {130306L, 0};

double ReadWindAngle() {
    return DegToRad(50);  // Read here the measured wind angle e.g. from analog input
}

double ReadWindSpeed() {
    return 10.3;  // Read here the wind speed e.g. from analog input
}

#define WindUpdatePeriod 1000

void SendN2kWind() {
    static unsigned long WindUpdated = millis();
    tN2kMsg N2kMsg;

    if (WindUpdated + WindUpdatePeriod < millis()) {
        SetN2kWindSpeed(N2kMsg, 1, ReadWindSpeed(), ReadWindAngle(), N2kWind_Apprent);
        WindUpdated = millis();
        NMEA2000.SendMsg(N2kMsg);
    }
}

void setup() {
    // Set Product information
    NMEA2000.SetProductInformation("00000002",               // Manufacturer's Model serial code
                                   100,                      // Manufacturer's product code
                                   "Simple wind monitor",    // Manufacturer's Model ID
                                   "1.1.0.22 (2016-12-31)",  // Manufacturer's Software version code
                                   "1.1.0.0 (2016-12-31)"    // Manufacturer's Model version
    );
    // Set device information
    NMEA2000.SetDeviceInformation(
        1,    // Unique number. Use e.g. Serial number.
        130,  // Device function=Atmospheric. See codes on
              // http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
        85,  // Device class=External Environment. See codes on
             // http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
        2046  // Just choosen free from code list on
              // http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
    );

    // Uncomment 2 rows below to see, what device will send to bus. Use e.g. OpenSkipper or
    // Actisense NMEA Reader
    Serial.begin(115200);
    NMEA2000.SetForwardStream(&Serial);

    // If you want to use simple ascii monitor like Arduino Serial Monitor, uncomment next line
    // Show in clear text. Leave uncommented for
    // default Actisense format.
    NMEA2000.SetForwardType(tNMEA2000::fwdt_Text);

    // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of
    // N2km_NodeOnly below
    // NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly, 23);
    NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, 23);

    // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
    // NMEA2000.SetDebugMode(tNMEA2000::dm_Actisense);
    //NMEA2000.SetDebugMode(tNMEA2000::dm_ClearText);

    // Disable all msg forwarding to USB (=Serial)
    //NMEA2000.EnableForward(false);
    NMEA2000.EnableForward(true);

    // Here we tell library, which PGNs we transmit
    NMEA2000.ExtendTransmitMessages(TransmitMessages);
    NMEA2000.Open();
}

void loop() {
    SendN2kWind();
    NMEA2000.ParseMessages();
}
