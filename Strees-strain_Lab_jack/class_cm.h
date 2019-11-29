#pragma once
#ifndef class_cm_IS_INCLUDED
#define class_cm_IS_INCLUDED
#include <zaber/motion/ascii.h>
#include <zaber/motion/measurement.h>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
using namespace std;

namespace zaber {
	namespace motion {
		namespace ascii {
			class Ascii {
			protected:
				int baud_rate;
				double test_vel;
				int move_time;
			public:
				Ascii();
				Connection conn2 = Connection::openSerialPort("COM7");
				Connection conn = move(conn2);
				Device device = conn.detectDevices()[0];
				Axis axis = device.getAxis(1);
				void Set_baud(int a);
				void Set_move_time(int c);
				int Get_baud();
				double Get_test_vel();
				int Get_move_time();
				void move_home();
				double get_position();
				bool is_idle();
				//void unit_conversion(int a, string unit_a, int c, string unit_b);
				void move_vel(double vel, string unit);
				void move_abs(double position, string unit);
				void move_rel(double position, string unit);
				void close();
				void stop();
			};
			Ascii::Ascii() {
				Set_baud(115200);
				Axis axis = device.getAxis(1);
			}
			void Ascii::Set_baud(int a) {
				baud_rate = a;
			}
			void Ascii::Set_move_time(int c) {
				move_time = c;
			}
			int Ascii::Get_baud() {
				return baud_rate;
			}
			double Ascii::Get_test_vel() {
				return test_vel;
			}
			int Ascii::Get_move_time() {
				return move_time;
			}
			void Ascii::move_home() {
				// Homes all axes. Axes return to their homing positions.
				axis.home();
			}
			double Ascii::get_position() {
				return axis.getPosition(Units::LENGTH_MILLIMETRES);
			}
			bool Ascii::is_idle() {
				if (axis.isBusy()) 
				{
					return false;
				}
				return true;
			}
			//void Ascii::unit_conversion(int a, string unit_a, int c, string unit_b) {
			//	// Formats parameters into a command and performs unit conversions.
			//	// Parameters in the command template are denoted by a question mark.
			//	//
			//	// input unit has to be a string in the following format: 
			//	//		unit= MILLIMETRES => millimetres
			//	//		unit= MICROMETRES => micrometre
			//	//		unit= NANOMETRES => nanometre
			//	//
			//	//      unit= SECONDS => second
			//	//		unit= MILLISECONDS => millisecond
			//	//		unit= MICROSECONDS => microsecond
			//	// for example :
			//	//		unit_conversion(10, millimetres, 1, second)
			//	// Any typo may result in failure in unit setting
			//	Units time;
			//	Units length;
			//	string mm = "millimetre", mic = "micrometre", na = "nanometre";
			//	string sec = "second", ms = "millisecond", mics = "microsecond";
			//	if (unit_a == mm) length = Units::LENGTH_MILLIMETRES;
			//	else if (unit_a == mic) length = Units::LENGTH_MICROMETRES;
			//	else if (unit_a == na) length = Units::LENGTH_NANOMETRES;
			//	else {
			//		cout << "CANNOT recognize the input unit, please check arguments." << endl;
			//		cout << " The unit of length will be default (millimetre)" << endl;
			//		length = Units::LENGTH_MILLIMETRES;
			//	}
			//	if (unit_b == sec) time = Units::TIME_SECONDS;
			//	else if (unit_b == ms) time = Units::TIME_MILLISECONDS;
			//	else if (unit_b == mics) time = Units::TIME_MICROSECONDS;
			//	else {
			//		cout << "CANNOT recognize the input unit, please check arguments." << endl;
			//		cout << " The unit of time will be default (Seconds)" << endl;
			//		time = Units::TIME_SECONDS;
			//	}
			//	/*string cmd = axis.prepareCommand("move sin ? ? ?",
			//		Measurement(a, length),
			//		Measurement(c, time),
			//		Measurement(3));*/
			//	string cmd = axis.prepareCommand("? ?",
			//		Measurement(10, Units::LENGTH_MILLIMETRES),  
			//		Measurement(1, Units::TIME_SECONDS)                                       
			//	);
			//	cout << cmd << endl;
			//	axis.genericCommand(cmd);
			//	axis.waitUntilIdle();
			//}
			void Ascii::move_vel(double vel, string unit) {
				// Moves the first axis of the lockstep group at the specified speed.
				// The other axis in the lockstep group maintains its offset throughout movement.
				//
				// input unit has to be a string in the following format:
				//		unit= Metres per second => metre/s
				//		unit= Centimetres per second => centimetre/s
				//		unit= Millimetres per second => millimetre/s
				//		unit= Micrometres per second => micrometre/s
				//		unit= Nanometres per second => nanometre/s
				// for example :
				//		move_vel(3.0,"millimetre/s")
				// Any typo may result in failure in unit setting
				test_vel = vel;
				const double v = test_vel;
				Units unt;
				string Mets = "metre/s", Cms = "centimetre/s";
				string Ms = "millimetre/s", Mics = "micrometre/s", Ns = "nanometre/s";
				if (unit == Ms) unt = Units::VELOCITY_MILLIMETRES_PER_SECOND;
				else if (unit == Mets) unt = Units::VELOCITY_METRES_PER_SECOND;
				else if (unit == Cms) unt = Units::VELOCITY_CENTIMETRES_PER_SECOND;
				else if(unit ==Mics) unt = Units::VELOCITY_MICROMETRES_PER_SECOND;
				else if(unit== Ns) unt = Units::VELOCITY_NANOMETRES_PER_SECOND;
				else {
					cout << "CANNOT recognize the input unit, please check arguments." << endl;
					cout << " The unit of velocity will be default (millimetre per second)" << endl;
					unt = Units::VELOCITY_MILLIMETRES_PER_SECOND;
				}
				axis.moveVelocity(v, unt);
			}
			void Ascii::move_abs(const double position, string unit) {
				// Moves the first axis of the lockstep group to an absolute position.
				// The other axis in the lockstep group maintains its offset throughout movement.
				//
				// input unit has to be a string in the following format: 
				//		unit= Millimetres per second => millimetre/s
				//		unit= Micrometres per second => micrometre/s
				//		unit= Nanometres per second => nanometre/s
				// for example :
				//		move_abs(1.0,"millimetre/s")
				// Any typo may result in failure in unit setting
				const double posi = position;
				Units unt;
				string Ms = "millimetre", Mics = "micrometre", Ns = "nanometre";
				if (unit == Ms) unt = Units::LENGTH_MILLIMETRES;
				else if (unit == Mics) unt = Units::LENGTH_MICROMETRES;
				else if (unit == Ns) unt = Units::LENGTH_NANOMETRES;
				else {
					cout << "CANNOT recognize the input unit, please check arguments." << endl;
					cout << " The unit of velocity will be default (millimetre)" << endl;
					unt = Units::LENGTH_MILLIMETRES;
				}
				axis.moveAbsolute(posi, unt);
			}
			void Ascii::move_rel(const double position, string unit) {
				// Moves the first axis of the lockstep group to a position relative to its current position.
				// The other axis in the lockstep group maintains its offset throughout movement.
				//
				// input unit has to be a string in the following format: 
				//		unit= Millimetres per second => millimetre/s
				//		unit= Micrometres per second => micrometre/s
				//		unit= Nanometres per second => nanometre/s
				// for example :
				//		move_rel(1.0,"millimetre/s")
				// Any typo may result in failure in unit setting
				const double posi = position;
				Units unt;
				string Ms = "millimetre", Mics = "micrometre", Ns = "nanometre";
				if (unit == Ms) unt = Units::LENGTH_MILLIMETRES;
				else if (unit == Mics) unt = Units::LENGTH_MICROMETRES;
				else if (unit == Ns) unt = Units::LENGTH_NANOMETRES;
				else {
					cout << "CANNOT recognize the input unit, please check arguments." << endl;
					cout << " The unit of velocity will be default (millimetre)" << endl;
					unt = Units::LENGTH_MILLIMETRES;
				}
				axis.moveRelative(posi, unt);
			}
			void Ascii::stop() {
				axis.stop();
			}
			void Ascii::close() {
				conn.close();
			}
		}
	}
}
#endif


