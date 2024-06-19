#include "gpios.h"

int current_music = 0;
static struct gpio_callback button0_cb_data;
static struct gpio_callback button1_cb_data;
static struct gpio_callback button2_cb_data;
static struct gpio_callback button3_cb_data;
// static struct gpio_callback button3_cb_data;

#define GPIO_P0_BASE 0x50000000
#define GPIO_OUT_REG_OFFSET 0x0504

#define REST   0.0
#define NOTE_C1		32.7032 
#define NOTE_CS1	34.6478 
#define NOTE_D1		36.7081 
#define NOTE_DS1	38.8909 
#define NOTE_E1		41.2034 
#define NOTE_F1		43.6535 
#define NOTE_FS1	46.2493
#define NOTE_G1		48.9994
#define NOTE_GS1	51.9130
#define NOTE_A1		55.0000
#define NOTE_AS1	58.2705
#define NOTE_B1		61.7354

#define NOTE_C2		(NOTE_C1*2)
#define NOTE_CS2	(NOTE_CS1*2)
#define NOTE_D2		(NOTE_D1*2)
#define NOTE_DS2	(NOTE_DS1*2)
#define NOTE_E2		(NOTE_E1*2)
#define NOTE_F2		(NOTE_F1*2)
#define NOTE_FS2	(NOTE_FS1*2)
#define NOTE_G2		(NOTE_G1*2)
#define NOTE_GS2	(NOTE_GS1*2)
#define NOTE_A2		(NOTE_A1*2)
#define NOTE_AS2	(NOTE_AS1*2)
#define NOTE_B2		(NOTE_B1*2)

#define NOTE_C3		(NOTE_C2*2)
#define NOTE_CS3	(NOTE_CS2*2)
#define NOTE_D3		(NOTE_D2*2)
#define NOTE_DS3	(NOTE_DS2*2)
#define NOTE_E3		(NOTE_E2*2)
#define NOTE_F3		(NOTE_F2*2)
#define NOTE_FS3	(NOTE_FS2*2)
#define NOTE_G3		(NOTE_G2*2)
#define NOTE_GS3	(NOTE_GS2*2)
#define NOTE_A3		(NOTE_A2*2)
#define NOTE_AS3	(NOTE_AS2*2)
#define NOTE_B3		(NOTE_B2*2)

#define NOTE_C4		(NOTE_C3*2)
#define NOTE_CS4	(NOTE_CS3*2)
#define NOTE_D4		(NOTE_D3*2)
#define NOTE_DS4	(NOTE_DS3*2)
#define NOTE_E4		(NOTE_E3*2)
#define NOTE_F4		(NOTE_F3*2)
#define NOTE_FS4	(NOTE_FS3*2)
#define NOTE_G4		(NOTE_G3*2)
#define NOTE_GS4	(NOTE_GS3*2)
#define NOTE_A4		(NOTE_A3*2)
#define NOTE_AS4	(NOTE_AS3*2)
#define NOTE_B4		(NOTE_B3*2)

#define NOTE_C5		(NOTE_C4*2)
#define NOTE_CS5	(NOTE_CS4*2)
#define NOTE_D5		(NOTE_D4*2)
#define NOTE_DS5	(NOTE_DS4*2)
#define NOTE_E5		(NOTE_E4*2)
#define NOTE_F5		(NOTE_F4*2)
#define NOTE_FS5	(NOTE_FS4*2)
#define NOTE_G5		(NOTE_G4*2)
#define NOTE_GS5	(NOTE_GS4*2)
#define NOTE_A5		(NOTE_A4*2)
#define NOTE_AS5	(NOTE_AS4*2)
#define NOTE_B5		(NOTE_B4*2)

#define NOTE_C6		(NOTE_C5*2)
#define NOTE_CS6	(NOTE_CS5*2)
#define NOTE_D6		(NOTE_D5*2)
#define NOTE_DS6	(NOTE_DS5*2)
#define NOTE_E6		(NOTE_E5*2)
#define NOTE_F6		(NOTE_F5*2)
#define NOTE_FS6	(NOTE_FS5*2)
#define NOTE_G6		(NOTE_G5*2)
#define NOTE_GS6	(NOTE_GS5*2)
#define NOTE_A6		(NOTE_A5*2)
#define NOTE_AS6	(NOTE_AS5*2)
#define NOTE_B6		(NOTE_B5*2)

