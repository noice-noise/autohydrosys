#include "esphome.h"
#include "DFRobot_ESP_PH.h"

// DALLAS
// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// END DALLAS

DFRobot_ESP_PH ph;
#define ESPADC 4096.0   //the esp Analog Digital Convertion value
#define ESPVOLTAGE 3300 //the esp voltage supply value
#define PH_PIN 36		//the esp gpio data pin number
float voltage, phValue, temperature; // = 25;

float readTemperature()
{
	//add your code here to get the temperature from your temperature sensor
	sensors.requestTemperatures();
	return sensors.getTempCByIndex(0);
}

class CustomDFRobotPhSensor : public PollingComponent, public Sensor, public CustomAPIDevice {
 public:
  // constructor
  CustomDFRobotPhSensor(uint32_t update_interval) : PollingComponent(update_interval) {}

  float get_setup_priority() const override { return esphome::setup_priority::IO; }

  void setup() override {
    // This will be called by App.setup()

    // services
    register_service(&CustomDFRobotPhSensor::enter_ph_calibration_mode, "enter_ph");
    register_service(&CustomDFRobotPhSensor::exit_ph_calibration_mode, "exit_ph");
    register_service(&CustomDFRobotPhSensor::calibrate_ph, "calibrate_ph");

  }
  void update() override {
		voltage = analogRead(PH_PIN) / ESPADC * ESPVOLTAGE; // read the voltage
    temperature = readTemperature();
    phValue = ph.readPH(voltage, temperature); // convert voltage to pH with temperature compensation
    // This will be called every "update_interval" milliseconds.
    publish_state(phValue);
  }
  void enter_ph_calibration_mode() {
    voltage = analogRead(PH_PIN) / ESPADC * ESPVOLTAGE; // read the voltage
    temperature = readTemperature();
    char ENTERPH[8] = "ENTERPH";
    ph.calibration(voltage, temperature, ENTERPH);
  }
  void exit_ph_calibration_mode() {
    voltage = analogRead(PH_PIN) / ESPADC * ESPVOLTAGE; // read the voltage
    temperature = readTemperature();
    char EXITPH[7] = "EXITPH";
    ph.calibration(voltage, temperature, EXITPH);
  }
  void calibrate_ph() {
    voltage = analogRead(PH_PIN) / ESPADC * ESPVOLTAGE; // read the voltage
    temperature = readTemperature();
    char CALPH[6] = "CALPH";
    ph.calibration(voltage, temperature, CALPH);
  }

};
