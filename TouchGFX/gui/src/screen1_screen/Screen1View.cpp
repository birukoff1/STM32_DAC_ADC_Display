#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/Color.hpp>

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

	// AC voltage
	touchgfx::Unicode::snprintfFloat(AC_voltageBuffer,AC_VOLTAGE_SIZE,"%.2f", V_AC);
	AC_voltage.invalidate();

	boxAC_voltage.setValue(V_AC);
	if (V_AC > 8.0f)
	{
		boxAC_voltage.setColor(touchgfx::Color::getColorFromRGB(200, 15, 60));
	}
	else
	{
		boxAC_voltage.setColor(touchgfx::Color::getColorFromRGB(0, 240, 255));
	}
	boxAC_voltage.invalidate();

	// AC voltage frequency
	touchgfx::Unicode::snprintf(FreqBuffer,FREQ_SIZE,"%i", Frequency);
	Freq.invalidate();

	boxFreq.setValue(Frequency);
	boxDC_current.invalidate();


	// DC voltage
	touchgfx::Unicode::snprintf(DC_voltageBuffer,DC_VOLTAGE_SIZE,"%i", V_DC);
	DC_voltage.invalidate();

    boxDC_voltage.setValue(V_DC);
	if (V_DC > 700)
	{
		boxDC_voltage.setColor(touchgfx::Color::getColorFromRGB(200, 15, 60));
	}
	else
	{
		boxDC_voltage.setColor(touchgfx::Color::getColorFromRGB(0, 240, 255));
	}
	boxDC_voltage.invalidate();

	// DC current
	touchgfx::Unicode::snprintf(DC_currentBuffer,DC_CURRENT_SIZE,"%i", I_DC);
	DC_current.invalidate();

	boxDC_current.setValue(I_DC);
	if (I_DC > 2000)
	{
		boxDC_current.setColor(touchgfx::Color::getColorFromRGB(200, 15, 60));
	}
	else
	{
		boxDC_current.setColor(touchgfx::Color::getColorFromRGB(0, 240, 255));
	}
	boxDC_current.invalidate();


	// Phase shifts

	// Phase shift 1
	touchgfx::Unicode::snprintf(Phase_shift_1Buffer,PHASE_SHIFT_1_SIZE,"%i", Phase_shift[0]);
	Phase_shift_1.invalidate();

	// Phase shift 2
	touchgfx::Unicode::snprintf(Phase_shift_2Buffer,PHASE_SHIFT_2_SIZE,"%i", Phase_shift[1]);
	Phase_shift_2.invalidate();

	// Phase shift 4
	touchgfx::Unicode::snprintf(Phase_shift_4Buffer,PHASE_SHIFT_4_SIZE,"%i", Phase_shift[3]);
	Phase_shift_4.invalidate();

	// Phase shift 5
	touchgfx::Unicode::snprintf(Phase_shift_5Buffer,PHASE_SHIFT_5_SIZE,"%i", Phase_shift[4]);
	Phase_shift_5.invalidate();

	/*

	to Screen1ViewBase.hpp
	#include <main.h>

	to Screen1ViewBase.cpp
	if (ShowImage)
    {
		if (systemMode == MODE_SETUP)
		{
			image1.setVisible(true);
			image1.invalidate();
			ShowImage = 0;
		}
		else
		{
			image1.setVisible(false);
			image1.invalidate();
			ShowImage = 0;
		}
    }
	 */


}
