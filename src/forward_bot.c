#include "librobutts.h"

robot_properties my_robot = {
	2.5, 1.5, 1, 0x00FFFF, "avance"
};

void update() {
	my_state.lin_eng_state = ENG_ON;
}
