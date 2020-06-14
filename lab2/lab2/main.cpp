#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

int width, height;

// структура для координаты точки
struct point {
	double x;
	double y;

	point(double x, double y) {
		(*this).x = x;
		(*this).y = y;
	}

	point() : x(0), y(0) {};
};

struct point_int : point {
	int x;
	int y;

	point_int(int x, int y) {
		(*this).x = x;
		(*this).y = y;
	}

	point_int() : x(0), y(0) {};
};

// swap для структуры point
void swap(point& first, point& second) {
	point swap_param = first;
	first = second;
	second = swap_param;
}

// дробная часть числа
double f_part_nmb(double num) {
	double ans = num, fractpart, intpart;
	fractpart = modf(ans, &intpart);
	return fractpart;
}

// запись в файл
bool new_file(int type_p, int width, int height, int maxColorValue, unsigned char* k, char* name_file, int i) {
	FILE* rFile;
	rFile = fopen(name_file, "wb");

	if (rFile == NULL) {
		fputs("File not found!", stderr);
		return false;
	}
	else {
		fprintf(rFile, "P%i\n%i %i\n%i\n", type_p, width, height, maxColorValue);
		fwrite(k, sizeof(unsigned char), width * height * i, rFile);
		fclose(rFile);
		return true;
	}
}

// рисование точки
void plot(point_int& current_p, double col, int& brightness, unsigned char* k, double& gamma)
{
	// проверяем col
	col = (col > 1) ? 1 : col;
	col = (col < 0) ? 0 : col;

	// рисуем только в случае разрешенных координат
	if ((current_p.y >= 0) && (current_p.y < height) && (current_p.x >= 0) && (current_p.x < width)) {
		double plot_was = k[current_p.y * width + current_p.x] / 255.0;
		double plot_add = brightness / 255.0;

		double srgb_was = (plot_was <= 0.04045) ? (25 * plot_was / 323) : (pow((200 * plot_was + 11) / 211, gamma));
		double srgb_add = (plot_add <= 0.04045) ? (25 * plot_add / 323) : (pow((200 * plot_add + 11) / 211, gamma));

		double srgb_final = srgb_was * (1 - col) + srgb_add * col;
		double plot_final = (srgb_final <= 0.0031308) ? (323 * srgb_final / 25) : ((211 * pow(srgb_final, 1 / gamma) - 11) / 200);
		k[current_p.y * width + current_p.x] = round(plot_final * 255);
	}
}

// поиск расстояния от точки до отрезка
double distance_to_point(double& d_x, double& d_y, point& start_p, point& end_p, point_int cur_p, double& thickness) {
	point projection;
	projection.x = ((d_y / d_x) * start_p.x + (d_x / d_y) * (double)cur_p.x - start_p.y + (double)cur_p.y) / (d_y / d_x + d_x / d_y);
	projection.y = -(d_x / d_y) * projection.x + (d_x / d_y) * (double)cur_p.x + (double)cur_p.y;

	double to_x = projection.x - cur_p.x;
	double to_y = projection.y - cur_p.y;
	double distance_to_projection = sqrt(to_x * to_x + to_y * to_y);
	if ((projection.x >= start_p.x) && (projection.x <= end_p.x))
		return (thickness + 1) / 2 - distance_to_projection;

	if (projection.x < start_p.x) {
		double from_x = start_p.x - projection.x;
		double from_y = start_p.y - projection.y;
		double distance_from_projection = sqrt(from_x * from_x + from_y * from_y);
		double sum_distance = sqrt(distance_from_projection * distance_from_projection + distance_to_projection * distance_to_projection);
		if ((thickness + 1) / 2 - sum_distance > 0)
			return min(1 - distance_from_projection, (thickness + 1) / 2 - distance_to_projection);
		return 0;
	}

	// if (projection.x > end_p.x)
	double from_x = end_p.x - projection.x;
	double from_y = end_p.y - projection.y;
	double distance_from_projection = sqrt(from_x * from_x + from_y * from_y);
	double sum_distance = sqrt(distance_from_projection * distance_from_projection + distance_to_projection * distance_to_projection);
	if ((thickness + 1) / 2 - sum_distance > 0)
		return min(1 - distance_from_projection, (thickness + 1) / 2 - distance_to_projection);
	return 0;
}

// обработка картинки на поиск точек, принадлежащих отрезку
void fill(point_int cur_point, double& d_x, double& d_y, point& start_point, point& end_point, int& brightness, double& thickness, unsigned char* k, double& gamma) {
	point_int cur_cur_point = cur_point;
	cur_cur_point.x -= 1;
	int i = 0;
	// переходы влево
	while (cur_cur_point.x >= 0) {
		point_int current = cur_cur_point;
		// переходы вверх
		while (current.y >= 0) {
			double dist_point = distance_to_point(d_x, d_y, start_point, end_point, current, thickness);
			// если точка принадлежит отрезку (с учётом толщины), тогда красим
			if (dist_point > 0) {
				plot(current, dist_point, brightness, k, gamma);
			}
			current.y -= 1;
		}

		current = cur_cur_point;
		current.y += 1;
		// переходы вниз
		while (current.y < height) {
			double dist_point = distance_to_point(d_x, d_y, start_point, end_point, current, thickness);
			// если точка принадлежит отрезку (с учётом толщины), тогда красим
			if (dist_point > 0) {
				plot(current, dist_point, brightness, k, gamma);
			}
			current.y += 1;
		}
		cur_cur_point.x -= 1;
	}

	cur_cur_point = cur_point;
	// переходы вправо
	while (cur_cur_point.x < width) {
		point_int current = cur_cur_point;
		// переходы вверх
		while (current.y >= 0) {
			double dist_point = distance_to_point(d_x, d_y, start_point, end_point, current, thickness);
			// если точка принадлежит отрезку (с учётом толщины), тогда красим
			if (dist_point > 0) {
				plot(current, dist_point, brightness, k, gamma);
			}
			current.y -= 1;
		}

		current = cur_cur_point;
		current.y += 1;
		// переходы вниз
		while (current.y < height) {
			double dist_point = distance_to_point(d_x, d_y, start_point, end_point, current, thickness);
			// если точка принадлежит отрезку (с учётом толщины), тогда красим
			if (dist_point > 0) {
				plot(current, dist_point, brightness, k, gamma);
			}
			current.y += 1;
		}
		cur_cur_point.x += 1;
	}
}

