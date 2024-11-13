#include <can2040.h>
#include <hardware/regs/intctrl.h>
#include <stdio.h>
#include <pico/stdlib.h>

static struct can2040 cbus;

// Callback function for handling received CAN messages
static void can_receive_callback(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    printf("Received message:\n");
    printf("ID: 0x%X\n", msg->id);
    // printf("Data: ");
    // for (int i = 0; i < msg->dlc; i++) {
    //     printf("0x%02X ", msg->data[i]);
    // }
    // printf("\n");
}

static void PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}

void canbus_setup(void)
{
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000, bitrate = 500000;
    uint32_t gpio_rx = 4, gpio_tx = 5;

    // Setup CAN bus
    can2040_setup(&cbus, pio_num);

    // Register the receive callback
    can2040_callback_config(&cbus, can_receive_callback);

    // Enable IRQs
    irq_set_exclusive_handler(PIO0_IRQ_0, PIOx_IRQHandler);
    irq_set_priority(PIO0_IRQ_0, PICO_DEFAULT_IRQ_PRIORITY - 1);
    irq_set_enabled(PIO0_IRQ_0, 1);

    // Start CAN bus
    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

int main(void)
{
    stdio_init_all();
    canbus_setup();

    while (1) {
        printf("waiting\n");
        //tight_loop_contents(); // Keeps the main loop running without busy-waiting
    }

    return 0;
}