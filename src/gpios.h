#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>

#define GPIO_FAIL -1
#define GPIO_OK    0

#define ir_rate PWM_HZ  (4)
#define FADE_DELAY_MS   10
#define FADE_DELAY      K_MSEC(FADE_DELAY_MS)
#define PWM_PULSE_RATE  100000

// pwm led
#define PWM_NODE    DT_ALIAS(pwm_led0)
#if !DT_NODE_HAS_STATUS(PWM_NODE, okay)
#error "Unsupported board: pwm_led0 devicetree alias is not defined"
#endif
static const struct pwm_dt_spec pwm_led = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

// seven segment
#define DEC0_NODE DT_ALIAS(dec0)
#if !DT_NODE_HAS_STATUS(DEC0_NODE, okay)
#error "Unsupported format"
#endif
static const struct gpio_dt_spec dec_input0 = GPIO_DT_SPEC_GET(DEC0_NODE, gpios);

#define DEC1_NODE DT_ALIAS(dec1)
#if !DT_NODE_HAS_STATUS(DEC1_NODE, okay)
#error "Unsupported format"
#endif
static const struct gpio_dt_spec dec_input1 = GPIO_DT_SPEC_GET(DEC1_NODE, gpios);

#define DEC2_NODE DT_ALIAS(dec2)
#if !DT_NODE_HAS_STATUS(DEC2_NODE, okay)
#error "Unsupported format"
#endif
static const struct gpio_dt_spec dec_input2 = GPIO_DT_SPEC_GET(DEC2_NODE, gpios);

#define DEC3_NODE DT_ALIAS(dec3)
#if !DT_NODE_HAS_STATUS(DEC3_NODE, okay)
#error "Unsupported format"
#endif
static const struct gpio_dt_spec dec_input3 = GPIO_DT_SPEC_GET(DEC3_NODE, gpios);



// buttons
#define SW0_NODE	DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

#define SW1_NODE	DT_ALIAS(sw1)
#if !DT_NODE_HAS_STATUS(SW1_NODE, okay)
#error "Unsupported board: sw1 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET(SW1_NODE, gpios);

#define SW2_NODE	DT_ALIAS(sw2)
#if !DT_NODE_HAS_STATUS(SW2_NODE, okay)
#error "Unsupported board: sw2 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET(SW2_NODE, gpios);

// #define SW3_NODE	DT_ALIAS(sw3)
// #if !DT_NODE_HAS_STATUS(SW3_NODE, okay)
// #error "Unsupported board: sw3 devicetree alias is not defined"
// #endif
// static const struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET(SW3_NODE, gpios);

int pwm_init(void);
int decoder_init(void);
void seven_segment(int);