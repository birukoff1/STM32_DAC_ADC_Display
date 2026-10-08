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
	// Voltages
	touchgfx::Unicode::snprintfFloat(AC_voltageBuffer,AC_VOLTAGE_SIZE,"%.2f", V_ADC[0] );
	AC_voltage.invalidate();
	//gauge3.updateValue(50, 0);

	touchgfx::Unicode::snprintfFloat(DC_voltageBuffer,DC_VOLTAGE_SIZE,"%.2f", V_ADC[1] );
	DC_voltage.invalidate();
	//gauge2.updateValue((int)10*V_ADC[1], 1);


	// Phase shifts

	// Phase shift 1
	touchgfx::Unicode::snprintf(Phase_shift_1Buffer,PHASE_SHIFT_1_SIZE,"%i", (int)(10.0*V_ADC[2]));
	Phase_shift_1.invalidate();

	// Phase shift 2
	touchgfx::Unicode::snprintf(Phase_shift_2Buffer,PHASE_SHIFT_2_SIZE,"%i", (int)(10.0*V_ADC[3]));
	Phase_shift_2.invalidate();

	// Phase shift 4
	touchgfx::Unicode::snprintf(Phase_shift_4Buffer,PHASE_SHIFT_4_SIZE,"%i", (int)(10.0*V_ADC[4]));
	Phase_shift_4.invalidate();

	// Phase shift 5
	touchgfx::Unicode::snprintf(Phase_shift_5Buffer,PHASE_SHIFT_5_SIZE,"%i", (int)(10.0*V_ADC[5]));
	Phase_shift_5.invalidate();


}
