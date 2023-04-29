// Markus Buchholz, 2023

#include <stdafx.hpp>
#include "imgui_helper.hpp"
#include <tuple>
#include <vector>
#include <math.h>
#include <random>
#include <algorithm>
#include <thread>
#include <chrono>
#include <list>
#include <tuple>

#include <fstream>
#include <string>
#include <sstream>

//---------------------------------------------------------------

int W = 800;
int H = 800;

float dt = 5.0f;

ImVec2 C(400.0f, 400.0f);
float step = 20.0f;
int scale = 5;
int mesh_size = 80;
float sim_offset = 150.0f;

//---------------------------------------------------------------

struct RGB
{

	float r;
	float g;
	float b;
};

//---------------------------------------------------------------

struct HSV
{
	float h;
	float s;
	float v;
};

//---------------------------------------------------------------

struct Counter
{

	float offset_x;
	float offset_y;
	int current;

	Counter(float x, float y) : offset_x(x), offset_y(y), current(0) {}
};

//---------------------------------------------------------------

std::tuple<std::vector<int>, std::vector<std::vector<float>>> readData(std::string file_name)
{

	std::cout << __FUNCTION__ << "\n";
	std::ifstream file(file_name);
	std::string line;
	int year;
	float f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12;

	std::vector<std::vector<float>> data;
	std::vector<int> y;

	while (std::getline(file, line))
	{

		std::vector<float> data_i;
		std::replace(line.begin(), line.end(), ',', ' ');
		std::istringstream iss(line);
		iss >> year >> f1 >> f2 >> f3 >> f4 >> f5 >> f6 >> f7 >> f8 >> f9 >> f10 >> f11 >> f12;

		y.push_back(year);
		data_i.push_back(f1);
		data_i.push_back(f2);
		data_i.push_back(f3);
		data_i.push_back(f4);
		data_i.push_back(f5);
		data_i.push_back(f6);
		data_i.push_back(f7);
		data_i.push_back(f8);
		data_i.push_back(f9);
		data_i.push_back(f10);
		data_i.push_back(f11);
		data_i.push_back(f12);

		data.push_back(data_i);
	}

	return std::make_tuple(y, data);
}

