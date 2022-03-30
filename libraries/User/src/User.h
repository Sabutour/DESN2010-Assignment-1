#ifndef User_h
#define User_h
#include "Arduino.h"

class User {
	public:
		int id;
		String password;
		String card_id;
		User(int id = -1, String password = "1234", String card_id = "00");
		void SetID(int newID) { id = newID; }
		void SetPassword(String newPassword) { password = newPassword; }
		void SetCardID(String newCardID) { card_id = newCardID; }
};
#endif
