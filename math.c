void subsTen(int *a, int *b, int *c, int *d, int *e, int x){
	switch(x){
		case 0:
			if((*a - 10) > 0){
				*a = *a - 10;
			}
			break;
		case 1:
			if((*b - 10) > 0){
				*b = *b - 10;
			}
			break;
		case 2:
			if((*c - 10) > 0){
				*c = *c - 10;
			}
			break;
		case 3:
			if((*d - 10) > 0){
				*d = *d - 10;
			}
			break;
		case 4:
			if((*e - 10) > 0){
				*e = *e - 10;
			}
			break;
	}
}

void plusTen(int *a, int *b, int *c, int *d, int *e, int x){
	switch(x){
		case 0:
			if((*a + 10) <= 255){
				*a = *a + 10;
			}
			break;
		case 1:
			if((*b + 10) <= 255){
				*b = *b + 10;
			}
			break;
		case 2:
			if((*c + 10) <= 255){
				*c = *c + 10;
			}
			break;
		case 3:
			if((*d + 10) <= 255){
				*d = *d + 10;
			}
			break;
		case 4:
			if((*e + 10) <= 999){
				*e = *e + 10;
			}
			break;
	}
}

void subsOne(int *a, int *b, int *c, int *d, int *e, int x){
	switch(x){
		case 0:
			if(*a > 0){
				*a = *a - 1;
			}
			break;
		case 1:
			if(*b > 0){
				*b = *b - 1;
			}
			break;
		case 2:
			if(*c > 0){
				*c = *c - 1;
			}
			break;
		case 3:
			if(*d > 0){
				*d = *d - 1;
			}
			break;
		case 4:
			if(*e > 0){
				*e = *e - 1;
			}
			break;
	}
}

void plusOne(int *a, int *b, int *c, int *d, int *e, int x){
	switch(x){
		case 0:
			if(*a < 255){
				*a = *a + 1;
			}
			break;
		case 1:
			if(*b < 255){
				*b = *b + 1;
			}
			break;
		case 2:
			if(*c < 255){
				*c = *c + 1;
			}
			break;
		case 3:
			if(*d < 255){
				*d = *d + 1;
			}
			break;
		case 4:
			if(*e < 999){
				*e = *e + 1;
			}
			break;
	}
}
