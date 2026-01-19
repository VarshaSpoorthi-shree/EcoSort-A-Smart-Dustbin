#include <CheapStepper.h>
#include <Servo.h>

// 1. PIN DEFINITIONS

// SENSORS
#define ULTRASONIC_TRIG_PIN 4       // New pin for Ultrasonic Sensor Trigger
#define ULTRASONIC_ECHO_PIN 6       // Renamed 'proxi' to ECHO pin for Fill Level Measurement
#define WASTE_DETECT_IR_PIN 5       // Renamed 'ir' to IR sensor for object detection at the top
#define MOISTURE_SENSOR_PIN A0      // Renamed 'potPin' to Moisture Sensor Analog Input

// ACTUATORS
#define BUZZER_PIN 12               // Buzzer for alerts
#define SERVO_PIN 7                 // Servo motor for the main lid/door
// Stepper Motor Pins (8, 9, 10, 11 are already defined by CheapStepper object)

// 2. CONSTANTS AND THRESHOLDS

// SERVO ANGLES (Degrees)
const int LID_OPEN_ANGLE = 180;     // Angle to open the lid (e.g., fully vertical)
const int LID_CLOSED_ANGLE = 70;    // Angle to keep the lid closed (e.g., slight angle to cover opening)

// STEPPER ANGLES (Degrees) - Assuming a 3-compartment system (Dry, Wet, Metallic)
// If the home position is 0, these are the positions for each bin.
const int DRY_WASTE_ANGLE = 120;    // Stepper rotation for Dry (was used in original code)
const int WET_WASTE_ANGLE = -120;   // Stepper rotation for Wet (CCW - adjust based on mechanism)
const int METALLIC_WASTE_ANGLE = 0; // Assuming metallic drops straight down at home position (0)

// SENSOR THRESHOLDS
const int MOISTURE_THRESHOLD = 20;  // Percentage: If moisture > 20%, classify as WET waste
const int FULL_DISTANCE_CM = 15;    // Centimeters: Distance at which the bin is considered FULL

// 3. GLOBAL VARIABLES AND OBJECTS

Servo servo1;
CheapStepper stepper(8, 9, 10, 11);

int fsoil = 0; // Final calculated moisture percentage (0-100%)

// 4. HELPER FUNCTION: ULTRASONIC DISTANCE MEASUREMENT

long getDistance() {
  // Clears the ULTRASONIC_TRIG_PIN condition
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Sets the ULTRASONIC_TRIG_PIN HIGH for 10 microsecond pulse
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  // Reads the ULTRASONIC_ECHO_PIN, returns the sound wave travel time in microseconds
  long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);

  // Calculate the distance: Speed of sound = 343 m/s or 0.0343 cm/us
  // Distance = (Duration * 0.0343) / 2 (divide by 2 because sound travels out and back)
  long distance_cm = duration * 0.034 / 2;
  return distance_cm;
}

// 5. SETUP

void setup()
{
  Serial.begin(9600);
  // Sensor pin modes
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  pinMode(WASTE_DETECT_IR_PIN, INPUT);

  // Actuator pin modes
  pinMode(BUZZER_PIN, OUTPUT);
  servo1.attach(SERVO_PIN);
  stepper.setRpm(17); // Set speed for stepper motor

  // Initial Calibration/Test of Servo Motor
  servo1.write(LID_CLOSED_ANGLE);
  delay(500);
  Serial.println("System Initialized. Bin ready.");
}

// 6. MAIN LOOP

void loop()
{
  // --- A. FILL LEVEL MONITORING (Using Ultrasonic Sensor) ---
  long binDistance = getDistance();
  Serial.print("Bin Level: ");
  Serial.print(binDistance);
  Serial.println(" cm");

  if (binDistance < FULL_DISTANCE_CM && binDistance > 0) {
    // BIN IS FULL - Trigger continuous alert
    Serial.println("!!! ALERT: BIN IS FULL !!!");
    tone(BUZZER_PIN, 500, 1000); // Low, continuous tone
    delay(2000);
    noTone(BUZZER_PIN);
  }

  // --- B. WASTE DETECTION (Using IR Sensor) ---
  // The IR sensor detects when an object is placed at the sorting area.
  if (digitalRead(WASTE_DETECT_IR_PIN) == LOW)
  {
    Serial.println("\n--- Waste Detected, Starting Sorting Cycle ---");
    tone(BUZZER_PIN, 1500, 300); // Short tone for detection confirmation
    delay(500);

    // 1. READ MOISTURE SENSOR (PotPin A0)
    int soil = 0;
    fsoil = 0;
    for (int i = 0; i < 3; i++) // Take 3 readings and average for stability
    {
      soil = analogRead(MOISTURE_SENSOR_PIN);
      // USER CALIBRATION: Constraining and mapping raw sensor values (485 to 1023) to 0-100%
      soil = constrain(soil, 485, 1023);
      fsoil += map(soil, 485, 1023, 100, 0); // Accumulate percentage
      delay(75);
    }
    fsoil /= 3; // Get the average moisture percentage
    Serial.print("Moisture Content: ");
    Serial.print(fsoil);
    Serial.println("%");

    // 2. WASTE CLASSIFICATION AND ACTUATION
    if (fsoil > MOISTURE_THRESHOLD) // WET WASTE
    {
      Serial.println("-> Classified as: WET WASTE (Biodegradable)");
      // Move Stepper to WET position
      stepper.moveDegreesCCW(WET_WASTE_ANGLE);
      delay(1000);
    }
    else // DRY WASTE (Includes metallic based on this simple two-stage logic)
    {
      Serial.println("-> Classified as: DRY WASTE (Non-Biodegradable)");
      // Move Stepper to DRY position
      stepper.moveDegreesCW(DRY_WASTE_ANGLE);
      delay(1000);
    }

    // 3. DISPOSAL CYCLE (Lid Open/Close)
    Serial.println("-> Disposing waste...");
    servo1.write(LID_OPEN_ANGLE);  // Open lid/flap to drop waste
    delay(2000);                   // Wait for waste to drop (adjustable)
    servo1.write(LID_CLOSED_ANGLE); // Close lid/flap
    delay(1000);

    // 4. RESET STEPPER
    Serial.println("-> Resetting sorting mechanism.");
    // Stepper must return to its home (0) position before next cycle.
    // If it was CW (Dry), move CCW. If it was CCW (Wet), move CW.
    if (fsoil > MOISTURE_THRESHOLD) {
      stepper.moveDegreesCW(WET_WASTE_ANGLE); // Move back from Wet position
    } else {
      stepper.moveDegreesCCW(DRY_WASTE_ANGLE); // Move back from Dry position
    }
    delay(1000);
    Serial.println("--- Cycle Complete, Ready for next disposal ---");
  }

  delay(500);
}
