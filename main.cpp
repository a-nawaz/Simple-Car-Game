#include <iostream>
#include <Windows.h>
#include <thread>
#include <time.h>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

const int screen_w = 21;
const int screen_h = 19;
int x_offset = 10;
int y_offset = 3;
int screen[screen_h][screen_w] = {}; //game screen
int car_pos = 10;
int score = 0;

void gotoXY(int, int);
void start_game();
void draw_car(int);
void draw_obstacle();
void listener();
bool check_collision();
void print_score();
void write_to_file();
int fit_write(int*);
void shift_left(string*, int);
void print_top_scores();

void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

int main() {
	ShowConsoleCursor(false);
	start_game();
	write_to_file();
	print_top_scores();
	return 0;
}

void start_game() {
	bool run = true;
	thread input(listener);
	Sleep(50);
	thread obstacle(draw_obstacle);
	Sleep(50);
	while (run) {
		draw_car(car_pos);
		for (int i = 0; i < screen_h; i++) {
			for (int j = 0; j < screen_w; j++) {
				if (screen[i][j] == 1) {
					gotoXY(j+x_offset, i+y_offset);
					printf("%c[%dm#", 0x1B, 32);
				}
				else if (screen[i][j] == 2) {
					gotoXY(j + x_offset, i + y_offset);
					printf("%c[%dm@", 0x1B, 31);
				}
				else {
					gotoXY(j+x_offset, i+y_offset);
					std::cout << " ";
				}
				if (j == 0 || j == screen_w - 1) {
					gotoXY(j + x_offset, i + y_offset);
					printf("%c[%dm|", 0x1B, 33);
				}
			}
		}
		Sleep(10);
		print_score();
		run = check_collision();
	}
	gotoXY(x_offset + 6, y_offset + 5);
	std::cout << "GAME OVER";
	input.detach();
	obstacle.detach();
}

void gotoXY(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void draw_car(int pos) {
	for (int i = 0; i < screen_w; i++) {
		if (i == pos || i == pos + 1) {
			screen[screen_h - 1][i] = 1;
			screen[screen_h - 2][i] = 1;
		}
		else {
			screen[screen_h - 1][i] = 0;
			screen[screen_h - 2][i] = 0;
		}
	}	
}

void draw_obstacle() {
	while (true) {
		srand(time(0));
		int r = rand() % 16;
		if (r == 0) r = 1;
		for (int i = 0; i < screen_h; i++) {
			if (i > 0) {
				screen[i - 1][r] = 0;
				screen[i - 1][r + 1] = 0;
				screen[i - 1][r + 2] = 0;
				screen[i - 1][r + 3] = 0;
				screen[i - 1][r + 4] = 0;
				screen[i - 1][r + 5] = 0;
			}
			screen[i][r] = 2;
			screen[i][r + 1] = 2;
			screen[i][r + 2] = 2;
			screen[i][r + 3] = 2;
			screen[i][r + 4] = 2;
			screen[i][r + 5] = 2;
			Sleep(200);
		}
		Sleep(50);
	}
}

void listener() {
	while (true) {
		if (GetAsyncKeyState(VK_LEFT) & 1)
			if (car_pos > 1)
				car_pos--;
		if (GetAsyncKeyState(VK_RIGHT) & 1)
			if (car_pos < screen_w - 3)
				car_pos++;
	}
}

bool check_collision() {
	for (int i = 1; i < screen_w-1; i++) {
		if (screen[screen_h - 2][i] == 1 && screen[screen_h - 3][i] == 2)
			return false;
	}
	return true;
}

void print_score() {
	score += 1;
	gotoXY(x_offset + screen_w + 5, y_offset + 3);
	std::cout << "Car Racing Game :)";
	gotoXY(x_offset + screen_w + 5, y_offset + 5);
	std::cout << "Score: " << score;
}

void write_to_file() {
	int high_s[5] = {};
	string Nhigh_s[5];
	string line;

	char username[20];
	DWORD username_len = 20;
	GetUserNameA(username, &username_len);
	int k = 0;
	ifstream file_r;
	file_r.open("highscores.txt");
	if (file_r.is_open()) {
		while (getline(file_r, line)) {
			Nhigh_s[k] = line;
			string f_score;
			for (int i = 0; line[i] != ':'; i++) {
				f_score += line[i];
			}
			high_s[k] = stoi(f_score);
			k++;
		}
		file_r.close();
	}
	else std::cout << "File Could Not Be Opened" << endl;
	if (score > high_s[0]) {
		Nhigh_s[0] = "0";
		int index = fit_write(high_s);
		shift_left(Nhigh_s, index);
		ofstream file_w;
		file_w.open("highscores.txt");
		if (file_w.is_open()) {
			for (int i = 0; i < 5; i++) {
				if (Nhigh_s[i] == "0")
					file_w << score << ":" << username << "\n";
				else {
					file_w << Nhigh_s[i] << "\n";
				}
			}
			file_w.close();
		}
		else
			std::cout << "File could not be opened" << endl;
	}
}

int fit_write(int* high_s) {
	int i = 0;
	for (i = 1; i < 5; i++) {
		if (high_s[i] <= score) {
			continue;
		}
		else break;
	}
	return i-1;
}

void shift_left(string* arr, int n) {
	for (int i = 0; i <= n; i++) {
		if(i != 4)
			arr[i] = arr[i + 1];
	}
	arr[n] = "0";
}

void print_top_scores() {
	string line;
	int i = 7;
	ifstream file_r;
	file_r.open("highscores.txt");
	if (file_r.is_open()) {
		gotoXY(x_offset + screen_w + 5, y_offset + i++);
		std::cout << "Top Scores:-";
		while (getline(file_r, line)) {
			gotoXY(x_offset + screen_w + 7, y_offset + i++);
			std::cout << line;
		}
	}
	gotoXY(0, y_offset + screen_h + 2);
}
