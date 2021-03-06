#include <SD.h>
#define FINISH  5000             // カウンタ終了値
#define INTERVAL 2             // タイマー割込の間隔(ms)
#include <TimeLib.h>
#include <Metro.h>
Metro metro0 = Metro(10);

// この値は使用しているシールドや基板に合わせて変更すること。たとえば、
// イーサーネットシールドは 4
// Adafruit のSDシールドは 10
// Sparkfun のSDシールドは 8
const int chipSelect = BUILTIN_SDCARD;
long unsigned int count1 = 0;
File dataFile;

void setup()
{
  setSyncProvider(getTeensy3Time);
  if (timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  // シリアルポート初期化
  Serial.begin(115200);
  while (!Serial) {
    ; // USBケーブルが接続されるのを待つ。この待ちループは Leonardo のみ必要。
  }

  Serial.print(F("Initializing SD card..."));

  // SSピン（Unoは10番、Megaは53番）は使わない場合でも出力にする必要があります。
  // そうしないと、SPIがスレーブモードに移行し、SDライブラリが動作しなくなります。
  pinMode(SS, OUTPUT);

  // SDライブラリを初期化
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Card failed, or not present"));
    // 失敗、何もしない
    while (1);
  }
  Serial.println(F("ok."));

  // 日付と時刻を返す関数を登録
  SdFile::dateTimeCallback( &dateTime );

  // ファイルが存在してたら削除
  if (SD.exists("datalog.csv")) {
    SD.remove("datalog.csv");
    Serial.println(F("File exist! Remove File."));
  }

  // ファイルを開く
  dataFile = SD.open("datalog.csv", FILE_WRITE);
  // ファイルが開ければストップ
  if (dataFile) {
    Serial.println(F("File opened. name:datalog.csv"));
  }
  else {
    Serial.println(F("error opening file. name:datalog.csv"));
    while (1);
  }

  setTime(0, 0, 0, 24, 7, 2017);
}

void loop() {
  if (metro0.check() == 1) { // check if the metro has passed its interval .
    if (count1 < FINISH ) {
      count1++;
      dataFile.print(hour());
      dataFile.print(":");
      dataFile.print(minute());
      dataFile.print(":");
      dataFile.print(second());
      dataFile.print(",");
      int value = analogRead(2);
      dataFile.println(value);
      // シリアルポートにも出力
      Serial.println(value);
    }
    else {
      dataFile.close();      // ファイルを閉じる
      Serial.println(F("Finished!!"));
      while (1);             // ストップ}
    }
  }
}
  void dateTime(uint16_t* date, uint16_t* time)
  {
    uint16_t year = 2013;
    uint8_t month = 2, day = 3, hour = 9, minute = 0, second = 0;

    // GPSやRTCから日付と時間を取得
    // FAT_DATEマクロでフィールドを埋めて日付を返す
    *date = FAT_DATE(year, month, day);

    // FAT_TIMEマクロでフィールドを埋めて時間を返す
    *time = FAT_TIME(hour, minute, second);
  }

  time_t getTeensy3Time()
  {
    return Teensy3Clock.get();
  }
