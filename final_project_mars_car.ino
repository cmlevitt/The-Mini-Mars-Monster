#include "Adafruit_VL53L1X.h"
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>  // Or #include <Adafruit_SH1106.h> for SH1106 based display
// #include <Irisoled.h>
// #include <IrisoledAnimation.h>
#include <IRremote.h>

//IR RECV
const int RECV_PIN = A3;

/************ OLED Fun *************/
// Adafruit_SSD1306 display(128, 64, &Wire, -1);
// const unsigned char* scan_frames[] = {
//   Irisoled::normal,
//   Irisoled::look_left,
//   Irisoled::look_right
// };
// const uint16_t delays[] = { 300, 300, 500 };
// IrisoledAnimation anim(scan_frames, sizeof(scan_frames) / sizeof(scan_frames[0]), delays, 200, true);

/*  LiDAR Pins  */
#define IRQ_PIN -1    // Not wired - using I2C polling only
#define XSHUT_PIN -1  // Not used

/**********  Motor Pins  ************/
// Left Motor
#define AIN1 4  // L1
#define AIN2 2  // L2
#define PWMA 5  // Lpwm_pin

// Right Motor
#define BIN1 8  // R1
#define BIN2 7  // R2
#define PWMB 6  // Rpwm_pin

// Standby
#define STBY 3

/*  Motor Direction States  */
#define FORWARD 1
#define BACKWARD 2
#define ROTATE_LEFT 3
#define ROTATE_RIGHT 4
#define STOP 5

/* CLAW pins and init*/
Servo shoulder;  // Create servo object
Servo elbow;
Servo base;
Servo claw;
#define LEFT_HOME 179
#define RIGHT_HOME 179
#define BASE_HOME 88
#define CLAW_CLOSED 105
#define BASE_PIN A1
#define CLAW_PIN 9
#define ELBOW_PIN A2
#define SHOULDER_PIN 11

/*  LiDAR Object  */
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);

/*  Distance Threshold  */
#define OBSTACLE_DISTANCE 450  // mm - stop if object closer than this

/*  Global Variables  */
int16_t current_distance = 0;


/*********************** State Machine enum ***********************/
enum CarState { Autonomous,
                RC };
CarState car_state = RC;

enum AutonomousState { scan,
                       move,
                       avoid };
AutonomousState aut_state = scan;  //start by scanning

enum RCState { stopped,
               forward,
               backward,
               right,
               left,
               base_left,
               base_right,
               shoulder_up,
               shoulder_down,
               elbow_up,
               elbow_down,
               claw_cmd };
RCState rc_state = stopped;
bool clawOpen = true;
int base_current_pos;
int shoulder_current_pos;
int elbow_current_pos;


/* IR Button State Assignment*/
void handleIR(unsigned long data_remote) {

  switch (data_remote) {

    // MODES
    case 0xB946FF00:
      car_state = Autonomous;
      break;
    case 0xB847FF00:
      car_state = RC;
      break;

      // RC MOVEMENT
    case 0xEA15FF00:
      rc_state = stopped;
      car_state = RC;   // force exit autonomous
      setMotors(0, 0);  // immediate stop
      break;
    case 0xF609FF00:
      rc_state = forward;
      break;
    case 0xF807FF00:
      rc_state = backward;
      break;
    case 0xBB44FF00:
      rc_state = left;
      break;
    case 0xBC43FF00:
      rc_state = right;
      break;
    // ARM CONTROL
    case 0xE916FF00:
      rc_state = base_left;
      break;
    case 0xF20DFF00:
      rc_state = base_right;
      break;
    case 0xF30CFF00:
      rc_state = shoulder_down;
      break;
    case 0xA15EFF00:
      rc_state = shoulder_up;
      break;
    case 0xF708FF00:
      rc_state = elbow_down;
      break;
    case 0xA55AFF00:
      rc_state = elbow_up;
      break;
    case 0xBF40FF00:
      rc_state = claw_cmd;
      break;
  }
}

/***********************  LIDAR FUNCTIONS  ***********************/
void Read_Distance() {

  while (!vl53.dataReady()) {
    delay(2);
  }
  int16_t distance = vl53.distance();
  current_distance = distance;
  Serial.print(F("Distance: "));
  Serial.print(distance);
  Serial.println(" mm");
  vl53.clearInterrupt();
}


