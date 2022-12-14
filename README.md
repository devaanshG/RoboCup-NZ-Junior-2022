# RoboCup NZ Junior Online 2022
Devaansh Gupta, William Leader, and David Toman

## Background
This is the self-driving car (obstacle avoiding) my friends and I made for RoboCup NZ Junior 2022 Online. We came 1st in the self-driving car category amongst years (grades) 9-13.

## Making the Car
We were going to 3D print it with a Tesla body (based on a Model X). Unfortunately, we couldn't get the body printed (although we did finish the design), so we had to make a last-minute cardboard body.

BOM (Bill of materials) :

Part         | Qty | Cost(each)
-------------|-----|-----------
Arduino Nano |  1  |  $5.00      
HC-SR04      |  6  |  $1.42      
N20 Motor    |  4  |  $4.00      
Wheel        |  4  |  $0.84     
L293D (IC)   |  1  |  $1.80     
Jumper Wires | NA  |  $5.72     
Perfboard    |  1  |  $0.50     
NiCd Battery |  1  |  $11.81      

[Link to list of parts on AliExpress](https://www.aliexpress.com/p/wishlist/shareReflux.html?groupId=qk3fcGzUnusWz8o%2FhIiAldcbagEDx85s7wd3pLeSDz8%3D)

Note that I don't recommend buying batteries from AliExpress as I have never tried this myself. I just had the battery from an old toy. An alternativ ecould be to get a 4 x AA battery holder and use those instead. Otherwise, any battery that is within 7 - 21 V (Arduino Nano vin pin requirement) will work. Note that the current draw of the car is around 2A (So my 700mAh 6V battery lasts around 20 minutes).

### Schematic
![Circuit](https://user-images.githubusercontent.com/104536642/196008855-7942d181-c24c-45bf-b4a3-3296955a5bc7.png)

### Construction

The ultrasonic sensors are all connected to the Arduino Nano board (as shown in schematic), using the single wire configuration using the NewPing library (see code). The 6 sensors use 6 digital pins of the Arduino. The L293D also uses 6 pins, 3 for each side.

The ultrasonic sensors are strategically positioned to maximise the robots detection range of obstacles. The diagram below shows the configuration:

![Ultrasonic sensors configuration](https://user-images.githubusercontent.com/104536642/195976834-000d139e-3866-4a0d-9438-a5228222d5a0.png)


The L293D is a dual motor controller, but we have 4 motors, so, the two wheels on the same side are connected to the same output of the L293D. N20 motors have a fairly low current draw, so the L293D can handle two motors on one output. 

The motors work in a tank-like configuration, with wheels on the same side moving in the same direction. Turning is achieved by moving the two sides in different directions (E.g . right forward, left back results in a left turn. Vice-versa).

## Code

The code consists of a single switch case statement, which branches off into a few if statements. It basically checks which sensor is being blocked (if any) and responds accordingly, whilst making sure we don't collide into some other obstacle. The robot doesn't move to any particular location, just moves randomly and responds to obstacles. The code is split into functions for motor control to make it neater.

## Improvements

In the future, I want to make an improved version with the following features:
 - GPS so the car can navigate to a specific location
 - Bluetooth/WiFi to pass instructions (such as GPS coordinates)
 - Smoother robot movement (rather than stopping and turning on the spot)