#define NOTE_C7		(NOTE_C6*2)
#define NOTE_CS7	(NOTE_CS6*2)
#define NOTE_D7		(NOTE_D6*2)
#define NOTE_DS7	(NOTE_DS6*2)
#define NOTE_E7		(NOTE_E6*2)
#define NOTE_F7		(NOTE_F6*2)
#define NOTE_FS7	(NOTE_FS6*2)
#define NOTE_G7		(NOTE_G6*2)
#define NOTE_GS7	(NOTE_GS6*2)
#define NOTE_A7		(NOTE_A6*2)
#define NOTE_AS7	(NOTE_AS6*2)
#define NOTE_B7		(NOTE_B6*2)


int mario[] = {

  // Super Mario Bros theme
  // Score available at https://musescore.com/user/2123/scores/2145
  // Theme by Koji Kondo
  
  
  NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,8, //1
  NOTE_G5,4, REST,4, NOTE_G4,8, REST,4, 
  NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // 3
  NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
  NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
  REST,8, NOTE_E5,4,NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,
  NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // repeats from 3
  NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
  NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
  REST,8, NOTE_E5,4,NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,

  
  REST,4, NOTE_G5,8, NOTE_FS5,8, NOTE_F5,8, NOTE_DS5,4, NOTE_E5,8,//7
  REST,8, NOTE_GS4,8, NOTE_A4,8, NOTE_C4,8, REST,8, NOTE_A4,8, NOTE_C5,8, NOTE_D5,8,
  REST,4, NOTE_DS5,4, REST,8, NOTE_D5,-4,
  NOTE_C5,2, REST,2,

  REST,4, NOTE_G5,8, NOTE_FS5,8, NOTE_F5,8, NOTE_DS5,4, NOTE_E5,8,//repeats from 7
  REST,8, NOTE_GS4,8, NOTE_A4,8, NOTE_C4,8, REST,8, NOTE_A4,8, NOTE_C5,8, NOTE_D5,8,
  REST,4, NOTE_DS5,4, REST,8, NOTE_D5,-4,
  NOTE_C5,2, REST,2,

  NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,4,//11
  NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2,

  NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,8, NOTE_E5,8,//13
  REST,1, 
  NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,4,
  NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2,
  NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,4,
  NOTE_G5,4, REST,4, NOTE_G4,4, REST,4, 
  NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // 19
  
  NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
  NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
  REST,8, NOTE_E5,4, NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,

  NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // repeats from 19
  NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
  NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
  REST,8, NOTE_E5,4, NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,

  NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,//23
  NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
  NOTE_D5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_G5,-8, NOTE_F5,-8,
  
  NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2, //26
  NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,
  NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
  NOTE_B4,8, NOTE_F5,4, NOTE_F5,8, NOTE_F5,-8, NOTE_E5,-8, NOTE_D5,-8,
  NOTE_C5,8, NOTE_E4,4, NOTE_E4,8, NOTE_C4,2,

  NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,//repeats from 23
  NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
  NOTE_D5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_G5,-8, NOTE_F5,-8,
  
  NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2, //26
  NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,
  NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
  NOTE_B4,8, NOTE_F5,4, NOTE_F5,8, NOTE_F5,-8, NOTE_E5,-8, NOTE_D5,-8,
  NOTE_C5,8, NOTE_E4,4, NOTE_E4,8, NOTE_C4,2,
  NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,8, NOTE_E5,8,
  REST,1,

  NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,4, //33
  NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2,
  NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,4,
  NOTE_G5,4, REST,4, NOTE_G4,4, REST,4, 
  NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,
  NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
  NOTE_D5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_G5,-8, NOTE_F5,-8,
  
  NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2, //40
  NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,
  NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
  NOTE_B4,8, NOTE_F5,4, NOTE_F5,8, NOTE_F5,-8, NOTE_E5,-8, NOTE_D5,-8,
  NOTE_C5,8, NOTE_E4,4, NOTE_E4,8, NOTE_C4,2,
  
  //game over sound
  NOTE_C5,-4, NOTE_G4,-4, NOTE_E4,4, //45
  NOTE_A4,-8, NOTE_B4,-8, NOTE_A4,-8, NOTE_GS4,-8, NOTE_AS4,-8, NOTE_GS4,-8,
  NOTE_G4,8, NOTE_D4,8, NOTE_E4,-2,  

};

