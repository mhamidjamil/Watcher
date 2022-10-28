#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;

static const int led_pin = LED_BUILTIN;
static const int led_pin2 = 16;
void toggleLED(void *parameter)
{
    while (1)
    {
        digitalWrite(led_pin, HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        digitalWrite(led_pin, LOW);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
void toggleLED2(void *parameter)
{
    while (1)
    {
        digitalWrite(led_pin2, HIGH);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        digitalWrite(led_pin2, LOW);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
void setup()
{
    pinMode(led_pin, OUTPUT);
    pinMode(led_pin2, OUTPUT);
    xTaskCreatePinnedToCore(toggleLED,
                            "Toggle LED",
                            1024,
                            NULL,
                            1,
                            &task_1,
                            app_cpu);
    xTaskCreatePinnedToCore(toggleLED2,
                            "Toggle LED",
                            1024,
                            NULL,
                            1,
                            &task_2,
                            app_cpu);
}
void loop() {}
