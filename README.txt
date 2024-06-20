# README.txt

## source code list
main.c :
게임 시작 및 스코어링 등의 주요 flow가 진행됨. 세 개의 thread로 나누어져 각각 main flow, BGM 재생, joystick과 rotary의 sensing이 이루어짐.
gpio.c :
button 및 rotary encoder의 설정과 interrupt routine이 정의됨. 부저를 이용하여 연주 할 수 있는 악보가 정의 됨.
gpio.h :
각종 디바이스 메크로 정의.
led.c :
LED dot matrix의 설정과 적절한 값을 display하기 위한 함수들이 정의됨. 
led.h :
led.c를 main.c에서 사용하기 위한 header file

## How to build

제작 환경:
zephyr 2.6.1 toolchain
SDK 2.6.99-cs1

빌드:
west build --build-dir <path/to/snake>
플래쉬:
west flash -d <path/to/snake/build> --skip-rebuild --dev-id 1050276581

## etc
writer: Garyun Kim, Jinju Lee 
date: 2024.06.19

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
