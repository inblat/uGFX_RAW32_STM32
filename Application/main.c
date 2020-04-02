#include "stm32f4xx.h"
#include "main.h"
#include "gfx.h"


//__IO uint32_t uwTick;
uint32_t uwTick;

systemticks_t gfxSystemTicks(void)
{
	return uwTick;
}

systemticks_t gfxMillisecondsToTicks(delaytime_t ms)
{
	return ms;
}


#include "math.h"

// A set of data points that will be displayed in the graph
static const point data[5] = {
    { -40, -40 },
    { 70, 40 },
    { 140, 60 },
    { 210, 60 },
    { 280, 200 }
};

// A graph styling
static GGraphStyle GraphStyle1 = {
    { GGRAPH_POINT_DOT, 0, Blue },          // Point
    { GGRAPH_LINE_NONE, 2, Gray },          // Line
    { GGRAPH_LINE_SOLID, 0, White },        // X axis
    { GGRAPH_LINE_SOLID, 0, White },        // Y axis
    { GGRAPH_LINE_DASH, 5, Gray, 50 },      // X grid
    { GGRAPH_LINE_DOT, 7, Yellow, 50 },     // Y grid
    GWIN_GRAPH_STYLE_POSITIVE_AXIS_ARROWS   // Flags
};

// Another graph styling
static const GGraphStyle GraphStyle2 = {
    { GGRAPH_POINT_SQUARE, 5, Red },        // Point
    { GGRAPH_LINE_DOT, 2, Pink },           // Line
    { GGRAPH_LINE_SOLID, 0, White },        // X axis
    { GGRAPH_LINE_SOLID, 0, White },        // Y axis
    { GGRAPH_LINE_DASH, 5, Gray, 50 },      // X grid
    { GGRAPH_LINE_DOT, 7, Yellow, 50 },     // Y grid
    GWIN_GRAPH_STYLE_POSITIVE_AXIS_ARROWS   // Flags
};


int main(void)
{
	clockConfig();

	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/1000);

	initFSMC();

    //GPIO
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODER7_0 | GPIO_MODER_MODER6_0;

	//uGFX reset pin
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODER5_0;
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5;
    GPIOA->BSRR |= GPIO_BSRR_BS5; //Set to logic one


    GHandle     gh;
    uint16_t    i;

    gfxInit();


    // Create the graph window
    {
        GWindowInit wi;

        wi.show = TRUE;
        wi.x = wi.y = 0;
        wi.width = gdispGetWidth();
        wi.height = gdispGetHeight();
        gh = gwinGraphCreate(0, &wi);
    }

    // Set the graph origin and style
    gwinGraphSetOrigin(gh, gwinGetWidth(gh)/2, gwinGetHeight(gh)/2);
    gwinGraphSetStyle(gh, &GraphStyle1);
    gwinGraphDrawAxis(gh);

    // Draw a sine wave
    for(i = 0; i < gwinGetWidth(gh); i++) {
        gwinGraphDrawPoint(gh, i-gwinGetWidth(gh)/2, 80*sin(2*0.2*M_PI*i/180));
    }

    // Modify the style
    gwinGraphStartSet(gh);
    GraphStyle1.point.color = Green;
    gwinGraphSetStyle(gh, &GraphStyle1);

    // Draw a different sine wave
    for(i = 0; i < gwinGetWidth(gh)*5; i++) {
        gwinGraphDrawPoint(gh, i/5-gwinGetWidth(gh)/2, 95*sin(2*0.2*M_PI*i/180));
    }

    // Change to a completely different style
    gwinGraphStartSet(gh);
    gwinGraphSetStyle(gh, &GraphStyle2);

    // Draw a set of points
    gwinGraphDrawPoints(gh, data, sizeof(data)/sizeof(data[0]));

	while(1)
	{
		gfxSleepMilliseconds(500);
		WRITE_REG(GPIOA->ODR, READ_REG(GPIOA->ODR) ^ GPIO_ODR_OD6);
	}
}

void SysTick_Handler(void)
{
	uwTick++;
	//WRITE_REG(GPIOA->ODR, READ_REG(GPIOA->ODR) ^ GPIO_ODR_OD6);
}