//---------------------------------------------------------------
int main(int argc, char const *argv[])
{

	auto dataX = readData("../data/climat_data.csv");
	auto year = std::get<0>(dataX);
	auto data = std::get<1>(dataX);

	std::string title = "Marching Squares";
	initImgui(W, H, title);

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.0f / 255.0, 0.0f / 255.0, 0.0f / 255.0, 1.00f);
	ImVec4 white_color = ImVec4(255.0f / 255.0, 255.0f / 255.0, 255.0f / 255.0, 1.00f);
	ImVec4 blue_color = ImVec4(0.0f / 255.0, 0.0f / 255.0, 170.0f / 255.0, 1.00f);
	ImVec4 pink_color = ImVec4(179.0f / 255.0, 12.0f / 255.0, 130.0f / 255.0, 1.00f);
	ImVec4 gray_color = ImVec4(150.0f / 255.0, 10.0f / 160.0, 170.0f / 255.0, 1.00f);
	ImVec4 black_color = ImVec4(0.0f / 255.0, 0.0f / 255.0, 0.0f / 255.0, 1.00f);

	bool flag = true;
	int counter0 = 0;
	int counter1 = 0;
	int counter2 = 0;
	int counter3 = 0;

	Counter c0(350.0f, 250.0f), c1(300.0f, 250.0f), c2(250.0f, 250.0f), c3(200.0f, 250.0f);
	int ci0{year[0] % 10}, ci1{year[0] / 10 % 10}, ci2{year[0] / 100 % 10}, ci3{year[0] / 1000 % 10};
	c0.current = ci0;
	c1.current = ci1;
	c2.current = ci2;
	c3.current = ci3;

	std::vector<ImVec2> climate;

	HSV hsv;
	RGB rgb;

	while (!glfwWindowShouldClose(window) && flag == true)
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		ImGuiWindowFlags window_flags = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(W, H), ImGuiCond_FirstUseEver);
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::Begin("Simulation", nullptr, window_flags);
		ImDrawList *draw_list = ImGui::GetWindowDrawList();

		// draw mesh

		for (int ii = 0; ii < mesh_size; ii++)
		{
			for (int jj = 0; jj < mesh_size; jj++)
			{
				draw_list->AddCircleFilled({ii * step, jj * step}, 0.3f, ImColor(white_color));
			}
		}

		// a
		auto a = [=](Counter c)
		{ draw_list->AddRectFilled({115.0f + c.offset_x, 100.0f + c.offset_y}, {135.0f + c.offset_x, 105.0f + c.offset_y}, ImColor(white_color)); };
		// f
		auto f = [=](Counter c)
		{ draw_list->AddRectFilled({110.0f + c.offset_x, 105.0f + c.offset_y}, {115.0f + c.offset_x, 125.0f + c.offset_y}, ImColor(white_color)); };
		// b
		auto b = [=](Counter c)
		{ draw_list->AddRectFilled({135.0f + c.offset_x, 105.0f + c.offset_y}, {140.0f + c.offset_x, 125.0f + c.offset_y}, ImColor(white_color)); };
		// g
		auto g = [=](Counter c)
		{ draw_list->AddRectFilled({115.0f + c.offset_x, 125.0f + c.offset_y}, {135.0f + c.offset_x, 130.0f + c.offset_y}, ImColor(white_color)); };
		// e
		auto e = [=](Counter c)
		{ draw_list->AddRectFilled({110.0f + c.offset_x, 130.0f + c.offset_y}, {115.0f + c.offset_x, 150.0f + c.offset_y}, ImColor(white_color)); };
		// c
		auto c = [=](Counter c)
		{ draw_list->AddRectFilled({135.0f + c.offset_x, 130.0f + c.offset_y}, {140.0f + c.offset_x, 150.0f + c.offset_y}, ImColor(white_color)); };
		// d
		auto d = [=](Counter c)
		{ draw_list->AddRectFilled({115.0f + c.offset_x, 150.0f + c.offset_y}, {135.0f + c.offset_x, 155.0f + c.offset_y}, ImColor(white_color)); };

		if (counter0 >= year.size() - 1)
		{
			std::this_thread::sleep_for(std::chrono::seconds(10));
			return -1;
		}
		counter0++;

		ci0 = counter0 % 10;
		c0.current = ci0;

		if (ci0 == 0)
		{
			ci1++;
			ci1 = ci1 % 10;
			c1.current = ci1;
		}

		if (ci1 == 0 && ci0 == 0)
		{
			ci2++;
			ci2 = ci2 % 10;
			c2.current = ci2;
		}

		if (ci2 == 0 && ci1 == 0 && ci0 == 0)
		{
			ci3++;
			ci3 = ci3 % 10;
			c3.current = ci3;
		}

		draw_list->AddCircle({400.0f, 400.0f}, 150.0f, ImColor(white_color)); //-1C
		draw_list->AddCircle({400.0f, 400.0f}, 200.0f, ImColor(blue_color));  // 0C
		draw_list->AddCircle({400.0f, 400.0f}, 250.0f, ImColor(white_color)); // 1C
		draw_list->AddCircle({400.0f, 400.0f}, 350.0f, ImColor(pink_color));

		for (auto &ii : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12})
		{

			float mx = C.x + 350.0f * std::cos(ii * M_PI * 2 / 12);
			float my = C.y + 350.0f * std::sin(ii * M_PI * 2 / 12);
			draw_list->AddCircleFilled({mx, my}, 5.0f, ImColor(pink_color)); 


		}

		auto data_i = data[counter0 - 1];

		for (int ii = 0; ii < data_i.size(); ii++)
		{

			float mx0 = C.x + (200.0f + data_i[ii] * 100.0f) * std::cos(ii * M_PI * 2 / 12);
			float my0 = C.y + (200.0f + data_i[ii] * 100.0f) * std::sin(ii * M_PI * 2 / 12);

			climate.push_back({mx0, my0});
			climate.push_back({mx0, my0});
		}
		if (climate.size() >= 2)
		{

			for (int ii = 0; ii < climate.size() - 1; ii++)
			{

				float D = std::sqrt(std::pow(C.x - climate[ii].x, 2) + std::pow(C.y - climate[ii].y, 2));
				float dx = (D * 0.1f * 0.6f) / 19.5f;
				hsv = {dx, 1.0f, 1.0f};
				ImGui::ColorConvertHSVtoRGB(hsv.h, hsv.s, hsv.v, rgb.r, rgb.g, rgb.b);

				ImVec4 change_color = ImVec4(rgb.r, rgb.g, rgb.b, 1.0f);

				draw_list->AddLine({climate[ii].x, climate[ii].y}, {climate[ii + 1].x, climate[ii + 1].y}, ImColor(change_color), 0.5f);
			}
		}
		int ii = 0;

		for (auto &ci : {c0, c1, c2, c3})
		{
			switch (ci.current)
			{
			case 0:
				a(ci);
				b(ci);
				c(ci);
				d(ci);
				e(ci);
				f(ci);
				break;

			case 1:
				b(ci);
				c(ci);
				break;

			case 2:
				a(ci);
				b(ci);
				d(ci);
				e(ci);
				g(ci);
				break;
			case 3:
				a(ci);
				b(ci);
				c(ci);
				d(ci);
				g(ci);
				break;
			case 4:
				c(ci);
				f(ci);
				g(ci);
				break;
			case 5:
				a(ci);
				c(ci);
				d(ci);
				f(ci);
				g(ci);
				break;
			case 6:
				a(ci);
				c(ci);
				d(ci);
				e(ci);
				f(ci);
				g(ci);
				break;
			case 7:
				a(ci);
				b(ci);
				c(ci);
				break;
			case 8:
				a(ci);
				b(ci);
				c(ci);
				d(ci);
				e(ci);
				f(ci);
				g(ci);
				break;
			case 9:
				a(ci);
				b(ci);
				c(ci);
				d(ci);
				f(ci);
				g(ci);
				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	termImgui();
	return 0;
}
