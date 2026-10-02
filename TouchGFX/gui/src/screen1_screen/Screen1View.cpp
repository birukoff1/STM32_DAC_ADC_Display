#include <gui/screen1_screen/Screen1View.hpp>

Screen1View::Screen1View()
{
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

// CHANGES

void Screen1View::UpdateValues()
{
	//touchgfx::Unicode::snprintf(DC_voltageBuffer,DC_VOLTAGE_SIZE,"%i", V_dc);
	// With float
	touchgfx::Unicode::snprintfFloat(DC_voltageBuffer,DC_VOLTAGE_SIZE,"%.2f", V_ADC[0] );
	DC_voltage.invalidate();

	gauge1.updateValue((int)10*V_ADC[0], 1);

}
