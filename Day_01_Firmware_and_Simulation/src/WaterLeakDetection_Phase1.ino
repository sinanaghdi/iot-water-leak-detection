/**
 * @file main.ino
 * @brief IoT Water Leak Detection System - Phase 1
 * @author Senior Embedded Systems Engineer
 * @date 2025-12-30
 * 
 * This firmware implements the physical layer and signal processing logic for an
 * IoT water leak detection system. It reads acoustic sensor data from an analog
 * input, applies a moving average filter to reduce noise, and transmits the filtered
 * data via UART serial communication in CSV format.
 * 
 * Hardware Target: Arduino UNO (ATmega328P)
 * Sensor Input: Analog Pin A0
 * Communication: UART Serial @ 9600 baud
 * 
 * Key Features:
 * - Moving Average Filter with 50-sample window size
 * - Timestamp generation using millis()
 * - CSV data format: "Timestamp_ms,Sensor_Value"
 * - Optimized memory usage with circular buffer implementation
 */

// Hardware pin definitions (using const for compile-time optimization)
const uint8_t ACOUSTIC_SENSOR_PIN = A0;    ///< Analog input pin for acoustic sensor
const uint32_t SERIAL_BAUD_RATE = 9600;    ///< UART communication baud rate
const uint16_t FILTER_WINDOW_SIZE = 50;    ///< Moving average filter window size
const uint8_t SERIAL_TRANSMIT_DELAY = 15;  ///< Delay between transmissions (ms)

// Moving Average Filter implementation using circular buffer
class MovingAverageFilter {
private:
  uint16_t buffer[FILTER_WINDOW_SIZE];  ///< Circular buffer to store samples
  uint16_t bufferIndex = 0;             ///< Current buffer index (circular pointer)
  uint32_t runningSum = 0;              ///< Running sum of all values in buffer
  bool isBufferFull = false;            ///< Flag to indicate when buffer is initially filled

public:
  /**
     * @brief Adds a new sample to the filter and returns the filtered average
     * @param newValue Raw analog reading to be filtered
     * @return Filtered average value (0-1023 range)
     * 
     * Implementation uses circular buffer technique for O(1) time complexity:
     * 1. Subtract oldest value from running sum
     * 2. Add new value to running sum
     * 3. Update buffer at current index
     * 4. Increment and wrap buffer index
     * 5. Calculate average using integer division
     * 
     * Note: Uses 32-bit integer for running sum to prevent overflow
     * (max possible sum: 50 * 1023 = 51,150 < 2^32)
     */
  uint16_t addSample(uint16_t newValue) {
    // Get the oldest value that will be replaced
    uint16_t oldestValue = buffer[bufferIndex];

    // Update running sum: subtract oldest, add newest
    runningSum = runningSum - oldestValue + newValue;

    // Store new value in buffer
    buffer[bufferIndex] = newValue;

    // Update buffer index with wrap-around
    bufferIndex = (bufferIndex + 1) % FILTER_WINDOW_SIZE;

    // Set buffer full flag after first complete cycle
    if (bufferIndex == 0) {
      isBufferFull = true;
    }

    // Calculate and return average
    uint16_t currentWindowSize = isBufferFull ? FILTER_WINDOW_SIZE : bufferIndex;
    return runningSum / currentWindowSize;
  }
};

// Global filter instance
MovingAverageFilter acousticFilter;

/**
 * @brief System initialization routine
 * 
 * Configures hardware peripherals:
 * - Serial communication at 9600 baud
 * - Analog reference voltage (default 5V)
 * - Initializes filter buffer with zeros
 */
void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);

  // Optional: Wait for serial connection to stabilize (for debugging)
  while (!Serial) {
    ;  // Wait for serial port to connect (needed for Leonardo/Micro only)
  }

  // Initialize filter buffer with zeros
  for (uint16_t i = 0; i < FILTER_WINDOW_SIZE; i++) {
    acousticFilter.addSample(0);
  }

  // Print header for CSV format
  Serial.println("Timestamp_ms,Sensor_Value");
}

/**
 * @brief Main application loop
 * 
 * Execution flow:
 * 1. Read raw analog value from sensor (0-1023)
 * 2. Apply moving average filter to reduce noise
 * 3. Get current timestamp using millis()
 * 4. Format and transmit data in CSV format
 * 5. Apply small delay to control sampling rate
 * 
 * Sampling rate: ~62.5 Hz (16ms effective period including processing time)
 * Output rate: Matches sampling rate (one filtered sample per loop iteration)
 */
void loop() {
  // Step 1: Read raw analog sensor value
  uint16_t rawSensorValue = analogRead(ACOUSTIC_SENSOR_PIN);

  // Step 2: Apply moving average filter
  uint16_t filteredValue = acousticFilter.addSample(rawSensorValue);

  // Step 3: Get current timestamp
  uint32_t timestamp = millis();

  // Step 4: Format and transmit data in CSV format
  Serial.print(timestamp);
  Serial.print(',');
  Serial.println(filteredValue);

  // Step 5: Control sampling rate
  delay(SERIAL_TRANSMIT_DELAY);
}