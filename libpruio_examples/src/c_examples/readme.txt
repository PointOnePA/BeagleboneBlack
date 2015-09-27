libpruio

1) Create a PruIo structure & configure 
    constructor PruIo::PruIo()
	customize configuration of analog, GPIO, PWM & Capture lines

2) Upload customize configuration to the subsystem registers and start operation 
    function PruIo::config()
	operate the configured subsystems

3) Cleanup. 
    Restore original register configuration
	Destroy the PruIo structure 
	   destructor PruIo::~PruIo

Note:
- create just one Pruio structure at a time.
- 3 subsystems
		TSC_ADCSS (Touch Screen Controler and Analog to Digital Converter SubSystem)
		GPIO      (4x General Purpose Input Output subsystem)
		PWMSS     (3x Pulse Width Modulation SubSystem, PWM, CAP, QEP)
			Pulse Width Modulation
			Capture
			Quadrature Encoder Pulse
- 3 modi using the "Samp" parameter in Pruio::config()
		Samp=1: immediate:  IO mode (inaccurate ADC timing). PRU runs in endless loop.
		Samp>1: rb_start(): RB mode (accurate ADC timing, GPIO handled in background), endless
		Samp>1: mm_start(): MM mode (accurate ADC timing, no GPIO).
		stop [IO & RB] by calling config() again, or calling destructor ~Pruio().

