This repository was created to deposit code and hardware designs for the Open Source Ventilator being created at the University of Florida. All of the design and code are Open Source. Here are the design parameters:

Design a bidirectional flow metering system for a ventilator.

The design flow rate is 90 liters/min or 1.5 liters/second.
The metering system must be made such that is can be made from readily available parts from the hardware store and electronics houses.
The flow meter must not exceed 10 kPa in pressure drop... and less is better.
The accuracy of the flow system needs to be about 10% or better.
Depending upon the location, some parts will be hardware dependent. Thus multi-sourced flow sensors are a consideration.
Analog or digital devices are appropriate as long as they can be integrated into the Arduino control system (see references).
Simple, cheap, available is better.
Flow sensor must be easy to calibrate.
Flow sensor must connect to a 1 inch PVC pipe. ID of Schedule 40 PVC is 1.029 inches.
Accompanying software for Arduino to read sensor and calculate Instantaneous flow, Averaged flow in liters/min.
Also calculate total volume per cycle for "Inhale" and "exhale" in liters. 13 Signal filtering for sensor measurements such that noise and randomness is minimized.

Solution: A simple orifice disc meter will satisfy all of the requirements. Why?:

Simplicity of construction. A simple disc in the pipe connection is sufficient.
Range of flow and accuracy easily met.
Simple fluid mechanics to solve: Bernoullis equations.
Tap locations have rules but are not critical to the design.
Operates bi-directionally.
Many alternatives available for sensors. Design will accommodate multiple sensor part numbers.