int harry[] = {


  // Hedwig's theme fromn the Harry Potter Movies
  // Socre from https://musescore.com/user/3811306/scores/4906610
  
  REST, 2, NOTE_D4, 4,
  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_G4, 2, NOTE_D5, 4,
  NOTE_C5, -2, 
  NOTE_A4, -2,
  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_F4, 2, NOTE_GS4, 4,
  NOTE_D4, -1, 
  NOTE_D4, 4,

  NOTE_G4, -4, NOTE_AS4, 8, NOTE_A4, 4, //10
  NOTE_G4, 2, NOTE_D5, 4,
  NOTE_F5, 2, NOTE_E5, 4,
  NOTE_DS5, 2, NOTE_B4, 4,
  NOTE_DS5, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_B4, 4,
  NOTE_G4, -1,
  NOTE_AS4, 4,
     
  NOTE_D5, 2, NOTE_AS4, 4,//18
  NOTE_D5, 2, NOTE_AS4, 4,
  NOTE_DS5, 2, NOTE_D5, 4,
  NOTE_CS5, 2, NOTE_A4, 4,
  NOTE_AS4, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_D4, 4,
  NOTE_D5, -1, 
  REST,4, NOTE_AS4,4,  

  NOTE_D5, 2, NOTE_AS4, 4,//26
  NOTE_D5, 2, NOTE_AS4, 4,
  NOTE_F5, 2, NOTE_E5, 4,
  NOTE_DS5, 2, NOTE_B4, 4,
  NOTE_DS5, -4, NOTE_D5, 8, NOTE_CS5, 4,
  NOTE_CS4, 2, NOTE_AS4, 4,
  NOTE_G4, -1, 
  
};

int tetris[] = {

  //Based on the arrangement at https://www.flutetunes.com/tunes.php?id=192
  
  NOTE_E5, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_C5,8,  NOTE_B4,8,
  NOTE_A4, 4,  NOTE_A4,8,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, -4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,8,  NOTE_A4,4,  NOTE_B4,8,  NOTE_C5,8,

  NOTE_D5, -4,  NOTE_F5,8,  NOTE_A5,4,  NOTE_G5,8,  NOTE_F5,8,
  NOTE_E5, -4,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST, 4,

  NOTE_E5, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_C5,8,  NOTE_B4,8,
  NOTE_A4, 4,  NOTE_A4,8,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, -4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,8,  NOTE_A4,4,  NOTE_B4,8,  NOTE_C5,8,

  NOTE_D5, -4,  NOTE_F5,8,  NOTE_A5,4,  NOTE_G5,8,  NOTE_F5,8,
  NOTE_E5, -4,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST, 4,
  

  NOTE_E5,2,  NOTE_C5,2,
  NOTE_D5,2,   NOTE_B4,2,
  NOTE_C5,2,   NOTE_A4,2,
  NOTE_GS4,2,  NOTE_B4,4,  REST,8, 
  NOTE_E5,2,   NOTE_C5,2,
  NOTE_D5,2,   NOTE_B4,2,
  NOTE_C5,4,   NOTE_E5,4,  NOTE_A5,2,
  NOTE_GS5,2,

};