/*********************** Function to test all moving and sensing components ***********************/
void Test_Everything() {
  Read_Distance();
  Serial.println("forward");
  setMotors(-150, -150);
  delay(2000);
  Serial.println("backward");
  setMotors(150, 150);
  delay(2000);
  Serial.println("left turn");
  setMotors(0, 150);  // L stopped, R forward
  delay(2000);
  Serial.println("right turn");
  setMotors(150, 0);  // R stopped, L forward
  delay(2000);
  test_servo("base", &base, 0, 180, 1500);
  test_servo("claw", &claw, 80, CLAW_CLOSED, 1000);
  test_servo("elbow", &elbow, 170, 50, 1200);
  test_servo("shoulder", &shoulder, RIGHT_HOME, 90, 1500);
  delay(100);
}
/***********************  MOTOR FUNCTIONS  ***********************/
void setMotors(int leftSpeed, int rightSpeed) {
  // left
  if (leftSpeed > 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else if (leftSpeed < 0) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    leftSpeed = -leftSpeed;
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
  }

  // right
  if (rightSpeed > 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } else if (rightSpeed < 0) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    rightSpeed = -rightSpeed;
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
  }

  // Constrain speeds to 0-255
  leftSpeed = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);

  analogWrite(PWMA, leftSpeed);
  analogWrite(PWMB, rightSpeed);
}

void motorStop() {
  setMotors(0, 0);
  digitalWrite(STBY, LOW);  // Standby mode
}

void motorStart() {
  digitalWrite(STBY, HIGH);  // Wake up from standby
}

/*********************** CLAW test function ***********************/
void test_servo(char* name, Servo* servo, int min, int max, int wait) {
  Serial.print("Testing Servo ");
  Serial.print(name);
  servo->write(min);
  delay(wait);
  servo->write(max);
  Serial.println("...done!");
  delay(wait);
}


/***********************  SETUP  ***********************/
void setup() {
  Serial.begin(9600);
  //delay(1000);

  /* init IR */
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);

  Wire.begin();
  if (!vl53.begin(0x29, &Wire)) {
    Serial.println("VL53L1X INIT FAILED - continuing anyway");
  } else {
    Serial.println("VL53L1X OK");

    if (!vl53.startRanging()) {
      Serial.println("Ranging failed");
    } else {
      Serial.println("Ranging started");
      vl53.setTimingBudget(50);
    }
  }
  Serial.println(F("VL53L1X sensor OK!"));
  Serial.print(F("Sensor ID: 0x"));
  Serial.println(vl53.sensorID(), HEX);
  if (!vl53.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  }
  Serial.println(F("Ranging started"));
  vl53.setTimingBudget(50);


  /* init oled */
  // display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // anim.start();

  /* initialize claw */
  elbow.attach(ELBOW_PIN);
  shoulder.attach(SHOULDER_PIN);
  base.attach(BASE_PIN);
  claw.attach(CLAW_PIN);

  //starting positions
  //edit shoulder and elbow for a tucked position
  elbow.write(160);
  shoulder.write(110);
  base.write(BASE_HOME);
  claw.write(CLAW_CLOSED);

  /*  Initialize Motors  */
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);

  // Force stable state
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  digitalWrite(STBY, HIGH);  // Enable motors

  Serial.println(F("Motors initialized"));

  /* Initialize LiDAR  */
  // while (!Serial) delay(10);

  Serial.println(F("Adafruit VL53L1X sensor demo"));
}
//   Wire.begin();
//   if (!vl53.begin(0x29, &Wire)) {
//     Serial.println("VL53L1X INIT FAILED - continuing anyway");
//   } else {
//     Serial.println("VL53L1X OK");

//     if (!vl53.startRanging()) {
//       Serial.println("Ranging failed");
//     } else {
//       Serial.println("Ranging started");
//       vl53.setTimingBudget(50);
//     }
//   }
//   Serial.println(F("VL53L1X sensor OK!"));
//   Serial.print(F("Sensor ID: 0x"));
//   Serial.println(vl53.sensorID(), HEX);
//   if (!vl53.startRanging()) {
//     Serial.print(F("Couldn't start ranging: "));
//     Serial.println(vl53.vl_status);
//     while (1) delay(10);
//   }
//   Serial.println(F("Ranging started"));
//   vl53.setTimingBudget(50);
// }

