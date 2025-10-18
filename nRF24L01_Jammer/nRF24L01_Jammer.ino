/*
 * Project: 2.4GHz Channel Sweeper using nRF24L01
 * Hardware: Arduino Uno + nRF24L01
 * 
 * Author: SecureTechware
 * Website: https://securetechware.com
 * 
 * Description:
 * This sketch initializes the nRF24L01 radio module and transmits a 
 * continuous carrier signal while cycling through all 2.4GHz channels.
 * It also covers BLE advertisement channels for comprehensive analysis.
 * 
 * Use Case:
 * - Controlled RF environment testing
 * - Interference or channel activity verification
 * - RF laboratory experiments
 * 
 * Dependencies:
 *   - RF24 Library (by TMRh20): https://github.com/tmrh20/RF24/
 */

#include <RF24.h>

// Initialize the nRF24L01 radio object with CE and CSN pins
RF24 radio(9, 10);

// Starting channel for transmission (default mid-band)
byte startChannel = 45;

// Reserved flag (currently unused, kept for future direction logic)
unsigned int directionFlag = 0;

/*
 * Function: sweepChannelsRecursive
 * --------------------------------
 * Recursively cycles through all 79 standard 2.4GHz channels.
 * Each call sets the nRF24L01 module to the next frequency channel.
 *
 * Parameters:
 *   ch - Current channel index (0–78)
 */
void sweepChannelsRecursive(int ch) {
  if (ch >= 79) return;              // Stop once all 79 channels are covered
  radio.setChannel(ch);              // Set current channel
  sweepChannelsRecursive(ch + 1);    // Recursively move to next channel
}

/*
 * Function: sweepBleRecursive
 * ----------------------------
 * Recursively cycles through BLE advertising channels (37, 38, 39).
 * These channels are critical for Bluetooth signal verification.
 *
 * Parameters:
 *   arr - Pointer to array containing BLE channels
 *   idx - Current index in the BLE array
 *   len - Total number of BLE channels
 */
void sweepBleRecursive(const byte *arr, int idx, int len) {
  if (idx >= len) return;            // Stop once all BLE channels are done
  radio.setChannel(arr[idx]);        // Set BLE channel
  sweepBleRecursive(arr, idx + 1, len);  // Move to next BLE channel
}

/*
 * Function: setup
 * ----------------
 * Initializes serial communication and configures the nRF24L01 radio.
 * Sets transmission parameters and starts a continuous carrier signal.
 */
void setup() {
  Serial.begin(115200);              // Begin serial communication for logs
  Serial.println("Initializing nRF24L01 module...");

  // Attempt to initialize radio module
  if (radio.begin()) {
    delay(500);  // Short delay for hardware stabilization
    Serial.println("nRF24L01 Module Working!");

    // Radio configuration
    radio.setAutoAck(false);             // Disable auto acknowledgment
    radio.stopListening();               // Switch to transmit mode
    radio.setRetries(0, 0);              // Disable retransmission
    radio.setPayloadSize(5);             // Minimal payload size
    radio.setAddressWidth(3);            // 3-byte address width
    radio.setPALevel(RF24_PA_MAX, true); // Set max power with high current
    radio.setDataRate(RF24_2MBPS);       // Highest data rate
    radio.setCRCLength(RF24_CRC_DISABLED); // Disable CRC for raw output

    // Begin transmitting a continuous carrier signal
    radio.startConstCarrier(RF24_PA_MAX, startChannel);
    Serial.println("Continuous carrier started on channel 45...");
  } else {
    // Error message if module initialization fails
    Serial.println("Error: Unable to initialize nRF24L01 module!");
  }
}

/*
 * Function: loop
 * ---------------
 * Main execution loop.
 * Continuously sweeps through all 2.4GHz and BLE channels
 * using recursive functions for smooth transitions.
 */
void loop() {
  // Sweep all standard 2.4GHz ISM channels
  sweepChannelsRecursive(0);

  // Define BLE advertisement channels
  byte bleChannels[] = {37, 38, 39};

  // Sweep BLE channels
  sweepBleRecursive(bleChannels, 0, 3);
}
