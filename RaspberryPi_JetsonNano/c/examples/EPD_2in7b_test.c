/*****************************************************************************
* | File      	:   EPD_2IN7b_test.c
* | Author      :   Waveshare team
* | Function    :   2.7inch e-paper b test demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2019-06-11
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "EPD_Test.h"
#include "EPD_2in7b.h"


// Function to add a shot to the list
void addShot(struct ShotList *shotList, int time, char* grade) {
    if (shotList->count < MAX_SHOTS) {
        shotList->shots[shotList->count].time = time;
        if (shotList->count == 0)//Empty so no split time
            shotList->shots[shotList->count].split = 0; 
        else {
            int split = time - shotList->shots[shotList->count-1].time;
            if (split > 0)
                shotList->shots[shotList->count].split = split;
            else 
                printf("ERROR: Invalid shot timing. Time went backwards. %d !> 0 On shot %d\n", split, shotList->count);
        }
        shotList->shots[shotList->count].grade = grade;
        shotList->count++;
    } else {
        printf("Shot list is full.\n");
    }
}
void emptyShotList(struct ShotList *shotList) {
    shotList->count = 0;
}


void print_Text_Field(struct ShotList *shotList, int black) {
    // printf("count:%d, %d,%d,%c,\n",shotList->count,shotList->shots[0].time, shotList->shots[0].split, shotList->shots[0].grade);
    if(black){
        Paint_DrawString_EN(0, 0, "  Time    Split  Zone", &Font12, BLACK, WHITE);
        for (int i = 0; i < shotList->count && i < 7; i++) {
            print_Line_Black(i, shotList->shots[i].time, shotList->shots[i].split, shotList->shots[i].grade);
        }
    }
    else{
        for (int i = 0; i < shotList->count && i < 7; i++) {
            print_Line_Red(i, shotList->shots[i].time, shotList->shots[i].split, shotList->shots[i].grade);
        }
    }
}
void print_Line_Black(int row, int cent_sec, int split_cent_sec, const char *zone)
{
    if (*zone == 'M') return;
    int row_height = 13 + (row*23);
    char _cent_sec[4];
    snprintf(_cent_sec, sizeof(_cent_sec), "%03d", cent_sec);
    char _split_cent_sec[4] = "";
    if (split_cent_sec != 0){
        snprintf(_split_cent_sec, sizeof(_split_cent_sec), "%03d", split_cent_sec);
    }

    Paint_DrawString_EN(0,  row_height, _cent_sec,  &Font24, BLACK, WHITE);
    Paint_DrawString_EN(62, row_height, _split_cent_sec, &Font24, BLACK, WHITE);
    // Paint_DrawString_EN(62, row_height, "0",  &Font24, BLACK, WHITE);
    // Paint_DrawString_EN(80, row_height, "95", &Font24, BLACK, WHITE);
    Paint_DrawString_EN(127, row_height, zone, &Font24, BLACK, WHITE);
    Paint_DrawCircle(   17, row_height+16, 1, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL); //First Period
    if (split_cent_sec != 0) Paint_DrawCircle(79, row_height+16, 1, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL); //Second Period
}
void print_Line_Red(int row, int cent_sec, int split_cent_sec, const char *zone)
{
    if (*zone != 'M') return;
    int row_height = 13 + (row*23);
    char _cent_sec[4];
    snprintf(_cent_sec, sizeof(_cent_sec), "%03d", cent_sec);
    char _split_cent_sec[4] = "";
    if (split_cent_sec != 0){
        snprintf(_split_cent_sec, sizeof(_split_cent_sec), "%03d", split_cent_sec);
        Paint_DrawCircle(79, row_height+16, 1, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL); //Second Period
    }
    Paint_DrawString_EN(0,  row_height, _cent_sec,  &Font24, WHITE, BLACK);
    Paint_DrawString_EN(62, row_height, _split_cent_sec, &Font24, WHITE, BLACK);
    // Paint_DrawString_EN(62, row_height, "0",  &Font24, BLACK, WHITE);
    // Paint_DrawString_EN(80, row_height, "95", &Font24, BLACK, WHITE);
    Paint_DrawString_EN(127, row_height, zone, &Font24, WHITE, BLACK);
    Paint_DrawCircle(   17, row_height+16, 1, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL); //First Period
}
int EPD_2in7b_test(void)
{
    printf("EPD_2IN7B_test Demo\r\n");
    if(DEV_Module_Init()!=0){
        return -1;
    }

    // int border = 0; 
    // int textWidth = 152;
    // int columnWidth[3] = {border,border + 65, border + 130};
    // int row_num = 0;
    // int y_pos = 0;
    
    printf("e-Paper Init and Clear...\r\n");
    EPD_2IN7B_Init();
    // EPD_2IN7B_Clear();
    // DEV_Delay_ms(500);
        //Create a new image cache named IMAGE_BW and fill it with white
    UBYTE *BlackImage, *RedImage;
    UWORD Imagesize = ((EPD_2IN7B_WIDTH % 8 == 0)? (EPD_2IN7B_WIDTH / 8 ): (EPD_2IN7B_WIDTH / 8 + 1)) * EPD_2IN7B_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    if((RedImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for red memory...\r\n");
        return -1;
    }
    printf("NewImage:BlackImage and RedImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN7B_WIDTH, EPD_2IN7B_HEIGHT, 270, WHITE);
    Paint_NewImage(RedImage,   EPD_2IN7B_WIDTH, EPD_2IN7B_HEIGHT, 270, WHITE);


    Paint_SelectImage(BlackImage);
    Paint_Clear(BLACK);
    //IPSC Target
    Paint_DrawLine(187,  1,223,  1, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(223,  1,223, 36, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(223, 36,246, 36, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(246, 36,258, 48, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(258, 48,258,142, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(258,142,240,176, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(240,176,170,176, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(170,176,152,142, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(152,142,152, 48, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(152, 48,164, 36, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(164, 36,187, 36, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(187, 36,187,  1, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(187, 36,170, 48, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(170, 48,170,114, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(170,114,182,142, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(182,142,228,142, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(228,142,240,114, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(240,114,240, 48, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(240, 48,223, 36, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawRectangle(187, 48, 223, 114, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY); // A Zone
    Paint_DrawRectangle(193, 7, 217, 19, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY); //A Zone Headbox

    //Text on IPSC target
    Paint_DrawString_EN(203, 10, "A", &Font8, BLACK, WHITE);
    Paint_DrawString_EN(158, 78, "D", &Font8, BLACK, WHITE);
    Paint_DrawString_EN(175, 78, "C", &Font8, BLACK, WHITE);
    Paint_DrawString_EN(203, 78, "A", &Font8, BLACK, WHITE);
    Paint_DrawString_EN(228, 78, "C", &Font8, BLACK, WHITE);
    Paint_DrawString_EN(247, 78, "D", &Font8, BLACK, WHITE);
    struct ShotList myShotList;
    myShotList.count = 0;
    addShot(&myShotList, 102, "M");
    addShot(&myShotList, 205, "A");
    addShot(&myShotList, 315, "B");
    addShot(&myShotList, 400, "C");
    addShot(&myShotList, 512, "D");
    addShot(&myShotList, 856, "A");
    addShot(&myShotList, 886, "M");
    for (int i = 0; i < myShotList.count; i++) {
        printf("Shot %d: x=%d, y=%d, label=%c\n", i + 1, myShotList.shots[i].time, myShotList.shots[i].split, *myShotList.shots[i].grade);
    }
    printf("print_Text_Field black\n");
    //Text
    print_Text_Field(&myShotList, 1);

    // print_Line_Black(0, 110, 85, "A");
    // print_Line_Black(1, 511, 305, "B");
    // print_Line_Black(2, 611, 105, "C");
    // print_Line_Black(3, 510, 305, "D");
    // Paint_SelectImage(RedImage);
    // Paint_Clear(WHITE);
    // print_Line_Red(4, 1, 1, "M");
    // print_Line_Red(5, 23, 23, "M");
    // print_Line_Red(6, 34, 456, "M");
    // // Paint_DrawString_EN(1, 17, (char)1.05, &Font12, BLACK, WHITE);
    // // Paint_DrawString_EN(1, 17, "2.15", &Font16, BLACK, WHITE);
    // char charArray[4];
    // // snprintf(charArray, sizeof(charArray), "%.3f", 2.15);

    // // Paint_DrawString_EN(1, 34, charArray, &Font16, BLACK, WHITE);
    // // Paint_DrawString_EN(1, 58, "510 095 M", &Font24, BLACK, WHITE);
    // // Paint_DrawCircle(19, 40, 2, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    // // Paint_DrawCircle(65, 40, 2, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    // Paint_DrawString_EN(0,  12, "5",  &Font24, BLACK, WHITE);
    // Paint_DrawString_EN(18, 12, "10", &Font24, BLACK, WHITE);
    // Paint_DrawString_EN(62, 12, "0",  &Font24, BLACK, WHITE);
    // Paint_DrawString_EN(80, 12, "95", &Font24, BLACK, WHITE);
    // Paint_DrawString_EN(127, 12, "M", &Font24, BLACK, WHITE);
    // Paint_DrawCircle(17, 28, 1, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    // Paint_DrawCircle(80, 28, 1, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);


    // Paint_DrawString_EN(1, 51, "3.15 0.95 A", &Font16, BLACK, WHITE);
    // Paint_DrawString_EN(1, 68, "4.15 0.95 A", &Font20, BLACK, WHITE);
    // Paint_DrawString_EN(1, 89, "5.10 0.95 M", &Font20, BLACK, WHITE);
    // Paint_DrawRectangle(5, 47, 36, 66, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    printf("RedImage");
    Paint_SelectImage(RedImage);
    Paint_Clear(WHITE);
    print_Text_Field(&myShotList, 0);


    EPD_2IN7B_Display(BlackImage, RedImage);
    DEV_Delay_ms(4000);

 /*   //Select Image
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_SelectImage(RedImage);
    Paint_Clear(WHITE);
    
    #if 1   // show bmp
        printf("show window BMP-----------------\r\n");
        Paint_SelectImage(BlackImage);
        GUI_ReadBmp("./pic/100x100.bmp", 50, 10);
        Paint_SelectImage(RedImage);
        Paint_Clear(WHITE);    
        EPD_2IN7B_Display(BlackImage, RedImage);
        DEV_Delay_ms(4000);

        printf("show bmp------------------------\r\n");
        Paint_SelectImage(BlackImage);
        GUI_ReadBmp("./pic/2in7b-b.bmp", 0, 0);
        Paint_SelectImage(RedImage);
        GUI_ReadBmp("./pic/2in7b-r.bmp", 0, 0);
        EPD_2IN7B_Display(BlackImage, RedImage);
        DEV_Delay_ms(4000);
    #endif

    #if 1   // show image for array   
        printf("show image for array\r\n");
        Paint_SelectImage(BlackImage);
        Paint_DrawBitMap(gImage_2in7b_Black);
        Paint_SelectImage(RedImage);
        Paint_DrawBitMap(gImage_2in7b_Red);
        EPD_2IN7B_Display(BlackImage, RedImage);
        DEV_Delay_ms(4000);
    #endif

    #if 1   // Drawing on the image
        //Horizontal screen
        //1.Draw black image
        Paint_SelectImage(BlackImage);
        Paint_Clear(WHITE);
        Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
        Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
        Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
        Paint_DrawPoint(10, 110, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
        Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
        Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
        Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
        Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
        Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
        Paint_DrawString_CN(130, 20, "΢ѩ����", &Font24CN, WHITE, BLACK);
        Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);

        //2.Draw red image
        Paint_SelectImage(RedImage);
        Paint_Clear(WHITE);
        Paint_DrawCircle(160, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
        Paint_DrawCircle(210, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
        Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
        Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
        Paint_DrawString_CN(130, 0,"���abc", &Font12CN, BLACK, WHITE);
        Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);
        Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);

        printf("EPD_Display\r\n");
        EPD_2IN7B_Display(BlackImage, RedImage);
        DEV_Delay_ms(4000);
    #endif



    printf("Clear...\r\n");
    EPD_2IN7B_Clear();
*/
    printf("Goto Sleep...\r\n");
    EPD_2IN7B_Sleep();
    free(BlackImage);
    BlackImage = NULL;

    DEV_Delay_ms(2000);//important, at least 2s
    // close 5V
    printf("close 5V, Module enters 0 power consumption ...\r\n");
    DEV_Module_Exit();
	DEV_Delay_ms(2000);    
    return 0;
}