int panther[] = {

  // Pink Panther theme
  // Score available at https://musescore.com/benedictsong/the-pink-panther
  // Theme by Masato Nakamura, arranged by Teddy Mason

  REST,2, REST,4, REST,8, NOTE_DS4,8, 
  NOTE_E4,-4, REST,8, NOTE_FS4,8, NOTE_G4,-4, REST,8, NOTE_DS4,8,
  NOTE_E4,-8, NOTE_FS4,8,  NOTE_G4,-8, NOTE_C5,8, NOTE_B4,-8, NOTE_E4,8, NOTE_G4,-8, NOTE_B4,8,   
  NOTE_AS4,2, NOTE_A4,-16, NOTE_G4,-16, NOTE_E4,-16, NOTE_D4,-16, 
  NOTE_E4,2, REST,4, REST,8, NOTE_DS4,4,

  NOTE_E4,-4, REST,8, NOTE_FS4,8, NOTE_G4,-4, REST,8, NOTE_DS4,8,
  NOTE_E4,-8, NOTE_FS4,8,  NOTE_G4,-8, NOTE_C5,8, NOTE_B4,-8, NOTE_G4,8, NOTE_B4,-8, NOTE_E5,8,
  NOTE_DS5,1,   
  NOTE_D5,2, REST,4, REST,8, NOTE_DS4,8, 
  NOTE_E4,-4, REST,8, NOTE_FS4,8, NOTE_G4,-4, REST,8, NOTE_DS4,8,
  NOTE_E4,-8, NOTE_FS4,8,  NOTE_G4,-8, NOTE_C5,8, NOTE_B4,-8, NOTE_E4,8, NOTE_G4,-8, NOTE_B4,8,   
  
  NOTE_AS4,2, NOTE_A4,-16, NOTE_G4,-16, NOTE_E4,-16, NOTE_D4,-16, 
  NOTE_E4,-4, REST,4,
  REST,4, NOTE_E5,-8, NOTE_D5,8, NOTE_B4,-8, NOTE_A4,8, NOTE_G4,-8, NOTE_E4,-8,
  NOTE_AS4,16, NOTE_A4,-8, NOTE_AS4,16, NOTE_A4,-8, NOTE_AS4,16, NOTE_A4,-8, NOTE_AS4,16, NOTE_A4,-8,   
  NOTE_G4,-16, NOTE_E4,-16, NOTE_D4,-16, NOTE_E4,16, NOTE_E4,16, NOTE_E4,2,
 
};

int* melody[] = {
    NULL,
    NULL,
    mario,
    harry,
    tetris, 
    panther
};

int melody_len[] = {
    0,
    0,
    sizeof(mario),
    sizeof(harry),
    sizeof(tetris),
    sizeof(panther)
};

extern int mode;
extern int pause_flag;
extern int mute_flag;
extern int change_note_flag;

static uint32_t now_pwm_pulse = 1500000;
void button0_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    printk("Button 0 pressed\n");
    if(mode == PLAY) mode = SCORE; 
    else if(mode == SCORE) {
        mode = PLAY;
        pause_flag = 0;
    }
}

void button1_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    printk("Button 1 pressed\n");
    pause_flag = !pause_flag;

}

void button2_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    printk("Button 2 pressed\n");
    mute_flag = !mute_flag;

    return;
}

void button3_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    printk("Button 3 pressed\n");
    change_note_flag = 1;
}

int decoder_init(void){
    int err;
    err = gpio_is_ready_dt(&dec_input0);
    if (!err) {
            printk("Error gpio_is_ready_dt dec_input0 pin %d\n", err);
            return GPIO_FAIL;
    }
    err = gpio_pin_configure_dt(&dec_input0, GPIO_OUTPUT_ACTIVE | DT_GPIO_FLAGS(DEC0_NODE, gpios));
    if (err < 0) {
            printk("Error configuring dec_input0 pin %d\n", err);
            return GPIO_FAIL;
    }
    err = gpio_is_ready_dt(&dec_input1);
    if (!err) {
            printk("Error gpio_is_ready_dt dec_input1 pin %d\n", err);
            return GPIO_FAIL;
    }
    err = gpio_pin_configure_dt(&dec_input1, GPIO_OUTPUT_ACTIVE | DT_GPIO_FLAGS(DEC1_NODE, gpios));
    if (err < 0) {
            printk("Error configuring dec_input1 pin %d\n", err);
            return GPIO_FAIL;
    } 
    err = gpio_is_ready_dt(&dec_input2);
    if (!err) {
            printk("Error gpio_is_ready_dt dec_input2 pin %d\n", err);
            return GPIO_FAIL;
    }
    err = gpio_pin_configure_dt(&dec_input2, GPIO_OUTPUT_ACTIVE | DT_GPIO_FLAGS(DEC2_NODE, gpios));
    if (err < 0) {
            printk("Error configuring dec_input2 pin %d\n", err);
            return GPIO_FAIL;
    } 
    return 0;
}

