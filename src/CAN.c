#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <can2040.h>
#include <hardware/regs/intctrl.h>
#include <stdio.h>
#include <pico/stdlib.h>


// Macros for activity 0
#define TRANSMIT 0
#define RECEIVE 0
// Macros for activity 1
#define TRANSMIT_RECEIVE 1
#define HIGHER_PRIORITY_BOARD 0
#define LOWER_PRIORITY_BOARD 1
#define TICKS 2000
#define BUSY_WAIT_DELAY 1

static struct can2040 cbus;
static QueueHandle_t msg_received;
 
static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    //For activity 0, tranmit board doesn't need any callback function to it.
    if (RECEIVE || TRANSMIT_RECEIVE){
        xQueueSend(msg_received, &(msg->data32[1]), portMAX_DELAY);
    }
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


   // Setup canbus
   can2040_setup(&cbus, pio_num);
   can2040_callback_config(&cbus, can2040_cb);


   // Enable irqs
   irq_set_exclusive_handler(PIO0_IRQ_0, PIOx_IRQHandler);
   irq_set_priority(PIO0_IRQ_0, PICO_DEFAULT_IRQ_PRIORITY - 1);
   irq_set_enabled(PIO0_IRQ_0, 1);


   // Start canbus
   can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}


void receive_task(void* params){
   uint8_t data = 0;
   while(xQueueReceive(msg_received, &data, portMAX_DELAY))
   {
       printf("Data Received: %u \n", data);
   }
}


void transmit_task(void* params){
   uint32_t count = 0;
   struct can2040_msg msg;
   msg.id = 0;
   msg.dlc = 8; //spec of the size over the bus
   int status = 0;
   while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        if(LOWER_PRIORITY_BOARD && BUSY_WAIT_DELAY){
            vTaskDelay(pdMS_TO_TICKS(TICKS));
        }
        //Assigning 0 to avoid any garbage value
        msg.data32[0] = 0;
        //Sending some random value
        if(LOWER_PRIORITY_BOARD){
            msg.data32[1] = 2;
        }
        else{
            msg.data32[1] = 5;
        }
        if(can2040_transmit(&cbus, &msg) != 0){
            printf("TRANSMISSION FAILED");
        }
        else{
            printf("Transmit: %u\n", msg.data32[1]);
        }
    }
}



void activity_0(){
    //Use macros to flash one board for just transmitting and other board to just receive.
    if (TRANSMIT){
        xTaskCreate(transmit_task, "Transmit Task",
               configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, NULL);
    }
    
    if (RECEIVE){
        xTaskCreate(receive_task, "Receive Task",
                configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, NULL);
    }
    vTaskStartScheduler();
}

void activity_1(){
    //Use macros to select boards based on priority level
    if(HIGHER_PRIORITY_BOARD){
        xTaskCreate(transmit_task, "Higher prior", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 5UL, NULL);
    }
    
    if(LOWER_PRIORITY_BOARD){
        xTaskCreate(transmit_task, "Lower prior", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, NULL);
    }

    xTaskCreate(receive_task, "Receive Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, NULL);

    vTaskStartScheduler();
    
}


void main(){
    stdio_init_all();
    canbus_setup();
    if (RECEIVE || TRANSMIT_RECEIVE){
        msg_received = xQueueCreate(20, sizeof(uint32_t));
    }
    if(!TRANSMIT_RECEIVE){
        activity_0();
    }
    if(TRANSMIT_RECEIVE){
        activity_1();
    }
}