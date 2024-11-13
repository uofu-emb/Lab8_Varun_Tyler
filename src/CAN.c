#include <can2040.h>
#include <hardware/regs/intctrl.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>


static struct can2040 cbus;
QueueHandle_t msgs;


static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    printf("Message Received");
    xQueueSendToBack(msgs, msg, portMAX_DELAY);
    // Handle received CAN messages here
    //printf("Received message with ID: %X\n", msg->id);
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
    can2040_callback_config(&cbus, can2040_cb);

    // Enable IRQs
    irq_set_exclusive_handler(PIO0_IRQ_0, PIOx_IRQHandler);
    irq_set_priority(PIO0_IRQ_0, PICO_DEFAULT_IRQ_PRIORITY - 1);
    irq_set_enabled(PIO0_IRQ_0, 1);

    // Start CAN bus
    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

void transmit(__unused void *params)
{
    struct can2040_msg tx_msg = {
        .id = 0x01,   
        .dlc = 8,    
        .data = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
    };

    // Infinite loop to send message every second
    while (1) {
        if(can2040_transmit(&cbus, &tx_msg)){
            printf("Sent message with ID: %X\n", tx_msg.id);
        }
        else{
            printf("FATAL ERROR : Transmission Failed");
        }
        sleep_ms(1000);
    }
}

void receive(__unused void *params){
    struct can2040_msg data;
    xQueueReceive(msgs, &data, portMAX_DELAY);
    printf("Got message in receive function \n");
}

int main(void)
{
    stdio_init_all();
    msgs = xQueueCreate(100, sizeof(struct can2040_msg));
    canbus_setup();
    TaskHandle_t task;
    // xTaskCreate(transmit, "MainThread",
    //             configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, &task);
    xTaskCreate(receive, "MainThread",
                configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, &task);
    vTaskStartScheduler();

    return 0;
}