void seven_segment(int num){
    if(num > 7) return;
    struct gpio_dt_spec inputs[] = {dec_input0, dec_input1, dec_input2};
    for(int i=0; i<3; i++){
        if(num & (1<<i)){
            gpio_pin_set_dt(&inputs[i], 0);
        }
        else{
            gpio_pin_set_dt(&inputs[i], 1);
        }

        
    }
}

int pwm_init(void) {
    int err = GPIO_FAIL;

    // Set butto0 interrupt
    printk("Setting button0 interrupt\n");
    err = gpio_is_ready_dt(&button0);
    if (!err) {
        printk("Error gpio_is_ready_dt led0 pin %d\n", err);
        return GPIO_FAIL;
    }
    err = gpio_pin_configure_dt(&button0, GPIO_INPUT | GPIO_PULL_UP);
    if (err < 0) {
        printk("Error configuring button0 pin %d\n", err);
        return GPIO_FAIL;
    }
    err = gpio_pin_interrupt_configure_dt(&button0, GPIO_INT_EDGE_TO_ACTIVE);
    if (err != 0) {
        printk("Error configuring interrupt on button0 pin %d\n", err);
        return GPIO_FAIL;
    }
    gpio_init_callback(&button0_cb_data, button0_callback, BIT(button0.pin));
    gpio_add_callback(button0.port, &button0_cb_data);

    // Set button1 interrupt
    printk("Setting button1 interrupt\n");
    err = gpio_is_ready_dt(&button1);
    if (!err) {
        printk("Error gpio_is_ready_dt led1 pin %d\n", err);
        return GPIO_FAIL;
    }
    err = gpio_pin_configure_dt(&button1, GPIO_INPUT | GPIO_PULL_UP);
    if (err < 0) {
        printk("Error configuring button1 pin %d\n", err);
        return GPIO_FAIL;
    }
    err = gpio_pin_interrupt_configure_dt(&button1, GPIO_INT_EDGE_TO_ACTIVE);
    if (err != 0) {
        printk("Error configuring interrupt on button1 pin %d\n", err);
        return GPIO_FAIL;
    }
    gpio_init_callback(&button1_cb_data, button1_callback, BIT(button1.pin));
    gpio_add_callback(button1.port, &button1_cb_data);

    // Set button2 interrupt
    printk("Setting button2 interrupt\n");
    err = gpio_is_ready_dt(&button2);
    if (!err) {
        printk("Error gpio_is_ready_dt led2 pin %d\n", err);
        return GPIO_FAIL;
    }
    err = gpio_pin_configure_dt(&button2, GPIO_INPUT | GPIO_PULL_UP);
    if (err < 0) {
        printk("Error configuring button2 pin %d\n", err);
        return GPIO_FAIL;
    }
    err = gpio_pin_interrupt_configure_dt(&button2, GPIO_INT_EDGE_TO_ACTIVE);
    if (err != 0) {
        printk("Error configuring interrupt on button2 pin %d\n", err);
        return GPIO_FAIL;
    }
    gpio_init_callback(&button2_cb_data, button2_callback, BIT(button2.pin));
    gpio_add_callback(button2.port, &button2_cb_data);

    // Set button3 interrupt
    printk("Setting button3 interrupt\n");
    err = gpio_is_ready_dt(&button3);
    if (!err) {
        printk("Error gpio_is_ready_dt led3 pin %d\n", err);
        return GPIO_FAIL;
    }
    err = gpio_pin_configure_dt(&button3, GPIO_INPUT | GPIO_PULL_UP);
    if (err < 0) {
        printk("Error configuring button3 pin %d\n", err);
        return GPIO_FAIL;
    }
    err = gpio_pin_interrupt_configure_dt(&button3, GPIO_INT_EDGE_TO_ACTIVE);
    if (err != 0) {
        printk("Error configuring interrupt on button3 pin %d\n", err);
        return GPIO_FAIL;
    }
    gpio_init_callback(&button3_cb_data, button3_callback, BIT(button3.pin));
    gpio_add_callback(button3.port, &button3_cb_data);

    // pwm_led0
    err = gpio_is_ready_dt(&pwm_led);
    if (!err) {
        printk("Error gpio_is_ready_dt pwm_led pin %d\n", err);
        return GPIO_FAIL;
    }
    pwm_set_dt(&pwm_led, 0, 0);
    return GPIO_OK;
}