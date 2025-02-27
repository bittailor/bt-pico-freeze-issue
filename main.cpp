#include <stdio.h>

#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>
#include <pico/btstack_cyw43.h>
#include <pico/async_context_poll.h>

#include <btstack.h>
#include <hci_dump_embedded_stdout.h>

#include "peripheral.h"

#define BT_LOG(format, ...)\
{  \
    BtLoggingTimestamp timestamp = btLoggingTimestamp();\
    printf("[%02u:%02u:%02u.%03u] " format, timestamp.hours, timestamp.minutes, timestamp.seconds, timestamp.ms, ##__VA_ARGS__);\
}  \

namespace {
    constexpr uint8_t sAdvertisementData[] = {
        // Flags general discoverable, BR/EDR not supported
        2, BLUETOOTH_DATA_TYPE_FLAGS, 0x06, 
        // Name
        9, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'B', 't','P', 'i', 'c', 'o', '-', 'F', 
        8, BLUETOOTH_DATA_TYPE_SERVICE_DATA, 0x1A, 0x18, 0x12, 0x34, 0x56, 0x78, 0x9A 
    };
    static_assert(sizeof(sAdvertisementData) <= 31, "Advertisement data too long");

    btstack_packet_callback_registration_t sHciEventCallbackRegistration;
}

typedef struct {
    uint32_t ms;
    uint32_t seconds;
    uint32_t minutes;
    uint32_t hours;
} BtLoggingTimestamp;

BtLoggingTimestamp btLoggingTimestamp(){
    uint32_t now = to_ms_since_boot(get_absolute_time());;
    uint32_t      seconds = now / 1000u;
    uint32_t      minutes = seconds / 60;
    uint32_t hours = minutes / 60;

    BtLoggingTimestamp timestamp = {
        .ms = (now - (seconds * 1000u)),
        .seconds = (seconds - (minutes * 60)),
        .minutes = (minutes - (hours * 60u)),
        .hours = hours
    };
    return timestamp;
}

void setupAdvertisements(){
    uint16_t advIntMin = 0x30;
    uint16_t advIntMax = 0x30;
    uint8_t advType = 0;
    bd_addr_t nullAddr{0};
    gap_advertisements_set_params(advIntMin, advIntMax, advType, 0, nullAddr, 0x07, 0x00);
    gap_advertisements_set_data(sizeof(sAdvertisementData), const_cast<uint8_t*>(sAdvertisementData));
    gap_advertisements_enable(1);
    BT_LOG("advertisement of length %d enabled.\n", sizeof(sAdvertisementData));
}

void packetHandler(uint8_t packetType, uint16_t channel, uint8_t *packet, uint16_t size) {
    if (packetType != HCI_EVENT_PACKET) return;
    uint8_t eventType = hci_event_packet_get_type(packet);
    switch(eventType){
        case BTSTACK_EVENT_STATE: {
            if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) return;
            bd_addr_t localAddress;
            gap_local_bd_addr(localAddress);
            BT_LOG("BTstack up and running on %s.\n", bd_addr_to_str(localAddress));
            setupAdvertisements();
        }break;
    }
}


int main()
{
    stdio_init_all();
    
    BT_LOG("STARTUP ...\n");
    BT_LOG("Versions: PICO_SDK = %s, BTSTACK = %s\n", PICO_SDK_VERSION_STRING, BTSTACK_VERSION_STRING);
    if (auto error = cyw43_arch_init()) {
        BT_LOG("cyw43_arch_init failed with %d\n", error);
        return -1;
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    l2cap_init();
    sm_init();
    sHciEventCallbackRegistration.callback = &packetHandler;
    hci_add_event_handler(&sHciEventCallbackRegistration);
    att_server_init(profile_data, nullptr, nullptr);
    
    if (auto error = hci_power_control(HCI_POWER_ON)) {
        BT_LOG("hci_power_control(HCI_POWER_ON) failed with %d\n", error);
        return -1;
    }
    
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    BT_LOG("... STARTUP done\n");

    int counter = 0;
    while (true) {
        BT_LOG("Loop %d \n", counter++);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        BT_LOG("sleep 100 ...\n");
        sleep_ms(100);
        BT_LOG(" ... sleep 100 done\n");
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        BT_LOG("sleep 900 ...\n");
        sleep_ms(900);   
        BT_LOG(" ... sleep 900 done\n");   
    }
}