/********************** MAIN LOOP **********************/
void loop() {
  //Test_Everything();
  //take and process IR remote input
  if (IrReceiver.decode()) {
    unsigned long decodedIR = IrReceiver.decodedIRData.decodedRawData;
    handleIR(decodedIR);
    IrReceiver.resume();
  }
  int dist;
  switch (car_state) {
    case Autonomous:
      //tucked in claw
      base.write(BASE_HOME);
      shoulder.write(70);
      elbow.write(140);
      claw.write(CLAW_CLOSED);

      switch (aut_state) {
        case scan:
          // anim.update(display, 0, 0, 128, 64);
          Read_Distance();
          dist = current_distance;
          if (dist > OBSTACLE_DISTANCE) {
            aut_state = move;
          } else {
            aut_state = avoid;
          }
          break;
        case move:
          // display.clearDisplay();
          // display.drawBitmap(0, 0, Irisoled::happy, 128, 64, 1);
          // display.display();

          Read_Distance();
          dist = current_distance;

          if (dist > OBSTACLE_DISTANCE) {
            Serial.println("forward");
            setMotors(50, 50);
          } else {
            setMotors(0, 0);
            delay(150);
            aut_state = avoid;
          }
          break;
        case avoid:
          setMotors(-50, -50);
          delay(500);

          setMotors(0, 0);
          delay(300);  // brief pause

          // Scan right side before committing
          setMotors(150, 0);
          delay(200);
          Read_Distance();
          dist = current_distance;

          if (dist < OBSTACLE_DISTANCE) {
            // right blocked, try left instead
            setMotors(-150, 150);  // Left turn
            delay(600);
          }

          setMotors(0, 0);
          aut_state = scan;
      }
      break;

    case RC:
      switch (rc_state) {
        case stopped:
          // Serial.println("stopped");
          setMotors(0, 0);

          break;
        case forward:
          Serial.println("forward");
          setMotors(150, 150);
          delay(500);
          rc_state = stopped;
          break;
        case backward:
          setMotors(-150, -150);
          delay(500);
          setMotors(0, 0);
          rc_state = stopped;
          break;
        case right:
          setMotors(-150, 0);  //try -150? weird polarity
          delay(1500);
          setMotors(0, 0);
          rc_state = stopped;
          break;
        case left:
          setMotors(150, 0);
          delay(1500);
          setMotors(0, 0);
          rc_state = stopped;
          break;

        case shoulder_up:
          shoulder_current_pos = shoulder.read();
          shoulder.write(constrain(shoulder_current_pos - 10, 0, 180));
          delay(200);  // delay to avoid double action
          rc_state = stopped;
          break;

        case shoulder_down:
          shoulder_current_pos = shoulder.read();
          shoulder.write(constrain(shoulder_current_pos + 10, 0, 180));
          delay(200);
          rc_state = stopped;
          break;

        case elbow_up:
          elbow_current_pos = elbow.read();
          elbow.write(constrain(elbow_current_pos + 10, 0, 180));
          delay(200);
          rc_state = stopped;
          break;

        case elbow_down:
          elbow_current_pos = elbow.read();
          elbow.write(constrain(elbow_current_pos - 10, 0, 180));
          delay(200);
          rc_state = stopped;
          Serial.println("elb");
          Serial.println(elbow_current_pos);
          break;

        case base_left:
          base_current_pos = base.read();
          base.write(constrain(base_current_pos + 10, 0, 180));
          delay(200);
          rc_state = stopped;
          break;

        case base_right:
          base_current_pos = base.read();
          base.write(constrain(base_current_pos - 10, 0, 180));
          delay(200);
          rc_state = stopped;
          break;

        case claw_cmd:
          // Serial.println("claw bttn");
          // claw.write(90);
          //delay(1000);
          clawOpen = !clawOpen;
          delay(100);
          if (clawOpen) {
            Serial.println("Claw Open");
            claw.write(50);  //check val for accuracy
          } else {
            Serial.println("Claw Closed");
            claw.write(CLAW_CLOSED);
          }
          rc_state = stopped;
          break;
      }
      break;
  }
}
