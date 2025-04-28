#include <SPI.h>
#include <LoRa.h>
#include <micro_ros_arduino.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>

// LoRa pin configuration
#define LORA_SS   5    
#define LORA_RST  6   
#define LORA_DIO0 4    

// Baud rates
#define LED_PIN 13
#define SERIAL_BAUD 115200
#define ROS_BAUD 57600

// ROS entities
rcl_publisher_t pub_ens160_tvoc;
rcl_publisher_t pub_ens160_co2;
rcl_publisher_t pub_ccs811_tvoc;
rcl_publisher_t pub_ccs811_co2;

std_msgs__msg__Int32 ens160_tvoc_msg;
std_msgs__msg__Int32 ens160_co2_msg;
std_msgs__msg__Int32 ccs811_tvoc_msg;
std_msgs__msg__Int32 ccs811_co2_msg;

rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

// Error handling
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();} }
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){} }

void error_loop() {
    while(1) {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        delay(100);
    }
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(SERIAL_BAUD);
    Serial1.begin(ROS_BAUD);
    set_microros_transports();

    // Initialize LoRa
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(915E6)) {
        error_loop();
    }

    delay(2000);

    // Initialize micro-ROS
    allocator = rcl_get_default_allocator();
    RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
    RCCHECK(rclc_node_init_default(&node, "lora_node", "", &support));

    // Create publishers
    RCCHECK(rclc_publisher_init_default(
        &pub_ens160_tvoc,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "ens160_tvoc"));

    RCCHECK(rclc_publisher_init_default(
        &pub_ens160_co2,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "ens160_co2"));

    RCCHECK(rclc_publisher_init_default(
        &pub_ccs811_tvoc,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "ccs811_tvoc"));

    RCCHECK(rclc_publisher_init_default(
        &pub_ccs811_co2,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "ccs811_co2"));
}

void loop() {
    int packetSize = LoRa.parsePacket();

    if (packetSize > 0) {
        String data = "";
        while (LoRa.available()) {
            data += (char)LoRa.read();
        }

        // Parse received data
        int ens160_tvoc = 0, ens160_co2 = 0, ccs811_tvoc = 0, ccs811_co2 = 0;

        int ens_idx = data.indexOf("ENS160:");
        int ccs_idx = data.indexOf(";CCS811:");

        if (ens_idx != -1 && ccs_idx != -1) {
            String ens_data = data.substring(ens_idx + 7, ccs_idx);
            String ccs_data = data.substring(ccs_idx + 8);

            int comma_idx_ens = ens_data.indexOf(",");
            int comma_idx_ccs = ccs_data.indexOf(",");

            if (comma_idx_ens != -1 && comma_idx_ccs != -1) {
                ens160_tvoc = ens_data.substring(0, comma_idx_ens).toInt();
                ens160_co2 = ens_data.substring(comma_idx_ens + 1).toInt();
                ccs811_tvoc = ccs_data.substring(0, comma_idx_ccs).toInt();
                ccs811_co2 = ccs_data.substring(comma_idx_ccs + 1).toInt();
            }
        }

        // Publish each value
        ens160_tvoc_msg.data = ens160_tvoc;
        ens160_co2_msg.data = ens160_co2;
        ccs811_tvoc_msg.data = ccs811_tvoc;
        ccs811_co2_msg.data = ccs811_co2;

        RCSOFTCHECK(rcl_publish(&pub_ens160_tvoc, &ens160_tvoc_msg, NULL));
        RCSOFTCHECK(rcl_publish(&pub_ens160_co2, &ens160_co2_msg, NULL));
        RCSOFTCHECK(rcl_publish(&pub_ccs811_tvoc, &ccs811_tvoc_msg, NULL));
        RCSOFTCHECK(rcl_publish(&pub_ccs811_co2, &ccs811_co2_msg, NULL));
    }

    delay(10);
}
