/*
 * MIT License
 *
 * Copyright (c) 2024 Garyun Kim, Jinju Lee
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <zephyr/kernel.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include "gpios.h"
#include "led.h"

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

// add for joystick
int32_t preX = 0 , perY = 0;
static const int ADC_MAX = 1023;
// static const int ADC_MIN = 0;
static const int AXIS_DEVIATION = ADC_MAX / 2;
int32_t nowX = 0;
int32_t nowY = 0;

//TODO sholud be replaced with Joystick event!
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
//TODO ==============================================

// thread stack size
#define STACK_SIZE 1024

// thread priority
#define PRIORITY 7

// thread stack memory allocation
K_THREAD_STACK_DEFINE(thread_stack_area, STACK_SIZE);
struct k_thread my_thread_data;

K_THREAD_STACK_DEFINE(thread_stack_area2, STACK_SIZE);
struct k_thread my_thread_data2;
//map size setted by display size
#define MAP_WIDTH 16
#define MAP_HEIGHT 8
//for map state
// #define EMPTY 0
#define SNAKE 1
#define APPLE 2
//for game mode

//initial values
#define INIT_LENGTH 3
#define INIT_SPEED 200
//for BGM
#define NOTE_CNT 3

int map[MAP_HEIGHT*MAP_WIDTH]; //whole map state. 0: empty / 1: snake / 2: apple
int x[128], y[128]; //save the position of snake
int apple_x, apple_y; //save the position of apple
int length = INIT_LENGTH; //current length of snake. can be changed by scoring
int speed = INIT_SPEED; //current speed of game. can be changed by rotary encoder event
extern int speed_delta;
int dir = LEFT; //current direction of snake. can be changed by joystick event

//for game logic
int idx(int x, int y);
void move(int dir); 
void create_apple(); 
//for button interrupt
void play_routine();  
void score_routine();
bool isChange(void);
void sensing();

int mode = SCORE;
int pause_flag = 0;
int mute_flag = 0;
extern int apple_on;
extern int current_music;
extern int* melody[];
extern int melody_len[];
extern const struct device *const dev;
int change_note_flag= 0;

// ========INTERRUPT FUNCS============================


//change BGM
void btn4_pushed(){
//    note_type = (note_type+1)%NOTE_CNT; 
   change_note_flag = 1;
}

//TODO change speed
void rotary_encoder_moved(){
    int s = /*value*/ 100;
    if(s < 0) speed = 1000; //minimum speed
    if(s > 99999 /*borderline*/) speed = 10; //maximum speed 
}

//=======================================================

void play_music_routine(){
    uint32_t now_pwm_pulse = 150000;
    current_music = 2;
    seven_segment(current_music);
    while(1){
        change_note_flag = 0;
        for(int i=0; i<melody_len[current_music]/sizeof(int); ){
            while(mute_flag == 1){
                k_sleep(K_MSEC(100));
            }
            if(change_note_flag == 1){
                current_music = (current_music+1)%6;
                if(current_music < 2) current_music = 2;
                seven_segment(current_music);
                break;
            }
            int ret = pwm_set_dt(&pwm_led, (uint32_t)(1/(float)melody[current_music][i++] * 1000000000), now_pwm_pulse);
            if(ret < 0){
            printk("Error setting pwm pulse %d\n", ret);
            }
            int delay = (60000 * 32);
            if(melody[current_music][i] < 0){
                k_busy_wait(delay /(-1 * melody[current_music][i++])* 1.5);
            }
            else{
                k_busy_wait(delay / melody[current_music][i++]);
            }
            ret = pwm_set_dt(&pwm_led, 1, 1);
            k_busy_wait(1000);
            // printk("index:%d\n",i/2);
        }
    }
    
    return;
}

// play the snake game
void play_routine(){
    mode = PLAY;
    printk("i am play routine\n");

    //init map state
    for(int i = 0; i < MAP_WIDTH*MAP_HEIGHT; i++) map[i] = 0;

    //reset values
    dir = LEFT; 
    length = INIT_LENGTH;  

    //set initial body position
    for(int i = 0; i < length; i++){  
        x[i]=MAP_WIDTH/2+i;
        y[i]=MAP_HEIGHT/2;
        map[idx(x[i], y[i])] = 1;
    }map[idx(x[0], y[0])] = 1;

    //create new apple
    create_apple();

    //start playing
    while(1){

        if(pause_flag == 0) move(dir); 
        k_sleep(K_MSEC(speed));

        if(mode == SCORE) break;

    }//while
    score_routine();
}

//display the last score
void score_routine(){  
    mode = SCORE;
    printk("i am score routine\n");
    led_num(length-INIT_LENGTH);
    while(1){
        // printk("Hello hello\n");
        k_sleep(K_MSEC(100));
        if(mode == PLAY) break; 
    }
    play_routine();
    // k_sleep(K_MSEC(5000));
    // btn1_pushed();
    //TODO ===================================================
}

//move using dir 
void move(int dir){
    int i;

    //eat apple case
    if(x[0] == apple_x && y[0] == apple_y){  
        create_apple();  
        length++;  
        x[length-1] = x[length-2];  
        y[length-1] = y[length-2];
    }
    //blocked by wall case
    if(x[0] == 0 || x[0] == MAP_WIDTH-1 || y[0]==0 || y[0] == MAP_HEIGHT-1){  
        score_routine();
        return;  
    }
    //blocked by body case
    for(i = 1; i < length; i++){  
        if(x[0] == x[i] && y[0] == y[i]){
            score_routine();
            return;
        }
    }

    //erase last tail
    map[idx(x[length-1], y[length-1])] = 0;
    for(i = length-1; i > 0; i--){  
        x[i] = x[i-1];
        y[i] = y[i-1];
    }
    //draw new snake
    map[idx(x[0], y[0])] = 1;
    if(dir == LEFT) --x[0];  
    if(dir == RIGHT) ++x[0];
    if(dir == UP) --y[0]; 
    if(dir == DOWN) ++y[0];     
    map[idx(x[i], y[i])] = 1;
    print_map(map); 

}

