#include <HTTPClient.h>
#include <SPI.h>
#include <WiFi.h>

#include "Config.h"
#include "Display_EPD_W21.h"
#include "Display_EPD_W21_spi.h"
#include "Secrets.h"

#define ROW_HEIGHT 3

unsigned char Bitmap800x480[EPD_WIDTH * EPD_HEIGHT / 8];
long lastTimestamp = 0;

void setup() {
  // Configure SPI pins
  pinMode(BUSY, INPUT);
  pinMode(RST, OUTPUT);
  pinMode(DC, OUTPUT);
  pinMode(CS, OUTPUT);

  // Initiate SPI drivers to communicate with the display
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  SPI.begin(SCK, MISO, MOSI, CS);

  // Initial clean up of the display
  cleanDisplay();

  /*
  // ONLY FOR TESTING
  downloadBitmap("http://192.168.0.75/view-dithered-09.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  delay(2 * 60 * 1000);
  downloadBitmap("http://192.168.0.75/view-dithered-10.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  delay(2 * 60 * 1000);
  downloadBitmap("http://192.168.0.75/view-dithered-11.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  delay(2 * 60 * 1000);
  downloadBitmap("http://192.168.0.75/view-dithered-12.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  delay(2 * 60 * 1000);
  downloadBitmap("http://192.168.0.75/view-dithered-13.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  delay(2 * 60 * 1000);
  downloadBitmap("http://192.168.0.75/view-dithered-14.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  delay(2 * 60 * 1000);
  downloadBitmap("http://192.168.0.75/view-dithered-15.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  delay(2 * 60 * 1000);
  downloadBitmap("http://192.168.0.75/view-dithered-16.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  delay(2 * 60 * 1000);
  downloadBitmap("http://192.168.0.75/view-dithered-17.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  downloadBitmap("http://192.168.0.75/view-dithered-18.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  delay(2 * 60 * 1000);
  downloadBitmap("http://192.168.0.75/view-dithered-19.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  delay(2 * 60 * 1000);
  downloadBitmap("http://192.168.0.75/view-dithered-20.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  delay(2 * 60 * 1000);
  downloadBitmap("http://192.168.0.75/view-dithered-21.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  delay(2 * 60 * 1000);
  downloadBitmap("http://192.168.0.75/view-dithered-22.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  delay(2 * 60 * 1000);
  downloadBitmap("http://192.168.0.75/view-dithered-23.bitmap", Bitmap800x480);
  refreshDisplay(Bitmap800x480);
  */
}

void loop() {
  // TODO: Use Config.h.template for URLS
  long newTimestamp = downloadTimestamp(CONFIG_TIMESTAMP_URL);
  if (newTimestamp != lastTimestamp) {
    lastTimestamp = newTimestamp;
    downloadBitmap(CONFIG_BITMAP_URL, Bitmap800x480);
    refreshDisplay(Bitmap800x480);
  }
  delay(1 * 60 * 1000);
}

// TODO: Handle timeout and new login attempt.
void connectWiFi() {
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}


// TODO: More resilient handling of failed stream.
long downloadTimestamp(const char *url) {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  HTTPClient http;
  http.begin(url);

  // Send a GET request.
  while (http.GET() != HTTP_CODE_OK)
    delay(3000);

  String timestamp = http.getString();

  // Free resources.
  http.end();

  return timestamp.toInt();
}

// TODO: More resilient handling of failed stream.
void downloadBitmap(const char *url, unsigned char *bitmap) {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  HTTPClient http;
  http.begin(url);

  // Send a GET request.
  while (http.GET() != HTTP_CODE_OK)
    delay(3000);

  // Get length of document (is -1 when Server sends no Content-Length header).
  int len = http.getSize();

  // Get TCP stream.
  NetworkClient *stream = http.getStreamPtr();

  // Create buffer for read.
  uint8_t buff[128] = {0};

  // Track possition within the bitmap written to.
  int bitmap_i = 0;

  // Read all data from server
  while (http.connected() && (len > 0 || len == -1)) {
    size_t size = stream->available();

    if (size) {
      int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

      int i;
      for (i = 0; i < c; i++) {
        bitmap[bitmap_i] = buff[i];
        bitmap_i++;
      }

      if (len != -1) {
        len -= c;
      }
    }

    delay(1);
  }

  // Free resources.
  http.end();
}

void cleanDisplay(void) {
  EPD_Full_Init();
  EPD_Full_Set_White_Basemap();
  EPD_DeepSleep();
}

void refreshDisplay(const unsigned char *bitmap) {
  EPD_Part_Init();

  int rows = EPD_HEIGHT / ROW_HEIGHT;
  int row_i;
  for (row_i = 0; row_i < rows; row_i++) {
    int row_start_y = row_i * ROW_HEIGHT;
    int row_stop_y = ROW_HEIGHT + 1;

    EPD_Part_Set_White(0, row_start_y, row_stop_y, EPD_WIDTH);
    EPD_Part_Set_Black(0, row_start_y, row_stop_y, EPD_WIDTH);

    unsigned char buff[EPD_WIDTH * ROW_HEIGHT / 8] = {0};
    int x, y;
    for (y = 0; y < ROW_HEIGHT; y++) { // Y coordinate within the current row.
      for (x = 0; x < EPD_WIDTH / 8; x++) { // X coordinate within the current row.
        int display_y = row_i * ROW_HEIGHT + y; // Y coordinate within the full display.
        int row_byte_i = y * EPD_WIDTH / 8 + x; // Byte index within the current row.
        int display_byte_i = (display_y * EPD_WIDTH) / 8 + x; // Byte index within the full display.
        buff[row_byte_i] = ~bitmap[display_byte_i];
      }
    }
    EPD_Part_Set_Data(0, row_start_y, buff, row_stop_y, EPD_WIDTH);
  }

  EPD_DeepSleep();
}
