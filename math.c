void subsTen(int *ip_array, int index) {
	if (index >= 0 && index < 4) {
		if ((ip_array[index] - 10) >= 0) {
			ip_array[index] -= 10;
		}
	} else if (index == 4) {  // Port number
		if ((ip_array[0] - 10) >= 0) {
			ip_array[0] -= 10;
		}
	}
}

void plusTen(int *ip_array, int index) {
	if (index >= 0 && index < 4) {
		if ((ip_array[index] + 10) <= 255) {
			ip_array[index] += 10;
		}
	} else if (index == 4) {  // Port number
		if ((ip_array[0] + 10) <= 999) {
			ip_array[0] += 10;
		}
	}
}

void subsOne(int *ip_array, int index) {
	if (index >= 0 && index < 4) {
		if (ip_array[index] > 0) {
			ip_array[index]--;
		}
	} else if (index == 4) {  // Port number
		if (ip_array[0] > 0) {
			ip_array[0]--;
		}
	}
}

void plusOne(int *ip_array, int index) {
	if (index >= 0 && index < 4) {
		if (ip_array[index] < 255) {
			ip_array[index]++;
		}
	} else if (index == 4) {  // Port number
		if (ip_array[0] < 999) {
			ip_array[0]++;
		}
	}
}