//create new apple
void create_apple(){
    int i;
    int dup_flag =0; 

    while(1){            
        //select next apple place
        apple_x = (rand() % (MAP_WIDTH-2)) + 1;     
        apple_y = (rand() % (MAP_HEIGHT-2)) + 1;
        
        dup_flag = 0;    
        for(i = 0; i < length; i++){   
            if(apple_x == x[i] && apple_y == y[i]){
                dup_flag = 1;  
                break;
            }
        }
        if(dup_flag == 1) continue; 

        //draw apple DROP     
        map[idx(apple_x, apple_y)] = 2;
        break;
        
    }
}

//calculate index for map using 
int idx(int x, int y){ 
    return (( y * 16 ) + x); 
}


bool isChange(void)
{
	if((nowX < (preX - 50)) || nowX > (preX+50)){
		preX = nowX;
		return true;
	}

	if((nowY < (perY - 50)) || nowY > (perY+50)){
		perY = nowY;
		return true;
	}
	return false;
}
//endif
void sensing() {
	int err;
	uint32_t count = 0;
	uint16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
	};

	/* Configure channels individually prior to sampling. */
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
		if (!adc_is_ready_dt(&adc_channels[i])) {
			printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
			return;
		}
		err = adc_channel_setup_dt(&adc_channels[i]);
		if (err < 0) {
			printk("Could not setup channel #%d (%d)\n", i, err);
			return;
		}
	}

    struct sensor_value val;
	while (1) {
        int rc = sensor_sample_fetch(dev);
		if (rc != 0) {
			printk("Failed to fetch sample (%d)\n", rc);
			return 0;
		}

		rc = sensor_channel_get(dev, SENSOR_CHAN_ROTATION, &val);
		if (rc != 0) {
			printk("Failed to get data (%d)\n", rc);
			return 0;
		}
        if(val.val1 > 0){
            speed_delta -= 5; //speed 와 게임 속도가 반비례
        }
        else if(val.val1 < 0){
            speed_delta += 5;
        }
		(void)adc_sequence_init_dt(&adc_channels[0], &sequence);
		err = adc_read(adc_channels[0].dev, &sequence);
		if (err < 0) {
			printk("Could not read (%d)\n", err);
			continue;
		}
		nowX = (int32_t)buf;

		(void)adc_sequence_init_dt(&adc_channels[1], &sequence);
		err = adc_read(adc_channels[1].dev, &sequence);
		if (err < 0) {
			printk("Could not read (%d)\n", err);
			continue;
		}
		nowY = (int32_t)buf;

		if (nowX >= 65500 || nowY >= 65500){
			printk("Out of Range\n");
			k_sleep(K_MSEC(100));
			continue;
		}

		bool checkFlag = isChange();
		if(!checkFlag){
            // printk("No Change\n");
			k_sleep(K_MSEC(100));
			continue;
		} 
        // else {
		// 	led_off_all();
		// }
//  if((dir==LEFT&&key!=RIGHT)||(dir==RIGHT&&key!=LEFT)||(dir==UP&&key!=DOWN)||(dir==DOWN&&key!=UP)) dir=key; 
		if (nowX == ADC_MAX && nowY == ADC_MAX){
			// led_on_center();
			// printk("Center\n");
		} else if (nowX < AXIS_DEVIATION && nowY == ADC_MAX){
			// led_on_left();
			printk("Left\n");
            if(dir != RIGHT) dir = LEFT;
		} else if (nowX > AXIS_DEVIATION && nowY == ADC_MAX){
			// led_on_right();
			printk("Right\n");
            if(dir != LEFT) dir = RIGHT;
		} else if (nowY > AXIS_DEVIATION && nowX == ADC_MAX){
			// led_on_up();
			printk("Up\n");
            if(dir != DOWN) dir = UP;
		} else if (nowY < AXIS_DEVIATION && nowX == ADC_MAX){
			// led_on_down();
			printk("Down\n");
            if(dir != UP) dir = DOWN;
		}

		k_sleep(K_MSEC(100));
	}
	return;
}

int main(){
    pwm_init();
    decoder_init();
    rotary_init();
	led_init();
    led_set_brightness(led, 0, 0);
    speed = INIT_SPEED;  
    //TODO setting interrupts
    printk("hallo night\n");
    k_tid_t sensing_tid = k_thread_create(&my_thread_data, thread_stack_area,
                                     K_THREAD_STACK_SIZEOF(thread_stack_area),
                                     sensing, NULL, NULL, NULL,
                                     PRIORITY, 0, K_NO_WAIT);

    k_thread_name_set(sensing_tid, "joystick_thread");
    
    k_tid_t bgm_tid = k_thread_create(&my_thread_data2, thread_stack_area2,
                                     K_THREAD_STACK_SIZEOF(thread_stack_area2),
                                     play_music_routine, NULL, NULL, NULL,
                                     8, 0, K_NO_WAIT);

    k_thread_name_set(bgm_tid, "bgm_thread");

    score_routine();

    return 0;

}