// рисование линии
void draw_line(point& start_point, point& end_point, int& brightness, double& thickness, unsigned char* k, double& gamma) {
	if (start_point.x > end_point.x) {
		swap(start_point, end_point);
	}

	double d_x = end_point.x - start_point.x; // изменение по оси Х
	double d_y = end_point.y - start_point.y; // изменение по оси Y

	if (d_x == 0) {
		d_x = 0.001; // так как значение пикселя лежит в диапазоне от 0 до 255, то 10^(-3) можно считать за незначительную погрешность
		end_point.x += 0.001;
	}
	if (d_y == 0) {
		d_y = 0.001; // так как значение пикселя лежит в диапазоне от 0 до 255, то 10^(-3) можно считать за незначительную погрешность
		end_point.y += 0.001;
	}

	double length = sqrt(d_x * d_x + d_y * d_y);
	double lim_x = d_x / length; // шаг по x
	double lim_y = d_y / length; // шаг по y
	double start_thickness = thickness;

	//обработка псевдостартовой точки
	point_int new_start_point;
	new_start_point.x = round(start_point.x);
	new_start_point.y = round(start_point.y);
	fill(new_start_point, d_x, d_y, start_point, end_point, brightness, thickness, k, gamma);
}

int main(int argc, char* argv[]) {
	FILE* input_file;

	/// получение имени входного файла
	char* input_file_name;
	if (argv[1] != NULL) {
		input_file_name = argv[1];
	}
	else {
		cerr << "input file name is not defined";
		exit(1);
	}

	/// получение имени выходного файла
	char* output_file_name = argv[2];
	if (argv[2] != NULL) {
		output_file_name = argv[2];
	}
	else {
		cerr << "output file name is not defined";
		exit(1);
	}

	/// получение яркости линии
	int brightness;
	if (argv[3] != NULL) {
		brightness = atoi(argv[3]);
	}
	else {
		cerr << "brightness is not defined";
		exit(1);
	}

	/// получение толщины линии
	double thickness;
	if (argv[4] != NULL) {
		thickness = atof(argv[4]);
	}
	else {
		cerr << "thickness is not defined";
		exit(1);
	}

	/// получение координат стартовой точки
	point start_point;
	if ((argv[5] != NULL) && (argv[6] != NULL)) {
		start_point.x = atof(argv[5]); // координата х_0
		start_point.y = atof(argv[6]); // координата y_0
	}
	else {
		cerr << "starting point is not defined";
		exit(1);
	}

	/// получение координат конечной точки
	point end_point;
	if ((argv[7] != NULL) && (argv[8] != NULL)) {
		end_point.x = atof(argv[7]); // координата х_1
		end_point.y = atof(argv[8]); // координата y_1
	}
	else {
		cerr << "end point is not defined";
		exit(1);
	}

	/// получение значения гаммы
	double gamma;
	if (argv[9] != NULL) {
		gamma = atof(argv[9]);
	}
	else {
		gamma = 2; // значение по умолчанию
	}

	/// получение данных из файла
	input_file = fopen(input_file_name, "rb");

	// файл не открылся
	if (input_file == NULL) {
		cerr << "File not found!";
		exit(1);
	}

	int maxColorValue;
	int type_p;
	char c_type;

	// чтение заголовка
	fscanf(input_file, "%c%i%i%i%i\n", &c_type, &type_p, &width, &height, &maxColorValue);

	// проверка первой буквы (должна быть 'P')
	if (c_type != 'P') {
		cerr << "Problem with type of file!";
		fclose(input_file);
		exit(1);
	}

	// проверка значения цвета
	if (maxColorValue != 255) {
		cerr << "Value of maxColor is not correct!";
		fclose(input_file);
		exit(1);
	}

	// проверка на тип файла
	if (!(type_p == 5)) {
		cerr << "Type of file not correct!";
		fclose(input_file);
		exit(1);
	}

	// сохраняем яркостные данные
	unsigned char* k;
	k = (unsigned char*)malloc(sizeof(unsigned char) * height * width);
	int check_size = fread(k, sizeof(unsigned char), width * height, input_file);

	fclose(input_file);

	draw_line(start_point, end_point, brightness, thickness, k, gamma);

	/* for (int i = 0; i < width * height; ++i) {
		k[i] = 0;
	} */

	new_file(type_p, width, height, maxColorValue, k, output_file_name, 1);
	free(k);
	/// конец программы
	exit(0);
}
